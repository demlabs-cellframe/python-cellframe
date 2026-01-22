/**
 * @file cf_compose_init.c
 * @brief Python TX Compose module initialization
 * @details Initializes Python TX Compose API and exports functions
 * @author Cellframe Development Team
 * @date 2025-01-11 (Phase 3.5 Refactoring)
 */

#include "cellframe.h"
#include "dap_chain_tx_compose_api.h"
#include "cf_compose.h"

#define LOG_TAG "python_cellframe_compose_init"

/**
 * @brief Python TX Compose module methods
 */
static PyMethodDef s_compose_methods[] = {
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
    
    // High-level Compose API
    {
        "dap_compose_config_create",
        (PyCFunction)(void(*)(void))dap_compose_config_create_py,
        METH_VARARGS | METH_KEYWORDS,
        "Create compose configuration.\n\n"
        "Args:\n"
        "    net_name (str): Network name\n"
        "    url (str, optional): URL string\n"
        "    port (int, optional): Port\n"
        "    cert_path (str, optional): Certificate path\n"
        "    enc (bool, optional): Enable encryption\n\n"
        "Returns:\n"
        "    dict: Configuration dictionary"
    },
    
    {
        "dap_compose_tx_create",
        dap_compose_tx_create_py,
        METH_VARARGS,
        "Create transaction using compose system.\n\n"
        "Args:\n"
        "    config_dict (dict): Configuration dictionary\n"
        "    tx_type (str): Transaction type\n"
        "    params_dict (dict): Parameters dictionary\n\n"
        "Returns:\n"
        "    capsule: Transaction datum capsule or None"
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
