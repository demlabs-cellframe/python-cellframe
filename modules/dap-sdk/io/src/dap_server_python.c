#include "dap_common.h"
#include "dap_server.h"
#include "dap_events.h"
#include "libdap-python.h"
#include "dap_events_python.h"
#include "dap_events_socket_python.h"
#include "dap_server_python.h"

#define LOG_TAG "libdap-server-python"

static PyObject *py_server_listen(PyObject *self, PyObject *args);

static PyMethodDef DapServerPythonMethods[] = {
        {"listen", py_server_listen, METH_VARARGS | METH_STATIC, ""},
        {}
};

PyTypeObject DapServerObjectType = DAP_PY_TYPE_OBJECT(
        "DAP.Network.Server", sizeof(PyDapServerObject),
        "DapServer object",
         .tp_methods = DapServerPythonMethods);

int py_server_init(uint32_t l_thread_cnt, size_t conn_t){
    int result = dap_server_init();
    if ( result != 0 ) {
       log_it( L_CRITICAL, "Can't initialize \"IO\" module" );
    }
    dap_events_init(l_thread_cnt, conn_t);
    return result;
}

void py_server_deinit(void){
    dap_server_deinit();
    dap_events_deinit();
}

static PyObject *py_server_listen(PyObject *self, PyObject *args)
{
    PyObject *obj_addrs;
    uint16_t l_count = 0;
    uint16_t l_type = 0;
    if (!PyArg_ParseTuple(args, "OHH", &obj_addrs, &l_count, &l_type) || !l_count || l_type > 1){
        return NULL;
    }

    if (!PyList_Check(obj_addrs)) {
        PyErr_SetString(PyExc_AttributeError, "An invalid function argument was specified. The first argument must be "
                                              "an array of server addresses.");
        return NULL;
    }

    char **l_addrs = DAP_NEW_Z_COUNT(char *, l_count);
    for (uint16_t i = 0; i < l_count; i++){
        char *l_current_addr;
        if (!PyArg_ParseTuple(PyList_GetItem(obj_addrs, i), "s", &l_current_addr)) {
            char *l_str_err = dap_strdup_printf("The %zu element in the list of server addresses is not a string.", i);
            PyErr_SetString(PyExc_AttributeError, l_str_err);
            DAP_DELETE(l_str_err);
            DAP_DELETE(l_addrs);
            return NULL;
        }
        l_addrs[i] = l_current_addr;
    }
    PyObject *obj = _PyObject_New(&DapServerObjectType);
    ((PyDapServerObject*)obj)->t_server = dap_server_new(l_addrs, l_count, l_type, NULL);
    DAP_DELETE(l_addrs);
    return Py_BuildValue("O", obj);
}
