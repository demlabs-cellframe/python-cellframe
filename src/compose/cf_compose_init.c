/**
 * @file cf_compose_init.c
 * @brief Python TX Compose module initialization
 * @details Initializes Python TX Compose API and exports functions
 * @author Cellframe Development Team
 * @date 2025-01-11 (Phase 3.5 Refactoring)
 */

#include "cellframe.h"
#include "dap_chain_tx_compose_api.h"

#define LOG_TAG "python_cellframe_compose_init"

// External declarations from cf_compose.c
extern PyObject* dap_chain_tx_compose_register_py(PyObject *a_self, PyObject *a_args);
extern PyObject* dap_chain_tx_compose_init_py(PyObject *a_self, PyObject *a_args);
extern PyObject* dap_chain_tx_compose_deinit_py(PyObject *a_self, PyObject *a_args);
extern PyObject* dap_chain_tx_compose_unregister_py(PyObject *a_self, PyObject *a_args);
extern PyObject* dap_chain_tx_compose_is_registered_py(PyObject *a_self, PyObject *a_args);
extern PyObject* dap_chain_tx_compose_create_py(PyObject *a_self, PyObject *a_args);
extern PyObject* dap_chain_tx_compose_registry_init_py(PyObject *a_self, PyObject *a_args);
extern PyObject* dap_chain_tx_compose_registry_deinit_py(PyObject *a_self, PyObject *a_args);
extern PyObject* dap_chain_tx_compose_registry_add_py(PyObject *a_self, PyObject *a_args);
extern PyObject* dap_chain_tx_compose_registry_remove_py(PyObject *a_self, PyObject *a_args);
extern PyObject* dap_chain_tx_compose_registry_find_py(PyObject *a_self, PyObject *a_args);

/**
 * @brief Python TX Compose module methods
 */
static PyMethodDef s_compose_methods[] = {
    {
        "dap_chain_tx_compose_init",
        dap_chain_tx_compose_init_py,
        METH_VARARGS,
        "Initialize TX compose API.\n\n"
        "Returns:\n"
        "    int: 0 on success, negative on error"
    },
    {
        "dap_chain_tx_compose_deinit",
        dap_chain_tx_compose_deinit_py,
        METH_VARARGS,
        "Deinitialize TX compose API."
    },
    // TX Builder Registration
    {
        "dap_chain_tx_compose_register",
        dap_chain_tx_compose_register_py,
        METH_VARARGS,
        "Register Python TX builder for specific type.\n\n"
        "Args:\n"
        "    tx_type (str): TX type identifier (e.g. 'transfer', 'stake')\n"
        "    callback (callable): Builder function(ledger, utxo_list, user_data) -> datum\n"
        "    user_data (optional): Optional user data\n\n"
        "Returns:\n"
        "    int: 0 on success, negative on error\n\n"
        "Example:\n"
        "    def my_builder(ledger, utxo_list, user_data):\n"
        "        # Build TX datum\n"
        "        return datum_capsule\n"
        "    dap_chain_tx_compose_register('my_tx', my_builder, None)"
    },
    
    {
        "dap_chain_tx_compose_unregister",
        dap_chain_tx_compose_unregister_py,
        METH_VARARGS,
        "Unregister TX builder.\n\n"
        "Args:\n"
        "    tx_type (str): TX type identifier"
    },
    
    {
        "dap_chain_tx_compose_is_registered",
        dap_chain_tx_compose_is_registered_py,
        METH_VARARGS,
        "Check if TX builder is registered.\n\n"
        "Args:\n"
        "    tx_type (str): TX type identifier\n\n"
        "Returns:\n"
        "    bool: True if registered"
    },
    
    // TX Creation (Dispatcher)
    {
        "dap_chain_tx_compose_create",
        dap_chain_tx_compose_create_py,
        METH_VARARGS,
        "Create TX via registered builder (dispatcher).\n\n"
        "Args:\n"
        "    tx_type (str): TX type identifier\n"
        "    ledger (capsule): dap_ledger_t capsule\n"
        "    utxo_list (capsule): dap_list_t capsule with UTXO\n"
        "    params (any): Parameters for TX builder\n\n"
        "Returns:\n"
        "    capsule: dap_chain_datum_t capsule or None on error\n\n"
        "Example:\n"
        "    datum = dap_chain_tx_compose_create(\n"
        "        'transfer',\n"
        "        ledger,\n"
        "        utxo_list,\n"
        "        {'addr_to': addr, 'value': value, 'fee': fee}\n"
        "    )"
    },

    {
        "dap_chain_tx_compose_registry_init",
        dap_chain_tx_compose_registry_init_py,
        METH_VARARGS,
        "Initialize compose registry.\n\n"
        "Returns:\n"
        "    int: 0 on success, negative on error"
    },
    {
        "dap_chain_tx_compose_registry_deinit",
        dap_chain_tx_compose_registry_deinit_py,
        METH_VARARGS,
        "Deinitialize compose registry."
    },
    {
        "dap_chain_tx_compose_registry_add",
        dap_chain_tx_compose_registry_add_py,
        METH_VARARGS,
        "Add entry to compose registry.\n\n"
        "Args:\n"
        "    tx_type (str): TX type identifier\n"
        "    callback (callable): Builder function(ledger, utxo_list, user_data) -> datum\n"
        "    user_data (optional): Optional user data\n\n"
        "Returns:\n"
        "    int: 0 on success, negative on error"
    },
    {
        "dap_chain_tx_compose_registry_remove",
        dap_chain_tx_compose_registry_remove_py,
        METH_VARARGS,
        "Remove entry from compose registry.\n\n"
        "Args:\n"
        "    tx_type (str): TX type identifier"
    },
    {
        "dap_chain_tx_compose_registry_find",
        dap_chain_tx_compose_registry_find_py,
        METH_VARARGS,
        "Find entry in compose registry.\n\n"
        "Args:\n"
        "    tx_type (str): TX type identifier\n\n"
        "Returns:\n"
        "    capsule: dap_chain_tx_compose_registry_entry_t or None"
    },
    
    {NULL, NULL, 0, NULL}  // Sentinel
};

/**
 * @brief Initialize TX Compose module
 * @param module Python module object
 * @return 0 on success, -1 on error
 */
int cellframe_compose_init(PyObject *module) {
    log_it(L_INFO, "Initializing TX Compose module...");
    
    // Add methods to module
    for (PyMethodDef *def = s_compose_methods; def->ml_name != NULL; def++) {
        PyObject *func = PyCFunction_New(def, NULL);
        if (!func) {
            log_it(L_ERROR, "Failed to create function '%s'", def->ml_name);
            return -1;
        }
        
        if (PyModule_AddObject(module, def->ml_name, func) < 0) {
            log_it(L_ERROR, "Failed to add function '%s' to module", def->ml_name);
            Py_DECREF(func);
            return -1;
        }
    }
    
    log_it(L_INFO, "TX Compose module initialized successfully");
    return 0;
}
