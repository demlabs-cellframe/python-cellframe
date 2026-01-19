/**
 * @file cf_verificator_registry.c
 * @brief Implementation of global verificator callback registry
 */

#include "cf_verificator_registry.h"
#include "dap_common.h"
#include "uthash.h"

#define LOG_TAG "cf_verificator_registry"

// Registry entry for verificator callbacks (keyed by subtype)
typedef struct {
    dap_chain_tx_out_cond_subtype_t subtype;  // Key
    python_verificator_ctx_t *context;        // Value
    UT_hash_handle hh;
} verificator_entry_t;

// Global state
static verificator_entry_t *s_verificators = NULL;
static python_voting_ctx_t *s_voting_verificator = NULL;
static pthread_mutex_t s_registry_mutex = PTHREAD_MUTEX_INITIALIZER;
static bool s_initialized = false;

/**
 * @brief Initialize verificator callback registry
 */
int cf_verificator_registry_init(void) {
    pthread_mutex_lock(&s_registry_mutex);
    
    if (s_initialized) {
        pthread_mutex_unlock(&s_registry_mutex);
        return 0;
    }
    
    s_verificators = NULL;
    s_voting_verificator = NULL;
    s_initialized = true;
    
    pthread_mutex_unlock(&s_registry_mutex);
    log_it(L_INFO, "Verificator callback registry initialized");
    return 0;
}

/**
 * @brief Cleanup all verificator callbacks
 */
void cf_verificator_registry_cleanup_all(void) {
    pthread_mutex_lock(&s_registry_mutex);
    
    if (!s_initialized) {
        pthread_mutex_unlock(&s_registry_mutex);
        return;
    }
    
    // Cleanup verificators
    verificator_entry_t *entry, *tmp;
    HASH_ITER(hh, s_verificators, entry, tmp) {
        HASH_DEL(s_verificators, entry);
        if (entry->context) {
            Py_XDECREF(entry->context->in_verify_callback);
            Py_XDECREF(entry->context->out_verify_callback);
            Py_XDECREF(entry->context->in_add_callback);
            Py_XDECREF(entry->context->out_add_callback);
            Py_XDECREF(entry->context->in_delete_callback);
            Py_XDECREF(entry->context->out_delete_callback);
            Py_XDECREF(entry->context->user_data);
            DAP_DELETE(entry->context);
        }
        DAP_DELETE(entry);
    }
    
    // Cleanup voting verificator
    if (s_voting_verificator) {
        Py_XDECREF(s_voting_verificator->voting_callback);
        Py_XDECREF(s_voting_verificator->vote_callback);
        Py_XDECREF(s_voting_verificator->delete_callback);
        Py_XDECREF(s_voting_verificator->expire_callback);
        Py_XDECREF(s_voting_verificator->user_data);
        DAP_DELETE(s_voting_verificator);
        s_voting_verificator = NULL;
    }
    
    s_verificators = NULL;
    
    pthread_mutex_unlock(&s_registry_mutex);
    log_it(L_INFO, "Verificator callback registry cleaned up");
}

/**
 * @brief Deinitialize verificator callback registry
 */
void cf_verificator_registry_deinit(void) {
    cf_verificator_registry_cleanup_all();
    
    pthread_mutex_lock(&s_registry_mutex);
    s_initialized = false;
    pthread_mutex_unlock(&s_registry_mutex);
    
    log_it(L_INFO, "Verificator callback registry deinitialized");
}

/**
 * @brief Register verificator callbacks for subtype
 */
int cf_verificator_register(dap_chain_tx_out_cond_subtype_t a_subtype,
                            PyObject *a_in_verify, PyObject *a_out_verify,
                            PyObject *a_in_add, PyObject *a_out_add,
                            PyObject *a_in_delete, PyObject *a_out_delete,
                            PyObject *a_user_data) {
    pthread_mutex_lock(&s_registry_mutex);
    
    if (!s_initialized) {
        pthread_mutex_unlock(&s_registry_mutex);
        log_it(L_ERROR, "Registry not initialized");
        return -1;
    }
    
    // Check if already exists
    verificator_entry_t *existing = NULL;
    HASH_FIND(hh, s_verificators, &a_subtype, sizeof(dap_chain_tx_out_cond_subtype_t), existing);
    if (existing) {
        // Replace existing
        Py_XDECREF(existing->context->in_verify_callback);
        Py_XDECREF(existing->context->out_verify_callback);
        Py_XDECREF(existing->context->in_add_callback);
        Py_XDECREF(existing->context->out_add_callback);
        Py_XDECREF(existing->context->in_delete_callback);
        Py_XDECREF(existing->context->out_delete_callback);
        Py_XDECREF(existing->context->user_data);
        
        existing->context->in_verify_callback = a_in_verify;
        existing->context->out_verify_callback = a_out_verify;
        existing->context->in_add_callback = a_in_add;
        existing->context->out_add_callback = a_out_add;
        existing->context->in_delete_callback = a_in_delete;
        existing->context->out_delete_callback = a_out_delete;
        existing->context->user_data = a_user_data;
        
        Py_XINCREF(a_in_verify);
        Py_XINCREF(a_out_verify);
        Py_XINCREF(a_in_add);
        Py_XINCREF(a_out_add);
        Py_XINCREF(a_in_delete);
        Py_XINCREF(a_out_delete);
        Py_XINCREF(a_user_data);
        
        pthread_mutex_unlock(&s_registry_mutex);
        log_it(L_DEBUG, "Replaced verificator for subtype %d", a_subtype);
        return 0;
    }
    
    // Create new entry
    verificator_entry_t *entry = DAP_NEW_Z(verificator_entry_t);
    if (!entry) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }
    
    entry->context = DAP_NEW_Z(python_verificator_ctx_t);
    if (!entry->context) {
        DAP_DELETE(entry);
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }
    
    entry->subtype = a_subtype;
    entry->context->subtype = a_subtype;
    entry->context->in_verify_callback = a_in_verify;
    entry->context->out_verify_callback = a_out_verify;
    entry->context->in_add_callback = a_in_add;
    entry->context->out_add_callback = a_out_add;
    entry->context->in_delete_callback = a_in_delete;
    entry->context->out_delete_callback = a_out_delete;
    entry->context->user_data = a_user_data;
    
    Py_XINCREF(a_in_verify);
    Py_XINCREF(a_out_verify);
    Py_XINCREF(a_in_add);
    Py_XINCREF(a_out_add);
    Py_XINCREF(a_in_delete);
    Py_XINCREF(a_out_delete);
    Py_XINCREF(a_user_data);
    
    HASH_ADD(hh, s_verificators, subtype, sizeof(dap_chain_tx_out_cond_subtype_t), entry);
    
    pthread_mutex_unlock(&s_registry_mutex);
    log_it(L_DEBUG, "Registered verificator for subtype %d", a_subtype);
    return 0;
}

/**
 * @brief Get verificator callback context for subtype
 */
python_verificator_ctx_t* cf_verificator_get(dap_chain_tx_out_cond_subtype_t a_subtype) {
    pthread_mutex_lock(&s_registry_mutex);
    
    verificator_entry_t *entry = NULL;
    HASH_FIND(hh, s_verificators, &a_subtype, sizeof(dap_chain_tx_out_cond_subtype_t), entry);
    
    python_verificator_ctx_t *result = entry ? entry->context : NULL;
    
    pthread_mutex_unlock(&s_registry_mutex);
    return result;
}

/**
 * @brief Unregister verificator callbacks for subtype
 */
void cf_verificator_unregister(dap_chain_tx_out_cond_subtype_t a_subtype) {
    pthread_mutex_lock(&s_registry_mutex);
    
    verificator_entry_t *entry = NULL;
    HASH_FIND(hh, s_verificators, &a_subtype, sizeof(dap_chain_tx_out_cond_subtype_t), entry);
    
    if (entry) {
        HASH_DEL(s_verificators, entry);
        if (entry->context) {
            Py_XDECREF(entry->context->in_verify_callback);
            Py_XDECREF(entry->context->out_verify_callback);
            Py_XDECREF(entry->context->in_add_callback);
            Py_XDECREF(entry->context->out_add_callback);
            Py_XDECREF(entry->context->in_delete_callback);
            Py_XDECREF(entry->context->out_delete_callback);
            Py_XDECREF(entry->context->user_data);
            DAP_DELETE(entry->context);
        }
        DAP_DELETE(entry);
        log_it(L_DEBUG, "Unregistered verificator for subtype %d", a_subtype);
    }
    
    pthread_mutex_unlock(&s_registry_mutex);
}

/**
 * @brief Register voting verificator callbacks (singleton)
 */
int cf_voting_verificator_register(PyObject *a_voting, PyObject *a_vote,
                                   PyObject *a_delete, PyObject *a_expire,
                                   PyObject *a_user_data) {
    pthread_mutex_lock(&s_registry_mutex);
    
    if (!s_initialized) {
        pthread_mutex_unlock(&s_registry_mutex);
        log_it(L_ERROR, "Registry not initialized");
        return -1;
    }
    
    // Replace existing if any
    if (s_voting_verificator) {
        Py_XDECREF(s_voting_verificator->voting_callback);
        Py_XDECREF(s_voting_verificator->vote_callback);
        Py_XDECREF(s_voting_verificator->delete_callback);
        Py_XDECREF(s_voting_verificator->expire_callback);
        Py_XDECREF(s_voting_verificator->user_data);
        
        s_voting_verificator->voting_callback = a_voting;
        s_voting_verificator->vote_callback = a_vote;
        s_voting_verificator->delete_callback = a_delete;
        s_voting_verificator->expire_callback = a_expire;
        s_voting_verificator->user_data = a_user_data;
        
        Py_XINCREF(a_voting);
        Py_XINCREF(a_vote);
        Py_XINCREF(a_delete);
        Py_XINCREF(a_expire);
        Py_XINCREF(a_user_data);
        
        pthread_mutex_unlock(&s_registry_mutex);
        log_it(L_DEBUG, "Replaced voting verificator");
        return 0;
    }
    
    // Create new
    s_voting_verificator = DAP_NEW_Z(python_voting_ctx_t);
    if (!s_voting_verificator) {
        pthread_mutex_unlock(&s_registry_mutex);
        return -1;
    }
    
    s_voting_verificator->voting_callback = a_voting;
    s_voting_verificator->vote_callback = a_vote;
    s_voting_verificator->delete_callback = a_delete;
    s_voting_verificator->expire_callback = a_expire;
    s_voting_verificator->user_data = a_user_data;
    
    Py_XINCREF(a_voting);
    Py_XINCREF(a_vote);
    Py_XINCREF(a_delete);
    Py_XINCREF(a_expire);
    Py_XINCREF(a_user_data);
    
    pthread_mutex_unlock(&s_registry_mutex);
    log_it(L_DEBUG, "Registered voting verificator");
    return 0;
}

/**
 * @brief Get voting verificator callback context
 */
python_voting_ctx_t* cf_voting_verificator_get(void) {
    pthread_mutex_lock(&s_registry_mutex);
    python_voting_ctx_t *result = s_voting_verificator;
    pthread_mutex_unlock(&s_registry_mutex);
    return result;
}

/**
 * @brief Unregister voting verificator callbacks
 */
void cf_voting_verificator_unregister(void) {
    pthread_mutex_lock(&s_registry_mutex);
    
    if (s_voting_verificator) {
        Py_XDECREF(s_voting_verificator->voting_callback);
        Py_XDECREF(s_voting_verificator->vote_callback);
        Py_XDECREF(s_voting_verificator->delete_callback);
        Py_XDECREF(s_voting_verificator->expire_callback);
        Py_XDECREF(s_voting_verificator->user_data);
        DAP_DELETE(s_voting_verificator);
        s_voting_verificator = NULL;
        log_it(L_DEBUG, "Unregistered voting verificator");
    }
    
    pthread_mutex_unlock(&s_registry_mutex);
}
