/**
 * @file cf_ledger_callback_registry.h
 * @brief Global registry for ledger callbacks without void *arg support
 * @details This module provides global state for ledger callbacks that cannot pass context
 *          through the SDK API (no void *arg parameter).
 * @author Cellframe Development Team
 * @date 2025-01-11
 * 
 * PROBLEM: Some SDK callbacks don't provide void *arg:
 * - dap_ledger_cache_tx_check_callback_t(ledger, tx_hash) - no arg
 * - dap_ledger_tag_check_callback_t(...) - no arg
 * - dap_ledger_tax_callback_t(...) - no arg
 * 
 * SOLUTION: Global thread-safe registry with lookup by ledger/uid
 */

#pragma once

#include <Python.h>
#include <pthread.h>
#include "dap_common.h"
#include "dap_chain_ledger.h"
#include "dap_chain_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Python callback context for ledger cache check
 */
typedef struct {
    PyObject *callback;      // Python callback function
    PyObject *user_data;     // Optional user data
    dap_ledger_t *ledger;    // Associated ledger
} python_ledger_cache_ctx_t;

/**
 * @brief Python callback context for service tag check
 */
typedef struct {
    PyObject *callback;      // Python callback function
    PyObject *user_data;     // Optional user data
    dap_chain_srv_uid_t uid; // Service UID
    char *tag_str;           // Tag string (owned)
} python_service_tag_ctx_t;

/**
 * @brief Python callback context for tax calculation
 */
typedef struct {
    PyObject *callback;      // Python callback function
    PyObject *user_data;     // Optional user data
} python_tax_ctx_t;

/**
 * @brief Initialize ledger callback registry
 * @return 0 on success, -1 on error
 */
int cf_ledger_callback_registry_init(void);

/**
 * @brief Deinitialize ledger callback registry (cleanup all)
 */
void cf_ledger_callback_registry_deinit(void);

/**
 * @brief Register cache check callback for ledger
 * @param a_ledger Target ledger
 * @param a_callback Python callback
 * @param a_user_data Optional user data
 * @return 0 on success, -1 on error
 */
int cf_ledger_cache_callback_register(dap_ledger_t *a_ledger, PyObject *a_callback, PyObject *a_user_data);

/**
 * @brief Get cache check callback context for ledger
 * @param a_ledger Target ledger
 * @return Context or NULL if not found
 */
python_ledger_cache_ctx_t* cf_ledger_cache_callback_get(dap_ledger_t *a_ledger);

/**
 * @brief Unregister cache check callback for ledger
 * @param a_ledger Target ledger
 */
void cf_ledger_cache_callback_unregister(dap_ledger_t *a_ledger);

/**
 * @brief Register service tag check callback
 * @param a_uid Service UID
 * @param a_tag_str Tag string (will be copied)
 * @param a_callback Python callback
 * @param a_user_data Optional user data
 * @return 0 on success, -1 on error
 */
int cf_ledger_service_callback_register(dap_chain_srv_uid_t a_uid, const char *a_tag_str, 
                                         PyObject *a_callback, PyObject *a_user_data);

/**
 * @brief Get service tag check callback context
 * @param a_uid Service UID
 * @return Context or NULL if not found
 */
python_service_tag_ctx_t* cf_ledger_service_callback_get(dap_chain_srv_uid_t a_uid);

/**
 * @brief Unregister service tag check callback
 * @param a_uid Service UID
 */
void cf_ledger_service_callback_unregister(dap_chain_srv_uid_t a_uid);

/**
 * @brief Register global tax callback (singleton)
 * @param a_callback Python callback
 * @param a_user_data Optional user data
 * @return 0 on success, -1 on error
 */
int cf_ledger_tax_callback_register(PyObject *a_callback, PyObject *a_user_data);

/**
 * @brief Get global tax callback context
 * @return Context or NULL if not registered
 */
python_tax_ctx_t* cf_ledger_tax_callback_get(void);

/**
 * @brief Unregister global tax callback
 */
void cf_ledger_tax_callback_unregister(void);

/**
 * @brief Cleanup all ledger callbacks (called on module unload)
 */
void cf_ledger_callback_registry_cleanup_all(void);

#ifdef __cplusplus
}
#endif
