#include "wrapping_dap_pkey.h"

PyTypeObject DapPkeyTypeObject_DapPkeyTypeObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.PkeyType",       /* tp_name */
        sizeof(PyDapPkeyTypeObject),/* tp_basicsize */
        0,                               /* tp_itemsize */
        0,                               /* tp_dealloc */
        0,                               /* tp_print */
        0,                               /* tp_getattr */
        0,                               /* tp_setattr */
        0,                               /* tp_reserved */
        0,                               /* tp_repr */
        0,                               /* tp_as_number */
        0,                               /* tp_as_sequence */
        0,                               /* tp_as_mapping */
        0,                               /* tp_hash  */
        0,                               /* tp_call */
        PyDapPkeyType_str,                               /* tp_str */
        0,                               /* tp_getattro */
        0,                               /* tp_setattro */
        0,                               /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,         /* tp_flags */
        "Pkey type object",        /* tp_doc */
        0,		                         /* tp_traverse */
        0,		                         /* tp_clear */
        0,		                         /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        0,		                         /* tp_iter */
        0,		                         /* tp_iternext */
        0,                               /* tp_methods */
        0,                               /* tp_members */
        0,                               /* tp_getset */
        0,                               /* tp_base */
        0,                               /* tp_dict */
        0,                               /* tp_descr_get */
        0,                               /* tp_descr_set */
        0,                               /* tp_dictoffset */
        0,                               /* tp_init */
        0,                               /* tp_alloc */
        PyType_GenericNew,               /* tp_new */
};

PyObject *PyDapPkeyType_str(PyObject *self){
    const char *str = dap_pkey_type_to_str(((PyDapPkeyTypeObject*)self)->pkey_type);
    return Py_BuildValue("s", str);
}

PyGetSetDef PyDapPkeyGetsSetsDef[] = {
        {"hash", (getter)wrapping_dap_pkey_get_hash, NULL, NULL, NULL},
        {"type", (getter)wrapping_dap_pkey_get_type, NULL, NULL, NULL},
        {"size", (getter)wrapping_dap_pkey_get_size, NULL, NULL, NULL},
        {NULL, NULL, NULL, NULL}
};

PyMethodDef PyDapPkeyMethods[]={
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapPkeyObject_DapPkeyObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.Pkey",       /* tp_name */
        sizeof(PyDapPkeyObject),/* tp_basicsize */
        0,                               /* tp_itemsize */
        0,                               /* tp_dealloc */
        0,                               /* tp_print */
        0,                               /* tp_getattr */
        0,                               /* tp_setattr */
        0,                               /* tp_reserved */
        0,                               /* tp_repr */
        0,                               /* tp_as_number */
        0,                               /* tp_as_sequence */
        0,                               /* tp_as_mapping */
        0,                               /* tp_hash  */
        0,                               /* tp_call */
        0,                               /* tp_str */
        0,                               /* tp_getattro */
        0,                               /* tp_setattro */
        0,                               /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,         /* tp_flags */
        "Pkey object",        /* tp_doc */
        0,		                             /* tp_traverse */
        0,		                        /* tp_clear */
        0,		                           /* tp_richcompare */
        0,                           /* tp_weaklistoffset */
        0,		                  /* tp_iter */
        0,		                         /* tp_iternext */
        PyDapPkeyMethods,                          /* tp_methods */
        0,                              /* tp_members */
        PyDapPkeyGetsSetsDef,                     /* tp_getset */
        0,                              /* tp_base */
        0,                               /* tp_dict */
        0,                               /* tp_descr_get */
        0,                               /* tp_descr_set */
        0,                               /* tp_dictoffset */
        0,                               /* tp_init */
        0,                               /* tp_alloc */
        PyType_GenericNew,               /* tp_new */
};

PyObject *wrapping_dap_pkey_get_type(PyObject *self, void *closure){
    (void)closure;
    PyDapPkeyTypeObject *obj_type_pkey = PyObject_New(PyDapPkeyTypeObject, &DapPkeyTypeObject_DapPkeyTypeObjectType);
    obj_type_pkey->pkey_type = ((PyDapPkeyObject*)self)->pkey->header.type;
    return (PyObject*)obj_type_pkey;
}
PyObject *wrapping_dap_pkey_get_hash(PyObject *self, void *closure){
    (void)closure;
    PyDapHashFastObject *obj_hash = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    dap_hash_fast(((PyDapPkeyObject*)self)->pkey->pkey, ((PyDapPkeyObject*)self)->pkey->header.size, obj_hash->hash_fast);
    obj_hash->origin = true;
    return (PyObject*)obj_hash;
}
PyObject *wrapping_dap_pkey_get_size(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("I", ((PyDapPkeyObject*)self)->pkey->header.size);
}
