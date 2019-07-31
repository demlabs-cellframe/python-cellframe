#include "libdap-server-core-python.h"


static PyObject *dap_server_core_init(PyObject *self, PyObject *args){
    uint32_t l_thread_cnt;
    size_t conn_t;
    if (!PyArg_ParseTuple(args, "I|n", &l_thread_cnt, &conn_t)){
        return NULL;
    }
    int32_t result = dap_server_init(l_thread_cnt);
    if ( result != 0 ) {
       log_it( L_CRITICAL, "Can't init socket server module" );
     }
    dap_events_init(l_thread_cnt, conn_t);
    dap_client_remote_init();
    return PyLong_FromLong(result);
}
static PyObject *dap_server_core_deinit(){
    dap_client_remote_deinit();
    dap_server_deinit();
    dap_events_deinit();
    return PyLong_FromLong(0);
}

static PyObject *dap_server_core_loop(PyObject *self, PyObject *args){
    int32_t result = dap_server_loop(t_server);
    return PyLong_FromLong(result);
}

static PyObject *dap_server_core_listen(PyObject *self, PyObject *args){
    const char *addr;
    uint16_t port;
    uint16_t type;
    if (!PyArg_ParseTuple(args, "s|H|H", &addr, &port, &type)){
        return NULL;
    }
    if (type > 1)
        return  NULL;
    t_server = dap_server_listen(addr, port, type);
    return PyLong_FromLong(0);
}

PyMODINIT_FUNC PyInit_libdap_server_core_python_module(void){
    return PyModule_Create(&dapservercorepythonmodule);
}

int main(int argc, char **argv) {
    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }

    /* Add a built-in module, before Py_Initialize */
    PyImport_AppendInittab("libdap_server_core_python_module", PyInit_libdap_server_core_python_module);

    /* Pass argv[0] to the Python interpreter */
    Py_SetProgramName(program);

    /* Initialize the Python interpreter.  Required. */
    Py_Initialize();

    /* Optionally import the module; alternatively,
       import can be deferred until the embedded script
       imports it. */
    PyImport_ImportModule("libdap_server_core_python_module");

    PyMem_RawFree(program);
    return 0;
}
