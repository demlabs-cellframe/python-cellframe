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
 * - dap_ledger_decree_set_callback(type, subtype, cb) - no arg
 * - dap_ledger_anchor_unload_set_callback(type, subtype, cb) - no arg
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
 * @brief Python callback context for balance change notifications
 */
typedef struct python_ledger_balance_ctx {
    PyObject *callback;                    // Python callback function
    PyObject *user_data;                   // Optional user data
    struct python_ledger_balance_ctx *next;
} python_ledger_balance_ctx_t;

/**
 * @brief Python callback context for per-ledger callbacks with user_data
 */
typedef struct {
    PyObject *callback;
    PyObject *user_data;
} python_ledger_simple_ctx_t;

/**
 * @brief Python callback context for wallet callbacks (global)
 */
typedef struct {
    PyObject *sign_callback;
    PyObject *get_pkey_hash_callback;
    PyObject *get_pkey_callback;
    PyObject *get_addr_callback;
    PyObject *check_sign_callback;
    PyObject *user_data;
    dap_hash_fast_t last_pkey_hash;
    bool has_last_pkey_hash;
    dap_chain_addr_t last_addr;
    bool has_last_addr;
    char *last_sign_info;
} python_ledger_wallet_ctx_t;

/**
 * @brief Python callback context for wallet cache callbacks (global)
 */
typedef struct {
    PyObject *tx_find_callback;
    PyObject *iter_create_callback;
    PyObject *iter_get_callback;
    PyObject *iter_delete_callback;
    PyObject *user_data;
    char *last_main_ticker;
} python_ledger_wallet_cache_ctx_t;

/**
 * @brief Python callback context for mempool callbacks (global)
 */
typedef struct {
    PyObject *add_datum_callback;
    PyObject *create_tx_callback;
    PyObject *user_data;
} python_ledger_mempool_ctx_t;

/**
 * @brief Python callback context for ledger callbacks (global)
 */
typedef struct {
    PyObject *addr_to_wallet_name_callback;
    PyObject *wallet_info_to_json_callback;
    PyObject *tx_added_callback;
    PyObject *user_data;
    char *last_wallet_name;
} python_ledger_callbacks_ctx_t;

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
 * @brief Register balance change callback for ledger
 * @return 1 if first callback for ledger, 0 if appended, -1 on error
 */
int cf_ledger_balance_callback_add(dap_ledger_t *a_ledger, PyObject *a_callback, PyObject *a_user_data);

/**
 * @brief Remove balance change callback for ledger
 * @return 1 if removed last callback, 0 if others remain, -1 if not found
 */
int cf_ledger_balance_callback_remove(dap_ledger_t *a_ledger, PyObject *a_callback);

/**
 * @brief Get balance change callbacks list for ledger
 */
python_ledger_balance_ctx_t* cf_ledger_balance_callback_get(dap_ledger_t *a_ledger);

/**
 * @brief Register per-ledger fee callback
 */
int cf_ledger_fee_callback_register(dap_ledger_t *a_ledger, PyObject *a_callback, PyObject *a_user_data);
python_ledger_simple_ctx_t* cf_ledger_fee_callback_get(dap_ledger_t *a_ledger);
void cf_ledger_fee_callback_unregister(dap_ledger_t *a_ledger);

/**
 * @brief Register per-ledger reward removed callback
 */
int cf_ledger_reward_removed_callback_register(dap_ledger_t *a_ledger, PyObject *a_callback, PyObject *a_user_data);
python_ledger_simple_ctx_t* cf_ledger_reward_removed_callback_get(dap_ledger_t *a_ledger);
void cf_ledger_reward_removed_callback_unregister(dap_ledger_t *a_ledger);

/**
 * @brief Register per-ledger decree set fee callback
 */
int cf_ledger_decree_set_fee_callback_register(dap_ledger_t *a_ledger, PyObject *a_callback, PyObject *a_user_data);
python_ledger_simple_ctx_t* cf_ledger_decree_set_fee_callback_get(dap_ledger_t *a_ledger);
void cf_ledger_decree_set_fee_callback_unregister(dap_ledger_t *a_ledger);

/**
 * @brief Register per-ledger decree generic callback
 */
int cf_ledger_decree_generic_callback_register(dap_ledger_t *a_ledger, PyObject *a_callback, PyObject *a_user_data);
python_ledger_simple_ctx_t* cf_ledger_decree_generic_callback_get(dap_ledger_t *a_ledger);
void cf_ledger_decree_generic_callback_unregister(dap_ledger_t *a_ledger);

/**
 * @brief Register per-ledger get current cell callback
 */
int cf_ledger_get_cur_cell_callback_register(dap_ledger_t *a_ledger, PyObject *a_callback, PyObject *a_user_data);
python_ledger_simple_ctx_t* cf_ledger_get_cur_cell_callback_get(dap_ledger_t *a_ledger);
void cf_ledger_get_cur_cell_callback_unregister(dap_ledger_t *a_ledger);

/**
 * @brief Register decree callback by type/subtype
 * @return 1 if new entry, 0 if updated, -1 on error
 */
int cf_ledger_decree_callback_register(uint16_t a_type, uint16_t a_subtype, PyObject *a_callback, PyObject *a_user_data);
python_ledger_simple_ctx_t* cf_ledger_decree_callback_get(uint16_t a_type, uint16_t a_subtype);
void cf_ledger_decree_callback_unregister(uint16_t a_type, uint16_t a_subtype);

/**
 * @brief Register anchor unload callback by decree type/subtype
 * @return 1 if new entry, 0 if updated, -1 on error
 */
int cf_ledger_anchor_unload_callback_register(uint16_t a_type, uint16_t a_subtype, PyObject *a_callback, PyObject *a_user_data);
python_ledger_simple_ctx_t* cf_ledger_anchor_unload_callback_get(uint16_t a_type, uint16_t a_subtype);
void cf_ledger_anchor_unload_callback_unregister(uint16_t a_type, uint16_t a_subtype);

/**
 * @brief Register wallet callbacks (global)
 */
int cf_ledger_wallet_callbacks_register(PyObject *a_sign, PyObject *a_get_pkey_hash,
                                        PyObject *a_get_pkey, PyObject *a_get_addr,
                                        PyObject *a_check_sign, PyObject *a_user_data);
python_ledger_wallet_ctx_t* cf_ledger_wallet_callbacks_get(void);
void cf_ledger_wallet_callbacks_unregister(void);

/**
 * @brief Register wallet cache callbacks (global)
 */
int cf_ledger_wallet_cache_callbacks_register(PyObject *a_tx_find, PyObject *a_iter_create,
                                              PyObject *a_iter_get, PyObject *a_iter_delete,
                                              PyObject *a_user_data);
python_ledger_wallet_cache_ctx_t* cf_ledger_wallet_cache_callbacks_get(void);
void cf_ledger_wallet_cache_callbacks_unregister(void);

/**
 * @brief Register mempool callbacks (global)
 */
int cf_ledger_mempool_callbacks_register(PyObject *a_add_datum, PyObject *a_create_tx, PyObject *a_user_data);
python_ledger_mempool_ctx_t* cf_ledger_mempool_callbacks_get(void);
void cf_ledger_mempool_callbacks_unregister(void);

/**
 * @brief Register ledger callbacks (global)
 */
int cf_ledger_callbacks_register(PyObject *a_addr_to_wallet, PyObject *a_wallet_info_json,
                                 PyObject *a_tx_added, PyObject *a_user_data);
python_ledger_callbacks_ctx_t* cf_ledger_callbacks_get(void);
void cf_ledger_callbacks_unregister(void);

/**
 * @brief Cleanup all ledger callbacks (called on module unload)
 */
void cf_ledger_callback_registry_cleanup_all(void);

#ifdef __cplusplus
}
#endif
