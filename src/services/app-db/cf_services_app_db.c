#include "cellframe.h"
#include "cf_services_ext.h"
#include "dap_chain_net_srv_app_db.h"

// =============================================================================
// APP-DB SERVICE
// =============================================================================

PyObject* dap_chain_net_srv_app_db_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    if (!PyArg_ParseTuple(a_args, ""))
        return NULL;
    int l_res = dap_chain_net_srv_app_db_init();
    return PyLong_FromLong(l_res);
}

PyObject* dap_chain_net_srv_app_db_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    if (!PyArg_ParseTuple(a_args, ""))
        return NULL;
    dap_chain_net_srv_app_db_deinit();
    Py_RETURN_NONE;
}
