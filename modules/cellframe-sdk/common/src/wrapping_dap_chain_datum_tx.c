#include "wrapping_dap_chain_datum_tx.h"

/* DAP chain tx iter type */

PyMethodDef PyDapChainTxItemTypeObjectMethods[] ={
        {"TX_ITEM_TYPE_IN", TX_ITEM_TYPE_IN_PY, METH_NOARGS | METH_STATIC, ""},
        {"TX_ITEM_TYPE_OUT", TX_ITEM_TYPE_OUT_PY, METH_NOARGS | METH_STATIC, ""},
        {"TX_ITEM_TYPE_PKEY", TX_ITEM_TYPE_PKEY_PY, METH_NOARGS | METH_STATIC, ""},
        {"TX_ITEM_TYPE_SIG", TX_ITEM_TYPE_SIG_PY, METH_NOARGS | METH_STATIC, ""},
        {"TX_ITEM_TYPE_TOKEN", TX_ITEM_TYPE_TOKEN_PY, METH_NOARGS | METH_STATIC, ""},
        {"TX_ITEM_TYPE_IN_COND", TX_ITEM_TYPE_IN_COND_PY, METH_NOARGS | METH_STATIC, ""},
        {"TX_ITEM_TYPE_OUT_COND", TX_ITEM_TYPE_OUT_COND_PY, METH_NOARGS | METH_STATIC, ""},
        {"TX_ITEM_TYPE_RECEIPT", TX_ITEM_TYPE_RECEIPT_PY, METH_NOARGS | METH_STATIC, ""},
        {"TX_ITEM_TYPE_TSD", TX_ITEM_TYPE_TSD_PY, METH_NOARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainTxItemTypeObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.Chain.TxItemType",        /* tp_name */
        sizeof(PyDapChainTxItemTypeObject), /* tp_basicsize */
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
        "Chain tx item type object",        /* tp_doc */
        0,		                            /* tp_traverse */
        0,		                            /* tp_clear */
        0,		                            /* tp_richcompare */
        0,                                  /* tp_weaklistoffset */
        0,		                            /* tp_iter */
        0,		                            /* tp_iternext */
        PyDapChainTxItemTypeObjectMethods,  /* tp_methods */
        0,                                  /* tp_members */
        0,                                  /* tp_getset */
        0,                                  /* tp_base */
        0,                                  /* tp_dict */
        0,                                  /* tp_descr_get */
        0,                                  /* tp_descr_set */
        0,                                  /* tp_dictoffset */
        0,                                  /* tp_init */
        0,                                  /* tp_alloc */
        PyType_GenericNew,                  /* tp_new */
};

PyObject *TX_ITEM_TYPE_IN_PY(PyObject *self, PyObject *args){
    return PyLong_FromLong(TX_ITEM_TYPE_IN);
}
PyObject *TX_ITEM_TYPE_OUT_PY(PyObject *self, PyObject *args){
        return PyLong_FromLong(TX_ITEM_TYPE_OUT);
}
PyObject *TX_ITEM_TYPE_PKEY_PY(PyObject *self, PyObject *args){
        return PyLong_FromLong(TX_ITEM_TYPE_PKEY);
}
PyObject *TX_ITEM_TYPE_SIG_PY(PyObject *self, PyObject *args){
        return PyLong_FromLong(TX_ITEM_TYPE_SIG);
}
PyObject *TX_ITEM_TYPE_TOKEN_PY(PyObject *self, PyObject *args){
        return PyLong_FromLong(TX_ITEM_TYPE_TOKEN);
}
PyObject *TX_ITEM_TYPE_IN_COND_PY(PyObject *self, PyObject *args){
        return PyLong_FromLong(TX_ITEM_TYPE_IN_COND);
}
PyObject *TX_ITEM_TYPE_OUT_COND_PY(PyObject *self, PyObject *args){
        return PyLong_FromLong(TX_ITEM_TYPE_OUT_COND);
}
PyObject *TX_ITEM_TYPE_RECEIPT_PY(PyObject *self, PyObject *args){
        return PyLong_FromLong(TX_ITEM_TYPE_RECEIPT);
}
PyObject *TX_ITEM_TYPE_TSD_PY(PyObject *self, PyObject *args){
        return PyLong_FromLong(TX_ITEM_TYPE_TSD);
}
/* -------------------------------------- */

/* DAP chain tx cond type */
PyMethodDef DapChainTxCondTypeMethods[] = {
        {"COND_SERVICE_PROVIDE", COND_SERVICE_PROVIDE_PY, METH_NOARGS | METH_STATIC, ""},
        {"COND_SERVICE_BILL", COND_SERVICE_BILL_PY, METH_NOARGS | METH_STATIC, ""},
        {NULL, NULL,0, NULL}
};

PyTypeObject DapChainTxCondTypeObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.Chain.TxCondType",       /* tp_name */
        sizeof(PyDapChainTxCondTypeObject),/* tp_basicsize */
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
        "Chain tx cond type object",             /* tp_doc */
        0,		                         /* tp_traverse */
        0,		                         /* tp_clear */
        0,		                         /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        0,		                         /* tp_iter */
        0,		                         /* tp_iternext */
        DapChainTxCondTypeMethods,       /* tp_methods */
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

PyObject *COND_SERVICE_PROVIDE_PY(){
    PyObject *obj = _PyObject_New(&DapChainTxCondTypeObjectType);
    ((PyDapChainTxCondTypeObject*)obj)->tx_cond_type_t = COND_SERVICE_PROVIDE;
    return Py_BuildValue("O", obj);
}
PyObject *COND_SERVICE_BILL_PY(){
    PyObject *obj = _PyObject_New(&DapChainTxCondTypeObjectType);
    ((PyDapChainTxCondTypeObject*)obj)->tx_cond_type_t = COND_SERVICE_BILL;
    return Py_BuildValue("O", obj);
}
/* -------------------------------------- */

/* DAP chain datum tx */
PyGetSetDef PyDaoChainDatumTxObjectGetsSets[] = {
        {"hash", (getter) wrapping_dap_chain_datum_tx_get_hash, NULL, NULL, NULL},
        {"dateCreated", (getter) wrapping_dap_chain_datum_tx_get_tsCreated, NULL, NULL, NULL},
        {NULL}
};

PyMethodDef PyDapChainDatumTxObjectMethods[] ={
        {"getSize", (PyCFunction)dap_chain_datum_tx_get_size_py, METH_VARARGS, ""},
        {"addItem", (PyCFunction)dap_chain_datum_tx_add_item_py, METH_VARARGS, ""},
        {"addInItem", (PyCFunction)dap_chain_datum_tx_add_in_item_py, METH_VARARGS, ""},
        {"addInCondItem", (PyCFunction)dap_chain_datum_tx_add_in_cond_item_py, METH_VARARGS, ""},
        {"addOutItem", (PyCFunction)dap_chain_datum_tx_add_out_item_py, METH_VARARGS, ""},
        {"addOutCond", (PyCFunction)dap_chain_datum_tx_add_out_cond_item_py, METH_VARARGS, ""},
        {"addSignItem", (PyCFunction)dap_chain_datum_tx_add_sign_item_py, METH_VARARGS, ""},
        {"verifySign", (PyCFunction)dap_chain_datum_tx_verify_sign_py, METH_VARARGS, ""},
        {"getItems", (PyCFunction)wrapping_dap_chain_datum_tx_get_items, METH_NOARGS, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainDatumTxObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.Chain.DatumTx",                      /* tp_name */
        sizeof(PyDapChainDatumTxObject),               /* tp_basicsize */
        0,                                             /* tp_itemsize */
        0,//(destructor)PyDapChainDatumTxObject_delete,    /* tp_dealloc */
        0,                                              /* tp_print */
        0,                                              /* tp_getattr */
        0,                                              /* tp_setattr */
        0,                                              /* tp_reserved */
        0,                                              /* tp_repr */
        0,                                              /* tp_as_number */
        0,                                              /* tp_as_sequence */
        0,                                              /* tp_as_mapping */
        0,                                              /* tp_hash  */
        0,                                              /* tp_call */
        0,                                              /* tp_str */
        0,                                              /* tp_getattro */
        0,                                              /* tp_setattro */
        0,                                              /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                        /* tp_flags */
        "Chain datum tx object",                        /* tp_doc */
        0,		                                        /* tp_traverse */
        0,		                                        /* tp_clear */
        0,		                                        /* tp_richcompare */
        0,                                              /* tp_weaklistoffset */
        0,		                                        /* tp_iter */
        0,		                                        /* tp_iternext */
        PyDapChainDatumTxObjectMethods,                 /* tp_methods */
        0,                                              /* tp_members */
        PyDaoChainDatumTxObjectGetsSets,                /* tp_getset */
        0,                                              /* tp_base */
        0,                                              /* tp_dict */
        0,                                              /* tp_descr_get */
        0,                                              /* tp_descr_set */
        0,                                              /* tp_dictoffset */
        0,                                              /* tp_init */
        0,                                              /* tp_alloc */
        PyDapChainDatumTxObject_create,                 /* tp_new */
};

bool DapChainDatumTx_Check(PyObject *self){
    return PyObject_TypeCheck(self, &DapChainDatumTxObjectType);
}

PyObject *PyDapChainDatumTxObject_create(PyTypeObject *type_object, PyObject *args, PyObject *kwds){
    PyDapChainDatumTxObject *obj = (PyDapChainDatumTxObject*)PyType_GenericNew(type_object, args, kwds);
    obj->datum_tx = dap_chain_datum_tx_create();
    return (PyObject *)obj;
}
void PyDapChainDatumTxObject_delete(PyDapChainDatumTxObject* datumTx){
    if (!datumTx->original) {
        dap_chain_datum_tx_delete(datumTx->datum_tx);
    }
    Py_TYPE(datumTx)->tp_free((PyObject*)datumTx);
}

PyObject *dap_chain_datum_tx_get_size_py(PyObject *self, PyObject *args){
    (void)args;
    size_t size = dap_chain_datum_tx_get_size(((PyDapChainDatumTxObject*)self)->datum_tx);
    return PyLong_FromSize_t(size);
}
PyObject *dap_chain_datum_tx_add_item_py(PyObject *self, PyObject *args){
    uint8_t *a_item;
    if (!PyArg_ParseTuple(args, "b", &a_item))
        return NULL;
    int res = dap_chain_datum_tx_add_item(&(((PyDapChainDatumTxObject*)self)->datum_tx), a_item);
    return PyLong_FromLong(res);
}
PyObject *dap_chain_datum_tx_add_in_item_py(PyObject *self, PyObject *args){
    PyObject *in_obj_hash_fast;
    uint32_t in_tx_out_pref_idx;
    if (!PyArg_ParseTuple(args, "O|I", &in_obj_hash_fast, &in_tx_out_pref_idx))
        return NULL;
    int res = dap_chain_datum_tx_add_in_item(&(((PyDapChainDatumTxObject*)self)->datum_tx),
                                             ((PyDapHashFastObject*)in_obj_hash_fast)->hash_fast,
                                             in_tx_out_pref_idx);
    return PyLong_FromLong(res);
}

PyObject *dap_chain_datum_tx_add_in_cond_item_py(PyObject *self, PyObject *args){
    PyObject *in_chain_hash_fast;
    unsigned int in_tx_out_prev_idx;
    unsigned int in_receipt_idx;
    if (!PyArg_ParseTuple(args, "O|I|I", &in_chain_hash_fast, &in_tx_out_prev_idx, &in_receipt_idx))
        return NULL;
    int res = dap_chain_datum_tx_add_in_cond_item(&(((PyDapChainDatumTxObject*)self)->datum_tx),
                                                  ((PyDapHashFastObject*)in_chain_hash_fast)->hash_fast,
                                                  in_tx_out_prev_idx,
                                                  in_receipt_idx);
    return PyLong_FromLong(res);
}

PyObject *dap_chain_datum_tx_add_out_item_py(PyObject *self, PyObject *args){
    PyObject *in_addr;
    uint256_t value;
    if (!PyArg_ParseTuple(args, "O|k", &in_addr, &value))
        return NULL;
    int res = dap_chain_datum_tx_add_out_item(&(((PyDapChainDatumTxObject*)self)->datum_tx),
                                              ((PyDapChainAddrObject*)in_addr)->addr,
                                              value);
    return PyLong_FromLong(res);
}
PyObject *dap_chain_datum_tx_add_out_cond_item_py(PyObject *self, PyObject *args){
    PyObject *obj_key;
    PyObject *obj_srv_uid;
    uint256_t value;
    uint256_t value_max_per_unit;
    PyObject *obj_srv_price_unit_uid;
    PyObject *obj_cond_bytes;
    Py_ssize_t cond_size;
    if (!PyArg_ParseTuple(args, "O|O|k|k|O|O|n", &obj_key, &obj_srv_uid, &value, &value_max_per_unit,
                          &obj_srv_price_unit_uid, &obj_cond_bytes, &cond_size))
        return NULL;
    void *cond = (void*)PyBytes_AsString(obj_cond_bytes);
    int res = dap_chain_datum_tx_add_out_cond_item(&(((PyDapChainDatumTxObject*)self)->datum_tx),
                                                   ((PyDapPkeyObject*)obj_key)->pkey,
                                                   ((PyDapChainNetSrvUIDObject*)obj_srv_uid)->net_srv_uid,
                                                   value, value_max_per_unit,
                                                   ((PyDapChainNetSrvPriceUnitUIDObject*)obj_srv_price_unit_uid)->price_unit_uid,
                                                   cond, (size_t)cond_size);
    return PyLong_FromLong(res);
}
PyObject *dap_chain_datum_tx_add_sign_item_py(PyObject *self, PyObject *args){
    PyObject *obj_key;
    if (!PyArg_ParseTuple(args, "O", &obj_key))
        return NULL;
    int res = dap_chain_datum_tx_add_sign_item(&(((PyDapChainDatumTxObject*)self)->datum_tx),
                                               ((PyCryptoKeyObject*)obj_key)->key);
    return PyLong_FromLong(res);
}

PyObject *dap_chain_datum_tx_verify_sign_py(PyObject *self, PyObject *args){
    (void)args;
    int res = dap_chain_datum_tx_verify_sign(((PyDapChainDatumTxObject*)self)->datum_tx);
    return PyLong_FromLong(res);
}

PyObject *wrapping_dap_chain_datum_tx_get_hash(PyObject *self, void* closure){
    (void)closure;
    PyDapHashFastObject *obj_hash_fast = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    obj_hash_fast->hash_fast = DAP_NEW(dap_chain_hash_fast_t);
    dap_hash_fast(((PyDapChainDatumTxObject*)self)->datum_tx,
                  dap_chain_datum_tx_get_size(((PyDapChainDatumTxObject*)self)->datum_tx),
                  obj_hash_fast->hash_fast);
    obj_hash_fast->origin = true;
    return  (PyObject*)obj_hash_fast;
}
PyObject *wrapping_dap_chain_datum_tx_get_tsCreated(PyObject *self, void* closure){
    (void)closure;
    PyObject *obj_ts_float = PyLong_FromLong(((PyDapChainDatumTxObject*)self)->datum_tx->header.ts_created);
    PyObject *obj_ts = Py_BuildValue("(O)", obj_ts_float);
    PyDateTime_IMPORT;
    PyObject *obj_dt = PyDateTime_FromTimestamp(obj_ts);
    return obj_dt;
}

PyObject *wrapping_dap_chain_datum_tx_get_items(PyObject *self, PyObject *args){
    (void)args;
    uint32_t l_tx_items_count = 0;
    uint32_t l_tx_items_size = ((PyDapChainDatumTxObject*)self)->datum_tx->header.tx_items_size;
    PyObject *obj_list = PyList_New(0);
    while(l_tx_items_count < l_tx_items_size){
        uint8_t *item = ((PyDapChainDatumTxObject*)self)->datum_tx->tx_items + l_tx_items_count;
        size_t l_tx_item_size = dap_chain_datum_item_tx_get_size(item);
        if (l_tx_item_size == 0){
            return NULL;
        }
        PyObject *obj_tx_item = NULL;
        switch (dap_chain_datum_tx_item_get_type(item)) {
            case TX_ITEM_TYPE_IN:
                obj_tx_item = (PyObject*)PyObject_New(PyDapChainTXInObject, &DapChainTxInObjectType);
                ((PyDapChainTXInObject*)obj_tx_item)->tx_in = ((dap_chain_tx_in_t*)item);
                break;
            case TX_ITEM_TYPE_OUT:
                obj_tx_item = (PyObject*)PyObject_New(PyDapChainTXOutObject, &DapChainTxOutObjectType);
                ((PyDapChainTXOutObject*)obj_tx_item)->tx_out = ((dap_chain_tx_out_t*)item);
                break;
            case TX_ITEM_TYPE_TOKEN:
                obj_tx_item = (PyObject*)PyObject_New(PyDapChainTxTokenObject, &DapChainTxTokenObjectType);
                ((PyDapChainTxTokenObject*)obj_tx_item)->token = (dap_chain_tx_token_t*)item;
                break;
            case TX_ITEM_TYPE_TOKEN_EXT:
                obj_tx_item = (PyObject*)PyObject_New(PyDapChainTxTokenExtObject, &DapChainTxTokenExtType);
                ((PyDapChainTxTokenExtObject*)obj_tx_item)->token_ext = (dap_chain_tx_token_ext_t*)item;
                break;
            case TX_ITEM_TYPE_SIG:
                obj_tx_item = (PyObject*)PyObject_New(PyDapChainTXSigObject, &DapChainTxSigObjectType);
                ((PyDapChainTXSigObject*)obj_tx_item)->tx_sig = (dap_chain_tx_sig_t*)item;
                break;
            case TX_ITEM_TYPE_RECEIPT:
                obj_tx_item = (PyObject*)PyObject_New(PyDapChainTXReceiptObject, &DapChainTxReceiptObjectType);
                ((PyDapChainTXReceiptObject*)obj_tx_item)->tx_receipt = (dap_chain_datum_tx_receipt_t*)item;
                break;
            case TX_ITEM_TYPE_PKEY:
                obj_tx_item = (PyObject*)PyObject_New(PyDapChainTXPkeyObject, &DapChainTxPkeyObjectType);
                ((PyDapChainTXPkeyObject*)obj_tx_item)->tx_pkey = ((dap_chain_tx_pkey_t*)item);
                break;
            case TX_ITEM_TYPE_IN_COND:
                obj_tx_item = (PyObject*)PyObject_New(PyDapChainTXInCondObject, &DapChainTxInCondObjectType);
                ((PyDapChainTXInCondObject*)obj_tx_item)->tx_in_cond = (dap_chain_tx_in_cond_t*)item;
                break;
            case TX_ITEM_TYPE_OUT_COND:
                switch (((dap_chain_tx_out_cond_t*)item)->header.subtype) {
                    case DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_PAY:
                        obj_tx_item = (PyObject*)PyObject_New(PyDapChainTxOutCondObject, &DapChainTxOutCondSubTypeSrvPayObjectType);
                        ((PyDapChainTxOutCondObject*)obj_tx_item)->out_cond = ((dap_chain_tx_out_cond_t*)item);
                        break;
                    case DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_LOCK:
                        obj_tx_item = (PyObject*)PyObject_New(PyDapChainTxOutCondObject, &DapChainTxOutCondSubTypeSrvStakeLockObjectType);
                        ((PyDapChainTxOutCondObject*)obj_tx_item)->out_cond = ((dap_chain_tx_out_cond_t*)item);
                        break;
                    case DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_POS_DELEGATE:
                        obj_tx_item = (PyObject*)PyObject_New(PyDapChainTxOutCondObject, &DapChainTxOutCondSubTypeSrvStakePosDelegateObjectType);
                        ((PyDapChainTxOutCondObject*)obj_tx_item)->out_cond = ((dap_chain_tx_out_cond_t*)item);
                        break;
                    case DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_XCHANGE:
                        obj_tx_item = (PyObject*)PyObject_New(PyDapChainTxOutCondObject, &DapChainTxOutCondSubTypeSrvXchangeObjectType);
                        ((PyDapChainTxOutCondObject*)obj_tx_item)->out_cond = ((dap_chain_tx_out_cond_t*)item);
                        break;
                    default:
                        obj_tx_item = (PyObject*)PyObject_New(PyDapChainTxOutCondObject, &DapChainTxOutCondObjectType);
                        ((PyDapChainTxOutCondObject*)obj_tx_item)->out_cond = ((dap_chain_tx_out_cond_t*)item);
                }
                break;
            case TX_ITEM_TYPE_OUT_EXT:
                obj_tx_item = (PyObject*)PyObject_New(PyDapChainTXOutExtObject, &DapChainTxOutExtObjectType);
                ((PyDapChainTXOutExtObject*)obj_tx_item)->out_ext = (dap_chain_tx_out_ext_t*)item;
                break;
            case TX_ITEM_TYPE_TSD:
                obj_tx_item = (PyObject*)PyObject_New(PyDapChainTxTSDObject, &DapChainTxTSDObjectType);
                ((PyDapChainTxTSDObject*)obj_tx_item)->tsd = (dap_chain_tx_tsd_t*)item;
                break;
            default:
                obj_tx_item = Py_None;
                break;
        }
        PyList_Append(obj_list, obj_tx_item);
        Py_XDECREF(obj_tx_item);
        l_tx_items_count += l_tx_item_size;
    }
    return obj_list;
}

/* -------------------------------------- */
