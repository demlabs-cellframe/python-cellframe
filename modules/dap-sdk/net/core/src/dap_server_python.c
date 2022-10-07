#include "dap_server_python.h"
#include "dap_events_python.h"

static PyMethodDef ServerPythonMethods[] = {
        {"listen", dap_server_listen_py, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapServerObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "DAP.Network.Server",                         /* tp_name */
    sizeof(PyDapServerObject),                    /* tp_basicsize */
    0,                                            /* tp_itemsize */
    0,                                            /* tp_dealloc */
    0,                                            /* tp_print */
    0,                                            /* tp_getattr */
    0,                                            /* tp_setattr */
    0,                                            /* tp_compare */
    0,                                            /* tp_repr */
    0,                                            /* tp_as_number */
    0,                                            /* tp_as_sequence */
    0,                                            /* tp_as_mapping */
    0,                                            /* tp_hash */
    0,                                            /* tp_call */
    0,                                            /* tp_str */
    0,                                            /* tp_getattro */
    0,                                            /* tp_setattro */
    0,                                            /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                           /* tp_flags */
    "DapServer objects",                          /* tp_doc */
    0,                                            /* tp_traverse */
    0,                                            /* tp_clear */
    0,                                            /* tp_richcompare */
    0,                                            /* tp_weaklistoffset */
    0,                                            /* tp_iter */
    0,                                            /* tp_iternext */
    ServerPythonMethods,                          /* tp_methods */
    0,                                            /* tp_members */
    0,                                            /* tp_getset */
    0,                                            /* tp_base */
    0,                                            /* tp_dict */
    0,                                            /* tp_descr_get */
    0,                                            /* tp_descr_set */
    0,                                            /* tp_dictoffset */
    0,                                            /* tp_init */
    0,                                            /* tp_alloc */
    PyType_GenericNew,                            /* tp_new */
};


PyObject *dap_server_listen_py(PyObject *self, PyObject *args)
{
    const char *addr;
    uint16_t port;
    uint16_t type;
    PyObject *events;
    if (!PyArg_ParseTuple(args, "O|s|H|H", &events, &addr, &port, &type)){
        return NULL;
    }
    if (type > 1)
        return  NULL;
    PyObject *obj = _PyObject_New(&DapServerObjectType);
    ((PyDapServerObject*)obj)->t_server = dap_server_new(((PyDapEventsObject*)events)->t_events, addr, port, type, NULL);
    return Py_BuildValue("O", obj);
}
