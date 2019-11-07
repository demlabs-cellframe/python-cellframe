#include "wrapping_dap_chain_ledger.h"

PyObject *DapChainLedgerObject_create(PyTypeObject *type_object, PyObject *args, PyObject *kwds){
    uint16_t check_flag;
    if (!PyArg_ParseTuple(args, "H", &check_flag))
        return NULL;
    PyDapChainLedgerObject *obj = (PyDapChainLedgerObject *)PyType_GenericNew(type_object, args, kwds);
    obj->ledger = dap_chain_ledger_create(check_flag);
    return (PyObject *)obj;
}
void DapChainLedgerObject_free(PyDapChainLedgerObject* object){
    dap_chain_ledger_handle_free(object->ledger);
    Py_TYPE(object)->tp_free(object);
}

PyObject *dap_chain_ledger_set_local_cell_id_py(PyObject *self, PyObject *args){
    PyObject *obj_local_cell_id;
    if (!PyArg_ParseTuple(args, "O", &obj_local_cell_id))
        return NULL;
    dap_chain_ledger_set_local_cell_id(((PyDapChainLedgerObject*)self)->ledger,
                                       ((PyDapChainCellIdObject*)obj_local_cell_id)->id);
    return  PyLong_FromLong(0);
}
PyObject *dap_chain_node_datum_tx_calc_hash_py(PyObject *self, PyObject *args){
    PyObject *obj_tx;
    if (!PyArg_ParseTuple(args, "O", &obj_tx))
        return NULL;
    PyObject *obj_h_fast = _PyObject_New(&DapChainHashFastObject_DapChainHashFastObjectType);
    ((PyDapChainHashFastObject*)obj_h_fast)->hash_fast = dap_chain_node_datum_tx_calc_hash(((PyDapChainDatumTxObject*)obj_tx)->datum_tx);
    return  Py_BuildValue("O", obj_h_fast);

}
PyObject *dap_chain_ledger_tx_add_py(PyObject *self, PyObject *args){
    PyObject *obj_datum_tx;
    if (!PyArg_ParseTuple(args, "O", &obj_datum_tx))
        return NULL;
    int res = dap_chain_ledger_tx_add(((PyDapChainLedgerObject*)self)->ledger, ((PyDapChainDatumTxObject*)obj_datum_tx)->datum_tx);
    return PyLong_FromLong(res);
}
PyObject *dap_chain_ledger_token_add_py(PyObject *self, PyObject *args)
{
    PyObject *token;
    size_t token_size;
    if (!PyArg_ParseTuple(args, "O|n", &token, &token_size))
        return NULL;
    int res = dap_chain_ledger_token_add(((PyDapChainLedgerObject*)self)->ledger,
                                         ((PyDapChainDatumTokenObject*)token)->token, token_size);
    return PyLong_FromLong(res);
}
PyObject *dap_chain_ledger_token_emission_add_py(PyObject *self, PyObject *args){
    PyObject *token_emission;
    size_t token_emissiom_size;
    if (!PyArg_ParseTuple(args, "O|n", &token_emission, &token_emissiom_size))
        return NULL;
    int res = dap_chain_ledger_token_emission_add(((PyDapChainLedgerObject*)self)->ledger, ((PyDapChainDatumTokenEmissionObject*)token_emission)->token_emission, token_emissiom_size);
    return PyLong_FromLong(res);
}
PyObject *dap_chain_ledger_token_emission_find_py(PyObject *self, PyObject *args){
    PyObject *h_fast;
    const char *token_ticker;
    if (!PyArg_ParseTuple(args, "s|O", &token_ticker, &h_fast))
        return NULL;
    PyObject *token_emission = _PyObject_New(&DapChainDatumTokenEmission_DapChainDatumTokenEmissionObjectType);
    ((PyDapChainDatumTokenEmissionObject*)token_emission)->token_emission = dap_chain_ledger_token_emission_find(
                ((PyDapChainLedgerObject*)self)->ledger, token_ticker, ((PyDapChainHashFastObject*)h_fast)->hash_fast);
    return Py_BuildValue("O", &token_emission);
}
PyObject *dap_chain_ledger_tx_get_token_ticker_by_hash_py(PyObject *self, PyObject *args){
    PyObject *obj_tx_hash;
    if (!PyArg_ParseTuple(args, "O", &obj_tx_hash))
        return NULL;
    const char *res = dap_chain_ledger_tx_get_token_ticker_by_hash(((PyDapChainHashFastObject*)obj_tx_hash)->hash_fast);
    return Py_BuildValue("s", res);
}
PyObject *dap_chain_ledger_addr_get_token_ticker_all_py(PyObject *self, PyObject *args){
    return NULL;
}
PyObject *dap_chain_ledger_addr_get_token_ticker_all_fast_py(PyObject *self, PyObject *args){
    return NULL;
}
PyObject *dap_chain_ledger_tx_cache_check_py(PyObject *self, PyObject *args){
    return NULL;
}
PyObject *dap_chain_node_datum_tx_cache_check_py(PyObject *self, PyObject *args){
    return NULL;
}
PyObject *dap_chain_ledger_tx_remove_py(PyObject *self, PyObject *args){
    PyObject *obj_h_fast;
    if (!PyArg_ParseTuple(args, "O", &obj_h_fast))
        return NULL;
    int res = dap_chain_ledger_tx_remove(((PyDapChainLedgerObject*)self)->ledger, ((PyDapChainHashFastObject*)obj_h_fast)->hash_fast);
    return PyLong_FromLong(res);
}
PyObject *dap_chain_ledger_purge_py(PyObject *self, PyObject *args){
    dap_chain_ledger_purge(((PyDapChainLedgerObject*)self)->ledger);
    return PyLong_FromLong(0);
}
PyObject *dap_chain_ledger_count_py(PyObject *self, PyObject *args){
    return NULL;
}
PyObject *dap_chain_ledger_count_from_to_py(PyObject *self, PyObject *args){
    return NULL;
}
PyObject *dap_chain_ledger_tx_hash_is_used_out_item_py(PyObject *self, PyObject *args){
    PyObject *obj_h_fast;
    int idx_out;
    if (!PyArg_ParseTuple(args, "O|i", &obj_h_fast, &idx_out))
            return NULL;
    bool res = dap_chain_ledger_tx_hash_is_used_out_item(((PyDapChainLedgerObject*)self)->ledger, ((PyDapChainHashFastObject*)obj_h_fast)->hash_fast, idx_out);
    if (res)
        return Py_BuildValue("O", Py_True);
    else
        return Py_BuildValue("O", Py_False);
}
PyObject *dap_chain_ledger_calc_balance_py(PyObject *self, PyObject *args){
    PyObject *addr;
    const char *token_ticker;
    if (!PyArg_ParseTuple(args, "O|s", &addr, &token_ticker))
        return NULL;
    uint64_t res = dap_chain_ledger_calc_balance(((PyDapChainLedgerObject*)self)->ledger, ((PyDapChainAddrObject*)addr)->addr, token_ticker);
    return Py_BuildValue("k", res);
}
PyObject *dap_chain_ledger_calc_balance_full_py(PyObject *self, PyObject *args){
    PyObject *addr;
    const char *token_ticker;
    if (!PyArg_ParseTuple(args, "O|s", &addr, &token_ticker))
        return NULL;
    uint64_t res = dap_chain_ledger_calc_balance_full(((PyDapChainLedgerObject*)self)->ledger, ((PyDapChainAddrObject*)addr)->addr, token_ticker);
    return Py_BuildValue("k", res);
}
PyObject *dap_chain_ledger_tx_find_by_hash_py(PyObject *self, PyObject *args){
    PyObject *h_fast;
    if (!PyArg_ParseTuple(args, "O", &h_fast))
        return NULL;
    PyObject *res = _PyObject_New(&DapChainDatumTx_DapChainDatumTxObjectType);
    ((PyDapChainDatumTxObject*)res)->datum_tx = dap_chain_ledger_tx_find_by_hash(((PyDapChainLedgerObject*)self)->ledger, ((PyDapChainHashFastObject*)h_fast)->hash_fast);
    return Py_BuildValue("O", res);
}
PyObject *dap_chain_ledger_tx_find_by_addr_py(PyObject *self, PyObject *args){
    const char *token;
    PyObject *addr;
    PyObject *first_hash;
    if (!PyArg_ParseTuple(args, "s|O|O", &token, &addr, &first_hash))
        return NULL;
    PyObject *res = _PyObject_New(&DapChainDatumTx_DapChainDatumTxObjectType);
    ((PyDapChainDatumTxObject*)res)->datum_tx = dap_chain_ledger_tx_find_by_addr(((PyDapChainLedgerObject*)self)->ledger, token, ((PyDapChainAddrObject*)addr)->addr, ((PyDapChainHashFastObject*)first_hash)->hash_fast);
    return Py_BuildValue("O", res);
}
PyObject *dap_chain_ledger_tx_find_by_pkey_py(PyObject *self, PyObject *args){
    char *p_key;
    size_t p_key_size;
    PyObject *obj_first_hash;
    if (!PyArg_ParseTuple(args, "s|n|O", &p_key, &p_key_size, &obj_first_hash))
        return NULL;
    PyObject *res = _PyObject_New(&DapChainDatumTx_DapChainDatumTxObjectType);
    ((PyDapChainDatumTxObject*)res)->datum_tx = dap_chain_ledger_tx_find_by_pkey(((PyDapChainLedgerObject*)self)->ledger, p_key, p_key_size, ((PyDapChainHashFastObject*)obj_first_hash)->hash_fast);
    return Py_BuildValue("O", res);
}
PyObject *dap_chain_ledger_tx_cache_find_out_cond_py(PyObject *self, PyObject *args){
    PyObject *obj_addr;
    PyObject *obj_first_hash;
    if (!PyArg_ParseTuple(args, "O|O", &obj_addr, &obj_first_hash))
        return NULL;
    PyObject *res = _PyObject_New(&DapChainDatumTx_DapChainDatumTxObjectType);
    ((PyDapChainDatumTxObject*)res)->datum_tx = dap_chain_ledger_tx_cache_find_out_cond(((PyDapChainLedgerObject*)self)->ledger, ((PyDapChainAddrObject*)obj_addr)->addr, ((PyDapChainHashFastObject*)obj_first_hash)->hash_fast);
    return Py_BuildValue("O", res);
}
PyObject *dap_chain_ledger_tx_cache_get_out_cond_value_py(PyObject *self, PyObject *args){
//    PyObject *obj_addr;
//    PyObject *obj_out_cond;
//    if (!PyArg_ParseTuple(args, "O|O", &obj_addr, &obj_out_cond))
//        return NULL;
//    uint64_t res = dap_chain_ledger_tx_cache_get_out_cond_value(((PyDapChainLedgerObject*)self)->ledger, ((PyDapChainAddrObject*)obj_addr)->addr, ());
}

