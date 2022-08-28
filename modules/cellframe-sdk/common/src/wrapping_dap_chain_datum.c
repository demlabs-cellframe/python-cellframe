#include "wrapping_dap_chain_datum.h"

//void PyDapChainDatumObject_dealloc(PyDapChainDatumObject* object){
//}

/* Dap chain datum type id */

PyTypeObject DapChainDatumTypeIdObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.Chain.DatumTypeId",          /* tp_name */
        sizeof(PyDapChainDatumTypeIdObject),   /* tp_basicsize */
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
        "Chain datum type id object",           /* tp_doc */
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

/* DAP chain datum */
PyMethodDef DapChainDatumMethods[] = {
        {"getSize", dap_chain_datum_size_py, METH_NOARGS, ""},
        {"isDatumTX", dap_chain_datum_is_type_tx, METH_NOARGS, ""},
        {"getDatumTX", wrapping_dap_chain_datum_get_datum_tx, METH_NOARGS, ""},
        {"isDatumToken", dap_chain_datum_is_type_token, METH_NOARGS, ""},
        {"getDatumToken", wrapping_dap_chain_datum_get_datum_token, METH_NOARGS, ""},
        {"isDatumTokenEmission", dap_chain_datum_is_type_emission, METH_NOARGS, ""},
        {"getDatumTokenEmission", wrapping_dap_chain_datum_get_datum_token_emission, METH_NOARGS, ""},
        {"isDatumCustom", dap_chain_datum_is_type_custom, METH_NOARGS, ""},
        {"getTypeStr", dap_chain_datum_get_type_str_py, METH_NOARGS, ""},
        {"getTypeId", dap_chain_datum_get_type_id_py, METH_NOARGS, ""},
        {NULL, NULL, 0, NULL}
};

PyGetSetDef  DapChainDatumGetSet[] = {
        {"versionStr", (getter)wrapping_dap_chain_datum_get_version_str_py, NULL, NULL, NULL},
        {"tsCreated", (getter)dap_chain_datum_get_ts_created_py, NULL, NULL, NULL},
        {"raw", (getter)wrapping_dap_chain_datum_get_raw_py, NULL, NULL, NULL},
        {"dataRaw", (getter)wrapping_dap_chain_datum_get_raw_data_py, NULL, NULL, NULL},
        {NULL}
};

PyTypeObject DapChainDatumObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.Chain.Datum",          /* tp_name */
        sizeof(PyDapChainDatumObject),   /* tp_basicsize */
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
        "Chain datum object",            /* tp_doc */
        0,		                         /* tp_traverse */
        0,		                         /* tp_clear */
        0,		                         /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        0,		                         /* tp_iter */
        0,		                         /* tp_iternext */
        DapChainDatumMethods,                               /* tp_methods */
        0,                               /* tp_members */
        DapChainDatumGetSet,                               /* tp_getset */
        0,                               /* tp_base */
        0,                               /* tp_dict */
        0,                               /* tp_descr_get */
        0,                               /* tp_descr_set */
        0,                               /* tp_dictoffset */
        0,                               /* tp_init */
        0,                               /* tp_alloc */
        PyDapChainDatumObject_new,       /* tp_new */
};

bool PyDapChainDatum_Check(PyDapChainDatumObject *self){
    return PyObject_TypeCheck(self, &DapChainDatumObjectType);
}

PyObject *PyDapChainDatumObject_new(PyTypeObject *type_object, PyObject *args, PyObject *kwds){
    PyObject *obj_arg_first;
    PyObject *obj_arg_second = NULL;
    if (!PyArg_ParseTuple(args, "O|O", &obj_arg_first, &obj_arg_second))
        return NULL;
    if (PyLong_Check(obj_arg_first)){
        if (!PyBytes_Check(obj_arg_second)){
            PyErr_SetString(PyExc_AttributeError, "The datum constructor can only take an instance of an object of "
                                                  "the bytes type as an instance");
            return NULL;
        }
        uint16_t type_id = (uint16_t)PyLong_AsUnsignedLong(obj_arg_first);
        void *l_bytes = (void*)PyBytes_AsString(obj_arg_second);
        size_t l_bytes_size = PyBytes_Size(obj_arg_second);
        PyDapChainDatumObject *obj = (PyDapChainDatumObject*)PyType_GenericNew(type_object, args, kwds);
        obj->datum = dap_chain_datum_create(type_id, l_bytes, l_bytes_size);
        return (PyObject *)obj;
    } else {
        if (!PyBytes_Check(obj_arg_first)){
            PyErr_SetString(PyExc_AttributeError, "The datum constructor can only take an instance of an object of "
                                                  "the bytes type as an instance");
            return NULL;
        }
        void *l_bytes = (void*)PyBytes_AsString(obj_arg_first);
        size_t l_bytes_size = PyBytes_Size(obj_arg_first);
        PyDapChainDatumObject *obj = (PyDapChainDatumObject*)PyType_GenericNew(type_object, args, kwds);
        obj->datum = (dap_chain_datum_t*)l_bytes;
        return (PyObject *)obj;
    }
}

PyObject *dap_chain_datum_size_py(PyObject *self, PyObject *args){
    (void)args;
    size_t size = dap_chain_datum_size(((PyDapChainDatumObject*)self)->datum);
    return PyLong_FromSize_t(size);
}

PyObject *dap_chain_datum_get_ts_created_py(PyObject *self, void* closure){
    (void)closure;
    PyDateTime_IMPORT;
    PyObject *obj_ts_long =  Py_BuildValue("(k)",((PyDapChainDatumObject*)self)->datum->header.ts_create);
    PyObject *obj_ts = PyDateTime_FromTimestamp(obj_ts_long);
    return obj_ts;
}

PyObject *dap_chain_datum_is_type_tx(PyObject *self, PyObject *args){
    (void)args;
    if (((PyDapChainDatumObject*)self)->datum->header.type_id == DAP_CHAIN_DATUM_TX){
        return Py_BuildValue("O", Py_True);
    } else {
        return Py_BuildValue("O", Py_False);
    }
}

PyObject *dap_chain_datum_is_type_token(PyObject *self, PyObject *args){
    (void)args;
    if (((PyDapChainDatumObject*)self)->datum->header.type_id == DAP_CHAIN_DATUM_TOKEN_DECL ){
        return Py_BuildValue("O", Py_True);
    } else {
        return Py_BuildValue("O", Py_False);
    }
}

PyObject *wrapping_dap_chain_datum_get_datum_token(PyObject *self, PyObject *args){
    (void)args;
    if (((PyDapChainDatumObject*)self)->datum->header.type_id == DAP_CHAIN_DATUM_TOKEN_DECL ){
        PyDapChainDatumTokenObject *obj_token = PyObject_New(PyDapChainDatumTokenObject,
                                                             &DapChainDatumTokenObjectType);
        PyObject_Dir((PyObject*)obj_token);
        size_t l_size_token = ((PyDapChainDatumObject*)self)->datum->header.data_size;
        obj_token->token = dap_chain_datum_token_read(((PyDapChainDatumObject*)self)->datum->data,
                                                      &l_size_token);
        return (PyObject*)obj_token;
    }else{
        PyErr_SetString(PyExc_Exception, "Due to the type of this datum, it is not possible to get the token datum.");
        return NULL;
    }
}

PyObject *dap_chain_datum_is_type_emission(PyObject *self, PyObject *args){
    (void)args;
    if (((PyDapChainDatumObject*)self)->datum->header.type_id == DAP_CHAIN_DATUM_TOKEN_EMISSION){
        return Py_BuildValue("O", Py_True);
    }else{
        return Py_BuildValue("O", Py_False);
    }
}

PyObject *wrapping_dap_chain_datum_get_datum_token_emission(PyObject *self, PyObject *args){
    (void)args;
    if (((PyDapChainDatumObject*)self)->datum->header.type_id == DAP_CHAIN_DATUM_TOKEN_EMISSION ){
        PyDapChainDatumTokenEmissionObject *obj_emission = PyObject_New(
                PyDapChainDatumTokenEmissionObject,
                &DapChainDatumTokenEmissionObjectType
                );
        PyObject_Dir((PyObject*)obj_emission);
        size_t l_token_emission_size = ((PyDapChainDatumObject*)self)->datum->header.data_size;
        obj_emission->token_emission = dap_chain_datum_emission_read(((PyDapChainDatumObject*)self)->datum->data,
                                                                     &l_token_emission_size);
        return (PyObject*)obj_emission;

    }else{
        PyErr_SetString(PyExc_Exception, "Incorrect of a datum type. Can't get a token datum");
        return NULL;
    }
}

PyObject *dap_chain_datum_is_type_custom(PyObject *self, PyObject *args){
    (void)args;
    if (((PyDapChainDatumObject*)self)->datum->header.type_id == DAP_CHAIN_DATUM_CUSTOM){
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

PyObject *wrapping_dap_chain_datum_get_datum_tx(PyObject *self, PyObject *args){
    (void)args;
    if(((PyDapChainDatumObject *)self)->datum->header.type_id == DAP_CHAIN_DATUM_TX){
        PyObject *obj_datum_tx = _PyObject_New(&DapChainDatumTxObjectType);
        obj_datum_tx = PyObject_Init(obj_datum_tx, &DapChainDatumTxObjectType);
        PyObject_Dir(obj_datum_tx);
        ((PyDapChainDatumTxObject*)obj_datum_tx)->datum_tx = (dap_chain_datum_tx_t *)((PyDapChainDatumObject*)self)->datum->data;
        ((PyDapChainDatumTxObject*)obj_datum_tx)->original = false;
        return obj_datum_tx;
    }else{
        PyErr_SetString(PyExc_Exception, "Due to the type of this datum, it is not possible to get the transaction datum.");
        return NULL;
    }
}

PyObject *dap_chain_datum_get_type_str_py(PyObject *self, PyObject *args){
    (void)args;
    const char *l_ret;
    DAP_DATUM_TYPE_STR(((PyDapChainDatumObject*)self)->datum->header.type_id, l_ret);
    if (l_ret == NULL)
        Py_RETURN_NONE;
    return Py_BuildValue("s", l_ret);
}

PyObject *dap_chain_datum_get_type_id_py(PyObject *self, PyObject *args){
    (void)args;
    return Py_BuildValue("H", ((PyDapChainDatumObject*)self)->datum->header.type_id);
}

PyObject *wrapping_dap_chain_datum_get_version_str_py(PyObject *self, void* closure){
    (void)closure;
    char *l_version = dap_strdup_printf("0x%02X",((PyDapChainDatumObject*)self)->datum->header.version_id);
    PyObject *l_obj_version = Py_BuildValue("s", l_version);
    DAP_FREE(l_version);
    return l_obj_version;
}

PyObject *wrapping_dap_chain_datum_get_raw_py(PyObject *self, void* closure){
    (void)closure;
    size_t l_size = dap_chain_datum_size(((PyDapChainDatumObject*)self)->datum);
    PyObject *l_obj_bytes = PyBytes_FromStringAndSize((char*)((PyDapChainDatumObject*)self)->datum, l_size);
    return l_obj_bytes;
}

PyObject *wrapping_dap_chain_datum_get_raw_data_py(PyObject *self, void* closure){
    (void)closure;
    size_t l_size = ((PyDapChainDatumObject*)self)->datum->header.data_size;
    PyObject *l_obj_bytes = PyBytes_FromStringAndSize((char*)((PyDapChainDatumObject*)self)->datum->data, l_size);
    return l_obj_bytes;
}

/* DAP chain datum iter */
PyTypeObject DapChainDatumIterObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.Chain.DatumIter",          /* tp_name */
        sizeof(PyDapChainDatumIterObject),   /* tp_basicsize */
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
        "Chain datum iter object",           /* tp_doc */
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
