#include "wrapping_dap_chain_common.h"

/* Chain hash slow  */
PyMethodDef DapChainHashSlowMethod[] = {
        {"toStr", (PyCFunction)dap_chain_hash_slow_to_str_py, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainHashSlowObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainHashSlow",       /* tp_name */
        sizeof(PyDapChainHashSlowObject),/* tp_basicsize */
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
        "Chain hash slow object",        /* tp_doc */
        0,		                         /* tp_traverse */
        0,		                         /* tp_clear */
        0,		                         /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        0,		                         /* tp_iter */
        0,		                         /* tp_iternext */
        DapChainHashSlowMethod,                               /* tp_methods */
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

/* Hash slow kind */
PyTypeObject DapChainHashSlowKindObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainSlowKind"  ,       /* tp_name */
        sizeof(PyDapChainHashSlowKindObject),  /* tp_basicsize */
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
        "Chain slow kind object",          /* tp_doc */
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

/* Chain addr */
PyMethodDef DapChainAddrMethods[] = {
        {"toStr", (PyCFunction)dap_chain_addr_to_str_py, METH_VARARGS, ""},
        {"fromStr", (PyCFunction)dap_chain_addr_from_str_py, METH_VARARGS | METH_STATIC, ""},
        {"fill", (PyCFunction)dap_chain_addr_fill_py, METH_VARARGS | METH_STATIC, ""},
        {"fillFromKey", (PyCFunction)dap_chain_addr_fill_from_key_py, METH_VARARGS | METH_STATIC, ""},
        {"checkSum", (PyCFunction)dap_chain_addr_check_sum_py, METH_VARARGS, ""},
        {"getNetId", (PyCFunction)dap_chain_addr_get_net_id_py, METH_NOARGS, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainAddrObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainAddr",       /* tp_name */
        sizeof(PyDapChainAddrObject),/* tp_basicsize */
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
        obj_addr_str,                               /* tp_str */
        0,                               /* tp_getattro */
        0,                               /* tp_setattro */
        0,                               /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,         /* tp_flags */
        "Chain address object",             /* tp_doc */
        0,		                         /* tp_traverse */
        0,		                         /* tp_clear */
        0,		                         /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        0,		                         /* tp_iter */
        0,		                         /* tp_iternext */
        DapChainAddrMethods,             /* tp_methods */
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

/* Chain net id */
PyMethodDef DapChainNetIdObjectMethods[] = {
        {"fromStr", (PyCFunction)dap_chain_net_id_from_str_py, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainNetIdObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainNetID",          /* tp_name */
        sizeof(PyDapChainNetIdObject),   /* tp_basicsize */
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
        "Chain net ID object",           /* tp_doc */
        0,		                         /* tp_traverse */
        0,		                         /* tp_clear */
        0,		                         /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        0,		                         /* tp_iter */
        0,		                         /* tp_iternext */
        DapChainNetIdObjectMethods,      /* tp_methods */
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

/* Chain net srv uid */
PyMethodDef DapChainNetSrvUIDObject[] = {
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainNetSrvUidObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainNetSrvUID",          /* tp_name */
        sizeof(dap_chain_net_srv_uid_t),   /* tp_basicsize */
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
        PyDapChainNetSrvUIDObject_str,   /* tp_str */
        0,                               /* tp_getattro */
        0,                               /* tp_setattro */
        0,                               /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,         /* tp_flags */
        "Chain net srv uid object",           /* tp_doc */
        0,		                         /* tp_traverse */
        0,		                         /* tp_clear */
        0,		                         /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        0,		                         /* tp_iter */
        0,		                         /* tp_iternext */
        DapChainNetSrvUIDObject,         /* tp_methods */
        0,                               /* tp_members */
        0,                               /* tp_getset */
        0,                               /* tp_base */
        0,                               /* tp_dict */
        0,                               /* tp_descr_get */
        0,                               /* tp_descr_set */
        0,                               /* tp_dictoffset */
        (initproc)PyDapChainNetSrvUIDObject_init, /* tp_init */
        0,                               /* tp_alloc */
        PyType_GenericNew,               /* tp_new */
};

/* Chain id */
PyTypeObject DapChainIdObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainID"  ,       /* tp_name */
        sizeof(PyDapChainIDObject),  /* tp_basicsize */
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
        DapChainIdObject_str,                               /* tp_str */
        0,                               /* tp_getattro */
        0,                               /* tp_setattro */
        0,                               /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,         /* tp_flags */
        "Chain id object",          /* tp_doc */
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

PyObject *DapChainIdObject_str(PyObject *self){
    char *l_str = dap_strdup_printf("0x%016"DAP_UINT64_FORMAT_x, ((PyDapChainIDObject*)self)->chain_id->uint64);
    PyObject *l_obj = Py_BuildValue("s", l_str);
    DAP_FREE(l_str);
    return l_obj;
}

/* Dap chain cell id */
PyTypeObject DapChainCellIdObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainCellID"  ,       /* tp_name */
        sizeof(PyDapChainCellIDObject),  /* tp_basicsize */
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
        PyDapChainCellIdObject_str,      /* tp_str */
        0,                               /* tp_getattro */
        0,                               /* tp_setattro */
        0,                               /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,         /* tp_flags */
        "Chain cell id object",          /* tp_doc */
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

PyObject *PyDapChainCellIdObject_str(PyObject *self){
    char *l_str = dap_strdup_printf("0x%016"DAP_UINT64_FORMAT_x, ((PyDapChainCellIDObject*)self)->cell_id.uint64);
    PyObject *l_obj = Py_BuildValue("s", l_str);
    DAP_FREE(l_str);
    return l_obj;
}

PyObject *dap_chain_hash_slow_to_str_py(PyObject *self, PyObject *args){
    PyObject *obj_hash_slow;
    char *str;
    size_t str_max;
    if (!PyArg_ParseTuple(args, "O|s|n", &obj_hash_slow, &str, &str_max))
            return NULL;
    size_t res = dap_chain_hash_slow_to_str(((PyDapChainHashSlowObject*)obj_hash_slow)->hash_slow, str, str_max);
    return Py_BuildValue("ns", res, str_max);
}

PyObject *dap_chain_addr_to_str_py(PyObject *self, PyObject *args){
    PyObject *obj_chain_addr;
    if (!PyArg_ParseTuple(args, "O", &obj_chain_addr))
        return NULL;
    const dap_chain_addr_t *addr = ((PyDapChainAddrObject*)obj_chain_addr)->addr;
    char *res = dap_chain_addr_to_str(addr);
    PyObject *l_obj_res =  Py_BuildValue("s", res);
    DAP_DELETE(res);
    return l_obj_res;
}

PyObject *dap_chain_addr_from_str_py(PyObject *self, PyObject *args){
    const char *str;
    if (!PyArg_ParseTuple(args, "s", &str))
        return NULL;
    dap_chain_addr_t *l_addr = dap_chain_addr_from_str(str);
    if (!l_addr){
        Py_RETURN_NONE;
    }
    PyDapChainAddrObject *obj = PyObject_New(PyDapChainAddrObject, &DapChainAddrObjectType);
    obj->addr = l_addr;
    return (PyObject*)obj;
}

PyObject *dap_chain_addr_fill_py(PyObject *self, PyObject *args){
    PyObject *obj_sign_type;
    PyObject *obj_pkey_hash;
    PyObject *obj_chain_net_id;
    if (!PyArg_ParseTuple(args, "OOO", &obj_sign_type, &obj_pkey_hash, &obj_chain_net_id)){
        PyErr_SetString(PyExc_AttributeError, "Function takes three arguments, signature type, public key hash, chain network ID.");
        return NULL;
    }
    if (self == NULL){
        PyDapChainAddrObject *obj_addr = PyObject_New(PyDapChainAddrObject, &DapChainAddrObjectType);
        obj_addr->addr = DAP_NEW(dap_chain_addr_t);
        dap_chain_addr_fill(
                obj_addr->addr,
                *((PyDapSignTypeObject*)obj_sign_type)->sign_type,
                ((PyDapHashFastObject*)obj_pkey_hash)->hash_fast,
                ((PyDapChainNetIdObject*)obj_chain_net_id)->net_id);
        return (PyObject*)obj_addr;
    }else{
        dap_chain_addr_fill(
                ((PyDapChainAddrObject*)self)->addr,
                *((PyDapSignTypeObject*)obj_sign_type)->sign_type,
                ((PyDapHashFastObject*)obj_pkey_hash)->hash_fast,
                ((PyDapChainNetIdObject*)obj_chain_net_id)->net_id);
        Py_RETURN_NONE;
    }
}

PyObject *dap_chain_addr_fill_from_key_py(PyObject *self, PyObject *args){
    (void)self;
    PyObject *key;
    PyObject *net_id;
    if (!PyArg_ParseTuple(args, "O|O", &key, &net_id))
        return NULL;
    PyDapChainAddrObject *obj_addr = PyObject_New(PyDapChainAddrObject, &DapChainAddrObjectType);
    obj_addr->addr = DAP_NEW(dap_chain_addr_t);
    dap_chain_addr_fill_from_key(obj_addr->addr, ((PyCryptoKeyObject*)key)->key, (((PyDapChainNetIdObject*)net_id)->net_id));
    return (PyObject*)obj_addr;
}

PyObject *dap_chain_addr_check_sum_py(PyObject *self, PyObject *args){
    int res = dap_chain_addr_check_sum(((PyDapChainAddrObject*)self)->addr);
    return PyLong_FromLong(res);
}

PyObject *obj_addr_str(PyObject *self){
    char *l_addr = dap_chain_addr_to_str(((PyDapChainAddrObject*)self)->addr);
    PyObject* l_obj_res = Py_BuildValue("s", l_addr);
    DAP_DELETE(l_addr);
    return l_obj_res;
}

PyObject* dap_chain_addr_get_net_id_py(PyObject *self, PyObject *args){
    (void)args;
    PyObject *obj_net_id = _PyObject_New(&DapChainNetIdObjectType);
    ((PyDapChainNetIdObject*)obj_net_id)->net_id = ((PyDapChainAddrObject*)self)->addr->net_id;
    return Py_BuildValue("O", obj_net_id);
}

PyObject *dap_chain_net_id_from_str_py(PyObject *self, PyObject *args){
    const char *str;
    if (!PyArg_ParseTuple(args, "s", &str))
        return NULL;
    PyObject *obj_net_id = _PyObject_New(&DapChainNetIdObjectType);
    if (dap_sscanf(str, "0x%016"DAP_UINT64_FORMAT_X, &((PyDapChainNetIdObject*)obj_net_id)->net_id.uint64) != 1) {
        char *l_err = dap_strdup_printf("Wrong id format (\"%s\"). Must be like \"0x0123456789ABCDE\"" , str);
        PyErr_SetString(PyExc_AttributeError, l_err);
        DAP_DELETE(l_err);
        return NULL;
    }
    return Py_BuildValue("O", obj_net_id);
}

/* Chain net srv price unit uid */
PyMethodDef PyDapChainNetSrvPriceUnitUID_Methods[] = {
        {"undefined", (PyCFunction)wrapping_dap_chain_net_srv_price_unit_uid_get_undefined, METH_NOARGS | METH_STATIC, ""},
        {"mb", (PyCFunction)wrapping_dap_chain_net_srv_price_unit_uid_get_mb, METH_NOARGS | METH_STATIC, ""},
        {"sec", (PyCFunction)wrapping_dap_chain_net_srv_price_unit_uid_get_sec, METH_NOARGS | METH_STATIC, ""},
        {"day", (PyCFunction)wrapping_dap_chain_net_srv_price_unit_uid_get_day, METH_NOARGS | METH_STATIC, ""},
        {"kb", (PyCFunction)wrapping_dap_chain_net_srv_price_unit_uid_get_kb, METH_NOARGS | METH_STATIC, ""},
        {"b", (PyCFunction)wrapping_dap_chain_net_srv_price_unit_uid_get_b, METH_NOARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainNetSrvPriceUnitUidObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainNetSrvPriceUnitUID",             /* tp_name */
        sizeof(PyDapChainNetSrvPriceUnitUIDObject),      /* tp_basicsize */
        0,                                               /* tp_itemsize */
        0,                                               /* tp_dealloc */
        0,                                               /* tp_print */
        0,                                               /* tp_getattr */
        0,                                               /* tp_setattr */
        0,                                               /* tp_reserved */
        0,                                               /* tp_repr */
        0,                                               /* tp_as_number */
        0,                                               /* tp_as_sequence */
        0,                                               /* tp_as_mapping */
        0,                                               /* tp_hash  */
        0,                                               /* tp_call */
        PyDapChainNetSrvPriceUnitUID_str,                /* tp_str */
        0,                                               /* tp_getattro */
        0,                                               /* tp_setattro */
        0,                                               /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                         /* tp_flags */
        "Chain net srv price unit uid object",           /* tp_doc */
        0,		                                         /* tp_traverse */
        0,		                                         /* tp_clear */
        0,		                                         /* tp_richcompare */
        0,                                               /* tp_weaklistoffset */
        0,		                                         /* tp_iter */
        0,		                                         /* tp_iternext */
        PyDapChainNetSrvPriceUnitUID_Methods,             /* tp_methods */
        0,                                               /* tp_members */
        0,                                               /* tp_getset */
        0,                                               /* tp_base */
        0,                                               /* tp_dict */
        0,                                               /* tp_descr_get */
        0,                                               /* tp_descr_set */
        0,                                               /* tp_dictoffset */
        0,                                               /* tp_init */
        0,                                               /* tp_alloc */
        PyType_GenericNew,                               /* tp_new */
};

int PyDapChainNetSrvUIDObject_init(PyObject *self, PyObject *args, PyObject *kwds){
    (void)kwds;
    uint64_t uid;
    if (!PyArg_ParseTuple(args, "k", &uid)){
        return -1;
    }
    PyDapChainNetSrvUIDObject *l_obj_srv_uid = (PyDapChainNetSrvUIDObject*)self;
    l_obj_srv_uid->net_srv_uid.uint64 = uid;
    return 0;
}

PyObject *PyDapChainNetSrvPriceUnitUID_str(PyObject *self){
    return Py_BuildValue("s", serv_unit_enum_to_str(&((PyDapChainNetSrvPriceUnitUIDObject*)self)->price_unit_uid.enm));
}

PyObject *wrapping_dap_chain_net_srv_price_unit_uid_get_undefined(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapChainNetSrvPriceUnitUIDObject *obj_srv_price_uid = PyObject_New(PyDapChainNetSrvPriceUnitUIDObject,
                                                                         &DapChainNetSrvPriceUnitUidObjectType);
    obj_srv_price_uid->price_unit_uid.uint32 = SERV_UNIT_UNDEFINED;
    obj_srv_price_uid->price_unit_uid.enm = SERV_UNIT_UNDEFINED;
    return (PyObject*)obj_srv_price_uid;
}
PyObject *wrapping_dap_chain_net_srv_price_unit_uid_get_mb(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapChainNetSrvPriceUnitUIDObject *obj_srv_price_uid = PyObject_New(PyDapChainNetSrvPriceUnitUIDObject,
                                                                         &DapChainNetSrvPriceUnitUidObjectType);
    obj_srv_price_uid->price_unit_uid.uint32 = SERV_UNIT_MB;
    obj_srv_price_uid->price_unit_uid.enm = SERV_UNIT_MB;
    return (PyObject*)obj_srv_price_uid;
}
PyObject *wrapping_dap_chain_net_srv_price_unit_uid_get_sec(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapChainNetSrvPriceUnitUIDObject *obj_srv_price_uid = PyObject_New(PyDapChainNetSrvPriceUnitUIDObject,
                                                                         &DapChainNetSrvPriceUnitUidObjectType);
    obj_srv_price_uid->price_unit_uid.uint32 = SERV_UNIT_SEC;
    obj_srv_price_uid->price_unit_uid.enm = SERV_UNIT_SEC;
    return (PyObject*)obj_srv_price_uid;
}
PyObject *wrapping_dap_chain_net_srv_price_unit_uid_get_day(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapChainNetSrvPriceUnitUIDObject *obj_srv_price_uid = PyObject_New(PyDapChainNetSrvPriceUnitUIDObject,
                                                                         &DapChainNetSrvPriceUnitUidObjectType);
    obj_srv_price_uid->price_unit_uid.uint32 = SERV_UNIT_DAY;
    obj_srv_price_uid->price_unit_uid.enm = SERV_UNIT_DAY;
    return (PyObject*)obj_srv_price_uid;
}
PyObject *wrapping_dap_chain_net_srv_price_unit_uid_get_kb(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapChainNetSrvPriceUnitUIDObject *obj_srv_price_uid = PyObject_New(PyDapChainNetSrvPriceUnitUIDObject,
                                                                         &DapChainNetSrvPriceUnitUidObjectType);
    obj_srv_price_uid->price_unit_uid.uint32 = SERV_UNIT_KB;
    obj_srv_price_uid->price_unit_uid.enm = SERV_UNIT_KB;
    return (PyObject*)obj_srv_price_uid;
}
PyObject *wrapping_dap_chain_net_srv_price_unit_uid_get_b(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapChainNetSrvPriceUnitUIDObject *obj_srv_price_uid = PyObject_New(PyDapChainNetSrvPriceUnitUIDObject,
                                                                         &DapChainNetSrvPriceUnitUidObjectType);
    obj_srv_price_uid->price_unit_uid.uint32 = SERV_UNIT_B;
    obj_srv_price_uid->price_unit_uid.enm = SERV_UNIT_B;
    return (PyObject*)obj_srv_price_uid;
}

/*  wrapping dap_chain_node_addr_t */
PyTypeObject DapChainNodeAddrObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainNodeAddr"  ,       /* tp_name */
        sizeof(PyDapChainCellIDObject),  /* tp_basicsize */
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
        PyDapChainNodeAddrObject_str,    /* tp_str */
        0,                               /* tp_getattro */
        0,                               /* tp_setattro */
        0,                               /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,         /* tp_flags */
        "Chain node addr object",          /* tp_doc */
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

PyObject* PyDapChainNodeAddrObject_str(PyObject* self){
    char *ret = dap_strdup_printf(NODE_ADDR_FP_STR, NODE_ADDR_FP_ARGS(((PyDapChainNodeAddrObject *) self)->node_addr));
    PyObject *l_obj = Py_BuildValue("s", ret);
    DAP_FREE(ret);
    return l_obj;
}

/* wrapping dap_chain_net_srv_uid_t */
PyObject* PyDapChainNetSrvUIDObject_str(PyObject *self){
    char *res = dap_strdup_printf("0x%016"DAP_UINT64_FORMAT_X, ((PyDapChainNetSrvUIDObject*)self)->net_srv_uid.uint64);
    PyObject *l_obj = Py_BuildValue("s", res);
    DAP_FREE(res);
    return l_obj;
}
