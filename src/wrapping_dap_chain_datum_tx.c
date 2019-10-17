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
PyObject *TX_ITEM_TYPE_OUT_SERVICE_RECEIPT_PY(void){
        return PyLong_FromLong(TX_ITEM_TYPE_OUT_SERVICE_RECEIPT);
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
    int res = dap_chain_datum_tx_add_in_item(a_txs, ((PyDapChainHashFastObject*)in_obj_hash_fast)->hash_fast,
                                             in_tx_out_pref_idx);
    if (res == -1){
        return NULL;
    } else {
        PyObject *out = DapChainDatumTxArrayToPyList(a_txs);
        return Py_BuildValue("O", out);
    }
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
    PyObject *obj_addr;
    uint64_t value;
    PyBytesObject *bytes_cond;
    size_t cond_size;
    if (!PyArg_ParseTuple(args, "O|O|O|k|S|n", &obj_txs, &obj_key, &obj_addr, &value, &bytes_cond, &cond_size))
        return NULL;
    dap_chain_datum_tx_t **txs = PyListToDapChainDatumTxArray(obj_txs);
    void *cond = PyBytes_AsString((PyObject *)bytes_cond);

    int res = dap_chain_datum_tx_add_out_cond_item(txs, ((PyCryptoKeyObject*)obj_key)->key,
                                                   ((PyDapChainAddrObject*)obj_addr)->addr, value, cond, cond_size);
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
    size_t len = sizeof(datum_txs) / sizeof(datum_txs[0]);
    PyObject* list = PyList_New((Py_ssize_t)len);
    for (size_t i=0; i < len; i++){
        PyObject *obj = _PyObject_New(&DapChainDatumTx_DapChainDatumTxObjectType);
        ((PyDapChainDatumTxObject*)obj)->datum_tx = datum_txs[i];
        PyList_Append(list, obj);
    }
    return list;
}

/* -------------------------------------- */
