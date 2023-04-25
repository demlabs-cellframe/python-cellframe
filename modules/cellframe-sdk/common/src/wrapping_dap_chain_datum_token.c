#include "wrapping_dap_chain_datum_token.h"
#include "wrapping_cert.h"

/* Token */
PyGetSetDef  PyDapChainDatumTokenGetsSetsDef[]={
        {"ticker", (getter)wrapping_dap_chain_datum_token_get_ticker, NULL, NULL, NULL},
        {"typeStr", (getter)wrapping_dap_chain_datum_token_get_type_str, NULL, NULL, NULL},
        {"data", (getter)wrapping_dap_chain_datum_token_get_data, NULL, NULL, NULL},
        {NULL}
};

PyMethodDef  PyDapChainDatumTokenMethods[] = {
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainDatumTokenObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.Chain.DatumTokenObject",       /* tp_name */
        sizeof(PyDapChainDatumTokenObject),      /* tp_basicsize */
        0,                                       /* tp_itemsize */
        0,                                       /* tp_dealloc */
        0,                                       /* tp_print */
        0,                                       /* tp_getattr */
        0,                                       /* tp_setattr */
        0,                                       /* tp_reserved */
        0,                                       /* tp_repr */
        0,                                       /* tp_as_number */
        0,                                       /* tp_as_sequence */
        0,                                       /* tp_as_mapping */
        0,                                       /* tp_hash  */
        0,                                       /* tp_call */
        0,                                       /* tp_str */
        0,                                       /* tp_getattro */
        0,                                       /* tp_setattro */
        0,                                       /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                 /* tp_flags */
        "Chain datum token object",              /* tp_doc */
        0,		                                 /* tp_traverse */
        0,		                                 /* tp_clear */
        0,		                                 /* tp_richcompare */
        0,                                       /* tp_weaklistoffset */
        0,		                                 /* tp_iter */
        0,		                                 /* tp_iternext */
        PyDapChainDatumTokenMethods,              /* tp_methods */
        0,                                       /* tp_members */
        PyDapChainDatumTokenGetsSetsDef,        /* tp_getset */
        0,                                       /* tp_base */
        0,                                       /* tp_dict */
        0,                                       /* tp_descr_get */
        0,                                       /* tp_descr_set */
        0,                                       /* tp_dictoffset */
        0,                                       /* tp_init */
        0,                                       /* tp_alloc */
        PyType_GenericNew,                       /* tp_new */
};

PyObject *wrapping_dap_chain_datum_token_get_ticker(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("s", ((PyDapChainDatumTokenObject*)self)->token->ticker);
}
PyObject *wrapping_dap_chain_datum_token_get_type_str(PyObject *self, void *closure){
    (void)closure;
    switch (((PyDapChainDatumTokenObject*)self)->token->type) {
        case DAP_CHAIN_DATUM_TOKEN_TYPE_OLD_SIMPLE:
        case DAP_CHAIN_DATUM_TOKEN_TYPE_SIMPLE:
            return Py_BuildValue("s", "SIMPLE");
        case DAP_CHAIN_DATUM_TOKEN_TYPE_PRIVATE_UPDATE:
            return Py_BuildValue("s", "PRIVATE_UPDATE");
        case DAP_CHAIN_DATUM_TOKEN_TYPE_PRIVATE_DECL:
            return Py_BuildValue("s", "PRIVATE_DECL");
        case DAP_CHAIN_DATUM_TOKEN_TYPE_OLD_PUBLIC:
        case DAP_CHAIN_DATUM_TOKEN_TYPE_PUBLIC:
            return Py_BuildValue("s", "PUBLIC");
        case DAP_CHAIN_DATUM_TOKEN_TYPE_NATIVE_DECL:
            return Py_BuildValue("s", "CF20_DECL");
        case DAP_CHAIN_DATUM_TOKEN_TYPE_NATIVE_UPDATE:
            return Py_BuildValue("s", "CF20_UPDATE");
        default:
            return Py_BuildValue("s", "UNKNOWN");
    }
//    dap_chain_datum_token_flags_dump()
}
//PyObject *wrapping_dap_chain_datum_token_get_size(PyObject *self, void *closure){
//    (void)closure;
//    return Py_BuildValue("", ((PyDapChainDatumTokenObject*)self)->token->)
//}
PyObject *wrapping_dap_chain_datum_token_get_data(PyObject *self, void *closure){
    (void)closure;
    dap_chain_datum_token_t  *l_token = ((PyDapChainDatumTokenObject*)self)->token;
//    ((PyDapChainDatumTokenObject*)self)->token
    PyObject *obj_dict = PyDict_New();
    PyObject *obj_dict_header_private = NULL;
    PyObject *obj = Py_None;
    switch(l_token->type){
        case DAP_CHAIN_DATUM_TOKEN_TYPE_OLD_SIMPLE:
        case DAP_CHAIN_DATUM_TOKEN_TYPE_SIMPLE:
        case DAP_CHAIN_DATUM_TOKEN_TYPE_NATIVE_DECL:
            
            PyDict_SetItemString(obj_dict, "ticker", Py_BuildValue("s", l_token->ticker));
            PyDict_SetItemString(obj_dict, "signs_total", Py_BuildValue("H", l_token->signs_total));
            PyDict_SetItemString(obj_dict, "signs_valid", Py_BuildValue("H", l_token->signs_valid));
            PyDict_SetItemString(obj_dict, "total_supply", Py_BuildValue("s", dap_chain_balance_print(l_token->total_supply)));
            PyDict_SetItemString(obj_dict, "flags", Py_BuildValue("H", l_token->header_native_decl.flags));
            PyDict_SetItemString(obj_dict, "decimals", Py_BuildValue("H", l_token->header_native_decl.decimals));
            PyDict_SetItemString(obj_dict, "tsd_total_size", Py_BuildValue("k", l_token->header_native_decl.tsd_total_size));

            //TODO: token signatures!

            break;
        case DAP_CHAIN_DATUM_TOKEN_TYPE_NATIVE_UPDATE:
            PyDict_SetItemString(obj_dict, "tsd_total_size", Py_BuildValue("k", l_token->header_native_update.tsd_total_size));
            break;
//        case DAP_CHAIN_DATUM_TOKEN_TYPE_OLD_PRIVATE_UPDATE:
//        case DAP_CHAIN_DATUM_TOKEN_TYPE_PRIVATE_UPDATE:
////            dap_tsd_t *l_tsd = dap_chain_datum_token_tsd_get(l_token, ((PyDapChainDatumTokenObject*)self)->token_size);
//            break;
//        case DAP_CHAIN_DATUM_TOKEN_TYPE_OLD_PRIVATE_DECL:
//        case DAP_CHAIN_DATUM_TOKEN_TYPE_PRIVATE_DECL:
//            break;
        default:
            Py_RETURN_NONE;
    }
    return obj_dict;
}

bool DapChainDatumToken_Check(PyObject *self){
    return PyObject_TypeCheck(self, &DapChainDatumTokenObjectType);
}

/* Token Emission */
PyGetSetDef PyDapChainDatumTokenEmissionGetsSetsDef[]={
        {"hash", (getter)wrapping_dap_chain_datum_token_emission_get_hash, NULL, NULL, NULL},
        {"version", (getter)wrapping_dap_chain_datum_token_emission_get_version, NULL, NULL, NULL},
        {"typeStr", (getter)wrapping_dap_chain_datum_token_emission_get_type_str, NULL, NULL, NULL},
        {"ticker", (getter)wrapping_dap_chain_datum_token_emission_get_ticker, NULL, NULL, NULL},
        {"addr", (getter)wrapping_dap_chain_datum_token_emission_get_addr, NULL, NULL, NULL},
        {"value", (getter)wrapping_dap_chain_datum_token_emission_get_value, NULL, NULL, NULL},
        {"data", (getter)wrapping_dap_chain_datum_token_emission_get_data, NULL, NULL, NULL},
        {"signCount", (getter)wrapping_dap_chain_datum_token_emission_get_sign_count, NULL, NULL, NULL},
        {"signs", (getter)wrapping_dap_chain_datum_token_emission_get_signs, NULL, NULL, NULL},
        {NULL}
};

PyMethodDef PyDapChainDatumTokenEmissionMethods[]={
        {"addSign", (PyCFunction)wrapping_dap_chain_datum_emission_add_sign, METH_VARARGS, ""},
        {"appendSign", (PyCFunction)wrapping_dap_chain_datum_emission_append_sign, METH_VARARGS, ""},
        {"addTSD", (PyCFunction)wrapping_dap_chain_datum_emission_add_tsd, METH_VARARGS, ""},
        {"getTSD", (PyCFunction)wrapping_dap_chain_datum_emission_get_tsd, METH_VARARGS, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainDatumTokenEmissionObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.Chain.DatumTokenEmission",             /* tp_name */
        sizeof(PyDapChainDatumTokenEmissionObject),      /* tp_basicsize */
        0,                                               /* tp_itemsize */
PyDapChainDatumTokenEmissionObject_dealloc,                                                 /* tp_dealloc */
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
        0,                                               /* tp_str */
        0,                                               /* tp_getattro */
        0,                                               /* tp_setattro */
        0,                                               /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                         /* tp_flags */
        "Chain datum token emission object",             /* tp_doc */
        0,		                                         /* tp_traverse */
        0,		                                         /* tp_clear */
        0,		                                         /* tp_richcompare */
        0,                                               /* tp_weaklistoffset */
        0,		                                         /* tp_iter */
        0,		                                         /* tp_iternext */
        PyDapChainDatumTokenEmissionMethods,  	     /* tp_methods */
        0,                                               /* tp_members */
        PyDapChainDatumTokenEmissionGetsSetsDef,        /* tp_getset */
        0,                                               /* tp_base */
        0,                                               /* tp_dict */
        0,                                               /* tp_descr_get */
        0,                                               /* tp_descr_set */
        0,                                               /* tp_dictoffset */
        (initproc)PyDapChainDatumTokenEmissionObject_init,   /* tp_init */
        0,                                               /* tp_alloc */
        PyType_GenericNew,                               /* tp_new */
};

void PyDapChainDatumTokenEmissionObject_dealloc(PyObject *self) {
    if (((PyDapChainDatumTokenEmissionObject*)self)->copy) {
        DAP_DELETE(((PyDapChainDatumTokenEmissionObject *) self)->token_emission);
        ((PyDapChainDatumTokenEmissionObject *) self)->token_size = 0;
    }
    PyTypeObject *tp = Py_TYPE(self);
    // free references and buffers here
    tp->tp_free(self);
}

int PyDapChainDatumTokenEmissionObject_init(PyDapChainDatumTokenEmissionObject *self, PyObject *argv, PyObject *kwds){
    const char *kwlist[] = {
            "value",
            "ticker",
            "addr",
            NULL
    };
    char* l_value_datoshi;
    char *l_ticker;
    PyDapChainAddrObject *obj_addr;
    if (!PyArg_ParseTupleAndKeywords(argv, kwds, "ssO", (char**)kwlist, &l_value_datoshi, &l_ticker, &obj_addr)){
        return -1;
    }
    if (!PyDapChainAddrObject_Check(obj_addr)){
        PyErr_SetString(PyExc_AttributeError, "The third argument to this function is invalid. "
                                              "It must be an instance of the Cellframe.Chain.ChainAddr object type.");
        return -1;
    }
    uint256_t l_value = dap_chain_balance_scan(l_value_datoshi);
    self->token_emission = dap_chain_datum_emission_create(l_value, l_ticker, obj_addr->addr);
    self->token_size = dap_chain_datum_emission_get_size((uint8_t*)self->token_emission);
    self->copy = false;
    return 0;
}

PyObject *wrapping_dap_chain_datum_token_emission_get_hash(PyObject *self, void *closure){
    (void)closure;
    if (!self)
        return NULL;
    PyDapHashFastObject *obj_hf = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    obj_hf->hash_fast = DAP_NEW(dap_chain_hash_fast_t);
    dap_hash_fast(
            ((PyDapChainDatumTokenEmissionObject*)self)->token_emission,
            ((PyDapChainDatumTokenEmissionObject*)self)->token_size, obj_hf->hash_fast);
    obj_hf->origin = true;
    return (PyObject*)obj_hf;
}

PyObject *wrapping_dap_chain_datum_token_emission_get_version(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("I", ((PyDapChainDatumTokenEmissionObject*)self)->token_emission->hdr.version);
}
PyObject *wrapping_dap_chain_datum_token_emission_get_type_str(PyObject *self, void *closure){
    (void)closure;
    const char *str;
    switch (((PyDapChainDatumTokenEmissionObject*)self)->token_emission->hdr.type) {
        case DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_UNDEFINED:
            str = "TOKEN_EMISSION_TYPE_UNDEFINED";
            break;
        case DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_AUTH:
            str = "TOKEN_EMISSION_TYPE_AUTH";
            break;
        case DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_ALGO:
            str = "TOKEN_EMISSION_TYPE_ALGO";
            break;
        case DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_ATOM_OWNER:
            str = "TOKEN_EMISSION_TYPE_ATOM_OWNER";
            break;
        case DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_SMART_CONTRACT:
            str = "TOKEN_EMISSION_TYPE_SMART_CONTRACT";
            break;
        default:
            str = "UNDEFINED";
    }
    return Py_BuildValue("s", str);
//    const char *str = "UNDEFINED";
//#define DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_UNDEFINED         0x00
//#define DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_AUTH              0x01
//#define DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_ALGO              0x02
//#define DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_ATOM_OWNER        0x03
//#define DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_SMART_CONTRACT    0x04
//    const char *str = c_dap_chain_datum_token_emission_type_str(((PyDapChainDatumTokenEmissionObject*)self)->token_emission->hdr.type);
//    return Py_BuildValue("s", str);
}
PyObject *wrapping_dap_chain_datum_token_emission_get_ticker(PyObject *self, void *closure){
    (void)closure;
//    log_it(L_NOTICE, "ticker in token: %s", ((PyDapChainDatumTokenEmissionObject*)self)->token_emission->hdr.ticker);
    return Py_BuildValue("s", ((PyDapChainDatumTokenEmissionObject*)self)->token_emission->hdr.ticker);
}
PyObject *wrapping_dap_chain_datum_token_emission_get_addr(PyObject *self, void *closure){
    (void)closure;
    PyDapChainAddrObject *obj_addr = PyObject_New(PyDapChainAddrObject, &DapChainAddrObjectType);
    obj_addr->addr = &((PyDapChainDatumTokenEmissionObject*)self)->token_emission->hdr.address;
    return (PyObject*)obj_addr;
}
PyObject *wrapping_dap_chain_datum_token_emission_get_value(PyObject *self, void *closure){
    (void)closure;
    DapMathObject *l_math = PyObject_New(DapMathObject, &DapMathObjectType);
    l_math->value = ((PyDapChainDatumTokenEmissionObject*)self)->token_emission->hdr.value_256;
    return (PyObject*)l_math;
}
PyObject *wrapping_dap_chain_datum_token_emission_get_data(PyObject *self, void *closure){
    (void)closure;
    PyObject *obj_dict = NULL;
    PyObject *obj_tmp = Py_None;
    dap_chain_datum_token_emission_t *token_emi = ((PyDapChainDatumTokenEmissionObject*)self)->token_emission;
    size_t token_emi_size = ((PyDapChainDatumTokenEmissionObject*)self)->token_size;
    PyDapSignObject *obj_tmp_sign = (PyDapSignObject *)Py_None;
    dap_sign_t *l_sign_ptr = NULL;
    size_t l_offset = 0;
    switch(token_emi->hdr.type){
        case DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_AUTH:
            l_sign_ptr = (dap_sign_t*)(token_emi->tsd_n_signs + token_emi->data.type_auth.tsd_total_size);
            l_offset = (byte_t*)l_sign_ptr - (byte_t*)token_emi;
            obj_tmp = PyList_New(0);
            for (size_t i = 0; i < token_emi->data.type_auth.signs_count && l_offset < token_emi_size; i++){
                if(dap_sign_verify_size(l_sign_ptr, ((PyDapChainDatumTokenEmissionObject*)self)->token_size - l_offset)){
                    obj_tmp_sign = PyObject_New(PyDapSignObject, &DapCryptoSignObjectType);
                    obj_tmp_sign->sign = DAP_NEW_Z_SIZE(dap_sign_t, dap_sign_get_size(l_sign_ptr));
                    memcpy(obj_tmp_sign->sign, l_sign_ptr, dap_sign_get_size(l_sign_ptr));
                    if (PyList_Append(obj_tmp, (PyObject*)obj_tmp_sign) == -1){
                        return NULL;
                    }
                    l_offset += dap_sign_get_size(l_sign_ptr);
                    l_sign_ptr = (dap_sign_t*)((byte_t*)token_emi + l_offset);
                } else {
                    break;
                }
            }
            return obj_tmp;
        case DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_ALGO:
            obj_dict = PyDict_New();
            obj_tmp = Py_BuildValue("s", token_emi->data.type_algo.codename);
            PyDict_SetItemString(obj_dict, "codename", obj_tmp);
            break;
        case DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_ATOM_OWNER:
            obj_dict = PyDict_New();
            obj_tmp = Py_BuildValue("k", token_emi->data.type_atom_owner.value_start);
            PyDict_SetItemString(obj_dict, "value_start", obj_tmp);
            obj_tmp = Py_BuildValue("s", token_emi->data.type_atom_owner.value_change_algo_codename);
            PyDict_SetItemString(obj_dict, "value_change_algo_codename", obj_tmp);
            break;
        case DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_SMART_CONTRACT:
//            obj_dict = PyDict_New();
            Py_RETURN_NONE;
            break;
        case DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_UNDEFINED:
            Py_RETURN_NONE;
            break;
        default:
            Py_RETURN_NONE;
    }
    return obj_dict;
}

PyObject *wrapping_dap_chain_datum_token_emission_get_sign_count(PyObject *self, void *closure){
    (void)closure;
    dap_chain_datum_token_emission_t *l_emi = ((PyDapChainDatumTokenEmissionObject*)self)->token_emission;
    uint16_t l_sign_count = 0;
    if (l_emi->hdr.type == DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_AUTH)
        l_sign_count = l_emi->data.type_auth.signs_count;
    return Py_BuildValue("H", l_sign_count);
}

PyObject *wrapping_dap_chain_datum_token_emission_get_signs(PyObject *self, void *closure) {
    (void)closure;
    dap_chain_datum_token_emission_t *l_emi = ((PyDapChainDatumTokenEmissionObject*)self)->token_emission;
    if (l_emi->hdr.type != DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_AUTH) {
        PyErr_SetString(PyExc_AttributeError, "Wrong datum type");
        return NULL;
     }
    if (!l_emi->data.type_auth.signs_count || l_emi->data.type_auth.size <= l_emi->data.type_auth.tsd_total_size) {
        PyErr_SetString(PyExc_AttributeError, "No signes found");
        return NULL;
    }
    dap_sign_t *l_sign = (dap_sign_t*)(l_emi->tsd_n_signs + l_emi->data.type_auth.tsd_total_size);
    PyObject *obj_list = PyList_New(0);
    PyDapSignObject *obj_sign = (PyDapSignObject*)Py_None;
    size_t l_count, l_sign_size;
    for (l_count = 0, l_sign_size = 0; l_count < l_emi->data.type_auth.signs_count && (l_sign_size = dap_sign_get_size(l_sign)); ++l_count) {
        if (!dap_sign_verify_size(l_sign, l_sign_size)) {
            break;
        }
        obj_sign = PyObject_New(PyDapSignObject, &DapCryptoSignObjectType);
        obj_sign->sign = DAP_NEW_Z_SIZE(dap_sign_t, l_sign_size);
        memcpy(obj_sign->sign, l_sign, l_sign_size);
        if (PyList_Append(obj_list, (PyObject*)obj_sign) == -1) {
            return NULL;
        }
        l_sign = (dap_sign_t *)((byte_t *)l_sign + l_sign_size);
    }
    return obj_list;
}

PyObject *wrapping_dap_chain_datum_emission_add_sign(PyObject *self, PyObject *args){
    PyCryptoCertObject *pyCert;
    if (!PyArg_ParseTuple(args, "O", &pyCert)) {
        return  NULL;
    }
    if (!PyObject_TypeCheck(pyCert, &DapCryptoCertObjectType)) {
        PyErr_SetString(PyExc_AttributeError, "An incorrect argument was passed to the function, it must"
                                              " be an instance of the DAP.Crypto.Key object.");
        return NULL;
    }
    ((PyDapChainDatumTokenEmissionObject*)self)->token_emission = dap_chain_datum_emission_add_sign(
            pyCert->cert->enc_key,
            ((PyDapChainDatumTokenEmissionObject*)self)->token_emission);
    ((PyDapChainDatumTokenEmissionObject*)self)->token_size = dap_chain_datum_emission_get_size(
            (byte_t*)((PyDapChainDatumTokenEmissionObject*)self)->token_emission
            );
    Py_RETURN_NONE;
}

PyObject *wrapping_dap_chain_datum_emission_append_sign(PyObject *self, PyObject *args){
    
    (void)self;
    PyObject *obj_data;
    
    if (!PyArg_ParseTuple(args, "O", &obj_data)){
        return NULL;
    }
    if (!PyBytes_Check(obj_data)){
        PyErr_SetString(PyExc_AttributeError, "The first argument was not passed correctly, the first argument "
                                              "must be an object of type Bytes.");
        return NULL;
    }
    size_t l_size = PyBytes_Size(obj_data);
    void *l_buff = PyBytes_AsString(obj_data);
    dap_sign_t *l_sign = (dap_sign_t*)l_buff;
    
    ((PyDapChainDatumTokenEmissionObject*)self)->token_emission = dap_chain_datum_emission_append_sign(
            l_sign,
            ((PyDapChainDatumTokenEmissionObject*)self)->token_emission);
            
    ((PyDapChainDatumTokenEmissionObject*)self)->token_size = dap_chain_datum_emission_get_size(
            (byte_t*)((PyDapChainDatumTokenEmissionObject*)self)->token_emission
            );
    Py_RETURN_TRUE;
}

PyObject *wrapping_dap_chain_datum_emission_add_tsd(PyObject*self, PyObject *args){
    int l_type;
    PyObject *obj_data;
    if (!PyArg_ParseTuple(args, "iO", &l_type, &obj_data)){
        return NULL;
    }
    if (!PyBytes_Check(obj_data)){
        PyErr_SetString(PyExc_AttributeError, "The second argument was not correctly passed to this "
                                              "function, the second argument must be an object of the Bytes type.");
        return NULL;
    }
    void *l_data = PyBytes_AsString(obj_data);
    size_t l_data_size = PyBytes_Size(obj_data);
    ((PyDapChainDatumTokenEmissionObject*)self)->token_emission = dap_chain_datum_emission_add_tsd(
            ((PyDapChainDatumTokenEmissionObject*)self)->token_emission,
            l_type, l_data_size, l_data);
    ((PyDapChainDatumTokenEmissionObject*)self)->token_size = dap_chain_datum_emission_get_size(
            (uint8_t*)((PyDapChainDatumTokenEmissionObject*)self)->token_emission);
    Py_RETURN_NONE;
}

PyObject *wrapping_dap_chain_datum_emission_get_tsd(PyObject*self, PyObject *args)
{
    int l_type;
    if (!PyArg_ParseTuple(args, "i", &l_type))
        return NULL;
    dap_chain_datum_token_emission_t *l_ems = ((PyDapChainDatumTokenEmissionObject *)self)->token_emission;
    if (!l_ems)
        return NULL;
    size_t l_data_size = 0;
    byte_t *l_data = dap_chain_emission_get_tsd(l_ems, l_type, &l_data_size);
    if (!l_data || !l_data_size)
        Py_RETURN_NONE;
    return PyBytes_FromStringAndSize((char *)l_data, (Py_ssize_t)l_data_size);
}
