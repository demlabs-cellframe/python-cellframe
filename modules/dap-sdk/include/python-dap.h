#pragma once

#define PY_SSIZE_T_CLEAN
#include "Python.h"
////#include "libdap-python.h"
// CRYPTO
#include "libdap-crypto-python.h"
#include "libdap_crypto_data_type.h"
#include "libdap_crypto_key_python.h"
#include "libdap_crypto_key_type_python.h"
#include "wrapping_cert.h"
#include "wrapping_cert_metadata.h"
#include "wrapping_dap_hash.h"
#include "wrapping_dap_pkey.h"
#include "wrapping_dap_sign.h"
#include "wrapping_base64.h"
#include "wrapping_base58.h"
#include "wrapping_dap_sign.h"
#include "wrapping_cert_metadata.h"
#include "wrapping_cert.h"
#include "wrapping_dap_pkey.h"
#include "wrapping_dap_hash.h"
#include "wrapping_dap_enc_key.h"
// net/core
#include "dap_events_python.h"
#include "dap_events_socket_python.h"
#include "dap_server_python.h"
#include "libdap-server-core-python.h"
// net/client
#include "libdap_client_python.h"
#include "wrapping_dap_client_error.h"
#include "wrapping_dap_client_stage.h"
#include "wrapping_dap_client_stage_status.h"
// net/server/enc
#include "wrapping_dap_enc_http.h"
// net/server/http
#include "wrapping_dap_http_simple.h"
#include "wrapping_http.h"
#include "wrapping_http_status_code.h"
#include "wrapping_dap_http_folder.h"
// net/stream
#include "wrapping_dap_stream.h"
#include "wrapping_dap_stream_ch.h"
#include "wrapping_dap_stream_ctl.h"
#include "wrapping_dap_stream_worker.h"
//app_context
#ifdef DAP_SUPPORT_PYTHON_PLUGINS
#include "dap_plugins_python_app_context.h"
#endif
//#include "libdap-crypto-python.h"
////#include "libdap-crypto-python.h"
//#include "libdap-server-core-python.h"
//#include "libdap_crypto_key_type_python.h"
//#include "libdap_crypto_data_type.h"
//// === CHAIN ==
//#include "libdap-chain-python.h"
//#include "libdap_chain_type_python.h"
//#include "libdap_chain_atom_iter_python.h"
//#include "wrapping_dap_chain_cell.h"
//#include "wrapping_dap_chain_common.h"
//#include "wrapping_dap_chain_cs.h"
//#include "wrapping_dap_chain_datum.h"
//#include "wrapping_dap_chain_datum_token.h"
//#include "wrapping_dap_chain_datum_tx.h"
//#include "wrapping_dap_chain_ledger.h"
//#include "wrapping_dap_chain_datum_tx.h"
//// ============
//// === Chain net ===
//#include "libdap_chain_net_python.h"
//#include "wrapping_dap_chain_net_node.h"
//#include "wrapping_dap_chain_net_node_client.h"
//#include "wrapping_dap_chain_net_node_info.h"
//#include "wrapping_dap_chain_net_state.h"
//// ============


//#include "wrapping_http.h"
//#include "wrapping_dap_enc_http.h"
//#include "wrapping_dap_stream.h"
//#include "wrapping_dap_stream_ctl.h"
//#include "wrapping_dap_mempool.h"
//#include "wrapping_dap_http_folder.h"
//#include "dap_chain_wallet_python.h"

////#include "dap_http_client_simple.h"
////#include "dap_chain_wallet.h"
//#include "dap_chain_cs.h"
////#include "dap_chain_cs_dag.h"
////#include "dap_chain_cs_dag_poa.h"
////#include "dap_chain_cs_dag_pos.h"
//#include "dap_chain_net_srv.h"
//#include "dap_http_simple.h"
//#include "dap_stream_ch_chain.h"
//#include "dap_stream_ch_chain_net.h"
//#include "dap_stream_ch_chain_net_srv.h"
//#include "dap_enc_ks.h"
//#include "dap_chain_cs_none.h"
//#include "libdap_chain_net_python.h"

//#include "dap_app_cli.h"
//#include "libdap-app-cli-python.h"
//#include "wrapping_dap_app_cli_server.h"

//#include "dap_file_utils.h"
//#include "dap_string.h"
//#include "generator_config_file.h"
//#include "wrapping_dap_chain_gdb.h"

#include "dap_common.h"
#include "dap_config.h"
#include "dap_list.h"
//#include "dap_server.h"
//#ifdef _WIN32
//#include "Windows.h"

//BOOL WINAPI consoleHandler(DWORD);
//#else
//#include "signal.h"
//#include "wrapping_http_status_code.h"
//#include "wrapping_dap_http_simple.h"
//#ifdef DAP_SUPPORT_PYTHON_PLUGINS
//    #include "../modules/plugins/include/dap_plugins_python_app_context.h"
//#endif


//void sigfunc(int sig);
//#endif

//PyObject *python_DAP_init(PyObject *self, PyObject *args);

//void deinit_modules(void);

//PyObject *python_DAP_deinit(PyObject *self, PyObject *args);

PyObject *dap_set_log_level(PyObject *self, PyObject *args);

PyObject* dap_log_it(PyObject* self, PyObject* args);

PyObject* dap_log_it_debug(PyObject* self, PyObject* args);
PyObject* dap_log_it_info(PyObject* self, PyObject* args);
PyObject* dap_log_it_notice(PyObject* self, PyObject* args);
PyObject* dap_log_it_message(PyObject* self, PyObject* args);
PyObject* dap_log_it_dap(PyObject* self, PyObject* args);
PyObject* dap_log_it_warning(PyObject* self, PyObject* args);
PyObject* dap_log_it_att(PyObject* self, PyObject* args);
PyObject* dap_log_it_error(PyObject* self, PyObject* args);
PyObject* dap_log_it_critical(PyObject* self, PyObject* args);

PyObject* py_m_dap_config_get_item(PyObject *self, PyObject *args);

PyObject* py_m_dap_config_get_item_default(PyObject *self, PyObject *args);

static PyMethodDef DAPPythonMethods[] = {
//        {"init", python_DAP_init, METH_VARARGS, "Initialization of the python-DAP interface DAP (Deus Applicaions Prototypes)"},
//        {"deinit", python_DAP_deinit, METH_VARARGS, "Deinitialization of the python-DAP interface DAP (Deus Applicaions Prototypes)"},
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

//        {"configGetItem", (PyCFunction)py_m_dap_config_get_item, METH_VARARGS, ""},
//        {"configGetItemDefault", (PyCFunction)py_m_dap_config_get_item_default, METH_VARARGS, ""},
        {NULL, NULL, 0, NULL}
};

static char DAPPythonModuleDoc[]=
"DAP SDK.Python v"DAP_VERSION" welcomes you!"
"";
static struct PyModuleDef DAPPythonModule = {
        PyModuleDef_HEAD_INIT,
        "DAP",   /* name of module */
        DAPPythonModuleDoc, /* module documentation, may be NULL */
        -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
        DAPPythonMethods,
        NULL,
        NULL,
        NULL,
        NULL
};

PyMODINIT_FUNC PyInit_libDAP(void);
