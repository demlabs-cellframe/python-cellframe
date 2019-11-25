#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include "libdap-python.h"
#include "libdap-crypto-python.h"
#include "libdap-server-core-python.h"
#include "libdap_crypto_key_type_python.h"
#include "libdap_crypto_data_type.h"
// === CHAIN ==
#include "libdap-chain-python.h"
#include "libdap_chain_type_python.h"
#include "libdap_chain_atom_iter_python.h"
#include "wrapping_dap_chain_cell.h"
#include "wrapping_dap_chain_common.h"
#include "wrapping_dap_chain_cs.h"
#include "wrapping_dap_chain_datum.h"
#include "wrapping_dap_chain_datum_token.h"
#include "wrapping_dap_chain_datum_tx.h"
#include "wrapping_dap_chain_ledger.h"
// ============
#include "dap_common.h"


#ifdef __cplusplus
extern "C" {
#endif

#undef LOG_TAG
#define LOG_TAG "python-cellframe"

static bool init_crypto;
static bool init_chain;

static PyObject* CellFrame_error;

static PyObject *python_cellframe_init(PyObject *self, PyObject *args);

static PyObject *python_cellframe_deinit(PyObject *self, PyObject *args);

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

static struct PyModuleDef CellFramePythonModule = {
        PyModuleDef_HEAD_INIT,
        "CellFrame",   /* name of module */
        NULL, /* module documentation, may be NULL */
        -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
        CellFramePythonMethods
};

PyMODINIT_FUNC PyInit_CellFrame(void);

#ifdef __cplusplus
}
#endif


