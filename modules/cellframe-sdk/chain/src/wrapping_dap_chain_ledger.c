#include "wrapping_dap_chain_ledger.h"

PyObject *DapChainLedgerObject_create(PyTypeObject *type_object, PyObject *args, PyObject *kwds){
    uint16_t check_flag;
    char *net_name;
    if (!PyArg_ParseTuple(args, "H|s", &check_flag, &net_name))
        return NULL;
    PyDapChainLedgerObject *obj = (PyDapChainLedgerObject *)PyType_GenericNew(type_object, args, kwds);
    obj->ledger = dap_chain_ledger_create(check_flag, net_name);
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
                                       ((PyDapChainCellIDObject*)obj_local_cell_id)->cell_id);
    return  PyLong_FromLong(0);
}
PyObject *dap_chain_node_datum_tx_calc_hash_py(PyObject *self, PyObject *args){
    (void)self;
    PyObject *obj_tx;
    if (!PyArg_ParseTuple(args, "O", &obj_tx))
        return NULL;
    PyObject *obj_h_fast = _PyObject_New(&DapHashFastObject_DapHashFastObjectType);
    ((PyDapHashFastObject*)obj_h_fast)->hash_fast = dap_chain_node_datum_tx_calc_hash(((PyDapChainDatumTxObject*)obj_tx)->datum_tx);
    return  Py_BuildValue("O", obj_h_fast);

}
PyObject *dap_chain_ledger_tx_add_py(PyObject *self, PyObject *args){
    PyObject *obj_datum_tx;
    if (!PyArg_ParseTuple(args, "O", &obj_datum_tx))
        return NULL;
    int res = dap_chain_ledger_tx_add(((PyDapChainLedgerObject*)self)->ledger, ((PyDapChainDatumTxObject*)obj_datum_tx)->datum_tx, NULL, false);
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
    int res = dap_chain_ledger_token_emission_add(((PyDapChainLedgerObject*)self)->ledger, (byte_t*)((PyDapChainDatumTokenEmissionObject*)token_emission)->token_emission, token_emissiom_size);
    return PyLong_FromLong(res);
}
PyObject *dap_chain_ledger_token_emission_find_py(PyObject *self, PyObject *args){
    PyObject *h_fast;
    const char *token_ticker;
    if (!PyArg_ParseTuple(args, "s|O", &token_ticker, &h_fast))
        return NULL;
    PyObject *token_emission = _PyObject_New(&DapChainDatumTokenEmission_DapChainDatumTokenEmissionObjectType);
    ((PyDapChainDatumTokenEmissionObject*)token_emission)->token_emission = dap_chain_ledger_token_emission_find(
                ((PyDapChainLedgerObject*)self)->ledger, token_ticker, ((PyDapHashFastObject*)h_fast)->hash_fast);
    return Py_BuildValue("O", &token_emission);
}
PyObject *dap_chain_ledger_tx_get_token_ticker_by_hash_py(PyObject *self, PyObject *args){
    PyObject *obj_hash;
    if (!PyArg_ParseTuple(args, "O", &obj_hash)){
        PyErr_SetString(PyExc_AttributeError, "Function takes exactly one argument which must be HashFast object");
        return NULL;
    }
    const char *l_ticker = dap_chain_ledger_tx_get_token_ticker_by_hash(
            ((PyDapChainLedgerObject*)self)->ledger,
            ((PyDapHashFastObject*)obj_hash)->hash_fast);
    return Py_BuildValue("s", l_ticker);
}
PyObject *dap_chain_ledger_addr_get_token_ticker_all_py(PyObject *self, PyObject *args){
    PyObject *obj_addr;
    PyObject *obj_tickers;
    PyObject *obj_tickers_size;
    if (!PyArg_ParseTuple(args, "O|O|O", &obj_addr, &obj_tickers, &obj_tickers_size))
        return NULL;
    char ***tickers = ListStringToArrayStringFormatChar(obj_tickers);
    size_t *tickers_size = ListIntToSizeT(obj_tickers_size);
    dap_chain_ledger_addr_get_token_ticker_all(((PyDapChainLedgerObject*)self)->ledger, ((PyDapChainAddrObject*)obj_addr)->addr, tickers, tickers_size);
    return PyLong_FromLong(0);
}
PyObject *dap_chain_ledger_addr_get_token_ticker_all_fast_py(PyObject *self, PyObject *args){
    PyObject *obj_addr;
    if (!PyArg_ParseTuple(args, "O", &obj_addr)) {
        PyErr_SetString(PyExc_AttributeError, "Function takes exactly one argument which must be ChainAddr object");
        return NULL;
    }
    char **tickers = NULL;
    size_t ticker_size = 0;
    dap_chain_ledger_addr_get_token_ticker_all_fast(((PyDapChainLedgerObject*)self)->ledger, ((PyDapChainAddrObject*)obj_addr)->addr, &tickers, &ticker_size);
    PyObject *obj_list = PyList_New(ticker_size);
    for (int i=0; i < ticker_size;i++){
        PyObject *str = PyUnicode_FromString(tickers[i]);
        PyList_SetItem(obj_list, i, str);
    }
    return obj_list;
}
PyObject *dap_chain_ledger_tx_cache_check_py(PyObject *self, PyObject *args){
    PyObject *obj_datum_tx;
    PyObject *list_bound_items;
    PyObject *list_tx_out;
    if (!PyArg_ParseTuple(args, "O|O|O", &obj_datum_tx, &list_bound_items, &list_tx_out))
        return NULL;
    Py_ssize_t size_list_bound_item = PyList_Size(list_bound_items);
    dap_list_t **bound_items = calloc(sizeof(dap_list_t**), (size_t)size_list_bound_item);
    for (Py_ssize_t i = 0; i < size_list_bound_item;i++){
        PyObject *obj = PyList_GetItem(list_bound_items, i);
        dap_list_t *l = pyListToDapList(obj);
        bound_items[i] = l;
    }
    Py_ssize_t size_tx_out = PyList_Size(list_tx_out);
    dap_list_t **tx_out = calloc(sizeof(dap_list_t**), (size_t)size_tx_out);
    for (Py_ssize_t i = 0; i < size_tx_out;i++){
        PyObject *obj = PyList_GetItem(list_bound_items, i);
        dap_list_t *l = pyListToDapList(obj);
        tx_out[i] = l;
    }
    int res = dap_chain_ledger_tx_cache_check(((PyDapChainLedgerObject*)self)->ledger, ((PyDapChainDatumTxObject*)obj_datum_tx)->datum_tx, bound_items, tx_out);
    return PyLong_FromLong(res);
}
PyObject *dap_chain_node_datum_tx_cache_check_py(PyObject *self, PyObject *args){
    //TODO
    //Missing implementation of dap_chain_node_datum_tx_cache_check function in dap_chain_ledger
    return NULL;
//    PyObject *obj_datum_tx;
//    PyObject *list_bound_items;
//    if (!PyArg_ParseTuple(args, "O|O", &obj_datum_tx, &list_bound_items))
//        return NULL;
//    Py_ssize_t size = PyList_Size(list_bound_items);
//    dap_list_t **bound_items = calloc(sizeof (dap_list_t**), (size_t)size);
//    for (int i = 0; i < size ; i++){
//        PyObject *obj = PyList_GetItem(list_bound_items, i);
//        dap_list_t *l = pyListToDapList(obj);
//        bound_items[i] = l;
//    }
//    int res = dap_chain_node_datum_tx_cache_check(((PyDapChainDatumTxObject*)obj_datum_tx)->datum_tx, bound_items);
//    return PyLong_FromLong(res);
}
PyObject *dap_chain_ledger_tx_remove_py(PyObject *self, PyObject *args){
    PyObject *obj_h_fast;
    if (!PyArg_ParseTuple(args, "O", &obj_h_fast))
        return NULL;
    int res = dap_chain_ledger_tx_remove(((PyDapChainLedgerObject*)self)->ledger, ((PyDapHashFastObject*)obj_h_fast)->hash_fast);
    return PyLong_FromLong(res);
}
PyObject *dap_chain_ledger_purge_py(PyObject *self, PyObject *args){
    PyObject *obj_boolean = NULL;
    if(!PyArg_ParseTuple(args, "O", &obj_boolean)){
        return NULL;
    }
    if(!PyBool_Check(obj_boolean)){
        return NULL;
    }
    bool preserve_db = obj_boolean == Py_True ? true : false;
    dap_chain_ledger_purge(((PyDapChainLedgerObject*)self)->ledger, preserve_db);
    return Py_None;
}
PyObject *dap_chain_ledger_count_py(PyObject *self, PyObject *args){
    long long  res = (long long)dap_chain_ledger_count(((PyDapChainLedgerObject*)self)->ledger);
    return PyLong_FromLongLong(res);
}
PyObject *dap_chain_ledger_count_from_to_py(PyObject *self, PyObject *args){
    long ts_from = 0, ts_to = 0;
    if (!PyArg_ParseTuple(args, "|ll", &ts_from, &ts_to))
        return NULL;
    uint64_t res = 0;
    if (ts_from && ts_to){
        res = dap_chain_ledger_count_from_to(((PyDapChainLedgerObject*)self)->ledger, (time_t)ts_from, (time_t)ts_to);
    }else if(ts_from){
        res = dap_chain_ledger_count_from_to(((PyDapChainLedgerObject*)self)->ledger, (time_t)ts_from, 0);
    }else if (ts_to){
        res = dap_chain_ledger_count_from_to(((PyDapChainLedgerObject*)self)->ledger, 0, ts_to);
    } else {
        res = dap_chain_ledger_count_from_to(((PyDapChainLedgerObject*)self)->ledger, 0, 0);
    }
    return PyLong_FromUnsignedLongLong(res);
}
PyObject *dap_chain_ledger_tx_hash_is_used_out_item_py(PyObject *self, PyObject *args){
    PyObject *obj_h_fast;
    int idx_out;
    if (!PyArg_ParseTuple(args, "O|i", &obj_h_fast, &idx_out))
            return NULL;
    bool res = dap_chain_ledger_tx_hash_is_used_out_item(((PyDapChainLedgerObject*)self)->ledger, ((PyDapHashFastObject*)obj_h_fast)->hash_fast, idx_out);
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
    uint128_t  balance = dap_chain_ledger_calc_balance(
            ((PyDapChainLedgerObject*)self)->ledger,
            ((PyDapChainAddrObject*)addr)->addr, token_ticker);
    uint64_t res = dap_chain_uint128_to(balance);
    char* coins = dap_chain_balance_to_coins(balance);
    return Py_BuildValue("sk", coins, res);
}
PyObject *dap_chain_ledger_calc_balance_full_py(PyObject *self, PyObject *args){
    PyObject *addr;
    const char *token_ticker;
    if (!PyArg_ParseTuple(args, "O|s", &addr, &token_ticker))
        return NULL;
    uint64_t res = dap_chain_uint128_to(
                        dap_chain_ledger_calc_balance_full(
                            ((PyDapChainLedgerObject*)self)->ledger,
                            ((PyDapChainAddrObject*)addr)->addr, token_ticker));
    return Py_BuildValue("k", res);
}
PyObject *dap_chain_ledger_tx_find_by_hash_py(PyObject *self, PyObject *args){
    PyObject *h_fast;
    if (!PyArg_ParseTuple(args, "O", &h_fast))
        return NULL;
    PyObject *res = _PyObject_New(&DapChainDatumTx_DapChainDatumTxObjectType);
    res = PyObject_Init(res, &DapChainDatumTx_DapChainDatumTxObjectType);
    PyObject_Dir(res);
    ((PyDapChainDatumTxObject*)res)->datum_tx = dap_chain_ledger_tx_find_by_hash(((PyDapChainLedgerObject*)self)->ledger, ((PyDapHashFastObject*)h_fast)->hash_fast);
    if (((PyDapChainDatumTxObject*)res)->datum_tx == NULL) {
        PyObject_DEL(res);
        return Py_None;
    }
    ((PyDapChainDatumTxObject*)res)->original = false;
    return Py_BuildValue("O", res);
}
PyObject *dap_chain_ledger_tx_find_by_addr_py(PyObject *self, PyObject *args){
    const char *token;
    PyObject *addr;
    PyObject *first_hash;
    if (!PyArg_ParseTuple(args, "s|O|O", &token, &addr, &first_hash))
        return NULL;
    PyObject *res = _PyObject_New(&DapChainDatumTx_DapChainDatumTxObjectType);
    ((PyDapChainDatumTxObject*)res)->datum_tx = dap_chain_ledger_tx_find_by_addr(((PyDapChainLedgerObject*)self)->ledger, token, ((PyDapChainAddrObject*)addr)->addr, ((PyDapHashFastObject*)first_hash)->hash_fast);
    ((PyDapChainDatumTxObject*)res)->original = false;
    return Py_BuildValue("O", res);
}
PyObject *dap_chain_ledger_tx_find_by_pkey_py(PyObject *self, PyObject *args){
    char *p_key;
    size_t p_key_size;
    PyObject *obj_first_hash;
    if (!PyArg_ParseTuple(args, "s|n|O", &p_key, &p_key_size, &obj_first_hash))
        return NULL;
    PyObject *res = _PyObject_New(&DapChainDatumTx_DapChainDatumTxObjectType);
    ((PyDapChainDatumTxObject*)res)->datum_tx = (dap_chain_datum_tx_t*)dap_chain_ledger_tx_find_by_pkey(
                ((PyDapChainLedgerObject*)self)->ledger,
                p_key,
                p_key_size,
                ((PyDapHashFastObject*)obj_first_hash)->hash_fast);
    ((PyDapChainDatumTxObject*)res)->original = false;
    return Py_BuildValue("O", res);
}
PyObject *dap_chain_ledger_tx_cache_find_out_cond_py(PyObject *self, PyObject *args){
    PyObject *obj_first_hash;
    if (!PyArg_ParseTuple(args, "O", &obj_first_hash))
        return NULL;
    PyObject *res = _PyObject_New(&DapChainDatumTx_DapChainDatumTxObjectType);
    dap_chain_tx_out_cond_t **out_conds = NULL;
    int *out_cond_idx = NULL;
    ((PyDapChainDatumTxObject*)res)->datum_tx = (dap_chain_datum_tx_t*)dap_chain_ledger_tx_cache_find_out_cond(
                ((PyDapChainLedgerObject*)self)->ledger,
                ((PyDapHashFastObject*)obj_first_hash)->hash_fast,
                out_conds, out_cond_idx, NULL);
    ((PyDapChainDatumTxObject*)res)->original = false;
    return Py_BuildValue("O", res);
}
PyObject *dap_chain_ledger_tx_cache_get_out_cond_value_py(PyObject *self, PyObject *args){
    PyObject *obj_addr;
    if (!PyArg_ParseTuple(args, "O", &obj_addr))
        return NULL;
    dap_chain_tx_out_cond_t **out_conds = NULL;
    uint64_t res = dap_chain_ledger_tx_cache_get_out_cond_value(((PyDapChainLedgerObject*)self)->ledger,
                                                                ((PyDapChainAddrObject*)obj_addr)->addr,
                                                                out_conds);
    PyObject *obj_out_conds = _PyObject_New(&DapChainTxOutCond_DapChainTxOutCondType);
    ((PyDapChainTxOutCondObject*)obj_out_conds)->out_cond = *out_conds;
    PyObject *obj_res = PyLong_FromUnsignedLongLong(res);
    return Py_BuildValue("OO", obj_res, obj_out_conds);
}

static char*** ListStringToArrayStringFormatChar(PyObject *list){
    Py_ssize_t size = PyList_Size(list);
    char ***data = calloc(sizeof(char**), (size_t)size);
    for (Py_ssize_t i = 0; i < size; i++){
        PyObject *obj_two = PyList_GetItem(list,i);
        Py_ssize_t size_seentenses = PyList_Size(obj_two);
        char **sentences = calloc(sizeof(char**), (size_t)size_seentenses);
        for (int j=0; j < size_seentenses;j++){
            PyObject *obj_byte = PyList_GetItem(obj_two, j);
            char *word = PyBytes_AsString(obj_byte);
            sentences[j] = word;
        }
        data[i] = sentences;
    }
    return data;
}

static size_t *ListIntToSizeT(PyObject *list){
    Py_ssize_t size = PyList_Size(list);
    size_t *res_size_t = calloc(sizeof(size_t), (size_t)size);
    for (Py_ssize_t i=0; i<size;i++){
        PyObject *obj = PyList_GetItem(list, i);
        res_size_t[i] = (size_t)PyLong_AsSsize_t(obj);
    }
    return res_size_t;
}

PyObject *dap_chain_ledger_get_txs_py(PyObject *self, PyObject *args){
    size_t count, page;
    if (!PyArg_ParseTuple(args, "nn",&count, &page)){
        return NULL;
    }
    dap_list_t *l_txs = dap_chain_ledger_get_txs(
            ((PyDapChainLedgerObject*)self)->ledger,
            count,
            page);
    if (!l_txs){
        return Py_None;
    }
    PyObject *obj_list = PyList_New(0);
    for (dap_list_t *l_iter = l_txs; l_iter != NULL; l_iter = l_iter->next){
        PyDapChainDatumTxObject *obj_tx = PyObject_New(PyDapChainDatumTxObject, &DapChainDatumTx_DapChainDatumTxObjectType);
        obj_tx->datum_tx = l_iter->data;
        PyObject_Dir((PyObject*) obj_tx);
        PyList_Append(obj_list, (PyObject*)obj_tx);
    }
    return obj_list;
}
