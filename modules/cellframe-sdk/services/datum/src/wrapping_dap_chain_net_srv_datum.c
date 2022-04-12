#include "wrapping_dap_chain_net_srv_datum.h"

PyMethodDef DapChainNetSrvDatum_method[] = {
        {"create", (PyCFunction) wrapping_dap_chain_net_srv_datum_create, METH_VARARGS | METH_STATIC, ""},
        {"read", (PyCFunction) wrapping_dap_chain_net_srv_datum_read, METH_VARARGS|METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainDatumCustomObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainNetSrvDatum",            /* tp_name */
        sizeof(PyDapChainNetSrvDatumObject),     /* tp_basicsize */
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
        "Chain net srv datum object",              /* tp_doc */
        0,		                         /* tp_traverse */
        0,		                         /* tp_clear */
        0,		                         /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        0,		                         /* tp_iter */
        0,		                         /* tp_iternext */
        DapChainNetSrvDatum_method,       /* tp_methods */
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

PyObject *wrapping_dap_chain_net_srv_datum_read(PyObject *self, PyObject *args){
    (void)self;
    char *str_path;
    if (!PyArg_ParseTuple(args, "s", &str_path)){
        return NULL;
    }
    size_t l_size_data = 0;
    void* l_data = dap_chain_net_srv_file_datum_data_read(str_path, &l_size_data);
    PyObject *obj_bytes = PyBytes_FromStringAndSize(l_data, l_size_data);
    return obj_bytes;
}

PyObject *wrapping_dap_chain_net_srv_datum_create(PyObject *self, PyObject *args){
    (void)self;
    PyObject *obj_chain;
    PyObject *obj_data_in_datum;
    if (!PyArg_ParseTuple(args, "OO", &obj_chain, &obj_data_in_datum)){
        return NULL;
    }
    if (!PyBytes_Check(obj_data_in_datum)){
        PyErr_SetString(PyExc_ValueError, "This function takes an object of type bytes as its second argument.");
        return NULL;
    }
    if (!PyDapChain_Check(obj_chain)){
        PyErr_SetString(PyExc_ValueError, "The first argument to this function must be an object of type Chain.");
        return NULL;
    }
    void *l_data = PyBytes_AsString(obj_data_in_datum);
    size_t l_data_size = PyBytes_Size(obj_data_in_datum);
    char* res = dap_chain_net_srv_datum_custom_add(((PyDapChainObject*)obj_chain)->chain_t, l_data, l_data_size);
    if (res == NULL)
        Py_RETURN_NONE;
    return Py_BuildValue("s", res);
}
