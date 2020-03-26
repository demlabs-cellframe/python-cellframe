#include "wrapping_dap_chain_datum_tx.h"

/* DAP chain tx iter type */

PyObject *TX_ITEM_TYPE_IN_PY(void){
    return PyLong_FromLong(TX_ITEM_TYPE_IN);
}
PyObject *TX_ITEM_TYPE_OUT_PY(void){
        return PyLong_FromLong(TX_ITEM_TYPE_OUT);
}
PyObject *TX_ITEM_TYPE_PKEY_PY(void){
        return PyLong_FromLong(TX_ITEM_TYPE_PKEY);
}
PyObject *TX_ITEM_TYPE_SIG_PY(void){
        return PyLong_FromLong(TX_ITEM_TYPE_SIG);
}
PyObject *TX_ITEM_TYPE_TOKEN_PY(void){
        return PyLong_FromLong(TX_ITEM_TYPE_TOKEN);
}
PyObject *TX_ITEM_TYPE_IN_COND_PY(void){
        return PyLong_FromLong(TX_ITEM_TYPE_IN_COND);
}
PyObject *TX_ITEM_TYPE_OUT_COND_PY(void){
        return PyLong_FromLong(TX_ITEM_TYPE_OUT_COND);
}
PyObject *TX_ITEM_TYPE_RECEIPT_PY(void){
        return PyLong_FromLong(TX_ITEM_TYPE_RECEIPT);
}

/* -------------------------------------- */

/* DAP chain tx cond type */
PyObject *COND_SERVICE_PROVIDE_PY(){
    PyObject *obj = _PyObject_New(&DapChainTxCondType_DapChainTxCondTypeObject);
    ((PyDapChainTxCondTypeObject*)obj)->tx_cond_type_t = COND_SERVICE_PROVIDE;
    return Py_BuildValue("O", obj);
}
PyObject *COND_SERVICE_BILL_PY(){
    PyObject *obj = _PyObject_New(&DapChainTxCondType_DapChainTxCondTypeObject);
    ((PyDapChainTxCondTypeObject*)obj)->tx_cond_type_t = COND_SERVICE_BILL;
    return Py_BuildValue("O", obj);
}
/* -------------------------------------- */

/* DAP chain datum tx */
PyObject *PyDapChainDatumTxObject_create(PyTypeObject *type_object, PyObject *args, PyObject *kwds){
    PyDapChainDatumTxObject *obj = (PyDapChainDatumTxObject*)PyType_GenericNew(type_object, args, kwds);
    obj->datum_tx = dap_chain_datum_tx_create();
    return (PyObject *)obj;
}
void PyDapChainDatumTxObject_delete(PyDapChainDatumTxObject* datumTx){
    dap_chain_datum_tx_delete(datumTx->datum_tx);
    Py_TYPE(datumTx)->tp_free((PyObject*)datumTx);
}

PyObject *dap_chain_datum_tx_get_size_py(PyObject *self, PyObject *args){
    size_t size = dap_chain_datum_tx_get_size(((PyDapChainDatumTxObject*)self)->datum_tx);
    return PyLong_FromSize_t(size);
}
PyObject *dap_chain_datum_tx_add_item_py(PyObject *self, PyObject *args){
    PyObject *in_obj;
    u_int8_t *a_item;
    if (!PyArg_ParseTuple(args, "O|b", &in_obj, &a_item))
        return NULL;
    dap_chain_datum_tx_t **txs = PyListToDapChainDatumTxArray(in_obj);
    int res = dap_chain_datum_tx_add_item(txs, a_item);
    if (res == -1){
        return NULL;
    } else {
        PyObject *out = DapChainDatumTxArrayToPyList(txs);
        return Py_BuildValue("O", out);
    }
}
PyObject *dap_chain_datum_tx_add_in_item_py(PyObject *self, PyObject *args){
    PyObject *in_obj_datum_txs;
    PyObject *in_obj_hash_fast;
    uint32_t in_tx_out_pref_idx;
    if (!PyArg_ParseTuple(args, "O|O|I", &in_obj_datum_txs, &in_obj_hash_fast, &in_tx_out_pref_idx))
        return NULL;
    dap_chain_datum_tx_t **a_txs = PyListToDapChainDatumTxArray(in_obj_datum_txs);
    int res = dap_chain_datum_tx_add_in_item(a_txs, ((PyDapHashFastObject*)in_obj_hash_fast)->hash_fast,
                                             in_tx_out_pref_idx);
    if (res == -1){
        return NULL;
    } else {
        PyObject *out = DapChainDatumTxArrayToPyList(a_txs);
        return Py_BuildValue("O", out);
    }
}

PyObject *dap_chain_datum_tx_add_in_cond_item_py(PyObject *self, PyObject *args){
    PyObject *in_obj_datum_txs;
    PyObject *in_chain_hash_fast;
    unsigned int in_tx_out_prev_idx;
    unsigned int in_receipt_idx;
    if (!PyArg_ParseTuple(args, "O|O|I|I", &in_obj_datum_txs, &in_chain_hash_fast, &in_tx_out_prev_idx, &in_receipt_idx))
        return NULL;
    dap_chain_datum_tx_t **a_txs = PyListToDapChainDatumTxArray(in_obj_datum_txs);
    int res = dap_chain_datum_tx_add_in_cond_item(a_txs, ((PyDapHashFastObject*)in_chain_hash_fast)->hash_fast, in_tx_out_prev_idx, in_receipt_idx);
    return PyLong_FromLong(res);
}

PyObject *dap_chain_datum_tx_add_out_item_py(PyObject *self, PyObject *args){
    PyObject *in_obj_datum_txs;
    PyObject *in_addr;
    uint64_t value;
    if (!PyArg_ParseTuple(args, "O|O|k", &in_obj_datum_txs, &in_addr, value))
        return NULL;
    dap_chain_datum_tx_t **txs = PyListToDapChainDatumTxArray(in_obj_datum_txs);
    int res = dap_chain_datum_tx_add_out_item(txs, ((PyDapChainAddrObject*)in_addr)->addr, value);
    if (res == -1){
        return NULL;
    } else {
        PyObject *out = DapChainDatumTxArrayToPyList(txs);
        return Py_BuildValue("O", out);
    }
}
PyObject *dap_chain_datum_tx_add_out_cond_item_py(PyObject *self, PyObject *args){
    PyObject *obj_txs;
    PyObject *obj_key;
    PyObject *obj_srv_uid;
    uint64_t value;
    uint64_t value_max_per_unit;
    PyObject *obj_srv_price_unit_uid;
    PyObject *obj_cond_bytes;
    Py_ssize_t cond_size;
    if (!PyArg_ParseTuple(args, "O|O|O|k|k|O|O|n", &obj_txs, &obj_key, &obj_srv_uid, &value, &value_max_per_unit,
                          &obj_srv_price_unit_uid, &obj_cond_bytes, &cond_size))
        return NULL;
    dap_chain_datum_tx_t **txs = PyListToDapChainDatumTxArray(obj_txs);
    void *cond = (void*)PyBytes_AsString(obj_cond_bytes);
    int res = dap_chain_datum_tx_add_out_cond_item(txs, ((PyCryptoKeyObject*)obj_key)->key, ((PyDapChainNetSrvUIDObject*)obj_srv_uid)->net_srv_uid,
                                                   value, value_max_per_unit, ((PyDapChainNetSrvPriceUnitUIDObject*)obj_srv_price_unit_uid)->price_unit_uid,
                                                   cond, (size_t)cond_size);
    if (res == -1){
        return NULL;
    } else {
        PyObject *out = DapChainDatumTxArrayToPyList(txs);
        return Py_BuildValue("O", out);
    }
}
PyObject *dap_chain_datum_tx_add_sign_item_py(PyObject *self, PyObject *args){
    PyObject *obj_txs;
    PyObject *obj_key;
    if (!PyArg_ParseTuple(args, "O|O", &obj_txs, &obj_key))
        return NULL;
    dap_chain_datum_tx_t **txs = PyListToDapChainDatumTxArray(obj_txs);
    int res = dap_chain_datum_tx_add_sign_item(txs, ((PyCryptoKeyObject*)obj_key)->key);
    if (res == -1){
        return NULL;
    } else {
        PyObject *out = DapChainDatumTxArrayToPyList(txs);
        return Py_BuildValue("O", out);
    }
}
PyObject *dap_chain_datum_tx_verify_sign_py(PyObject *self, PyObject *args){
    int res = dap_chain_datum_tx_verify_sign(((PyDapChainDatumTxObject*)self)->datum_tx);
    return PyLong_FromLong(res);
}

static dap_chain_datum_tx_t **PyListToDapChainDatumTxArray(PyObject *a_in_obj){
    Py_ssize_t size = PyList_Size(a_in_obj);
    dap_chain_datum_tx_t **txs = calloc(sizeof(dap_chain_datum_tx_t), (size_t)size);
    for (Py_ssize_t i = 0; i < size;i++){
        txs[i] = ((PyDapChainDatumTxObject*)PyList_GetItem(a_in_obj, i))->datum_tx;
    }
    return txs;
}

static PyObject* DapChainDatumTxArrayToPyList(dap_chain_datum_tx_t** datum_txs){
    size_t len = sizeof(datum_txs) / sizeof(*datum_txs);
    PyObject* list = PyList_New((Py_ssize_t)len);
    for (size_t i=0; i < len; i++){
        PyObject *obj = _PyObject_New(&DapChainDatumTx_DapChainDatumTxObjectType);
        ((PyDapChainDatumTxObject*)obj)->datum_tx = datum_txs[i];
        PyList_Append(list, obj);
    }
    return list;
}

dap_chain_tx_out_cond_t **PyListToDapChainTxOutCond(PyObject *list){
    Py_ssize_t size = PyList_Size(list);
    dap_chain_tx_out_cond_t **out_conds = calloc(sizeof(dap_chain_tx_out_cond_t), (size_t)size);
    for (Py_ssize_t i=0; i < size; i++){
        out_conds[i] = ((PyDapChainTxOutCondObject*)PyList_GetItem(list, i))->out_cond;
    }
    return out_conds;
}

PyObject *DapChainTxOutCondObjectToPyList(dap_chain_tx_out_cond_t **out_cond){
    size_t len = sizeof(out_cond) / sizeof(*out_cond);
    PyObject *list = PyList_New((Py_ssize_t)len);
    for (size_t i=0; i< len;i++ ){
        PyObject *obj = _PyObject_New(&DapChainTxOutCond_DapChainTxOutCondObjectType);
        ((PyDapChainTxOutCondObject*)obj)->out_cond = out_cond[i];
        PyList_Append(list, obj);
    }
    return list;
}

/* -------------------------------------- */
