/**
 * @file cf_callbacks_registry.h
 * @brief Global callback registry for Python callback lifecycle management
 * @details This module provides centralized cleanup for Python callbacks to prevent memory leaks.
 *          All Python callbacks must be registered here to ensure proper Py_DECREF on cleanup.
 * @author Cellframe Development Team
 * @date 2025-01-11
 * 
 * CRITICAL: This registry solves memory leaks where Py_INCREF'd callbacks are never freed.
 */

#pragma once

#include <Python.h>
#include <pthread.h>
#include "dap_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Callback context types for different SDK modules
 */
typedef enum {
    CF_CALLBACK_TYPE_CHAIN_MEMPOOL,       ///< Chain mempool notify
    CF_CALLBACK_TYPE_CHAIN_NODELIST,      ///< Chain net nodelist notify
    CF_CALLBACK_TYPE_CHAIN_SERVICE_ORDER, ///< Chain net service order notify
    CF_CALLBACK_TYPE_LEDGER_TX,           ///< Ledger TX notify
    CF_CALLBACK_TYPE_LEDGER_EVENT,        ///< Ledger event notify
    CF_CALLBACK_TYPE_LEDGER_BRIDGED_TX,   ///< Ledger bridged TX notify
    CF_CALLBACK_TYPE_COMPOSE_SERVICE,     ///< TX Compose service callback
    CF_CALLBACK_TYPE_CLI_COMMAND,         ///< CLI command callback
    CF_CALLBACK_TYPE_CHAIN_DATUM_INDEX,   ///< Chain datum index notify
    CF_CALLBACK_TYPE_CHAIN_DATUM_REMOVED, ///< Chain datum removed notify
    CF_CALLBACK_TYPE_DATUM_DUMP_DECREE,   ///< Datum decree JSON dump callback
    CF_CALLBACK_TYPE_DATUM_DUMP_ANCHOR,   ///< Datum anchor JSON dump callback
    CF_CALLBACK_TYPE_DECREE_REGISTRY,    ///< Decree registry handler callback
    CF_CALLBACK_TYPE_CHAIN_ATOM_NOTIFY,   ///< Chain atom notify
    CF_CALLBACK_TYPE_CHAIN_ATOM_CONFIRMED,///< Chain atom confirmed notify
    CF_CALLBACK_TYPE_CHAIN_TIMER,         ///< Chain timer callback
    CF_CALLBACK_TYPE_LEDGER_CACHE_CHECK,  ///< Ledger cache TX check
    CF_CALLBACK_TYPE_LEDGER_VERIFICATOR,  ///< Ledger verificator (6 callbacks)
    CF_CALLBACK_TYPE_LEDGER_VOTING,       ///< Ledger voting verificator (4 callbacks)
    CF_CALLBACK_TYPE_LEDGER_SERVICE,      ///< Ledger service registration
    CF_CALLBACK_TYPE_LEDGER_TAX,          ///< Ledger tax callback
    CF_CALLBACK_TYPE_BLOCK_SOVEREIGN_TAX, ///< Block sovereign tax callback
    CF_CALLBACK_TYPE_BLOCK_FORK_RESOLVED, ///< Block fork resolved notify
    CF_CALLBACK_TYPE_RPC_CONSENSUS,       ///< RPC consensus callback
    CF_CALLBACK_TYPE_RPC_STORAGE,         ///< RPC storage callback
    CF_CALLBACK_TYPE_RPC_SERVICE,         ///< RPC service callback
    CF_CALLBACK_TYPE_RPC_WALLET,          ///< RPC wallet callback
    CF_CALLBACK_TYPE_RPC_TX_NOTIFY,       ///< RPC TX notify callback
    CF_CALLBACK_TYPE_WALLET_OPENED,       ///< Wallet opened notify callback
    CF_CALLBACK_TYPE_WALLET_CREATED,      ///< Wallet created notify callback
    CF_CALLBACK_TYPE_LAST                 ///< Sentinel value for callback counts
} cf_callback_type_t;

/**
 * @brief Python callback context entry
 */
typedef struct cf_callback_entry {
    cf_callback_type_t type;              ///< Callback type
    PyObject *py_callback;                ///< Python callable object (INCREF'd)
    PyObject *py_arg;                     ///< Optional Python argument (INCREF'd if not NULL)
    void *sdk_context;                    ///< SDK callback context (for unregistration)
    char *identifier;                     ///< Unique identifier (e.g., chain ID, net name)
    struct cf_callback_entry *next;       ///< Next entry in linked list
} cf_callback_entry_t;

/**
 * @brief Global callback registry
 */
typedef struct {
    cf_callback_entry_t *head;            ///< Head of linked list
    pthread_mutex_t lock;                 ///< Thread-safe access
    size_t count;                         ///< Total registered callbacks
} cf_callbacks_registry_t;

/**
 * @brief Initialize the global callback registry
 * @return 0 on success, negative error code on failure
 */
int cf_callbacks_registry_init(void);

/**
 * @brief Register a Python callback in the global registry
 * @param type Callback type
 * @param py_callback Python callable object (will be INCREF'd)
 * @param py_arg Optional Python argument (will be INCREF'd if not NULL)
 * @param sdk_context SDK callback context pointer
 * @param identifier Optional unique identifier (e.g., chain ID, net name)
 * @return 0 on success, negative error code on failure
 */
int cf_callbacks_registry_add(cf_callback_type_t type, PyObject *py_callback, PyObject *py_arg, 
                               void *sdk_context, const char *identifier);

/**
 * @brief Unregister and cleanup a specific callback
 * @param sdk_context SDK callback context pointer to match
 * @return 0 on success, negative error code if not found
 */
int cf_callbacks_registry_remove(void *sdk_context);

/**
 * @brief Unregister and cleanup all callbacks of a specific type
 * @param type Callback type to remove
 * @return Number of callbacks removed
 */
int cf_callbacks_registry_remove_by_type(cf_callback_type_t type);

/**
 * @brief Unregister and cleanup all callbacks for a specific identifier
 * @param identifier Unique identifier (e.g., chain ID, net name)
 * @return Number of callbacks removed
 */
int cf_callbacks_registry_remove_by_id(const char *identifier);

/**
 * @brief Cleanup all registered callbacks and free memory
 * @details MUST be called on module finalization to prevent memory leaks
 */
void cf_callbacks_registry_cleanup_all(void);

/**
 * @brief Deinitialize the callback registry
 */
void cf_callbacks_registry_deinit(void);

/**
 * @brief Get registry statistics
 * @param out_count Output: total callbacks count
 * @param out_by_type Output: array of counts by type (must have CF_CALLBACK_TYPE_LAST slots)
 */
void cf_callbacks_registry_stats(size_t *out_count, size_t *out_by_type);

#ifdef __cplusplus
}
#endif
