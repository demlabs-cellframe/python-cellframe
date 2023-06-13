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
    const char *addr;
    uint16_t port;
    uint16_t type;
    if (!PyArg_ParseTuple(args, "sHH", &addr, &port, &type)){
        return NULL;
    }
    if (type > 1)
        return  NULL;
    PyObject *obj = _PyObject_New(&DapServerObjectType);
    ((PyDapServerObject*)obj)->t_server = dap_server_new( addr, port, type, NULL);
    return Py_BuildValue("O", obj);
}
