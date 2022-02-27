#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include "libdap-python.h"
#include "libdap-crypto-python.h"
#include "libdap-server-core-python.h"
#include "libdap_crypto_key_type_python.h"
#include "libdap_crypto_data_type.h"
#include "wrapping_dap_sign.h"
// === CHAIN ==
#include "libdap-chain-python.h"
#include "libdap_chain_type_python.h"
#include "libdap_chain_atom_iter_python.h"
#include "wrapping_dap_chain_cell.h"
#include "wrapping_dap_chain_common.h"
#include "wrapping_dap_chain_cs.h"
#include "wrapping_dap_chain_datum.h"
#include "wrapping_dap_chain_datum_token.h"
#include "wrapping_dap_chain_tx_token_ext.h"
#include "wrapping_dap_chain_datum_tx.h"
#include "wrapping_dap_chain_ledger.h"
#include "wrapping_dap_chain_datum_tx.h"
#include "wrapping_dap_chain_tx_in.h"
#include "wrapping_dap_chain_tx_in_cond.h"
#include "wrapping_dap_chain_tx_out.h"
#include "wrapping_dap_chain_tx_out_cond.h"
#include "wrapping_dap_chain_tx_out_cond_subtype_srv_pay.h"
#include "wrapping_dap_chain_tx_out_cond_subtype_srv_stake.h"
#include "wrapping_dap_chain_tx_out_cond_subtype_srv_xchange.h"
#include "wrapping_dap_chain_tx_pkey.h"
#include "wrapping_dap_chain_tx_receipt.h"
#include "wrapping_dap_chain_tx_out_ext.h"
// ============
// === Chain net ===
#include "libdap_chain_net_python.h"
#include "wrapping_dap_chain_net_node.h"
#include "wrapping_dap_chain_net_node_client.h"
#include "wrapping_dap_chain_net_node_info.h"
#include "wrapping_dap_chain_net_state.h"
// ============
// === Chain net srv ===
#include "wrapping_dap_chain_net_srv.h"
#include "wrapping_dap_chain_net_srv_client.h"
#include "wrapping_dap_chain_net_srv_common.h"
#include "wrapping_dap_chain_net_srv_order.h"
// ============


#include "wrapping_http.h"
#include "wrapping_dap_enc_http.h"
#include "wrapping_dap_stream.h"
#include "wrapping_dap_stream_ctl.h"
#include "wrapping_dap_mempool.h"
#include "wrapping_dap_http_folder.h"
#include "dap_chain_wallet_python.h"

//#include "dap_http_client_simple.h"
//#include "dap_chain_wallet.h"
#include "dap_chain_cs.h"
#include "wrapping_dap_chain_atom_ptr.h"
//#include "dap_chain_cs_dag.h"
//#include "dap_chain_cs_dag_poa.h"
//#include "dap_chain_cs_dag_pos.h"
#include "dap_chain_net_srv.h"
#include "dap_http_simple.h"
#include "dap_stream_ch_chain.h"
#include "dap_stream_ch_chain_net.h"
#include "dap_stream_ch_chain_net_srv.h"
#include "dap_enc_ks.h"
#include "dap_chain_cs_none.h"
#include "libdap_chain_net_python.h"

#include "dap_app_cli.h"
#include "libdap-app-cli-python.h"
#include "wrapping_dap_app_cli_server.h"

#include "dap_file_utils.h"
#include "dap_string.h"
#include "generator_config_file.h"
#include "wrapping_dap_chain_gdb.h"

#include "dap_common.h"
#include "dap_server.h"
#include "wrapping_json_rpc_request.h"
#include "wrapping_json_rpc_response.h"
#ifdef _WIN32
#include "Windows.h"

BOOL WINAPI consoleHandler(DWORD);
#else
#include "signal.h"
#include "wrapping_http_status_code.h"
#include "wrapping_dap_http_simple.h"
#ifdef DAP_SUPPORT_PYTHON_PLUGINS
    #include "../modules/plugins/include/dap_plugins_python_app_context.h"
#endif


void sigfunc(int sig);
#endif

PyObject *python_cellframe_init(PyObject *self, PyObject *args);

void deinit_modules(void);

PyObject *python_cellframe_deinit(PyObject *self, PyObject *args);

static PyMethodDef CellFramePythonMethods[] = {
        {"init", python_cellframe_init, METH_VARARGS, "Initialization of the python-cellframe interface DAP (Deus Applicaions Prototypes)"},
        {"deinit", python_cellframe_deinit, METH_VARARGS, "Deinitialization of the python-cellframe interface DAP (Deus Applicaions Prototypes)"},
        {"setLogLevel", (PyCFunction)dap_set_log_level, METH_VARARGS, "Setting the logging level"},
        {"logIt", (PyCFunction)dap_log_it, METH_VARARGS, "The wrapper of the log_it function for the libdap library"},
        {"logItDebug", (PyCFunction)dap_log_it_debug, METH_VARARGS, "The log_it wrapper for the libdap library displays information with the logging level DEBUG"},
        {"logItInfo", (PyCFunction)dap_log_it_info, METH_VARARGS, "The log_it wrapper for the libdap library displays information with the logging level INFO"},
        {"logItNotice", (PyCFunction)dap_log_it_notice, METH_VARARGS, "The log_it wrapper for the libdap library displays information with the logging level NOTICE"},
        {"logItMessage", (PyCFunction)dap_log_it_message, METH_VARARGS, "The log_it wrapper for the libdap library displays information with the logging level MESSAGE"},
        {"logItDap", (PyCFunction)dap_log_it_dap, METH_VARARGS, "The log_it wrapper for the libdap library displays information with the logging level DAP"},
        {"logItWarning", (PyCFunction)dap_log_it_warning, METH_VARARGS, "The log_it wrapper for the libdap library displays information with the logging level WARNING"},
        {"logItAtt", (PyCFunction)dap_log_it_att, METH_VARARGS, "The log_it wrapper for the libdap library displays information with the logging level ATT"},
        {"logItError", (PyCFunction)dap_log_it_error, METH_VARARGS, "The log_it wrapper for the libdap library displays information with the logging level ERROR"},
        {"logItCritical", (PyCFunction)dap_log_it_critical, METH_VARARGS, "The log_it wrapper for the libdap library displays information with the logging level CRITICAL"},

        {"configGetItem", (PyCFunction)py_m_dap_config_get_item, METH_VARARGS, ""},
        {"configGetItemDefault", (PyCFunction)py_m_dap_config_get_item_default, METH_VARARGS, ""},
        {NULL, NULL, 0, NULL}
};

static char CellFramePythonModuleDoc[]=
"CellFrame SDK.Python v"DAP_VERSION" welcomes you!"
"";
static struct PyModuleDef CellFramePythonModule = {
        PyModuleDef_HEAD_INIT,
        "CellFrame",   /* name of module */
        CellFramePythonModuleDoc, /* module documentation, may be NULL */
        -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
        CellFramePythonMethods,
        NULL,
        NULL,
        NULL,
        NULL
};

PyMODINIT_FUNC PyInit_libCellFrame(void);
