#include "wrapping_http.h"

PyMethodDef DapHttpMethods[] = {
        {"new", dap_http_new_py, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapHttpObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.DapHttp",             /* tp_name */
        sizeof(PyDapHttpObject),                   /* tp_basicsize */
        0,                                                 /* tp_itemsize */
        0,                                                 /* tp_dealloc */
        0,                                                 /* tp_print */
        0,                                                 /* tp_getattr */
        0,                                                 /* tp_setattr */
        0,                                                 /* tp_reserved */
        0,                                                 /* tp_repr */
        0,                                                 /* tp_as_number */
        0,                                                 /* tp_as_sequence */
        0,                                                 /* tp_as_mapping */
        0,                                                 /* tp_hash  */
        0,                                                 /* tp_call */
        0,                                                 /* tp_str */
        0,                                                 /* tp_getattro */
        0,                                                 /* tp_setattro */
        0,                                                 /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                           /* tp_flags */
        "Dap Http object",                         /* tp_doc */
        0,		                                       /* tp_traverse */
        0,                        		               /* tp_clear */
        0,		                                       /* tp_richcompare */
        0,                        		               /* tp_weaklistoffset */
        0,		                                       /* tp_iter */
        0,                        		               /* tp_iternext */
        DapHttpMethods,                   /* tp_methods */
        0,                                                 /* tp_members */
        0,                                                 /* tp_getset */
        0,                                                 /* tp_base */
        0,                                                 /* tp_dict */
        0,                                                 /* tp_descr_get */
        0,                                                 /* tp_descr_set */
        0,                                                 /* tp_dictoffset */
        0,                                                 /* tp_init */
        0,                                                 /* tp_alloc */
        PyType_GenericNew,                                 /* tp_new */
};

PyObject *dap_http_new_py(PyObject *self, PyObject *args){
    PyObject *obj_server;
    const char * name;
    if (!PyArg_ParseTuple(args, "O|s", &obj_server, &name))
        return NULL;
    int res = dap_http_new(((PyDapServerObject*)obj_server)->t_server, name);
    return PyLong_FromLong(res);
}
