#include "libdap-python.h"
#include "libdap-io-python.h"

#define LOG_TAG "libdap-io-python"

static PyMethodDef DapIOPythonMethods[] = {
        {"listen", dap_io_listen, METH_VARARGS | METH_STATIC, ""},
        {}
};


PyTypeObject DapIOObjectType = DAP_PY_TYPE_OBJECT(
        "DAP.IO", sizeof(PyIOObject),
        "IO objects",
        .tp_methods = DapIOPythonMethods);

int dap_io_init(uint32_t l_thread_cnt, size_t conn_t){
    int result = dap_server_init();
    if ( result != 0 ) {
       log_it( L_CRITICAL, "Can't initialize \"IO\" module" );
    }
    dap_events_init(l_thread_cnt, conn_t);
    return result;
}

void dap_io_deinit(void){
    dap_server_deinit();
    dap_events_deinit();
}

PyObject *dap_io_listen(PyObject *self, PyObject *args){
    const char *addr;
    uint16_t port;
    uint16_t type;
    if (!PyArg_ParseTuple(args, "s|H|H", &addr, &port, &type)){
        return NULL;
    }
    if (type > 1)
        return  NULL;
    PyObject *obj = _PyObject_New(&dapServer_dapServerType);
    ((PyDapServerObject*)obj)->t_server = dap_server_new( addr, port, type, NULL);
    return Py_BuildValue("O", obj);
}
