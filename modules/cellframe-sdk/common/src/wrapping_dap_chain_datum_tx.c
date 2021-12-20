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
                                                   ((PyCryptoKeyObject*)obj_key)->key,
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

/* -------------------------------------- */
