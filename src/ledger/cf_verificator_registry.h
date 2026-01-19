/**
 * @file cf_verificator_registry.h
 * @brief Global registry for verificator callbacks by subtype
 * @details This module provides global state for verificator callbacks that cannot pass context
 *          through the SDK API (no void *arg parameter).
 * @author Cellframe Development Team
 * @date 2025-01-11
 * 
 * PROBLEM: Verificator callbacks don't provide void *arg:
 * - dap_ledger_cond_in_verify_callback_t(...) - no arg
 * - dap_ledger_cond_out_verify_callback_t(...) - no arg
 * - etc. (6 callbacks total)
 * 
 * SOLUTION: Global thread-safe registry with lookup by subtype
 */

#pragma once

#include <Python.h>
#include <pthread.h>
#include "dap_common.h"
#include "dap_chain_ledger.h"
#include "dap_chain_datum_tx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Python callback context for verificator (6 callbacks per subtype)
 */
typedef struct {
    PyObject *in_verify_callback;      // cond_in_verify
    PyObject *out_verify_callback;     // cond_out_verify
    PyObject *in_add_callback;         // cond_in_add
    PyObject *out_add_callback;        // cond_out_add
    PyObject *in_delete_callback;      // cond_in_delete
    PyObject *out_delete_callback;     // cond_out_delete
    PyObject *user_data;               // Optional user data
    dap_chain_tx_out_cond_subtype_t subtype; // Associated subtype
} python_verificator_ctx_t;

/**
 * @brief Python callback context for voting verificator (4 callbacks)
 */
typedef struct {
    PyObject *voting_callback;         // voting callback
    PyObject *vote_callback;           // vote callback
    PyObject *delete_callback;         // delete callback
    PyObject *expire_callback;         // expire callback
    PyObject *user_data;               // Optional user data
} python_voting_ctx_t;

/**
 * @brief Initialize verificator callback registry
 * @return 0 on success, -1 on error
 */
int cf_verificator_registry_init(void);

/**
 * @brief Deinitialize verificator callback registry (cleanup all)
 */
void cf_verificator_registry_deinit(void);

/**
 * @brief Register verificator callbacks for subtype
 * @param a_subtype Conditional output subtype
 * @param a_in_verify Python callback for in_verify (can be NULL)
 * @param a_out_verify Python callback for out_verify (can be NULL)
 * @param a_in_add Python callback for in_add (can be NULL)
 * @param a_out_add Python callback for out_add (can be NULL)
 * @param a_in_delete Python callback for in_delete (can be NULL)
 * @param a_out_delete Python callback for out_delete (can be NULL)
 * @param a_user_data Optional user data
 * @return 0 on success, -1 on error
 */
int cf_verificator_register(dap_chain_tx_out_cond_subtype_t a_subtype,
                            PyObject *a_in_verify, PyObject *a_out_verify,
                            PyObject *a_in_add, PyObject *a_out_add,
                            PyObject *a_in_delete, PyObject *a_out_delete,
                            PyObject *a_user_data);

/**
 * @brief Get verificator callback context for subtype
 * @param a_subtype Conditional output subtype
 * @return Context or NULL if not found
 */
python_verificator_ctx_t* cf_verificator_get(dap_chain_tx_out_cond_subtype_t a_subtype);

/**
 * @brief Unregister verificator callbacks for subtype
 * @param a_subtype Conditional output subtype
 */
void cf_verificator_unregister(dap_chain_tx_out_cond_subtype_t a_subtype);

/**
 * @brief Register voting verificator callbacks (singleton)
 * @param a_voting Python callback for voting
 * @param a_vote Python callback for vote
 * @param a_delete Python callback for delete
 * @param a_expire Python callback for expire
 * @param a_user_data Optional user data
 * @return 0 on success, -1 on error
 */
int cf_voting_verificator_register(PyObject *a_voting, PyObject *a_vote,
                                   PyObject *a_delete, PyObject *a_expire,
                                   PyObject *a_user_data);

/**
 * @brief Get voting verificator callback context
 * @return Context or NULL if not registered
 */
python_voting_ctx_t* cf_voting_verificator_get(void);

/**
 * @brief Unregister voting verificator callbacks
 */
void cf_voting_verificator_unregister(void);

/**
 * @brief Cleanup all verificator callbacks (called on module unload)
 */
void cf_verificator_registry_cleanup_all(void);

#ifdef __cplusplus
}
#endif
