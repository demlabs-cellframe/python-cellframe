#include "libdap-server-core-python.h"

int dap_server_core_init(uint32_t l_thread_cnt, size_t conn_t){
    int result = dap_server_init(l_thread_cnt);
    if ( result != 0 ) {
       log_it( L_CRITICAL, "Can't init socket server module" );
    }
    dap_events_init(l_thread_cnt, conn_t);
    dap_client_remote_init();
    return result;
}

void dap_server_core_deinit(void){
    dap_client_remote_deinit();
    dap_server_deinit();
    dap_events_deinit();
}

PyObject *dap_server_core_loop(PyObject *self, PyObject *args){
    PyObject *obj_server;
    if (!PyArg_ParseTuple(args, "O", &obj_server)){
        return NULL;
    }
    int32_t result = dap_server_loop(((PyDapServerObject*)obj_server)->t_server);
    log_it( result ? L_CRITICAL : L_NOTICE, "Server loop stopped with return code %d", ((PyDapServerObject*)obj_server)->t_server );
    return PyLong_FromLong(result);
}

PyObject *dap_server_core_listen(PyObject *self, PyObject *args){
    const char *addr;
    uint16_t port;
    uint16_t type;
    if (!PyArg_ParseTuple(args, "s|H|H", &addr, &port, &type)){
        return NULL;
    }
    if (type > 1)
        return  NULL;
    PyObject *obj = _PyObject_New(&dapServer_dapServerType);
    ((PyDapServerObject*)obj)->t_server = dap_server_listen(addr, port, type);
    return Py_BuildValue("O", obj);
}
