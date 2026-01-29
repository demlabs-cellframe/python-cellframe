/**
 * @file cf_ledger_callback_registry.c
 * @brief Implementation of global ledger callback registry
 */

#include "cf_ledger_callback_registry.h"
#include "dap_common.h"
#include "uthash.h"

#define LOG_TAG "cf_ledger_callback_registry"

// Registry entry for cache callbacks (keyed by ledger pointer)
typedef struct {
    dap_ledger_t *ledger;               // Key
    python_ledger_cache_ctx_t *context; // Value
    UT_hash_handle hh;
} cache_callback_entry_t;

// Registry entry for service callbacks (keyed by service UID)
typedef struct {
    dap_chain_srv_uid_t uid;            // Key
    python_service_tag_ctx_t *context;  // Value
    UT_hash_handle hh;
} service_callback_entry_t;

// Registry entry for callbacks keyed by decree type/subtype
typedef struct {
    uint32_t key;                        // (type << 16) | subtype
    python_ledger_simple_ctx_t *context; // Value
    UT_hash_handle hh;
} typed_callback_entry_t;

// Registry entry for per-ledger callbacks (keyed by ledger pointer)
typedef struct {
    dap_ledger_t *ledger;                       // Key
    python_ledger_balance_ctx_t *balance_list;  // Balance change callbacks
    python_ledger_simple_ctx_t fee_callback;
    python_ledger_simple_ctx_t reward_removed_callback;
    python_ledger_simple_ctx_t decree_set_fee_callback;
    python_ledger_simple_ctx_t decree_generic_callback;
    python_ledger_simple_ctx_t get_cur_cell_callback;
    UT_hash_handle hh;
} ledger_callback_entry_t;

// Global state
static cache_callback_entry_t *s_cache_callbacks = NULL;
static service_callback_entry_t *s_service_callbacks = NULL;
static python_tax_ctx_t *s_tax_callback = NULL;
static typed_callback_entry_t *s_decree_callbacks = NULL;
static typed_callback_entry_t *s_anchor_unload_callbacks = NULL;
static ledger_callback_entry_t *s_ledger_callbacks = NULL;
static python_ledger_wallet_ctx_t *s_wallet_callbacks = NULL;
static python_ledger_wallet_cache_ctx_t *s_wallet_cache_callbacks = NULL;
static python_ledger_mempool_ctx_t *s_mempool_callbacks = NULL;
static python_ledger_callbacks_ctx_t *s_ledger_global_callbacks = NULL;
static pthread_mutex_t s_registry_mutex = PTHREAD_MUTEX_INITIALIZER;
static bool s_initialized = false;

static uint32_t cf_ledger_type_key(uint16_t a_type, uint16_t a_subtype) {
    return ((uint32_t)a_type << 16) | (uint32_t)a_subtype;
}

/**
 * @brief Initialize ledger callback registry
 */
int cf_ledger_callback_registry_init(void) {
    pthread_mutex_lock(&s_registry_mutex);
    
    if (s_initialized) {
        pthread_mutex_unlock(&s_registry_mutex);
        return 0;
    }
    
    s_cache_callbacks = NULL;
    s_service_callbacks = NULL;
    s_tax_callback = NULL;
    s_decree_callbacks = NULL;
    s_anchor_unload_callbacks = NULL;
    s_ledger_callbacks = NULL;
    s_wallet_callbacks = NULL;
    s_wallet_cache_callbacks = NULL;
    s_mempool_callbacks = NULL;
    s_ledger_global_callbacks = NULL;
    s_initialized = true;
    
    pthread_mutex_unlock(&s_registry_mutex);
    log_it(L_INFO, "Ledger callback registry initialized");
    return 0;
}

/**
 * @brief Cleanup all ledger callbacks
 */
void cf_ledger_callback_registry_cleanup_all(void) {
    pthread_mutex_lock(&s_registry_mutex);
    
    if (!s_initialized) {
        pthread_mutex_unlock(&s_registry_mutex);
        return;
    }
    
    // Cleanup cache callbacks
    cache_callback_entry_t *cache_entry, *cache_tmp;
    HASH_ITER(hh, s_cache_callbacks, cache_entry, cache_tmp) {
        HASH_DEL(s_cache_callbacks, cache_entry);
        if (cache_entry->context) {
            Py_XDECREF(cache_entry->context->callback);
            Py_XDECREF(cache_entry->context->user_data);
            DAP_DELETE(cache_entry->context);
        }
        DAP_DELETE(cache_entry);
    }
    
    // Cleanup service callbacks
    service_callback_entry_t *service_entry, *service_tmp;
    HASH_ITER(hh, s_service_callbacks, service_entry, service_tmp) {
        HASH_DEL(s_service_callbacks, service_entry);
        if (service_entry->context) {
            Py_XDECREF(service_entry->context->callback);
            Py_XDECREF(service_entry->context->user_data);
            DAP_DELETE(service_entry->context->tag_str);
            DAP_DELETE(service_entry->context);
        }
        DAP_DELETE(service_entry);
    }

    // Cleanup decree callbacks
    typed_callback_entry_t *typed_entry, *typed_tmp;
    HASH_ITER(hh, s_decree_callbacks, typed_entry, typed_tmp) {
        HASH_DEL(s_decree_callbacks, typed_entry);
        if (typed_entry->context) {
            Py_XDECREF(typed_entry->context->callback);
            Py_XDECREF(typed_entry->context->user_data);
            DAP_DELETE(typed_entry->context);
        }
        DAP_DELETE(typed_entry);
    }

    // Cleanup anchor unload callbacks
    HASH_ITER(hh, s_anchor_unload_callbacks, typed_entry, typed_tmp) {
        HASH_DEL(s_anchor_unload_callbacks, typed_entry);
        if (typed_entry->context) {
            Py_XDECREF(typed_entry->context->callback);
            Py_XDECREF(typed_entry->context->user_data);
            DAP_DELETE(typed_entry->context);
        }
        DAP_DELETE(typed_entry);
    }
    
    // Cleanup tax callback
    if (s_tax_callback) {
        Py_XDECREF(s_tax_callback->callback);
        Py_XDECREF(s_tax_callback->user_data);
        DAP_DELETE(s_tax_callback);
        s_tax_callback = NULL;
    }

    // Cleanup per-ledger callbacks
    ledger_callback_entry_t *ledger_entry, *ledger_tmp;
    HASH_ITER(hh, s_ledger_callbacks, ledger_entry, ledger_tmp) {
        HASH_DEL(s_ledger_callbacks, ledger_entry);
        python_ledger_balance_ctx_t *bal_ctx = ledger_entry->balance_list;
        while (bal_ctx) {
            python_ledger_balance_ctx_t *next_ctx = bal_ctx->next;
            Py_XDECREF(bal_ctx->callback);
            Py_XDECREF(bal_ctx->user_data);
            DAP_DELETE(bal_ctx);
            bal_ctx = next_ctx;
        }
        Py_XDECREF(ledger_entry->fee_callback.callback);
        Py_XDECREF(ledger_entry->fee_callback.user_data);
        Py_XDECREF(ledger_entry->reward_removed_callback.callback);
        Py_XDECREF(ledger_entry->reward_removed_callback.user_data);
        Py_XDECREF(ledger_entry->decree_set_fee_callback.callback);
        Py_XDECREF(ledger_entry->decree_set_fee_callback.user_data);
        Py_XDECREF(ledger_entry->decree_generic_callback.callback);
        Py_XDECREF(ledger_entry->decree_generic_callback.user_data);
        Py_XDECREF(ledger_entry->get_cur_cell_callback.callback);
        Py_XDECREF(ledger_entry->get_cur_cell_callback.user_data);
        DAP_DELETE(ledger_entry);
    }

    // Cleanup wallet callbacks
    if (s_wallet_callbacks) {
        Py_XDECREF(s_wallet_callbacks->sign_callback);
        Py_XDECREF(s_wallet_callbacks->get_pkey_hash_callback);
        Py_XDECREF(s_wallet_callbacks->get_pkey_callback);
        Py_XDECREF(s_wallet_callbacks->get_addr_callback);
        Py_XDECREF(s_wallet_callbacks->check_sign_callback);
        Py_XDECREF(s_wallet_callbacks->user_data);
        DAP_DELETE(s_wallet_callbacks->last_sign_info);
        DAP_DELETE(s_wallet_callbacks);
        s_wallet_callbacks = NULL;
    }

    // Cleanup wallet cache callbacks
    if (s_wallet_cache_callbacks) {
        Py_XDECREF(s_wallet_cache_callbacks->tx_find_callback);
        Py_XDECREF(s_wallet_cache_callbacks->iter_create_callback);
        Py_XDECREF(s_wallet_cache_callbacks->iter_get_callback);
        Py_XDECREF(s_wallet_cache_callbacks->iter_delete_callback);
        Py_XDECREF(s_wallet_cache_callbacks->user_data);
        DAP_DELETE(s_wallet_cache_callbacks->last_main_ticker);
        DAP_DELETE(s_wallet_cache_callbacks);
        s_wallet_cache_callbacks = NULL;
    }

    // Cleanup mempool callbacks
    if (s_mempool_callbacks) {
        Py_XDECREF(s_mempool_callbacks->add_datum_callback);
        Py_XDECREF(s_mempool_callbacks->create_tx_callback);
        Py_XDECREF(s_mempool_callbacks->user_data);
        DAP_DELETE(s_mempool_callbacks);
        s_mempool_callbacks = NULL;
    }

    // Cleanup ledger callbacks
    if (s_ledger_global_callbacks) {
        Py_XDECREF(s_ledger_global_callbacks->addr_to_wallet_name_callback);
        Py_XDECREF(s_ledger_global_callbacks->wallet_info_to_json_callback);
        Py_XDECREF(s_ledger_global_callbacks->tx_added_callback);
        Py_XDECREF(s_ledger_global_callbacks->user_data);
        DAP_DELETE(s_ledger_global_callbacks->last_wallet_name);
        DAP_DELETE(s_ledger_global_callbacks);
        s_ledger_global_callbacks = NULL;
    }
    
    s_cache_callbacks = NULL;
    s_service_callbacks = NULL;
    s_decree_callbacks = NULL;
    s_anchor_unload_callbacks = NULL;
    s_ledger_callbacks = NULL;
    
    pthread_mutex_unlock(&s_registry_mutex);
    log_it(L_INFO, "Ledger callback registry cleaned up");
}

/**
 * @brief Deinitialize ledger callback registry
 */
void cf_ledger_callback_registry_deinit(void) {
    cf_ledger_callback_registry_cleanup_all();
    
    pthread_mutex_lock(&s_registry_mutex);
    s_initialized = false;
    pthread_mutex_unlock(&s_registry_mutex);
    
    log_it(L_INFO, "Ledger callback registry deinitialized");
}

/**
 * @brief Register cache check callback for ledger
 */
int cf_ledger_cache_callback_register(dap_ledger_t *a_ledger, PyObject *a_callback, PyObject *a_user_data) {
    if (!a_ledger || !a_callback) {
        log_it(L_ERROR, "Invalid arguments for cache callback registration");
        return -1;
    }
    
    pthread_mutex_lock(&s_registry_mutex);
    
    if (!s_initialized) {
        pthread_mutex_unlock(&s_registry_mutex);
        log_it(L_ERROR, "Registry not initialized");
        return -1;
    }
    
    // Check if already exists
    cache_callback_entry_t *existing = NULL;
    HASH_FIND_PTR(s_cache_callbacks, &a_ledger, existing);
    if (existing) {
        // Replace existing
        Py_XDECREF(existing->context->callback);
        Py_XDECREF(existing->context->user_data);
        existing->context->callback = a_callback;
        existing->context->user_data = a_user_data;
        Py_INCREF(a_callback);
        Py_INCREF(a_user_data);
        pthread_mutex_unlock(&s_registry_mutex);
        log_it(L_DEBUG, "Replaced cache callback for ledger %p", a_ledger);
        return 0;
    }
    
    // Create new entry
    cache_callback_entry_t *entry = DAP_NEW_Z(cache_callback_entry_t);
    if (!entry) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }
    
    entry->context = DAP_NEW_Z(python_ledger_cache_ctx_t);
    if (!entry->context) {
        DAP_DELETE(entry);
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }
    
    entry->ledger = a_ledger;
    entry->context->ledger = a_ledger;
    entry->context->callback = a_callback;
    entry->context->user_data = a_user_data;
    
    Py_INCREF(a_callback);
    Py_INCREF(a_user_data);
    
    HASH_ADD_PTR(s_cache_callbacks, ledger, entry);
    
    pthread_mutex_unlock(&s_registry_mutex);
    log_it(L_DEBUG, "Registered cache callback for ledger %p", a_ledger);
    return 0;
}

/**
 * @brief Get cache check callback context for ledger
 */
python_ledger_cache_ctx_t* cf_ledger_cache_callback_get(dap_ledger_t *a_ledger) {
    if (!a_ledger) {
        return NULL;
    }
    
    pthread_mutex_lock(&s_registry_mutex);
    
    cache_callback_entry_t *entry = NULL;
    HASH_FIND_PTR(s_cache_callbacks, &a_ledger, entry);
    
    python_ledger_cache_ctx_t *result = entry ? entry->context : NULL;
    
    pthread_mutex_unlock(&s_registry_mutex);
    return result;
}

/**
 * @brief Unregister cache check callback for ledger
 */
void cf_ledger_cache_callback_unregister(dap_ledger_t *a_ledger) {
    if (!a_ledger) {
        return;
    }
    
    pthread_mutex_lock(&s_registry_mutex);
    
    cache_callback_entry_t *entry = NULL;
    HASH_FIND_PTR(s_cache_callbacks, &a_ledger, entry);
    
    if (entry) {
        HASH_DEL(s_cache_callbacks, entry);
        if (entry->context) {
            Py_XDECREF(entry->context->callback);
            Py_XDECREF(entry->context->user_data);
            DAP_DELETE(entry->context);
        }
        DAP_DELETE(entry);
        log_it(L_DEBUG, "Unregistered cache callback for ledger %p", a_ledger);
    }
    
    pthread_mutex_unlock(&s_registry_mutex);
}

/**
 * @brief Register service tag check callback
 */
int cf_ledger_service_callback_register(dap_chain_srv_uid_t a_uid, const char *a_tag_str, 
                                         PyObject *a_callback, PyObject *a_user_data) {
    if (!a_callback || !a_tag_str) {
        log_it(L_ERROR, "Invalid arguments for service callback registration");
        return -1;
    }
    
    pthread_mutex_lock(&s_registry_mutex);
    
    if (!s_initialized) {
        pthread_mutex_unlock(&s_registry_mutex);
        log_it(L_ERROR, "Registry not initialized");
        return -1;
    }
    
    // Check if already exists
    service_callback_entry_t *existing = NULL;
    HASH_FIND(hh, s_service_callbacks, &a_uid, sizeof(dap_chain_srv_uid_t), existing);
    if (existing) {
        // Replace existing
        Py_XDECREF(existing->context->callback);
        Py_XDECREF(existing->context->user_data);
        DAP_DELETE(existing->context->tag_str);
        
        existing->context->callback = a_callback;
        existing->context->user_data = a_user_data;
        existing->context->tag_str = dap_strdup(a_tag_str);
        
        Py_INCREF(a_callback);
        Py_INCREF(a_user_data);
        
        pthread_mutex_unlock(&s_registry_mutex);
        log_it(L_DEBUG, "Replaced service callback for UID 0x%llx", a_uid.uint64);
        return 0;
    }
    
    // Create new entry
    service_callback_entry_t *entry = DAP_NEW_Z(service_callback_entry_t);
    if (!entry) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }
    
    entry->context = DAP_NEW_Z(python_service_tag_ctx_t);
    if (!entry->context) {
        DAP_DELETE(entry);
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }
    
    entry->uid = a_uid;
    entry->context->uid = a_uid;
    entry->context->callback = a_callback;
    entry->context->user_data = a_user_data;
    entry->context->tag_str = dap_strdup(a_tag_str);
    
    Py_INCREF(a_callback);
    Py_INCREF(a_user_data);
    
    HASH_ADD(hh, s_service_callbacks, uid, sizeof(dap_chain_srv_uid_t), entry);
    
    pthread_mutex_unlock(&s_registry_mutex);
    log_it(L_DEBUG, "Registered service callback for UID 0x%llx, tag '%s'", a_uid.uint64, a_tag_str);
    return 0;
}

/**
 * @brief Get service tag check callback context
 */
python_service_tag_ctx_t* cf_ledger_service_callback_get(dap_chain_srv_uid_t a_uid) {
    pthread_mutex_lock(&s_registry_mutex);
    
    service_callback_entry_t *entry = NULL;
    HASH_FIND(hh, s_service_callbacks, &a_uid, sizeof(dap_chain_srv_uid_t), entry);
    
    python_service_tag_ctx_t *result = entry ? entry->context : NULL;
    
    pthread_mutex_unlock(&s_registry_mutex);
    return result;
}

/**
 * @brief Unregister service tag check callback
 */
void cf_ledger_service_callback_unregister(dap_chain_srv_uid_t a_uid) {
    pthread_mutex_lock(&s_registry_mutex);
    
    service_callback_entry_t *entry = NULL;
    HASH_FIND(hh, s_service_callbacks, &a_uid, sizeof(dap_chain_srv_uid_t), entry);
    
    if (entry) {
        HASH_DEL(s_service_callbacks, entry);
        if (entry->context) {
            Py_XDECREF(entry->context->callback);
            Py_XDECREF(entry->context->user_data);
            DAP_DELETE(entry->context->tag_str);
            DAP_DELETE(entry->context);
        }
        DAP_DELETE(entry);
        log_it(L_DEBUG, "Unregistered service callback for UID 0x%llx", a_uid.uint64);
    }
    
    pthread_mutex_unlock(&s_registry_mutex);
}

/**
 * @brief Register global tax callback (singleton)
 */
int cf_ledger_tax_callback_register(PyObject *a_callback, PyObject *a_user_data) {
    if (!a_callback) {
        log_it(L_ERROR, "Invalid arguments for tax callback registration");
        return -1;
    }
    
    pthread_mutex_lock(&s_registry_mutex);
    
    if (!s_initialized) {
        pthread_mutex_unlock(&s_registry_mutex);
        log_it(L_ERROR, "Registry not initialized");
        return -1;
    }
    
    // Replace existing if any
    if (s_tax_callback) {
        Py_XDECREF(s_tax_callback->callback);
        Py_XDECREF(s_tax_callback->user_data);
        s_tax_callback->callback = a_callback;
        s_tax_callback->user_data = a_user_data;
        Py_INCREF(a_callback);
        Py_INCREF(a_user_data);
        pthread_mutex_unlock(&s_registry_mutex);
        log_it(L_DEBUG, "Replaced global tax callback");
        return 0;
    }
    
    // Create new
    s_tax_callback = DAP_NEW_Z(python_tax_ctx_t);
    if (!s_tax_callback) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }
    
    s_tax_callback->callback = a_callback;
    s_tax_callback->user_data = a_user_data;
    
    Py_INCREF(a_callback);
    Py_INCREF(a_user_data);
    
    pthread_mutex_unlock(&s_registry_mutex);
    log_it(L_DEBUG, "Registered global tax callback");
    return 0;
}

/**
 * @brief Get global tax callback context
 */
python_tax_ctx_t* cf_ledger_tax_callback_get(void) {
    pthread_mutex_lock(&s_registry_mutex);
    python_tax_ctx_t *result = s_tax_callback;
    pthread_mutex_unlock(&s_registry_mutex);
    return result;
}

/**
 * @brief Unregister global tax callback
 */
void cf_ledger_tax_callback_unregister(void) {
    pthread_mutex_lock(&s_registry_mutex);
    
    if (s_tax_callback) {
        Py_XDECREF(s_tax_callback->callback);
        Py_XDECREF(s_tax_callback->user_data);
        DAP_DELETE(s_tax_callback);
        s_tax_callback = NULL;
        log_it(L_DEBUG, "Unregistered global tax callback");
    }
    
    pthread_mutex_unlock(&s_registry_mutex);
}

// =============================================================================
// PER-LEDGER CALLBACKS REGISTRY
// =============================================================================

static ledger_callback_entry_t *s_ledger_entry_get_or_create(dap_ledger_t *a_ledger) {
    ledger_callback_entry_t *entry = NULL;
    HASH_FIND_PTR(s_ledger_callbacks, &a_ledger, entry);
    if (entry) {
        return entry;
    }
    entry = DAP_NEW_Z(ledger_callback_entry_t);
    if (!entry) {
        return NULL;
    }
    entry->ledger = a_ledger;
    HASH_ADD_PTR(s_ledger_callbacks, ledger, entry);
    return entry;
}

static void s_ledger_simple_callback_set(python_ledger_simple_ctx_t *slot, PyObject *a_callback, PyObject *a_user_data) {
    if (!slot) {
        return;
    }
    Py_XDECREF(slot->callback);
    Py_XDECREF(slot->user_data);
    slot->callback = a_callback;
    slot->user_data = a_user_data;
    Py_XINCREF(a_callback);
    Py_XINCREF(a_user_data);
}

int cf_ledger_balance_callback_add(dap_ledger_t *a_ledger, PyObject *a_callback, PyObject *a_user_data) {
    if (!a_ledger || !a_callback) {
        log_it(L_ERROR, "Invalid arguments for balance callback registration");
        return -1;
    }

    pthread_mutex_lock(&s_registry_mutex);

    if (!s_initialized) {
        pthread_mutex_unlock(&s_registry_mutex);
        log_it(L_ERROR, "Registry not initialized");
        return -1;
    }

    ledger_callback_entry_t *entry = s_ledger_entry_get_or_create(a_ledger);
    if (!entry) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    bool was_empty = entry->balance_list == NULL;
    python_ledger_balance_ctx_t *ctx = DAP_NEW_Z(python_ledger_balance_ctx_t);
    if (!ctx) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    ctx->callback = a_callback;
    ctx->user_data = a_user_data;
    Py_INCREF(a_callback);
    Py_XINCREF(a_user_data);

    ctx->next = entry->balance_list;
    entry->balance_list = ctx;

    pthread_mutex_unlock(&s_registry_mutex);
    log_it(L_DEBUG, "Registered balance change callback for ledger %p", a_ledger);
    return was_empty ? 1 : 0;
}

int cf_ledger_balance_callback_remove(dap_ledger_t *a_ledger, PyObject *a_callback) {
    if (!a_ledger || !a_callback) {
        return -1;
    }

    pthread_mutex_lock(&s_registry_mutex);

    if (!s_initialized) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    ledger_callback_entry_t *entry = NULL;
    HASH_FIND_PTR(s_ledger_callbacks, &a_ledger, entry);
    if (!entry) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    python_ledger_balance_ctx_t **current = &entry->balance_list;
    while (*current) {
        if ((*current)->callback == a_callback) {
            python_ledger_balance_ctx_t *to_delete = *current;
            *current = to_delete->next;
            Py_XDECREF(to_delete->callback);
            Py_XDECREF(to_delete->user_data);
            DAP_DELETE(to_delete);
            bool now_empty = entry->balance_list == NULL;
            pthread_mutex_unlock(&s_registry_mutex);
            log_it(L_DEBUG, "Unregistered balance change callback for ledger %p", a_ledger);
            return now_empty ? 1 : 0;
        }
        current = &(*current)->next;
    }

    pthread_mutex_unlock(&s_registry_mutex);
    return -1;
}

python_ledger_balance_ctx_t* cf_ledger_balance_callback_get(dap_ledger_t *a_ledger) {
    if (!a_ledger) {
        return NULL;
    }

    pthread_mutex_lock(&s_registry_mutex);
    ledger_callback_entry_t *entry = NULL;
    HASH_FIND_PTR(s_ledger_callbacks, &a_ledger, entry);
    python_ledger_balance_ctx_t *result = entry ? entry->balance_list : NULL;
    pthread_mutex_unlock(&s_registry_mutex);
    return result;
}

int cf_ledger_fee_callback_register(dap_ledger_t *a_ledger, PyObject *a_callback, PyObject *a_user_data) {
    if (!a_ledger) {
        return -1;
    }

    pthread_mutex_lock(&s_registry_mutex);
    if (!s_initialized) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    ledger_callback_entry_t *entry = s_ledger_entry_get_or_create(a_ledger);
    if (!entry) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    s_ledger_simple_callback_set(&entry->fee_callback, a_callback, a_user_data);
    pthread_mutex_unlock(&s_registry_mutex);
    return 0;
}

python_ledger_simple_ctx_t* cf_ledger_fee_callback_get(dap_ledger_t *a_ledger) {
    if (!a_ledger) {
        return NULL;
    }
    pthread_mutex_lock(&s_registry_mutex);
    ledger_callback_entry_t *entry = NULL;
    HASH_FIND_PTR(s_ledger_callbacks, &a_ledger, entry);
    python_ledger_simple_ctx_t *result = entry ? &entry->fee_callback : NULL;
    pthread_mutex_unlock(&s_registry_mutex);
    return result;
}

void cf_ledger_fee_callback_unregister(dap_ledger_t *a_ledger) {
    if (!a_ledger) {
        return;
    }
    pthread_mutex_lock(&s_registry_mutex);
    ledger_callback_entry_t *entry = NULL;
    HASH_FIND_PTR(s_ledger_callbacks, &a_ledger, entry);
    if (entry) {
        s_ledger_simple_callback_set(&entry->fee_callback, NULL, NULL);
    }
    pthread_mutex_unlock(&s_registry_mutex);
}

int cf_ledger_reward_removed_callback_register(dap_ledger_t *a_ledger, PyObject *a_callback, PyObject *a_user_data) {
    if (!a_ledger) {
        return -1;
    }

    pthread_mutex_lock(&s_registry_mutex);
    if (!s_initialized) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    ledger_callback_entry_t *entry = s_ledger_entry_get_or_create(a_ledger);
    if (!entry) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    s_ledger_simple_callback_set(&entry->reward_removed_callback, a_callback, a_user_data);
    pthread_mutex_unlock(&s_registry_mutex);
    return 0;
}

python_ledger_simple_ctx_t* cf_ledger_reward_removed_callback_get(dap_ledger_t *a_ledger) {
    if (!a_ledger) {
        return NULL;
    }
    pthread_mutex_lock(&s_registry_mutex);
    ledger_callback_entry_t *entry = NULL;
    HASH_FIND_PTR(s_ledger_callbacks, &a_ledger, entry);
    python_ledger_simple_ctx_t *result = entry ? &entry->reward_removed_callback : NULL;
    pthread_mutex_unlock(&s_registry_mutex);
    return result;
}

void cf_ledger_reward_removed_callback_unregister(dap_ledger_t *a_ledger) {
    if (!a_ledger) {
        return;
    }
    pthread_mutex_lock(&s_registry_mutex);
    ledger_callback_entry_t *entry = NULL;
    HASH_FIND_PTR(s_ledger_callbacks, &a_ledger, entry);
    if (entry) {
        s_ledger_simple_callback_set(&entry->reward_removed_callback, NULL, NULL);
    }
    pthread_mutex_unlock(&s_registry_mutex);
}

int cf_ledger_decree_set_fee_callback_register(dap_ledger_t *a_ledger, PyObject *a_callback, PyObject *a_user_data) {
    if (!a_ledger) {
        return -1;
    }

    pthread_mutex_lock(&s_registry_mutex);
    if (!s_initialized) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    ledger_callback_entry_t *entry = s_ledger_entry_get_or_create(a_ledger);
    if (!entry) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    s_ledger_simple_callback_set(&entry->decree_set_fee_callback, a_callback, a_user_data);
    pthread_mutex_unlock(&s_registry_mutex);
    return 0;
}

python_ledger_simple_ctx_t* cf_ledger_decree_set_fee_callback_get(dap_ledger_t *a_ledger) {
    if (!a_ledger) {
        return NULL;
    }
    pthread_mutex_lock(&s_registry_mutex);
    ledger_callback_entry_t *entry = NULL;
    HASH_FIND_PTR(s_ledger_callbacks, &a_ledger, entry);
    python_ledger_simple_ctx_t *result = entry ? &entry->decree_set_fee_callback : NULL;
    pthread_mutex_unlock(&s_registry_mutex);
    return result;
}

void cf_ledger_decree_set_fee_callback_unregister(dap_ledger_t *a_ledger) {
    if (!a_ledger) {
        return;
    }
    pthread_mutex_lock(&s_registry_mutex);
    ledger_callback_entry_t *entry = NULL;
    HASH_FIND_PTR(s_ledger_callbacks, &a_ledger, entry);
    if (entry) {
        s_ledger_simple_callback_set(&entry->decree_set_fee_callback, NULL, NULL);
    }
    pthread_mutex_unlock(&s_registry_mutex);
}

int cf_ledger_decree_generic_callback_register(dap_ledger_t *a_ledger, PyObject *a_callback, PyObject *a_user_data) {
    if (!a_ledger) {
        return -1;
    }

    pthread_mutex_lock(&s_registry_mutex);
    if (!s_initialized) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    ledger_callback_entry_t *entry = s_ledger_entry_get_or_create(a_ledger);
    if (!entry) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    s_ledger_simple_callback_set(&entry->decree_generic_callback, a_callback, a_user_data);
    pthread_mutex_unlock(&s_registry_mutex);
    return 0;
}

python_ledger_simple_ctx_t* cf_ledger_decree_generic_callback_get(dap_ledger_t *a_ledger) {
    if (!a_ledger) {
        return NULL;
    }
    pthread_mutex_lock(&s_registry_mutex);
    ledger_callback_entry_t *entry = NULL;
    HASH_FIND_PTR(s_ledger_callbacks, &a_ledger, entry);
    python_ledger_simple_ctx_t *result = entry ? &entry->decree_generic_callback : NULL;
    pthread_mutex_unlock(&s_registry_mutex);
    return result;
}

void cf_ledger_decree_generic_callback_unregister(dap_ledger_t *a_ledger) {
    if (!a_ledger) {
        return;
    }
    pthread_mutex_lock(&s_registry_mutex);
    ledger_callback_entry_t *entry = NULL;
    HASH_FIND_PTR(s_ledger_callbacks, &a_ledger, entry);
    if (entry) {
        s_ledger_simple_callback_set(&entry->decree_generic_callback, NULL, NULL);
    }
    pthread_mutex_unlock(&s_registry_mutex);
}

int cf_ledger_get_cur_cell_callback_register(dap_ledger_t *a_ledger, PyObject *a_callback, PyObject *a_user_data) {
    if (!a_ledger) {
        return -1;
    }

    pthread_mutex_lock(&s_registry_mutex);
    if (!s_initialized) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    ledger_callback_entry_t *entry = s_ledger_entry_get_or_create(a_ledger);
    if (!entry) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    s_ledger_simple_callback_set(&entry->get_cur_cell_callback, a_callback, a_user_data);
    pthread_mutex_unlock(&s_registry_mutex);
    return 0;
}

python_ledger_simple_ctx_t* cf_ledger_get_cur_cell_callback_get(dap_ledger_t *a_ledger) {
    if (!a_ledger) {
        return NULL;
    }
    pthread_mutex_lock(&s_registry_mutex);
    ledger_callback_entry_t *entry = NULL;
    HASH_FIND_PTR(s_ledger_callbacks, &a_ledger, entry);
    python_ledger_simple_ctx_t *result = entry ? &entry->get_cur_cell_callback : NULL;
    pthread_mutex_unlock(&s_registry_mutex);
    return result;
}

void cf_ledger_get_cur_cell_callback_unregister(dap_ledger_t *a_ledger) {
    if (!a_ledger) {
        return;
    }
    pthread_mutex_lock(&s_registry_mutex);
    ledger_callback_entry_t *entry = NULL;
    HASH_FIND_PTR(s_ledger_callbacks, &a_ledger, entry);
    if (entry) {
        s_ledger_simple_callback_set(&entry->get_cur_cell_callback, NULL, NULL);
    }
    pthread_mutex_unlock(&s_registry_mutex);
}

int cf_ledger_decree_callback_register(uint16_t a_type, uint16_t a_subtype, PyObject *a_callback, PyObject *a_user_data) {
    if (!a_callback) {
        return -1;
    }

    pthread_mutex_lock(&s_registry_mutex);
    if (!s_initialized) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    uint32_t key = cf_ledger_type_key(a_type, a_subtype);
    typed_callback_entry_t *entry = NULL;
    HASH_FIND(hh, s_decree_callbacks, &key, sizeof(key), entry);
    if (entry) {
        Py_XDECREF(entry->context->callback);
        Py_XDECREF(entry->context->user_data);
        entry->context->callback = a_callback;
        entry->context->user_data = a_user_data;
        Py_INCREF(a_callback);
        Py_XINCREF(a_user_data);
        pthread_mutex_unlock(&s_registry_mutex);
        return 0;
    }

    entry = DAP_NEW_Z(typed_callback_entry_t);
    if (!entry) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    entry->context = DAP_NEW_Z(python_ledger_simple_ctx_t);
    if (!entry->context) {
        DAP_DELETE(entry);
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    entry->key = key;
    entry->context->callback = a_callback;
    entry->context->user_data = a_user_data;
    Py_INCREF(a_callback);
    Py_XINCREF(a_user_data);

    HASH_ADD(hh, s_decree_callbacks, key, sizeof(entry->key), entry);
    pthread_mutex_unlock(&s_registry_mutex);
    return 1;
}

python_ledger_simple_ctx_t* cf_ledger_decree_callback_get(uint16_t a_type, uint16_t a_subtype) {
    pthread_mutex_lock(&s_registry_mutex);
    uint32_t key = cf_ledger_type_key(a_type, a_subtype);
    typed_callback_entry_t *entry = NULL;
    HASH_FIND(hh, s_decree_callbacks, &key, sizeof(key), entry);
    python_ledger_simple_ctx_t *result = entry ? entry->context : NULL;
    pthread_mutex_unlock(&s_registry_mutex);
    return result;
}

void cf_ledger_decree_callback_unregister(uint16_t a_type, uint16_t a_subtype) {
    pthread_mutex_lock(&s_registry_mutex);
    uint32_t key = cf_ledger_type_key(a_type, a_subtype);
    typed_callback_entry_t *entry = NULL;
    HASH_FIND(hh, s_decree_callbacks, &key, sizeof(key), entry);
    if (entry) {
        HASH_DEL(s_decree_callbacks, entry);
        if (entry->context) {
            Py_XDECREF(entry->context->callback);
            Py_XDECREF(entry->context->user_data);
            DAP_DELETE(entry->context);
        }
        DAP_DELETE(entry);
    }
    pthread_mutex_unlock(&s_registry_mutex);
}

int cf_ledger_anchor_unload_callback_register(uint16_t a_type, uint16_t a_subtype, PyObject *a_callback, PyObject *a_user_data) {
    if (!a_callback) {
        return -1;
    }

    pthread_mutex_lock(&s_registry_mutex);
    if (!s_initialized) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    uint32_t key = cf_ledger_type_key(a_type, a_subtype);
    typed_callback_entry_t *entry = NULL;
    HASH_FIND(hh, s_anchor_unload_callbacks, &key, sizeof(key), entry);
    if (entry) {
        Py_XDECREF(entry->context->callback);
        Py_XDECREF(entry->context->user_data);
        entry->context->callback = a_callback;
        entry->context->user_data = a_user_data;
        Py_INCREF(a_callback);
        Py_XINCREF(a_user_data);
        pthread_mutex_unlock(&s_registry_mutex);
        return 0;
    }

    entry = DAP_NEW_Z(typed_callback_entry_t);
    if (!entry) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    entry->context = DAP_NEW_Z(python_ledger_simple_ctx_t);
    if (!entry->context) {
        DAP_DELETE(entry);
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    entry->key = key;
    entry->context->callback = a_callback;
    entry->context->user_data = a_user_data;
    Py_INCREF(a_callback);
    Py_XINCREF(a_user_data);

    HASH_ADD(hh, s_anchor_unload_callbacks, key, sizeof(entry->key), entry);
    pthread_mutex_unlock(&s_registry_mutex);
    return 1;
}

python_ledger_simple_ctx_t* cf_ledger_anchor_unload_callback_get(uint16_t a_type, uint16_t a_subtype) {
    pthread_mutex_lock(&s_registry_mutex);
    uint32_t key = cf_ledger_type_key(a_type, a_subtype);
    typed_callback_entry_t *entry = NULL;
    HASH_FIND(hh, s_anchor_unload_callbacks, &key, sizeof(key), entry);
    python_ledger_simple_ctx_t *result = entry ? entry->context : NULL;
    pthread_mutex_unlock(&s_registry_mutex);
    return result;
}

void cf_ledger_anchor_unload_callback_unregister(uint16_t a_type, uint16_t a_subtype) {
    pthread_mutex_lock(&s_registry_mutex);
    uint32_t key = cf_ledger_type_key(a_type, a_subtype);
    typed_callback_entry_t *entry = NULL;
    HASH_FIND(hh, s_anchor_unload_callbacks, &key, sizeof(key), entry);
    if (entry) {
        HASH_DEL(s_anchor_unload_callbacks, entry);
        if (entry->context) {
            Py_XDECREF(entry->context->callback);
            Py_XDECREF(entry->context->user_data);
            DAP_DELETE(entry->context);
        }
        DAP_DELETE(entry);
    }
    pthread_mutex_unlock(&s_registry_mutex);
}

int cf_ledger_wallet_callbacks_register(PyObject *a_sign, PyObject *a_get_pkey_hash,
                                        PyObject *a_get_pkey, PyObject *a_get_addr,
                                        PyObject *a_check_sign, PyObject *a_user_data) {
    pthread_mutex_lock(&s_registry_mutex);
    if (!s_initialized) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    if (!s_wallet_callbacks) {
        s_wallet_callbacks = DAP_NEW_Z(python_ledger_wallet_ctx_t);
        if (!s_wallet_callbacks) {
            pthread_mutex_unlock(&s_registry_mutex);
            return -1;
        }
    } else {
        Py_XDECREF(s_wallet_callbacks->sign_callback);
        Py_XDECREF(s_wallet_callbacks->get_pkey_hash_callback);
        Py_XDECREF(s_wallet_callbacks->get_pkey_callback);
        Py_XDECREF(s_wallet_callbacks->get_addr_callback);
        Py_XDECREF(s_wallet_callbacks->check_sign_callback);
        Py_XDECREF(s_wallet_callbacks->user_data);
        DAP_DELETE(s_wallet_callbacks->last_sign_info);
        s_wallet_callbacks->last_sign_info = NULL;
        s_wallet_callbacks->has_last_pkey_hash = false;
        s_wallet_callbacks->has_last_addr = false;
    }

    s_wallet_callbacks->sign_callback = a_sign;
    s_wallet_callbacks->get_pkey_hash_callback = a_get_pkey_hash;
    s_wallet_callbacks->get_pkey_callback = a_get_pkey;
    s_wallet_callbacks->get_addr_callback = a_get_addr;
    s_wallet_callbacks->check_sign_callback = a_check_sign;
    s_wallet_callbacks->user_data = a_user_data;

    Py_XINCREF(a_sign);
    Py_XINCREF(a_get_pkey_hash);
    Py_XINCREF(a_get_pkey);
    Py_XINCREF(a_get_addr);
    Py_XINCREF(a_check_sign);
    Py_XINCREF(a_user_data);

    pthread_mutex_unlock(&s_registry_mutex);
    return 0;
}

python_ledger_wallet_ctx_t* cf_ledger_wallet_callbacks_get(void) {
    pthread_mutex_lock(&s_registry_mutex);
    python_ledger_wallet_ctx_t *result = s_wallet_callbacks;
    pthread_mutex_unlock(&s_registry_mutex);
    return result;
}

void cf_ledger_wallet_callbacks_unregister(void) {
    pthread_mutex_lock(&s_registry_mutex);
    if (s_wallet_callbacks) {
        Py_XDECREF(s_wallet_callbacks->sign_callback);
        Py_XDECREF(s_wallet_callbacks->get_pkey_hash_callback);
        Py_XDECREF(s_wallet_callbacks->get_pkey_callback);
        Py_XDECREF(s_wallet_callbacks->get_addr_callback);
        Py_XDECREF(s_wallet_callbacks->check_sign_callback);
        Py_XDECREF(s_wallet_callbacks->user_data);
        DAP_DELETE(s_wallet_callbacks->last_sign_info);
        DAP_DELETE(s_wallet_callbacks);
        s_wallet_callbacks = NULL;
    }
    pthread_mutex_unlock(&s_registry_mutex);
}

int cf_ledger_wallet_cache_callbacks_register(PyObject *a_tx_find, PyObject *a_iter_create,
                                              PyObject *a_iter_get, PyObject *a_iter_delete,
                                              PyObject *a_user_data) {
    pthread_mutex_lock(&s_registry_mutex);
    if (!s_initialized) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    if (!s_wallet_cache_callbacks) {
        s_wallet_cache_callbacks = DAP_NEW_Z(python_ledger_wallet_cache_ctx_t);
        if (!s_wallet_cache_callbacks) {
            pthread_mutex_unlock(&s_registry_mutex);
            return -1;
        }
    } else {
        Py_XDECREF(s_wallet_cache_callbacks->tx_find_callback);
        Py_XDECREF(s_wallet_cache_callbacks->iter_create_callback);
        Py_XDECREF(s_wallet_cache_callbacks->iter_get_callback);
        Py_XDECREF(s_wallet_cache_callbacks->iter_delete_callback);
        Py_XDECREF(s_wallet_cache_callbacks->user_data);
        DAP_DELETE(s_wallet_cache_callbacks->last_main_ticker);
        s_wallet_cache_callbacks->last_main_ticker = NULL;
    }

    s_wallet_cache_callbacks->tx_find_callback = a_tx_find;
    s_wallet_cache_callbacks->iter_create_callback = a_iter_create;
    s_wallet_cache_callbacks->iter_get_callback = a_iter_get;
    s_wallet_cache_callbacks->iter_delete_callback = a_iter_delete;
    s_wallet_cache_callbacks->user_data = a_user_data;

    Py_XINCREF(a_tx_find);
    Py_XINCREF(a_iter_create);
    Py_XINCREF(a_iter_get);
    Py_XINCREF(a_iter_delete);
    Py_XINCREF(a_user_data);

    pthread_mutex_unlock(&s_registry_mutex);
    return 0;
}

python_ledger_wallet_cache_ctx_t* cf_ledger_wallet_cache_callbacks_get(void) {
    pthread_mutex_lock(&s_registry_mutex);
    python_ledger_wallet_cache_ctx_t *result = s_wallet_cache_callbacks;
    pthread_mutex_unlock(&s_registry_mutex);
    return result;
}

void cf_ledger_wallet_cache_callbacks_unregister(void) {
    pthread_mutex_lock(&s_registry_mutex);
    if (s_wallet_cache_callbacks) {
        Py_XDECREF(s_wallet_cache_callbacks->tx_find_callback);
        Py_XDECREF(s_wallet_cache_callbacks->iter_create_callback);
        Py_XDECREF(s_wallet_cache_callbacks->iter_get_callback);
        Py_XDECREF(s_wallet_cache_callbacks->iter_delete_callback);
        Py_XDECREF(s_wallet_cache_callbacks->user_data);
        DAP_DELETE(s_wallet_cache_callbacks->last_main_ticker);
        DAP_DELETE(s_wallet_cache_callbacks);
        s_wallet_cache_callbacks = NULL;
    }
    pthread_mutex_unlock(&s_registry_mutex);
}

int cf_ledger_mempool_callbacks_register(PyObject *a_add_datum, PyObject *a_create_tx, PyObject *a_user_data) {
    pthread_mutex_lock(&s_registry_mutex);
    if (!s_initialized) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    if (!s_mempool_callbacks) {
        s_mempool_callbacks = DAP_NEW_Z(python_ledger_mempool_ctx_t);
        if (!s_mempool_callbacks) {
            pthread_mutex_unlock(&s_registry_mutex);
            return -1;
        }
    } else {
        Py_XDECREF(s_mempool_callbacks->add_datum_callback);
        Py_XDECREF(s_mempool_callbacks->create_tx_callback);
        Py_XDECREF(s_mempool_callbacks->user_data);
    }

    s_mempool_callbacks->add_datum_callback = a_add_datum;
    s_mempool_callbacks->create_tx_callback = a_create_tx;
    s_mempool_callbacks->user_data = a_user_data;

    Py_XINCREF(a_add_datum);
    Py_XINCREF(a_create_tx);
    Py_XINCREF(a_user_data);

    pthread_mutex_unlock(&s_registry_mutex);
    return 0;
}

python_ledger_mempool_ctx_t* cf_ledger_mempool_callbacks_get(void) {
    pthread_mutex_lock(&s_registry_mutex);
    python_ledger_mempool_ctx_t *result = s_mempool_callbacks;
    pthread_mutex_unlock(&s_registry_mutex);
    return result;
}

void cf_ledger_mempool_callbacks_unregister(void) {
    pthread_mutex_lock(&s_registry_mutex);
    if (s_mempool_callbacks) {
        Py_XDECREF(s_mempool_callbacks->add_datum_callback);
        Py_XDECREF(s_mempool_callbacks->create_tx_callback);
        Py_XDECREF(s_mempool_callbacks->user_data);
        DAP_DELETE(s_mempool_callbacks);
        s_mempool_callbacks = NULL;
    }
    pthread_mutex_unlock(&s_registry_mutex);
}

int cf_ledger_callbacks_register(PyObject *a_addr_to_wallet, PyObject *a_wallet_info_json,
                                 PyObject *a_tx_added, PyObject *a_user_data) {
    pthread_mutex_lock(&s_registry_mutex);
    if (!s_initialized) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }

    if (!s_ledger_global_callbacks) {
        s_ledger_global_callbacks = DAP_NEW_Z(python_ledger_callbacks_ctx_t);
        if (!s_ledger_global_callbacks) {
            pthread_mutex_unlock(&s_registry_mutex);
            return -1;
        }
    } else {
        Py_XDECREF(s_ledger_global_callbacks->addr_to_wallet_name_callback);
        Py_XDECREF(s_ledger_global_callbacks->wallet_info_to_json_callback);
        Py_XDECREF(s_ledger_global_callbacks->tx_added_callback);
        Py_XDECREF(s_ledger_global_callbacks->user_data);
        DAP_DELETE(s_ledger_global_callbacks->last_wallet_name);
        s_ledger_global_callbacks->last_wallet_name = NULL;
    }

    s_ledger_global_callbacks->addr_to_wallet_name_callback = a_addr_to_wallet;
    s_ledger_global_callbacks->wallet_info_to_json_callback = a_wallet_info_json;
    s_ledger_global_callbacks->tx_added_callback = a_tx_added;
    s_ledger_global_callbacks->user_data = a_user_data;

    Py_XINCREF(a_addr_to_wallet);
    Py_XINCREF(a_wallet_info_json);
    Py_XINCREF(a_tx_added);
    Py_XINCREF(a_user_data);

    pthread_mutex_unlock(&s_registry_mutex);
    return 0;
}

python_ledger_callbacks_ctx_t* cf_ledger_callbacks_get(void) {
    pthread_mutex_lock(&s_registry_mutex);
    python_ledger_callbacks_ctx_t *result = s_ledger_global_callbacks;
    pthread_mutex_unlock(&s_registry_mutex);
    return result;
}

void cf_ledger_callbacks_unregister(void) {
    pthread_mutex_lock(&s_registry_mutex);
    if (s_ledger_global_callbacks) {
        Py_XDECREF(s_ledger_global_callbacks->addr_to_wallet_name_callback);
        Py_XDECREF(s_ledger_global_callbacks->wallet_info_to_json_callback);
        Py_XDECREF(s_ledger_global_callbacks->tx_added_callback);
        Py_XDECREF(s_ledger_global_callbacks->user_data);
        DAP_DELETE(s_ledger_global_callbacks->last_wallet_name);
        DAP_DELETE(s_ledger_global_callbacks);
        s_ledger_global_callbacks = NULL;
    }
    pthread_mutex_unlock(&s_registry_mutex);
}
