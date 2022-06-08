#include "libdap-io-python.h"

#define LOG_TAG "libdap-io-python"

PyMethodDef DapIOPythonMethods[] = {
        {"listen", dap_io_listen, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};


PyTypeObject DapIOObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "DAP.IO",             /* tp_name */
        sizeof(PyIOObject),         /* tp_basicsize */
        0,                                  /* tp_itemsize */
        0,                                  /* tp_dealloc */
        0,                                  /* tp_print */
        0,                                  /* tp_getattr */
        0,                                  /* tp_setattr */
        0,                                  /* tp_reserved */
        0,                                  /* tp_repr */
        0,                                  /* tp_as_number */
        0,                                  /* tp_as_sequence */
        0,                                  /* tp_as_mapping */
        0,                                  /* tp_hash  */
        0,                                  /* tp_call */
        0,                                  /* tp_str */
        0,                                  /* tp_getattro */
        0,                                  /* tp_setattro */
        0,                                  /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,            /* tp_flags */
        "IO objects",               /* tp_doc */
        0,		                        /* tp_traverse */
        0,         		                /* tp_clear */
        0,		                        /* tp_richcompare */
        0,		                        /* tp_weaklistoffset */
        0,		                        /* tp_iter */
        0,         		                /* tp_iternext */
        DapIOPythonMethods,            /* tp_methods */
        0,                                  /* tp_members */
        0,                                  /* tp_getset */
        0,                                  /* tp_base */
        0         ,                         /* tp_dict */
        0,                                  /* tp_descr_get */
        0,                                  /* tp_descr_set */
        0,                                  /* tp_dictoffset */
        0,                                  /* tp_init */
        0,                                  /* tp_alloc */
        PyType_GenericNew,                  /* tp_new */
};

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
