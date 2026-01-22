#ifndef CELLFRAME_NETWORK_H
#define CELLFRAME_NETWORK_H

#include <Python.h>

#ifdef __cplusplus
extern "C" {
#endif

// =========================================
// NETWORK TYPE DEFINITION
// =========================================

typedef struct {
    PyObject_HEAD
    void *network_ptr;
    char *network_name;
} PyCellframeNetwork;

// =========================================
// NETWORK FUNCTION DECLARATIONS
// =========================================

// Legacy type (for compatibility)
extern PyTypeObject PyCellframeNetworkType;
PyObject* PyCellframeNetwork_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCellframeNetwork_init(PyCellframeNetwork *self, PyObject *args, PyObject *kwds);
void PyCellframeNetwork_dealloc(PyCellframeNetwork *self);
PyObject* PyCellframeNetwork_connect(PyCellframeNetwork *self, PyObject *args);

// Modern Network API functions (DAP SDK Binding Standards compliant)
// Rule DAP_NAMING_002: All Python binding functions have _py suffix
PyObject* dap_chain_net_by_name_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_by_id_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_id_by_name_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_count_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_iter_start_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_iter_next_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_name_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_id_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_state_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_state_go_to_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_start_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_stop_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_native_ticker_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_load_all_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_try_online_all_py(PyObject *self, PyObject *args);

// Chain access functions 
PyObject* dap_chain_net_get_chain_by_name_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_chain_by_id_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_chain_by_chain_type_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_default_chain_by_chain_type_py(PyObject *self, PyObject *args);
PyObject* dap_chain_get_name_py(PyObject *self, PyObject *args);
PyObject* dap_chain_get_id_py(PyObject *self, PyObject *args);

// Node info functions
PyObject* dap_chain_net_get_role_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_is_my_node_authorized_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_cur_addr_int_py(PyObject *self, PyObject *args);

// Network operations functions
PyObject* dap_chain_net_links_dump_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_link_add_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_target_state_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_sync_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_links_establish_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_purge_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_announce_addr_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_announce_addr_all_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_tx_get_fee_py(PyObject *self, PyObject *args);

// Advanced functions with complex structures
PyObject* dap_chain_net_get_cur_cell_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_my_node_info_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_authorized_nodes_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_verify_datum_for_add_py(PyObject *self, PyObject *args);

// Callback functions
PyObject* dap_chain_add_mempool_notify_callback_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_add_nodelist_notify_callback_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_srv_order_add_notify_callback_py(PyObject *self, PyObject *args);

// Module initialization
int cellframe_network_init(PyObject *module);

#ifdef __cplusplus
}
#endif


#endif // CELLFRAME_NETWORK_H 