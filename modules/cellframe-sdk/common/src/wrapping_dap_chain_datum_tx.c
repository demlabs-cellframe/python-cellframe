#include "wrapping_dap_chain_datum_tx.h"
#include "dap_chain_datum_tx_sig.h"

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
        {"addSignItem", (PyCFunction)dap_chain_datum_tx_add_sign_item_py, METH_VARARGS, ""},
        {"appendSignItem", (PyCFunction)dap_chain_datum_tx_append_sign_item_py, METH_VARARGS, ""},
        {"verifySign", (PyCFunction)dap_chain_datum_tx_verify_sign_py, METH_VARARGS, ""},
        {"getItems", (PyCFunction)wrapping_dap_chain_datum_tx_get_items, METH_NOARGS, ""},
        {}
};

PyTypeObject DapChainDatumTxObjectType = {
        .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "CellFrame.Chain.DatumTx",
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

PyObject *dap_chain_datum_tx_append_sign_item_py(PyObject *self, PyObject *args){
    PyDapSignObject *obj_sign;
    if (!PyArg_ParseTuple(args, "O", &obj_sign))
        return NULL;
    
    dap_chain_tx_sig_t *l_sign = dap_chain_datum_tx_item_sign_create_from_sign(obj_sign->sign);
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
    if (!obj_list) {
        return NULL;
    }

    uint64_t l_out_idx = 0;
    dap_hash_fast_t *l_tx_hf = DAP_NEW(dap_hash_fast_t);
    if (!l_tx_hf) {
        Py_DECREF(obj_list);
        return NULL;
    }

    dap_hash_fast(((PyDapChainDatumTxObject*)self)->datum_tx,
                  dap_chain_datum_tx_get_size(((PyDapChainDatumTxObject*)self)->datum_tx),
                  l_tx_hf);

    while(l_tx_items_count < l_tx_items_size){
        uint8_t *item = ((PyDapChainDatumTxObject*)self)->datum_tx->tx_items + l_tx_items_count;
        size_t l_tx_item_size = dap_chain_datum_item_tx_get_size(item, 0);
        if (l_tx_item_size == 0) {
            DAP_DELETE(l_tx_hf);
            Py_DECREF(obj_list);
            return PyList_New(0);
        }

        PyObject *obj_tx_item = NULL;
        switch (*item) {
            case TX_ITEM_TYPE_IN: {
                PyDapChainTXInObject *tx_in = PyObject_New(PyDapChainTXInObject, &DapChainTxInObjectType);
                if (!tx_in) {
                    DAP_DELETE(l_tx_hf);
                    Py_DECREF(obj_list);
                    return NULL;
                }
                tx_in->tx_in = ((dap_chain_tx_in_t*)item);
                obj_tx_item = (PyObject*)tx_in;
                break;
            }
            case TX_ITEM_TYPE_OUT: {
                PyDapChainTXOutObject *tx_out = PyObject_New(PyDapChainTXOutObject, &DapChainTxOutObjectType);
                if (!tx_out) {
                    DAP_DELETE(l_tx_hf);
                    Py_DECREF(obj_list);
                    return NULL;
                }
                tx_out->tx_out = ((dap_chain_tx_out_t*)item);
                tx_out->tx_hash = DAP_NEW(dap_hash_fast_t);
                if (!tx_out->tx_hash) {
                    Py_DECREF(tx_out);
                    DAP_DELETE(l_tx_hf);
                    Py_DECREF(obj_list);
                    return NULL;
                }
                memcpy(tx_out->tx_hash, l_tx_hf, sizeof(dap_hash_fast_t));
                tx_out->idx = l_out_idx;
                l_out_idx++;
                obj_tx_item = (PyObject*)tx_out;
                break;
            }
            case TX_ITEM_TYPE_IN_EMS: {
                PyDapChainTxTokenObject *token = PyObject_New(PyDapChainTxTokenObject, &DapChainTxTokenObjectType);
                if (!token) {
                    DAP_DELETE(l_tx_hf);
                    Py_DECREF(obj_list);
                    return NULL;
                }
                token->token = (dap_chain_tx_in_ems_t*)item;
                obj_tx_item = (PyObject*)token;
                break;
            }
            case TX_ITEM_TYPE_SIG: {
                PyDapChainTXSigObject *sig = PyObject_New(PyDapChainTXSigObject, &DapChainTxSigObjectType);
                if (!sig) {
                    DAP_DELETE(l_tx_hf);
                    Py_DECREF(obj_list);
                    return NULL;
                }
                sig->tx_sig = (dap_chain_tx_sig_t*)item;
                obj_tx_item = (PyObject*)sig;
                break;
            }
            case TX_ITEM_TYPE_RECEIPT: {
                PyDapChainTXReceiptObject *receipt = PyObject_New(PyDapChainTXReceiptObject, &DapChainTxReceiptObjectType);
                if (!receipt) {
                    DAP_DELETE(l_tx_hf);
                    Py_DECREF(obj_list);
                    return NULL;
                }
                receipt->tx_receipt = (dap_chain_datum_tx_receipt_t*)item;
                obj_tx_item = (PyObject*)receipt;
                break;
            }
            case TX_ITEM_TYPE_PKEY: {
                PyDapChainTXPkeyObject *pkey = PyObject_New(PyDapChainTXPkeyObject, &DapChainTxPkeyObjectType);
                if (!pkey) {
                    DAP_DELETE(l_tx_hf);
                    Py_DECREF(obj_list);
                    return NULL;
                }
                pkey->tx_pkey = ((dap_chain_tx_pkey_t*)item);
                obj_tx_item = (PyObject*)pkey;
                break;
            }
            case TX_ITEM_TYPE_IN_COND: {
                PyDapChainTXInCondObject *in_cond = PyObject_New(PyDapChainTXInCondObject, &DapChainTxInCondObjectType);
                if (!in_cond) {
                    DAP_DELETE(l_tx_hf);
                    Py_DECREF(obj_list);
                    return NULL;
                }
                in_cond->tx_in_cond = (dap_chain_tx_in_cond_t*)item;
                obj_tx_item = (PyObject*)in_cond;
                break;
            }
            case TX_ITEM_TYPE_OUT_COND: {
                PyDapChainTxOutCondObject *out_cond = NULL;
                switch (((dap_chain_tx_out_cond_t*)item)->header.subtype) {
                    case DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_PAY:
                        out_cond = PyObject_New(PyDapChainTxOutCondObject, &DapChainTxOutCondSubTypeSrvPayObjectType);
                        break;
                    case DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_LOCK:
                        out_cond = PyObject_New(PyDapChainTxOutCondObject, &DapChainTxOutCondSubTypeSrvStakeLockObjectType);
                        break;
                    case DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_POS_DELEGATE:
                        out_cond = PyObject_New(PyDapChainTxOutCondObject, &DapChainTxOutCondSubTypeSrvStakePosDelegateObjectType);
                        break;
                    case DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_XCHANGE:
                        out_cond = PyObject_New(PyDapChainTxOutCondObject, &DapChainTxOutCondSubTypeSrvXchangeObjectType);
                        break;
                    default:
                        out_cond = PyObject_New(PyDapChainTxOutCondObject, &DapChainTxOutCondObjectType);
                }
                if (!out_cond) {
                    DAP_DELETE(l_tx_hf);
                    Py_DECREF(obj_list);
                    return NULL;
                }
                out_cond->out_cond = ((dap_chain_tx_out_cond_t*)item);
                out_cond->tx_hash = DAP_NEW(dap_hash_fast_t);
                if (!out_cond->tx_hash) {
                    Py_DECREF(out_cond);
                    DAP_DELETE(l_tx_hf);
                    Py_DECREF(obj_list);
                    return NULL;
                }
                memcpy(out_cond->tx_hash, l_tx_hf, sizeof(dap_hash_fast_t));
                out_cond->idx = l_out_idx;
                l_out_idx++;
                obj_tx_item = (PyObject*)out_cond;
                break;
            }
            case TX_ITEM_TYPE_OUT_EXT: {
                PyDapChainTXOutExtObject *out_ext = PyObject_New(PyDapChainTXOutExtObject, &DapChainTxOutExtObjectType);
                if (!out_ext) {
                    DAP_DELETE(l_tx_hf);
                    Py_DECREF(obj_list);
                    return NULL;
                }
                out_ext->out_ext = (dap_chain_tx_out_ext_t*)item;
                out_ext->tx_hash = DAP_NEW(dap_hash_fast_t);
                if (!out_ext->tx_hash) {
                    Py_DECREF(out_ext);
                    DAP_DELETE(l_tx_hf);
                    Py_DECREF(obj_list);
                    return NULL;
                }
                memcpy(out_ext->tx_hash, l_tx_hf, sizeof(dap_hash_fast_t));
                out_ext->idx = l_out_idx;
                l_out_idx++;
                obj_tx_item = (PyObject*)out_ext;
                break;
            }
            case TX_ITEM_TYPE_TSD: {
                PyDapChainTxTSDObject *tsd = PyObject_New(PyDapChainTxTSDObject, &DapChainTxTSDObjectType);
                if (!tsd) {
                    DAP_DELETE(l_tx_hf);
                    Py_DECREF(obj_list);
                    return NULL;
                }
                tsd->tsd = (dap_chain_tx_tsd_t*)item;
                obj_tx_item = (PyObject*)tsd;
                break;
            }
            case TX_ITEM_TYPE_VOTE: {
                PyDapChainTXVoteObject *vote = PyObject_New(PyDapChainTXVoteObject, &PyDapChainTXVoteObjectType);
                if (!vote) {
                    DAP_DELETE(l_tx_hf);
                    Py_DECREF(obj_list);
                    return NULL;
                }
                vote->vote = (dap_chain_tx_vote_t*)item;
                obj_tx_item = (PyObject*)vote;
                break;
            }
            case TX_ITEM_TYPE_VOTING: {
                PyDapChainTXVotingObject *voting = PyObject_New(PyDapChainTXVotingObject, &PyDapChainTxVotingObjectType);
                if (!voting) {
                    DAP_DELETE(l_tx_hf);
                    Py_DECREF(obj_list);
                    return NULL;
                }
                voting->voting = dap_chain_voting_parse_tsd(((PyDapChainDatumTxObject*)self)->datum_tx);
                obj_tx_item = (PyObject*)voting;
                break;
            }
            default:
                obj_tx_item = Py_None;
                Py_INCREF(Py_None);
                break;
        }

        if (PyList_Append(obj_list, obj_tx_item) < 0) {
            if (obj_tx_item != Py_None) {
                Py_DECREF(obj_tx_item);
            }
            DAP_DELETE(l_tx_hf);
            Py_DECREF(obj_list);
            return NULL;
        }

        if (obj_tx_item != Py_None) {
            Py_DECREF(obj_tx_item);
        }
        l_tx_items_count += l_tx_item_size;
    }

    DAP_DELETE(l_tx_hf);
    return obj_list;
}

/* -------------------------------------- */
