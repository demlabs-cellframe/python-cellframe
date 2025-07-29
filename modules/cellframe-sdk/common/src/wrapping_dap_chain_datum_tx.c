#include "wrapping_dap_chain_datum_tx.h"
#include "dap_chain_datum_tx_sig.h"

#include "dap_chain_wallet_python.h"

#define LOG_TAG "wrapping_datum_tx"

/* DAP chain tx iter type */

static PyMethodDef PyDapChainTxItemTypeObjectMethods[] ={
        {"TX_ITEM_TYPE_IN", (PyCFunction)TX_ITEM_TYPE_IN_PY, METH_NOARGS | METH_STATIC, ""},
        {"TX_ITEM_TYPE_OUT", (PyCFunction)TX_ITEM_TYPE_OUT_PY, METH_NOARGS | METH_STATIC, ""},
        {"TX_ITEM_TYPE_PKEY", (PyCFunction)TX_ITEM_TYPE_PKEY_PY, METH_NOARGS | METH_STATIC, ""},
        {"TX_ITEM_TYPE_SIG", (PyCFunction)TX_ITEM_TYPE_SIG_PY, METH_NOARGS | METH_STATIC, ""},
        {"TX_ITEM_TYPE_IN_EMS", (PyCFunction)TX_ITEM_TYPE_IN_EMS_PY, METH_NOARGS | METH_STATIC, ""},
        {"TX_ITEM_TYPE_IN_COND", (PyCFunction)TX_ITEM_TYPE_IN_COND_PY, METH_NOARGS | METH_STATIC, ""},
        {"TX_ITEM_TYPE_OUT_COND", (PyCFunction)TX_ITEM_TYPE_OUT_COND_PY, METH_NOARGS | METH_STATIC, ""},
        {"TX_ITEM_TYPE_RECEIPT", (PyCFunction)TX_ITEM_TYPE_RECEIPT_PY, METH_NOARGS | METH_STATIC, ""},
        {"TX_ITEM_TYPE_RECEIPT_OLD", (PyCFunction)TX_ITEM_TYPE_RECEIPT_OLD_PY, METH_NOARGS | METH_STATIC, ""},
        {"TX_ITEM_TYPE_TSD", (PyCFunction)TX_ITEM_TYPE_TSD_PY, METH_NOARGS | METH_STATIC, ""},
        {}
};

PyTypeObject DapChainTxItemTypeObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.Chain.TxItemType", sizeof(PyDapChainTxItemTypeObject),
        "Chain tx item type object",
        .tp_methods = PyDapChainTxItemTypeObjectMethods);

PyObject *TX_ITEM_TYPE_IN_PY(__attribute__((unused)) PyObject *self, __attribute__((unused)) PyObject *args)
{
    return PyLong_FromLong(TX_ITEM_TYPE_IN);
}
PyObject *TX_ITEM_TYPE_OUT_PY(__attribute__((unused)) PyObject *self, __attribute__((unused)) PyObject *args)
{
        return PyLong_FromLong(TX_ITEM_TYPE_OUT);
}
PyObject *TX_ITEM_TYPE_PKEY_PY(__attribute__((unused)) PyObject *self, __attribute__((unused)) PyObject *args)
{
        return PyLong_FromLong(TX_ITEM_TYPE_PKEY);
}
PyObject *TX_ITEM_TYPE_SIG_PY(__attribute__((unused)) PyObject *self, __attribute__((unused)) PyObject *args)
{
        return PyLong_FromLong(TX_ITEM_TYPE_SIG);
}
PyObject *TX_ITEM_TYPE_IN_EMS_PY(__attribute__((unused)) PyObject *self, __attribute__((unused)) PyObject *args)
{
        return PyLong_FromLong(TX_ITEM_TYPE_IN_EMS);
}
PyObject *TX_ITEM_TYPE_IN_COND_PY(__attribute__((unused)) PyObject *self, __attribute__((unused)) PyObject *args)
{
        return PyLong_FromLong(TX_ITEM_TYPE_IN_COND);
}
PyObject *TX_ITEM_TYPE_OUT_COND_PY(__attribute__((unused)) PyObject *self, __attribute__((unused)) PyObject *args)
{
        return PyLong_FromLong(TX_ITEM_TYPE_OUT_COND);
}
PyObject *TX_ITEM_TYPE_RECEIPT_PY(__attribute__((unused)) PyObject *self, __attribute__((unused)) PyObject *args)
{
        return PyLong_FromLong(TX_ITEM_TYPE_RECEIPT);
}
PyObject *TX_ITEM_TYPE_RECEIPT_OLD_PY(__attribute__((unused)) PyObject *self, __attribute__((unused)) PyObject *args)
{
        return PyLong_FromLong(TX_ITEM_TYPE_RECEIPT);
}
PyObject *TX_ITEM_TYPE_TSD_PY(__attribute__((unused)) PyObject *self, __attribute__((unused)) PyObject *args)
{
    return PyLong_FromLong(TX_ITEM_TYPE_TSD);
}

/* -------------------------------------- */

/* DAP chain datum tx */
static PyGetSetDef PyDaoChainDatumTxObjectGetsSets[] = {
        {"hash", (getter) wrapping_dap_chain_datum_tx_get_hash, NULL, NULL, NULL},
        {"dateCreated", (getter) wrapping_dap_chain_datum_tx_get_tsCreated, NULL, NULL, NULL},
        {}
};

static PyMethodDef PyDapChainDatumTxObjectMethods[] ={
        {"getSize", (PyCFunction)dap_chain_datum_tx_get_size_py, METH_VARARGS, ""},
        {"addItem", (PyCFunction)dap_chain_datum_tx_add_item_py, METH_VARARGS, ""},
        {"addInItem", (PyCFunction)dap_chain_datum_tx_add_in_item_py, METH_VARARGS, ""},
        {"addInCondItem", (PyCFunction)dap_chain_datum_tx_add_in_cond_item_py, METH_VARARGS, ""},
        {"addOutItem", (PyCFunction)dap_chain_datum_tx_add_out_item_py, METH_VARARGS, ""},
        {"addOutCond", (PyCFunction)dap_chain_datum_tx_add_out_cond_item_py, METH_VARARGS, ""},
        {"addOutStdItem", (PyCFunction)dap_chain_datum_tx_add_out_std_item_py, METH_VARARGS, ""},
        {"addSignItem", (PyCFunction)dap_chain_datum_tx_add_sign_item_py, METH_VARARGS, ""},
        {"addFeeItem", (PyCFunction)dap_chain_datum_tx_add_fee_item_py, METH_VARARGS, ""},
        {"appendSignItem", (PyCFunction)dap_chain_datum_tx_append_sign_item_py, METH_VARARGS, ""},
        {"verifySign", (PyCFunction)dap_chain_datum_tx_verify_sign_py, METH_VARARGS, ""},
        {"addTSDItem", (PyCFunction)dap_chain_datum_tx_add_tsd_item_py, METH_VARARGS, ""},
        {"getItems", (PyCFunction)wrapping_dap_chain_datum_tx_get_items, METH_NOARGS, ""},
        {"getServiceTags", (PyCFunction)wrapping_dap_chain_datum_tx_get_service_tags, METH_VARARGS, ""},
        {}
};

PyTypeObject DapChainDatumTxObjectType = {
        .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "CellFrame.Common.DatumTx",
        .tp_basicsize = sizeof(PyDapChainDatumTxObject),
        .tp_doc = "Chain datum tx object",
        .tp_methods = PyDapChainDatumTxObjectMethods,
        .tp_getset = PyDaoChainDatumTxObjectGetsSets,
        .tp_new = PyDapChainDatumTxObject_create,
        .tp_dealloc = (destructor) PyDapChainDatumTxObject_delete
};

bool DapChainDatumTx_Check(PyObject *self){
    return PyObject_TypeCheck(self, &DapChainDatumTxObjectType);
}

PyObject *PyDapChainDatumTxObject_create(PyTypeObject *type_object, PyObject *args, PyObject *kwds){
    PyDapChainDatumTxObject *obj = (PyDapChainDatumTxObject*)PyType_GenericNew(type_object, args, kwds);
    obj->datum_tx = dap_chain_datum_tx_create();
    obj->original = true;
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

PyObject *dap_chain_datum_tx_sign_py(PyObject *self, PyObject *args){
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O", &obj))
        return NULL;
    dap_enc_key_t *l_enc_key = NULL;
    int res = 0;
    if (PyCryptoKeyObject_check(obj)) {
        res = dap_chain_datum_tx_add_sign_item(&((PyDapChainDatumTxObject*)self)->datum_tx, ((PyCryptoKeyObject*)obj)->key);
    } else if (PyDapChainWalletObject_Check(obj)) {
        dap_enc_key_t *l_key = dap_chain_wallet_get_key(((PyDapChainWalletObject*)obj)->wallet, 0);
        res = dap_chain_datum_tx_add_sign_item(&((PyDapChainDatumTxObject*)self)->datum_tx, l_key);
    } else if (PyDapCryptoCertObject_Check(obj)) {
        res = dap_chain_datum_tx_add_sign_item(&((PyDapChainDatumTxObject*)self)->datum_tx, ((PyCryptoCertObject*)obj)->cert->enc_key);
    } else {
        PyErr_SetString(PyExc_Exception, "An invalid object type was passed. The sign function accepts an "
                                         "instance with DAP.Crypto.Key or CellFrame.Chain.Wallet.");
        return NULL;
    }
    if (res == 1)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
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


PyObject *dap_chain_datum_tx_add_sign_item_py(PyObject *self, PyObject *args){
    PyObject *obj_key;
    if (!PyArg_ParseTuple(args, "O", &obj_key))
        return NULL;
    int res = dap_chain_datum_tx_add_sign_item(&(((PyDapChainDatumTxObject*)self)->datum_tx),
                                               ((PyCryptoKeyObject*)obj_key)->key);
    return PyLong_FromLong(res);
}

PyObject *dap_chain_datum_tx_append_sign_item_py(PyObject *self, PyObject *args){
    PyDapSignObject *obj_sign;
    if (!PyArg_ParseTuple(args, "O", &obj_sign))
        return NULL;
    
    dap_chain_tx_sig_t *l_sign = dap_chain_tx_sig_create(obj_sign->sign);
    if (!l_sign) Py_RETURN_FALSE;

    if(dap_chain_datum_tx_add_item(&((PyDapChainDatumTxObject*)self)->datum_tx, l_sign))
        Py_RETURN_TRUE;
    
    Py_RETURN_FALSE;
}


PyObject *dap_chain_datum_tx_verify_sign_py(PyObject *self, PyObject *args){
    (void)args;
    int res = dap_chain_datum_tx_verify_sign(((PyDapChainDatumTxObject*)self)->datum_tx, 0);
    return PyLong_FromLong(res);
}

PyObject *wrapping_dap_chain_datum_tx_get_hash(PyObject *self, void* closure){
    (void)closure;
    PyDapHashFastObject *obj_hash_fast = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    if (!obj_hash_fast) {
        log_it(L_CRITICAL, "Failed to create hash fast object");
        return NULL;
    }
    
    obj_hash_fast->hash_fast = DAP_NEW(dap_chain_hash_fast_t);
    if (!obj_hash_fast->hash_fast) {
        log_it(L_CRITICAL, "Memory allocation error for hash fast");
        Py_DECREF(obj_hash_fast);
        return NULL;
    }
    
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

PyObject *wrapping_dap_chain_datum_tx_get_items(PyObject *self, PyObject *args) {
    (void)args;
    uint32_t l_tx_items_count = 0;
    uint32_t l_tx_items_size = ((PyDapChainDatumTxObject*)self)->datum_tx->header.tx_items_size;
    PyObject *obj_list = PyList_New(0);
    if (!obj_list) {
        log_it(L_CRITICAL, "Failed to create items list");
        return NULL;
    }
    
    uint64_t l_out_idx = 0;
    dap_hash_fast_t l_tx_hf;
    dap_hash_fast(((PyDapChainDatumTxObject*)self)->datum_tx,
                  dap_chain_datum_tx_get_size(((PyDapChainDatumTxObject*)self)->datum_tx),
                  &l_tx_hf);
    
    while(l_tx_items_count < l_tx_items_size) {
        uint8_t *item = ((PyDapChainDatumTxObject*)self)->datum_tx->tx_items + l_tx_items_count;
        size_t l_tx_item_size = dap_chain_datum_item_tx_get_size(item, 0);
        if (l_tx_item_size == 0) {
            Py_DECREF(obj_list);       
            PyObject *empty_list = PyList_New(0);
            return empty_list;
        }
        PyObject *obj_tx_item = NULL;
        switch (*item) {
            case TX_ITEM_TYPE_IN:
                obj_tx_item = (PyObject*)PyObject_New(PyDapChainTXInObject, &DapChainTxInObjectType);
                if (!obj_tx_item) {
                    log_it(L_CRITICAL, "Failed to create TX IN object");
                    Py_DECREF(obj_list);
                    return NULL;
                }
                ((PyDapChainTXInObject*)obj_tx_item)->tx_in = ((dap_chain_tx_in_t*)item);
                break;
            case TX_ITEM_TYPE_OUT:
                obj_tx_item = (PyObject*)PyObject_New(PyDapChainTXOutObject, &DapChainTxOutObjectType);
                if (!obj_tx_item) {
                    log_it(L_CRITICAL, "Failed to create TX OUT object");
                    Py_DECREF(obj_list);
                    return NULL;
                }
                ((PyDapChainTXOutObject*)obj_tx_item)->tx_hash = DAP_NEW(dap_hash_fast_t);
                if (!((PyDapChainTXOutObject*)obj_tx_item)->tx_hash) {
                    log_it(L_CRITICAL, "Memory allocation error for TX OUT hash");
                    Py_DECREF(obj_tx_item);
                    Py_DECREF(obj_list);
                    return NULL;
                }
                ((PyDapChainTXOutObject*)obj_tx_item)->tx_out = ((dap_chain_tx_out_t*)item);
                memcpy(((PyDapChainTXOutObject*)obj_tx_item)->tx_hash, &l_tx_hf, sizeof(dap_hash_fast_t));
                ((PyDapChainTXOutObject*)obj_tx_item)->idx = l_out_idx;
                l_out_idx++;
                break;
            case TX_ITEM_TYPE_IN_EMS:
                obj_tx_item = (PyObject*)PyObject_New(PyDapChainTxTokenObject, &DapChainTxTokenObjectType);
                if (!obj_tx_item) {
                    log_it(L_CRITICAL, "Failed to create TX token object");
                    Py_DECREF(obj_list);
                    return NULL;
                }
                ((PyDapChainTxTokenObject*)obj_tx_item)->token = (dap_chain_tx_in_ems_t*)item;
                break;
            case TX_ITEM_TYPE_SIG:
                obj_tx_item = (PyObject*)PyObject_New(PyDapChainTXSigObject, &DapChainTxSigObjectType);
                if (!obj_tx_item) {
                    log_it(L_CRITICAL, "Failed to create TX signature object");
                    Py_DECREF(obj_list);
                    return NULL;
                }
                ((PyDapChainTXSigObject*)obj_tx_item)->tx_sig = (dap_chain_tx_sig_t*)item;
                break;
            case TX_ITEM_TYPE_RECEIPT:
                obj_tx_item = (PyObject*)PyObject_New(PyDapChainTXReceiptObject, &DapChainTxReceiptObjectType);
                if (!obj_tx_item) {
                    log_it(L_CRITICAL, "Failed to create TX receipt object");
                    Py_DECREF(obj_list);
                    return NULL;
                }
                ((PyDapChainTXReceiptObject*)obj_tx_item)->tx_receipt = (dap_chain_datum_tx_receipt_t*)item;
                break;
            case TX_ITEM_TYPE_RECEIPT_OLD:
                obj_tx_item = (PyObject*)PyObject_New(PyDapChainTXReceiptOldObject, &DapChainTxReceiptOldObjectType);
                ((PyDapChainTXReceiptOldObject*)obj_tx_item)->tx_receipt = (dap_chain_datum_tx_receipt_old_t*)item;
                break;
            case TX_ITEM_TYPE_PKEY:
                obj_tx_item = (PyObject*)PyObject_New(PyDapChainTXPkeyObject, &DapChainTxPkeyObjectType);
                if (!obj_tx_item) {
                    log_it(L_CRITICAL, "Failed to create TX pkey object");
                    Py_DECREF(obj_list);
                    return NULL;
                }
                ((PyDapChainTXPkeyObject*)obj_tx_item)->tx_pkey = ((dap_chain_tx_pkey_t*)item);
                break;
            case TX_ITEM_TYPE_IN_COND:
                obj_tx_item = (PyObject*)PyObject_New(PyDapChainTXInCondObject, &DapChainTxInCondObjectType);
                if (!obj_tx_item) {
                    log_it(L_CRITICAL, "Failed to create TX in cond object");
                    Py_DECREF(obj_list);
                    return NULL;
                }
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
                }
                if (!obj_tx_item) {
                    log_it(L_CRITICAL, "Failed to create TX out cond object");
                    Py_DECREF(obj_list);
                    return NULL;
                }
                dap_hash_fast_t *l_tx_hash_out = DAP_NEW(dap_hash_fast_t);
                if (!l_tx_hash_out) {
                    log_it(L_CRITICAL, "Memory allocation error for TX out cond hash");
                    Py_DECREF(obj_tx_item);
                    Py_DECREF(obj_list);
                    return NULL;
                }
                ((PyDapChainTxOutCondObject*)obj_tx_item)->out_cond = ((dap_chain_tx_out_cond_t*)item);
                memcpy(l_tx_hash_out, &l_tx_hf, sizeof(dap_hash_fast_t));
                ((PyDapChainTxOutCondObject*)obj_tx_item)->tx_hash = l_tx_hash_out;
                ((PyDapChainTxOutCondObject*)obj_tx_item)->idx = l_out_idx;
                l_out_idx++;
                break;
            case TX_ITEM_TYPE_OUT_EXT:
                obj_tx_item = (PyObject*)PyObject_New(PyDapChainTXOutExtObject, &DapChainTxOutExtObjectType);
                if (!obj_tx_item) {
                    log_it(L_CRITICAL, "Failed to create TX out ext object");
                    Py_DECREF(obj_list);
                    return NULL;
                }
                ((PyDapChainTXOutExtObject*)obj_tx_item)->out_ext = (dap_chain_tx_out_ext_t*)item;
                ((PyDapChainTXOutExtObject*)obj_tx_item)->tx_hash = l_tx_hf;
                ((PyDapChainTXOutExtObject*)obj_tx_item)->idx = l_out_idx;
                l_out_idx++;
                break;
            case TX_ITEM_TYPE_OUT_STD: {
                PyDapChainTXOutStdObject *obj_out = PyObject_New(PyDapChainTXOutStdObject, &DapChainTxOutStdObjectType);
                if (!obj_out) {
                    log_it(L_CRITICAL, "Failed to create TX out std object");
                    Py_DECREF(obj_list);
                    return NULL;
                }
                obj_out->out = (dap_chain_tx_out_std_t *)item;
                obj_out->tx_hash = l_tx_hf;
                obj_out->idx = l_out_idx++;
                obj_tx_item = (PyObject *)obj_out;
            } break;
            case TX_ITEM_TYPE_TSD:
                obj_tx_item = (PyObject*)PyObject_New(PyDapChainTxTSDObject, &DapChainTxTSDObjectType);
                if (!obj_tx_item) {
                    log_it(L_CRITICAL, "Failed to create TX TSD object");
                    Py_DECREF(obj_list);
                    return NULL;
                }
                ((PyDapChainTxTSDObject*)obj_tx_item)->tsd = (dap_chain_tx_tsd_t*)item;
                break;
            case TX_ITEM_TYPE_VOTE:
                obj_tx_item = (PyObject*) PyObject_New(PyDapChainTXVoteObject, &PyDapChainTXVoteObjectType);
                if (!obj_tx_item) {
                    log_it(L_CRITICAL, "Failed to create TX vote object");
                    Py_DECREF(obj_list);
                    return NULL;
                }
                ((PyDapChainTXVoteObject*)obj_tx_item)->vote = (dap_chain_tx_vote_t*)item;
                break;
            case TX_ITEM_TYPE_VOTING:
                obj_tx_item = (PyObject*)PyObject_New(PyDapChainTXVotingObject, &PyDapChainTxVotingObjectType);
                if (!obj_tx_item) {
                    log_it(L_CRITICAL, "Failed to create TX voting object");
                    Py_DECREF(obj_list);
                    return NULL;
                }
                ((PyDapChainTXVotingObject*)obj_tx_item)->voting = dap_chain_datum_tx_voting_parse_tsd(((PyDapChainDatumTxObject*)self)->datum_tx);
                if (!((PyDapChainTXVotingObject*)obj_tx_item)->voting) {
                    log_it(L_CRITICAL, "Failed to parse voting TSD");
                    Py_DECREF(obj_tx_item);
                    Py_DECREF(obj_list);
                    return NULL;
                }
                break;
            case TX_ITEM_TYPE_EVENT:
                obj_tx_item = (PyObject *)PyObject_New(PyDapChainTxEventObject, &DapChainTxEventObjectType);
                if (!obj_tx_item) {
                    log_it(L_CRITICAL, "Failed to create TX EVENT object");
                    Py_DECREF(obj_list);
                    return NULL;
                }
                ((PyDapChainTxEventObject *)obj_tx_item)->tx_event = (dap_chain_tx_item_event_t *)item;
                break;
            default:
                obj_tx_item = Py_None;
                break;
        }
        if (PyList_Append(obj_list, obj_tx_item) < 0) {
            log_it(L_CRITICAL, "Failed to append item to list");
            if (obj_tx_item != Py_None)
                Py_DECREF(obj_tx_item);
            Py_DECREF(obj_list);
            return NULL;
        }
        if (obj_tx_item != Py_None)
            Py_DECREF(obj_tx_item);
        l_tx_items_count += l_tx_item_size;
    }
    return obj_list;
}

PyObject *wrapping_dap_chain_datum_tx_get_service_tags(PyObject *self, PyObject *args) {
    PyObject *obj_ledger;
    if (!PyArg_ParseTuple(args, "O", &obj_ledger))
        return NULL;
     
    if (!self || !DapChainDatumTx_Check(self)) {
        log_it(L_ERROR, "Invalid transaction object");
        Py_RETURN_NONE;
    }
    
    if (!obj_ledger) {
        log_it(L_ERROR, "Ledger parameter is required");
        Py_RETURN_NONE;
    }
    
    PyDapChainDatumTxObject *l_tx_obj = (PyDapChainDatumTxObject*)self;
    if (!l_tx_obj->datum_tx) {
        log_it(L_ERROR, "Transaction object has no datum_tx");
        Py_RETURN_NONE;
    }
    
    PyDapChainLedgerObject *l_ledger_obj = (PyDapChainLedgerObject*)obj_ledger;
    if (!l_ledger_obj->ledger) {
        log_it(L_ERROR, "Ledger object has no ledger");
        Py_RETURN_NONE;
    }
    
    // Variables to store service tag information
    char *l_service_name = NULL;
    dap_chain_srv_uid_t l_service_uid = { .uint64 = 0 };
    dap_chain_tx_tag_action_type_t l_action = DAP_CHAIN_TX_TAG_ACTION_UNKNOWN;
    
    // Create Python dictionary to return
    PyObject *l_result_dict = PyDict_New();
    if (!l_result_dict) {
        log_it(L_CRITICAL, "Failed to create result dictionary");
        return NULL;
    }
    
    // Try to deduce transaction tags from the transaction with the provided ledger
    bool l_tag_found = dap_ledger_deduct_tx_tag(l_ledger_obj->ledger, l_tx_obj->datum_tx, &l_service_name, &l_service_uid, &l_action);
    
    // Convert action to string
    const char *l_action_str = dap_ledger_tx_action_str(l_action);
    
    // Set action in dictionary
    PyObject *l_action_py = PyUnicode_FromString(l_action_str ? l_action_str : "unknown");
    if (!l_action_py) {
        log_it(L_CRITICAL, "Failed to create action string");
        Py_DECREF(l_result_dict);
        return NULL;
    }
    if (PyDict_SetItemString(l_result_dict, "action", l_action_py) < 0) {
        log_it(L_CRITICAL, "Failed to set action in dictionary");
        Py_DECREF(l_action_py);
        Py_DECREF(l_result_dict);
        return NULL;
    }
    Py_DECREF(l_action_py);
    
    // Set service_name in dictionary
    PyObject *l_service_name_py = PyUnicode_FromString(l_service_name ? l_service_name : "unknown");
    if (!l_service_name_py) {
        log_it(L_CRITICAL, "Failed to create service name string");
        Py_DECREF(l_result_dict);
        return NULL;
    }
    if (PyDict_SetItemString(l_result_dict, "service_name", l_service_name_py) < 0) {
        log_it(L_CRITICAL, "Failed to set service_name in dictionary");
        Py_DECREF(l_service_name_py);
        Py_DECREF(l_result_dict);
        return NULL;
    }
    Py_DECREF(l_service_name_py);
    
    // Set service_id in dictionary (as integer)
    PyObject *l_service_id_py = PyLong_FromUnsignedLongLong(l_service_uid.uint64);
    if (!l_service_id_py) {
        log_it(L_CRITICAL, "Failed to create service ID");
        Py_DECREF(l_result_dict);
        return NULL;
    }
    if (PyDict_SetItemString(l_result_dict, "service_id", l_service_id_py) < 0) {
        log_it(L_CRITICAL, "Failed to set service_id in dictionary");
        Py_DECREF(l_service_id_py);
        Py_DECREF(l_result_dict);
        return NULL;
    }
    Py_DECREF(l_service_id_py);
    
    return l_result_dict;
}

PyObject *dap_chain_datum_tx_add_tsd_item_py(PyObject *self, PyObject *args)
{
    int tsd_type;
    Py_buffer tsd_value;

    if (!PyArg_ParseTuple(args, "iy*", &tsd_type, &tsd_value))
        return NULL;

    dap_chain_datum_tx_t *tx =
        ((PyDapChainDatumTxObject *)self)->datum_tx;

    uint32_t offset = 0;
    uint32_t total  = tx->header.tx_items_size;

    while (offset < total) {
        uint8_t *item = tx->tx_items + offset;

        if (*item == TX_ITEM_TYPE_SIG) {
            PyBuffer_Release(&tsd_value);
            PyErr_SetString(PyExc_RuntimeError,
                            "TSD items cannot be added after signatures");
            return NULL;
        }
        size_t item_sz = dap_chain_datum_item_tx_get_size(item, 0);
        if (item_sz == 0) {
            PyBuffer_Release(&tsd_value);
            PyErr_SetString(PyExc_RuntimeError,
                            "corrupted transaction items");
            return NULL;
        }
        offset += item_sz;
    }

    dap_chain_tx_tsd_t *item =
        dap_chain_datum_tx_item_tsd_create(tsd_value.buf,
                                           tsd_type,
                                           tsd_value.len);
    if (!item) {
        PyBuffer_Release(&tsd_value);
        return PyErr_Format(PyExc_RuntimeError,
                            "dap_chain_datum_tx_item_tsd_create() failed");
    }

    int rc = dap_chain_datum_tx_add_item(
                 &(((PyDapChainDatumTxObject *)self)->datum_tx),
                 (uint8_t *)item);

    if (rc != 1)
        DAP_DELETE(item);

    PyBuffer_Release(&tsd_value);
    return PyLong_FromLong(rc);
}



PyObject *dap_chain_datum_tx_add_out_std_item_py(PyObject *self, PyObject *args)
{
    PyObject   *py_addr  = NULL;
    PyObject   *py_value = NULL;
    const char *token    = NULL;

    if (!PyArg_ParseTuple(args, "OO|s", &py_addr, &py_value, &token))
        return NULL;

    uint256_t value = {0};

    if (PyLong_Check(py_value)) {
        PyObject *s = PyObject_Str(py_value);
        if (!s) return NULL;
        value = dap_chain_balance_scan(PyUnicode_AsUTF8(s));
        Py_DECREF(s);
    } else if (PyUnicode_Check(py_value))
        value = dap_chain_balance_scan(PyUnicode_AsUTF8(py_value));
    else if (PyBytes_Check(py_value))
        value = dap_chain_balance_scan(PyBytes_AsString(py_value));
    else {
        PyErr_SetString(PyExc_TypeError,
                        "value must be int, str or bytes with decimal number");
        return NULL;
    }

    int rc = dap_chain_datum_tx_add_out_std_item(
                 &(((PyDapChainDatumTxObject *)self)->datum_tx),
                 ((PyDapChainAddrObject *)py_addr)->addr,
                 value, token, 0);

    return PyLong_FromLong(rc);
}


PyObject *dap_chain_datum_tx_add_fee_item_py(PyObject *self, PyObject *args)
{
    PyObject *py_val = NULL;
    if (!PyArg_ParseTuple(args, "O", &py_val))
        return NULL;

    uint256_t val = {0};

    if (PyLong_Check(py_val)) {
        char buf[64];
        sprintf(buf, "%llu",
                (unsigned long long)PyLong_AsUnsignedLongLong(py_val));
        val = dap_chain_balance_scan(buf);
    } else if (PyUnicode_Check(py_val))
        val = dap_chain_balance_scan(PyUnicode_AsUTF8(py_val));
    else if (PyBytes_Check(py_val))
        val = dap_chain_balance_scan(PyBytes_AsString(py_val));
    else {
        PyErr_SetString(PyExc_TypeError,
                        "fee must be int, str or bytes with decimal number");
        return NULL;
    }

    int rc = dap_chain_datum_tx_add_fee_item(
                 &(((PyDapChainDatumTxObject *)self)->datum_tx), val);

    return PyLong_FromLong(rc);
}


/* -------------------------------------- */
