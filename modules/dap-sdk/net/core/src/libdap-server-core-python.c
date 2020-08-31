#include "libdap-server-core-python.h"

#define LOG_TAG "libdap-server-core-python"

int dap_server_core_init(uint32_t l_thread_cnt, size_t conn_t){
    int result = dap_server_init();
    if ( result != 0 ) {
       log_it( L_CRITICAL, "Can't init socket server module" );
    }
    dap_events_init(l_thread_cnt, conn_t);
    return result;
}

void dap_server_core_deinit(void){
    dap_server_deinit();
    dap_events_deinit();
}

PyObject *dap_server_core_listen(PyObject *self, PyObject *args){
    const char *addr;
    uint16_t port;
    uint16_t type;
    PyObject *events;
    if (!PyArg_ParseTuple(args, "O|s|H|H", &events, &addr, &port, &type)){
        return NULL;
    }
    if (type > 1)
        return  NULL;
    PyObject *obj = _PyObject_New(&dapServer_dapServerType);
    ((PyDapServerObject*)obj)->t_server = dap_server_new(((PyDapEventsObject*)events)->t_events, addr, port, type);
    return Py_BuildValue("O", obj);
}
