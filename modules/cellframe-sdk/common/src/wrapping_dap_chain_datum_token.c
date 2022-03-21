#include "wrapping_dap_chain_datum_token.h"

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
        case DAP_CHAIN_DATUM_TOKEN_TYPE_OLD_PRIVATE_UPDATE:
        case DAP_CHAIN_DATUM_TOKEN_TYPE_PRIVATE_UPDATE:
            return Py_BuildValue("s", "PRIVATE_UPDATE");
        case DAP_CHAIN_DATUM_TOKEN_TYPE_OLD_PRIVATE_DECL:
        case DAP_CHAIN_DATUM_TOKEN_TYPE_PRIVATE_DECL:
            return Py_BuildValue("s", "PRIVATE_DECL");
        case DAP_CHAIN_DATUM_TOKEN_TYPE_PUBLIC:
            return Py_BuildValue("s", "PUBLIC");
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
            obj = Py_BuildValue("H", l_token->header_private.signs_total);
            PyDict_SetItemString(obj_dict, "signs_total", obj);
            obj = Py_BuildValue("H", l_token->header_private.signs_valid);
            PyDict_SetItemString(obj_dict, "signs_valid", obj);
            if (dap_chain_datum_token_is_old(l_token->type))
                obj = Py_BuildValue("k", l_token->header_private.total_supply);
            else
                obj = Py_BuildValue("s", dap_chain_balance_print(l_token->header_private.total_supply_256));
            PyDict_SetItemString(obj_dict, "total_supply", obj);
            break;
//        case DAP_CHAIN_DATUM_TOKEN_TYPE_OLD_PRIVATE_UPDATE:
//        case DAP_CHAIN_DATUM_TOKEN_TYPE_PRIVATE_UPDATE:
////            dap_tsd_t *l_tsd = dap_chain_datum_token_tsd_get(l_token, ((PyDapChainDatumTokenObject*)self)->token_size);
//            break;
//        case DAP_CHAIN_DATUM_TOKEN_TYPE_OLD_PRIVATE_DECL:
//        case DAP_CHAIN_DATUM_TOKEN_TYPE_PRIVATE_DECL:
//            break;
        default:
            return Py_None;
    }
    return obj_dict;
}

/* Token Emission */
PyGetSetDef PyDapChainDatumTokenEmissionGetsSetsDef[]={
        {"version", (getter)wrapping_dap_chain_datum_token_emission_get_version, NULL, NULL},
        {"typeStr", (getter)wrapping_dap_chain_datum_token_emission_get_type_str, NULL, NULL},
        {"ticker", (getter)wrapping_dap_chain_datum_token_emission_get_ticker, NULL, NULL},
        {"addr", (getter)wrapping_dap_chain_datum_token_emission_get_addr, NULL, NULL},
        {"value", (getter)wrapping_dap_chain_datum_token_emission_get_value, NULL, NULL},
        {"data", (getter)wrapping_dap_chain_datum_token_emission_get_data, NULL, NULL},
        {NULL}
};

PyMethodDef PyDapChainDatumTokenEmissionMethods[]={
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainDatumTokenEmissionObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.Chain.DatumTokenEmission",             /* tp_name */
        sizeof(PyDapChainDatumTokenEmissionObject),      /* tp_basicsize */
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
        0,                                               /* tp_init */
        0,                                               /* tp_alloc */
        PyType_GenericNew,                               /* tp_new */
};

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
    PyObject_Dir((PyObject*)obj_addr);
    obj_addr->addr = &((PyDapChainDatumTokenEmissionObject*)self)->token_emission->hdr.address;
    return (PyObject*)obj_addr;
}
PyObject *wrapping_dap_chain_datum_token_emission_get_value(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", ((PyDapChainDatumTokenEmissionObject*)self)->token_emission->hdr.value);
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
            l_sign_ptr = (dap_sign_t*)token_emi->data.type_auth.signs;
            l_offset = (byte_t*)l_sign_ptr - (byte_t*)token_emi;
            obj_tmp = PyList_New(0);
            for (size_t i = 0; i < token_emi->data.type_auth.signs_count && l_offset < token_emi_size; i++){
                if(dap_sign_verify_size(l_sign_ptr, ((PyDapChainDatumTokenEmissionObject*)self)->token_size - l_offset)){
                    obj_tmp_sign = PyObject_New(PyDapSignObject, &DapCryptoSignObjectType);
                    PyObject_Dir((PyObject*)obj_tmp_sign);
                    obj_tmp_sign->sign = DAP_NEW_Z_SIZE(dap_sign_t, dap_sign_get_size(l_sign_ptr));
                    memcpy(obj_tmp_sign->sign, l_sign_ptr, dap_sign_get_size(l_sign_ptr));
                    if (PyList_Append(obj_tmp, (PyObject*)obj_tmp_sign) == -1){
                        return NULL;
                    }
                    l_offset += dap_sign_get_size(l_sign_ptr);
                    l_sign_ptr = (dap_sign_t*)(byte_t*)token_emi + l_offset;
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
            return Py_None;
            break;
        case DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_UNDEFINED:
            return Py_None;
            break;
        default:
            return Py_None;
    }
    return obj_dict;
}
