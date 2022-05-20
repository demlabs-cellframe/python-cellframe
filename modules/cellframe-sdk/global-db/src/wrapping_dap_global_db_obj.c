#include "wrapping_dap_global_db_obj.h"

PyGetSetDef DapChainGlobalDBContainerGetSet[] = {
        {"id", (getter)wrapping_dap_chain_global_db_obj_get_id, NULL, NULL, NULL},
        {"key", (getter)wrapping_dap_chain_global_db_obj_get_key, NULL, NULL, NULL},
        {"value", (getter)wrapping_dap_chain_global_db_obj_get_value, NULL, NULL, NULL},
        {NULL}
};

PyTypeObject DapChainGlobalDBContainerObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainGlobalDBContainer",                                            /* tp_name */
        sizeof(PyDapChainGlobalDBContainerObject),                                     /* tp_basicsize */
        0,                                                            /* tp_itemsize */
        0,                                                            /* tp_dealloc */
        0,                                                            /* tp_print */
        0,                                                            /* tp_getattr */
        0,                                                            /* tp_setattr */
        0,                                                            /* tp_reserved */
        0,                                                            /* tp_repr */
        0,                                                            /* tp_as_number */
        0,                                                            /* tp_as_sequence */
        0,                                                            /* tp_as_mapping */
        0,                                                            /* tp_hash  */
        0,                                                            /* tp_call */
        0,                                                            /* tp_str */
        0,                                                            /* tp_getattro */
        0,                                                            /* tp_setattro */
        0,                                                            /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                                      /* tp_flags */
        "Chain GlobalDB container object",                                              /* tp_doc */
        0,		                                                      /* tp_traverse */
        0,		                                                      /* tp_clear */
        0,		                                                      /* tp_richcompare */
        0,		                                                      /* tp_weaklistoffset */
        0,		                                                      /* tp_iter */
        0,		                                                      /* tp_iternext */
        0,                                      /* tp_methods */
        0,                                                            /* tp_members */
        DapChainGlobalDBContainerGetSet,                                                            /* tp_getset */
        0,                                                            /* tp_base */
        0,                                                            /* tp_dict */
        0,                                                            /* tp_descr_get */
        0,                                                            /* tp_descr_set */
        0,                                                            /* tp_dictoffset */
        0,                                                            /* tp_init */
        0,                                                            /* tp_alloc */
        PyType_GenericNew,                                            /* tp_new */
};

PyObject *wrapping_dap_chain_global_db_obj_get_id(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", ((PyDapChainGlobalDBContainerObject*)self)->obj.id);
}
PyObject *wrapping_dap_chain_global_db_obj_get_key(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("s", ((PyDapChainGlobalDBContainerObject*)self)->obj.key);
}
PyObject *wrapping_dap_chain_global_db_obj_get_value(PyObject *self, void *closure){
    (void)closure;
    size_t l_size_value = ((PyDapChainGlobalDBContainerObject*)self)->obj.value_len;
    if (l_size_value == 0 || !((PyDapChainGlobalDBContainerObject*)self)->obj.value){
        Py_RETURN_NONE;
    }
    PyObject *obj_bytes = PyBytes_FromStringAndSize((char *)((PyDapChainGlobalDBContainerObject*)self)->obj.value, l_size_value);
    return obj_bytes;
}
