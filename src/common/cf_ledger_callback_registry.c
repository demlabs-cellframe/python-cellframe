/**
 * @file cf_ledger_callback_registry.c
 * @brief Implementation of global ledger callback registry
 */

#include "cf_ledger_callback_registry.h"
#include "dap_common.h"
#include "dap_ht.h"

#define LOG_TAG "cf_ledger_callback_registry"

// Registry entry for cache callbacks (keyed by ledger pointer)
typedef struct {
    dap_ledger_t *ledger;               // Key
    python_ledger_cache_ctx_t *context; // Value
    dap_ht_handle_t hh;
} cache_callback_entry_t;

// Registry entry for service callbacks (keyed by service UID)
typedef struct {
    dap_chain_srv_uid_t uid;            // Key
    python_service_tag_ctx_t *context;  // Value
    dap_ht_handle_t hh;
} service_callback_entry_t;

// Global state
static cache_callback_entry_t *s_cache_callbacks = NULL;
static service_callback_entry_t *s_service_callbacks = NULL;
static python_tax_ctx_t *s_tax_callback = NULL;
static pthread_mutex_t s_registry_mutex = PTHREAD_MUTEX_INITIALIZER;
static bool s_initialized = false;

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
    dap_ht_foreach(s_cache_callbacks, cache_entry, cache_tmp) {
        dap_ht_del(s_cache_callbacks, cache_entry);
        if (cache_entry->context) {
            Py_XDECREF(cache_entry->context->callback);
            Py_XDECREF(cache_entry->context->user_data);
            DAP_DELETE(cache_entry->context);
        }
        DAP_DELETE(cache_entry);
    }
    
    // Cleanup service callbacks
    service_callback_entry_t *service_entry, *service_tmp;
    dap_ht_foreach(s_service_callbacks, service_entry, service_tmp) {
        dap_ht_del(s_service_callbacks, service_entry);
        if (service_entry->context) {
            Py_XDECREF(service_entry->context->callback);
            Py_XDECREF(service_entry->context->user_data);
            DAP_DELETE(service_entry->context->tag_str);
            DAP_DELETE(service_entry->context);
        }
        DAP_DELETE(service_entry);
    }
    
    // Cleanup tax callback
    if (s_tax_callback) {
        Py_XDECREF(s_tax_callback->callback);
        Py_XDECREF(s_tax_callback->user_data);
        DAP_DELETE(s_tax_callback);
        s_tax_callback = NULL;
    }
    
    s_cache_callbacks = NULL;
    s_service_callbacks = NULL;
    
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
    dap_ht_find_ptr(s_cache_callbacks, &a_ledger, existing);
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
    
    dap_ht_add_ptr(s_cache_callbacks, ledger, entry);
    
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
    dap_ht_find_ptr(s_cache_callbacks, &a_ledger, entry);
    
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
    dap_ht_find_ptr(s_cache_callbacks, &a_ledger, entry);
    
    if (entry) {
        dap_ht_del(s_cache_callbacks, entry);
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
    dap_ht_find(s_service_callbacks, &a_uid, sizeof(dap_chain_srv_uid_t), existing);
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
        log_it(L_DEBUG, "Replaced service callback for UID 0x%llx", (unsigned long long)a_uid.uint64);
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
    
    dap_ht_add(s_service_callbacks, uid, entry);
    
    pthread_mutex_unlock(&s_registry_mutex);
    log_it(L_DEBUG, "Registered service callback for UID 0x%llx, tag '%s'", (unsigned long long)a_uid.uint64, a_tag_str);
    return 0;
}

/**
 * @brief Get service tag check callback context
 */
python_service_tag_ctx_t* cf_ledger_service_callback_get(dap_chain_srv_uid_t a_uid) {
    pthread_mutex_lock(&s_registry_mutex);
    
    service_callback_entry_t *entry = NULL;
    dap_ht_find(s_service_callbacks, &a_uid, sizeof(dap_chain_srv_uid_t), entry);
    
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
    dap_ht_find(s_service_callbacks, &a_uid, sizeof(dap_chain_srv_uid_t), entry);
    
    if (entry) {
        dap_ht_del(s_service_callbacks, entry);
        if (entry->context) {
            Py_XDECREF(entry->context->callback);
            Py_XDECREF(entry->context->user_data);
            DAP_DELETE(entry->context->tag_str);
            DAP_DELETE(entry->context);
        }
        DAP_DELETE(entry);
        log_it(L_DEBUG, "Unregistered service callback for UID 0x%llx", (unsigned long long)a_uid.uint64);
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
