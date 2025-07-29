#include "wrapping_dap_chain_ledger.h"
#include "python-cellframe_common.h"
#include "dap_proc_thread.h"
#include "dap_events.h"

#define LOG_TAG "ledger wrapper"

static PyObject *s_bridged_tx_notify_add(PyObject *self, PyObject *args);
static PyObject *dap_chain_ledger_get_final_multi_wallet_tx_hash_py(PyObject *self, PyObject *args);
static PyObject *dap_chain_ledger_get_unspent_outputs_for_amount_py(PyObject *self, PyObject *args);

static PyMethodDef DapChainLedgerMethods[] = {
        {"nodeDatumTxCalcHash", (PyCFunction)dap_chain_node_datum_tx_calc_hash_py, METH_VARARGS, ""},
        {"txAdd", (PyCFunction)dap_chain_ledger_tx_add_py, METH_VARARGS, ""},
        {"tokenAdd", (PyCFunction)dap_chain_ledger_token_add_py, METH_VARARGS, ""},
        {"tokenEmissionLoad", (PyCFunction)dap_chain_ledger_token_emission_load_py, METH_VARARGS, ""},
        {"tokenEmissionFind", (PyCFunction)dap_chain_ledger_token_emission_find_py, METH_VARARGS, ""},
        {"tokenAuthSignsTotal", (PyCFunction)dap_chain_ledger_token_auth_signs_total_py, METH_VARARGS, ""},
        {"tokenAuthSignsValid", (PyCFunction)dap_chain_ledger_token_auth_signs_valid_py, METH_VARARGS, ""},
        {"tokenAuthPkeysHashes", (PyCFunction)dap_chain_ledger_token_auth_pkeys_hashes_py, METH_VARARGS, ""},
        {"txGetMainTickerAndLedgerRc", (PyCFunction)dap_chain_ledger_tx_get_main_ticker_py, METH_VARARGS, ""},
        {"txGetTokenTickerByHash", (PyCFunction)dap_chain_ledger_tx_get_token_ticker_by_hash_py, METH_VARARGS, ""},
        {"addrGetTokenTickerAll", (PyCFunction)dap_chain_ledger_addr_get_token_ticker_all_py, METH_VARARGS, ""},
        //{"txAddCheck", (PyCFunction)dap_chain_ledger_tx_add_check_py, METH_VARARGS, ""},
        {"datumTxCacheCheck", (PyCFunction)dap_chain_node_datum_tx_cache_check_py, METH_VARARGS, ""},
        {"purge", (PyCFunction)dap_chain_ledger_purge_py, METH_VARARGS, ""},
        {"count", (PyCFunction)dap_chain_ledger_count_py, METH_VARARGS, ""},
        {"countFromTo", (PyCFunction)dap_chain_ledger_count_from_to_py, METH_VARARGS, ""},
        {"txHashIsUsedOutItem", (PyCFunction)dap_chain_ledger_tx_hash_is_used_out_item_py, METH_VARARGS, ""},
        {"calcBalance", (PyCFunction)dap_chain_ledger_calc_balance_py, METH_VARARGS, ""},
        {"calcBalanceFull", (PyCFunction)dap_chain_ledger_calc_balance_full_py, METH_VARARGS, ""},
        {"txFindByHash", (PyCFunction)dap_chain_ledger_tx_find_by_hash_py, METH_VARARGS, ""},
        {"txFindByAddr", (PyCFunction)dap_chain_ledger_tx_find_by_addr_py, METH_VARARGS, ""},
        {"getTransactions", (PyCFunction) dap_chain_ledger_get_txs_py, METH_VARARGS, ""},
        {"txAddNotify", (PyCFunction)dap_chain_ledger_tx_add_notify_py, METH_VARARGS, ""},
        {"bridgedTxNotifyAdd", (PyCFunction)s_bridged_tx_notify_add, METH_VARARGS, ""},
        {"txHashIsUsedOutItemHash", (PyCFunction)dap_chain_ledger_tx_hash_is_used_out_item_hash_py, METH_VARARGS, ""},
        {"getFinalMultiWalletTxHash", (PyCFunction)dap_chain_ledger_get_final_multi_wallet_tx_hash_py, METH_VARARGS, ""},
        {"listUnspent", (PyCFunction)dap_chain_ledger_get_unspent_outputs_for_amount_py, METH_VARARGS, ""},
        {"eventFind", (PyCFunction)dap_chain_ledger_event_find_py, METH_VARARGS, "Find event by hash"},
        {"eventGetList", (PyCFunction)dap_chain_ledger_event_get_list_py, METH_VARARGS, "Get list of events by group name"},
        {"eventPkeyList", (PyCFunction)dap_chain_ledger_event_pkey_list_py, METH_VARARGS, "Get list of allowed public keys for events"},
        {"eventNotifyAdd", (PyCFunction)dap_chain_ledger_event_notify_add_py, METH_VARARGS, "Add notification callback for events"},
        {}
};

static PyMethodDef DapChainLedgerEventMethods[] = {
        {"getGroupName", (PyCFunction)dap_chain_ledger_event_get_group_name_py, METH_NOARGS, "Get event group name"},
        {"getType", (PyCFunction)dap_chain_ledger_event_get_type_py, METH_NOARGS, "Get event type"},
        {"getEventData", (PyCFunction)dap_chain_ledger_event_get_data_py, METH_NOARGS, "Get event data as bytes"},
        {"getTimestamp", (PyCFunction)dap_chain_ledger_event_get_timestamp_py, METH_NOARGS, "Get event timestamp"},
        {}
};

PyTypeObject DapChainLedgerObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.ChainLedger", sizeof(PyDapChainLedgerObject),
        "Chain ledger objects",
        .tp_methods = DapChainLedgerMethods);

PyTypeObject DapChainLedgerEventObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.ChainLedgerEvent", sizeof(PyDapChainLedgerEventObject),
        "Chain ledger event objects",
        .tp_methods = DapChainLedgerEventMethods);

PyObject *DapChainLedgerObject_create(PyTypeObject *type_object, PyObject *args, PyObject *kwds){
    uint16_t check_flag;
    char *net_name;
    if (!PyArg_ParseTuple(args, "H|s", &check_flag, &net_name))
        return NULL;
    PyDapChainLedgerObject *obj = (PyDapChainLedgerObject *)PyType_GenericNew(type_object, args, kwds);
    // TODO add relevant arguments for ledger create
    //obj->ledger = dap_chain_ledger_create(check_flag, net_name);
    return (PyObject *)obj;
}
void DapChainLedgerObject_free(PyDapChainLedgerObject* object){
    dap_ledger_handle_free(object->ledger);
    Py_TYPE(object)->tp_free(object);
}

PyObject *DapChainLedgerEventObject_create(PyTypeObject *type_object, PyObject *args, PyObject *kwds){
    (void)args;
    (void)kwds;
    PyDapChainLedgerEventObject *obj = (PyDapChainLedgerEventObject *)PyType_GenericNew(type_object, args, kwds);
    obj->event = NULL;
    obj->origin = false;
    return (PyObject *)obj;
}

void DapChainLedgerEventObject_free(PyDapChainLedgerEventObject* object){
    if (object->event && object->origin)
        dap_chain_datum_tx_event_delete(object->event);
    Py_TYPE(object)->tp_free(object);
}

PyObject *dap_chain_node_datum_tx_calc_hash_py(PyObject *self, PyObject *args){
    (void)self;
    PyObject *obj_tx;
    if (!PyArg_ParseTuple(args, "O", &obj_tx))
        return NULL;
    PyDapHashFastObject *obj_hash_fast = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    dap_hash_fast_t l_hash = dap_chain_node_datum_tx_calc_hash(((PyDapChainDatumTxObject*)obj_tx)->datum_tx);
    obj_hash_fast->hash_fast = DAP_DUP(&l_hash);
    obj_hash_fast->origin = true;
    return (PyObject*)obj_hash_fast;

}
PyObject *dap_chain_ledger_tx_add_py(PyObject *self, PyObject *args){
    PyDapChainDatumTxObject *obj_datum_tx;
    if (!PyArg_ParseTuple(args, "O", &obj_datum_tx))
        return NULL;
    dap_hash_fast_t l_tx_hash;
    dap_hash_fast(obj_datum_tx->datum_tx, dap_chain_datum_tx_get_size(obj_datum_tx->datum_tx), &l_tx_hash);
    int res = dap_ledger_tx_add(((PyDapChainLedgerObject*)self)->ledger, obj_datum_tx->datum_tx, &l_tx_hash, false, NULL);
    return PyLong_FromLong(res);
}
PyObject *dap_chain_ledger_token_add_py(PyObject *self, PyObject *args)
{
    PyObject *token;
    size_t token_size;
    if (!PyArg_ParseTuple(args, "O|n", &token, &token_size))
        return NULL;
    int res = dap_ledger_token_add(((PyDapChainLedgerObject*)self)->ledger,
                                         (byte_t *)((PyDapChainDatumTokenObject*)token)->token, token_size);
    return PyLong_FromLong(res);
}
PyObject *dap_chain_ledger_token_emission_load_py(PyObject *self, PyObject *args){
    PyDapChainDatumTokenEmissionObject *token_emission;
    size_t token_emissiom_size;
    if (!PyArg_ParseTuple(args, "O|n", &token_emission, &token_emissiom_size))
        return NULL;
    dap_hash_fast_t l_emission_hash;
    dap_hash_fast(token_emission->token_emission, token_emissiom_size, &l_emission_hash);
    int res = dap_ledger_token_emission_load(((PyDapChainLedgerObject*)self)->ledger,
                                                   (byte_t *)token_emission->token_emission,
                                                   token_emissiom_size,
                                                   &l_emission_hash);
    return PyLong_FromLong(res);
}

PyObject *dap_chain_ledger_token_emission_find_py(PyObject *self, PyObject *args){
    PyObject *h_fast;

    if (!PyArg_ParseTuple(args, "O", &h_fast))
        return NULL;
    
    PyDapChainDatumTokenEmissionObject *token_emission = PyObject_New(PyDapChainDatumTokenEmissionObject,
                                                                      &DapChainDatumTokenEmissionObjectType);

    token_emission->token_emission = dap_ledger_token_emission_find(
                ((PyDapChainLedgerObject*)self)->ledger, ((PyDapHashFastObject*)h_fast)->hash_fast);
    if (token_emission->token_emission)
    {
        token_emission->token_size = dap_chain_datum_emission_get_size((uint8_t*) token_emission->token_emission);
        token_emission->copy = false;
    
        return (PyObject *)token_emission;
    }
    else
        Py_RETURN_NONE;
}

PyObject *dap_chain_ledger_token_auth_signs_total_py(PyObject *self, PyObject *args) {
    const char *token_ticker;
    if (!PyArg_ParseTuple(args, "s", &token_ticker)) {
        PyErr_SetString(PyExc_AttributeError, "This function, as the first argument, must take the"
                                              " token ticker string");
        return NULL;
    }
    size_t res = dap_ledger_token_get_auth_signs_total(((PyDapChainLedgerObject*)self)->ledger, token_ticker);
    if (res == 0)
        Py_RETURN_NONE;
    return Py_BuildValue("i", res);
}

PyObject *dap_chain_ledger_token_auth_signs_valid_py(PyObject *self, PyObject *args) {
    const char *token_ticker;
    if (!PyArg_ParseTuple(args, "s", &token_ticker)) {
        PyErr_SetString(PyExc_AttributeError, "This function, as the first argument, must take the"
                                              " token ticker string");
        return NULL;
    }
    size_t res = dap_ledger_token_get_auth_signs_valid(((PyDapChainLedgerObject*)self)->ledger, token_ticker);
    if (res == 0)
        Py_RETURN_NONE;

    return Py_BuildValue("i", res);
}

PyObject *dap_chain_ledger_token_auth_pkeys_hashes_py(PyObject *self, PyObject *args)
{
    const char *token_ticker;
    if (!PyArg_ParseTuple(args, "s", &token_ticker)) {
        PyErr_SetString(PyExc_AttributeError, "This function, as the first argument, must take the"
                                              " token ticker string");
        return NULL;
    }
    dap_list_t * l_hashes = dap_ledger_token_get_auth_pkeys_hashes(((PyDapChainLedgerObject*)self)->ledger, token_ticker);
    PyObject *obj_list = PyList_New(dap_list_length(l_hashes));
    size_t i = 0;
    for (dap_list_t *l_iter = l_hashes; l_iter != NULL; l_iter = l_iter->next, ++i){
        PyDapHashFastObject *obj_hash = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
        obj_hash->hash_fast = (dap_chain_hash_fast_t *)l_iter->data;
        obj_hash->origin = false;
        PyList_SetItem(obj_list, i, (PyObject*)obj_hash);
    }

    dap_list_free(l_hashes);
    return obj_list;
}


PyObject *dap_chain_ledger_tx_get_token_ticker_by_hash_py(PyObject *self, PyObject *args){
    PyObject *obj_hash;
    if (!PyArg_ParseTuple(args, "O", &obj_hash)){
        PyErr_SetString(PyExc_AttributeError, "Function takes exactly one argument which must be HashFast object");
        return NULL;
    }
    const char *l_ticker = dap_ledger_tx_get_token_ticker_by_hash(
            ((PyDapChainLedgerObject*)self)->ledger,
            ((PyDapHashFastObject*)obj_hash)->hash_fast);
    return Py_BuildValue("s", l_ticker);
}
PyObject *dap_chain_ledger_addr_get_token_ticker_all_py(PyObject *self, PyObject *args){
    PyObject *obj_addr = NULL;
    if (!PyArg_ParseTuple(args, "|O", &obj_addr))
        return NULL;
    dap_chain_addr_t *l_addr = NULL;
    if (obj_addr) {
        if (!PyDapChainAddrObject_Check((PyDapChainAddrObject*)obj_addr)) {
            PyErr_SetString(PyExc_AttributeError, "An invalid argument was passed, the first argument "
                                                  "is optional and must be an object of type ChainAddr.");
            return NULL;
        }
        l_addr = PY_DAP_CHAIN_ADDR(obj_addr);
    }
    char **l_tickers = NULL;
    size_t l_ticker_count = 0;
    dap_ledger_addr_get_token_ticker_all(((PyDapChainLedgerObject*)self)->ledger, l_addr, &l_tickers, &l_ticker_count);
    PyObject *l_obj_tickers = PyList_New((Py_ssize_t)l_ticker_count);
    for (size_t i = 0; i < l_ticker_count; i++) {
        PyObject *obj_unicode = PyUnicode_FromString(l_tickers[i]);
        DAP_DELETE(l_tickers[i]);
        PyList_SetItem(l_obj_tickers, (Py_ssize_t)i, obj_unicode);
    }
    DAP_DELETE(l_tickers);
    return l_obj_tickers;
}

// TODO implement tx_add_check wrapping

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

PyObject *dap_chain_ledger_purge_py(PyObject *self, PyObject *args){
    dap_ledger_purge(((PyDapChainLedgerObject*)self)->ledger, false);
    return PyLong_FromLong(0);
}
PyObject *dap_chain_ledger_count_py(PyObject *self, PyObject *args){
    long long  res = (long long)dap_ledger_count(((PyDapChainLedgerObject*)self)->ledger);
    return PyLong_FromLongLong(res);
}
PyObject *dap_chain_ledger_count_from_to_py(PyObject *self, PyObject *args){
    long ts_from = 0, ts_to = 0;
    if (!PyArg_ParseTuple(args, "|ll", &ts_from, &ts_to))
        return NULL;
    uint64_t res = 0;
    if (ts_from && ts_to){
        res = dap_ledger_count_from_to(((PyDapChainLedgerObject*)self)->ledger, (time_t)ts_from, (time_t)ts_to);
    }else if(ts_from){
        res = dap_ledger_count_from_to(((PyDapChainLedgerObject*)self)->ledger, (time_t)ts_from, 0);
    }else if (ts_to){
        res = dap_ledger_count_from_to(((PyDapChainLedgerObject*)self)->ledger, 0, ts_to);
    } else {
        res = dap_ledger_count_from_to(((PyDapChainLedgerObject*)self)->ledger, 0, 0);
    }
    return PyLong_FromUnsignedLongLong(res);
}
PyObject *dap_chain_ledger_tx_hash_is_used_out_item_py(PyObject *self, PyObject *args){
    PyObject *obj_h_fast;
    int idx_out;
    if (!PyArg_ParseTuple(args, "O|i", &obj_h_fast, &idx_out))
            return NULL;
    bool res = dap_ledger_tx_hash_is_used_out_item(((PyDapChainLedgerObject*)self)->ledger, ((PyDapHashFastObject*)obj_h_fast)->hash_fast, idx_out, NULL);
    if (res)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}
PyObject *dap_chain_ledger_calc_balance_py(PyObject *self, PyObject *args){
    PyObject *addr;
    const char *token_ticker;
    if (!PyArg_ParseTuple(args, "Os", &addr, &token_ticker))
        return NULL;
    uint256_t balance = dap_ledger_calc_balance(
            ((PyDapChainLedgerObject*)self)->ledger,
            PY_DAP_CHAIN_ADDR(addr),
            token_ticker
    );
    DapMathObject *l_obj_balance = PyObject_New(DapMathObject, &DapMathObjectType);
    l_obj_balance->value = balance;
    return (PyObject*)l_obj_balance;
}
PyObject *dap_chain_ledger_calc_balance_full_py(PyObject *self, PyObject *args){
    PyObject *addr;
    const char *token_ticker;
    if (!PyArg_ParseTuple(args, "Os", &addr, &token_ticker))
        return NULL;
    DapMathObject *l_obj_balance = PyObject_New(DapMathObject, &DapMathObjectType);
    l_obj_balance->value = dap_ledger_calc_balance_full(
        ((PyDapChainLedgerObject*)self)->ledger,
        PY_DAP_CHAIN_ADDR(addr), token_ticker);
    return (PyObject*)l_obj_balance;
}
PyObject *dap_chain_ledger_tx_find_by_hash_py(PyObject *self, PyObject *args){
    PyObject *h_fast;
    if (!PyArg_ParseTuple(args, "O", &h_fast))
        return NULL;
    PyDapChainDatumTxObject *res = PyObject_NEW(PyDapChainDatumTxObject, &DapChainDatumTxObjectType);
    res->datum_tx = dap_ledger_tx_find_by_hash(((PyDapChainLedgerObject*)self)->ledger, ((PyDapHashFastObject*)h_fast)->hash_fast);
    
    res->original = false;
    if (res->datum_tx == NULL) {
        PyObject_DEL(res);
        Py_RETURN_NONE;
    }
    return Py_BuildValue("O", res);
}
PyObject *dap_chain_ledger_tx_find_by_addr_py(PyObject *self, PyObject *args){
    const char *token;
    PyObject *addr;
    PyObject *first_hash;
    if (!PyArg_ParseTuple(args, "s|O|O", &token, &addr, &first_hash))
        return NULL;
    PyDapChainDatumTxObject *res = PyObject_New(PyDapChainDatumTxObject, &DapChainDatumTxObjectType);
    res->datum_tx = dap_ledger_tx_find_by_addr(((PyDapChainLedgerObject*)self)->ledger, token, PY_DAP_CHAIN_ADDR(addr), ((PyDapHashFastObject*)first_hash)->hash_fast, false);
    res->original = false;
    
    return (PyObject*)res;
}

static char*** ListStringToArrayStringFormatChar(PyObject *list){
    Py_ssize_t size = PyList_Size(list);
    char ***data = calloc(sizeof(char**), (size_t)size);
    if(!data) {
        log_it(L_CRITICAL, "Memory allocation error");
        return NULL;
    }
    for (Py_ssize_t i = 0; i < size; i++){
        PyObject *obj_two = PyList_GetItem(list,i);
        Py_ssize_t size_seentenses = PyList_Size(obj_two);
        char **sentences = calloc(sizeof(char**), (size_t)size_seentenses);
        if(!sentences) {
        log_it(L_CRITICAL, "Memory allocation error");
            DAP_DELETE(data);
            return NULL;
        }
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
    if(!res_size_t) {
        log_it(L_CRITICAL, "Memory allocation error");
        return NULL;
    }
    for (Py_ssize_t i=0; i<size;i++){
        PyObject *obj = PyList_GetItem(list, i);
        res_size_t[i] = (size_t)PyLong_AsSsize_t(obj);
    }
    return res_size_t;
}

PyObject *dap_chain_ledger_get_txs_py(PyObject *self, PyObject *args){
    size_t count, page;
    PyObject *obj_reverse, *obj_unspent;
    if (!PyArg_ParseTuple(args, "nnOO",&count, &page, &obj_reverse, &obj_unspent)){
        return NULL;
    }
    if (!PyBool_Check(obj_reverse)){
        PyErr_SetString(PyExc_AttributeError, "");
        return NULL;
    }
    bool    reverse = obj_reverse == Py_True ? true : false,
            unspent = obj_unspent == Py_True ? true : false;
    dap_list_t *l_txs = dap_ledger_get_txs(
            ((PyDapChainLedgerObject*)self)->ledger,
            count,
            page,
            reverse, unspent);
    if (!l_txs){
        Py_RETURN_NONE;
    }
    PyObject *obj_list = PyList_New(dap_list_length(l_txs));
    size_t i = 0;
    for (dap_list_t *l_iter = l_txs; l_iter != NULL; l_iter = l_iter->next, ++i) {
        PyDapChainDatumTxObject *obj_tx = PyObject_New(PyDapChainDatumTxObject, &DapChainDatumTxObjectType);
        obj_tx->datum_tx = l_iter->data;
        obj_tx->original = false;
        PyList_SetItem(obj_list, i, (PyObject*)obj_tx);
    }
    dap_list_free(l_txs);
    return obj_list;
}

typedef struct pvt_ledger_notify{
    PyObject *func;
    PyObject *argv;
}pvt_ledger_notify_t;

typedef struct {
    pvt_ledger_notify_t callback_data;
    dap_ledger_t *ledger;
    dap_chain_datum_tx_t *datum_tx;
    size_t datum_tx_size;
    dap_ledger_notify_opcodes_t opcode;
} pvt_ledger_notify_thread_args_t;

static bool pvt_ledger_notify_proc_callback(void *a_arg) {
    pvt_ledger_notify_thread_args_t *l_args = (pvt_ledger_notify_thread_args_t*)a_arg;
    
    if (!l_args) {
        log_it(L_ERROR, "Null args passed to ledger notifier callback");
        return false;
    }
    
    log_it(L_DEBUG, "[GIL-DEBUG] Ledger notifier proc acquire thread=%lu args=%p datum_tx=%p", 
           (unsigned long)pthread_self(), l_args, l_args->datum_tx);
    PyGILState_STATE state = PyGILState_Ensure();
    log_it(L_DEBUG, "[GIL-DEBUG] Ledger notifier proc acquired state=%d thread=%lu", state, (unsigned long)pthread_self());
    
    PyDapChainLedgerObject *obj_ledger = NULL;
    PyDapChainDatumTxObject *obj_tx = NULL;
    PyObject *notify_arg = NULL;
    PyObject *argv = NULL;
    PyObject *result = NULL;
    
    // Validate callback data before proceeding
    if (!l_args->callback_data.func || !PyCallable_Check(l_args->callback_data.func)) {
        log_it(L_ERROR, "Invalid callback function in ledger notifier");
        goto cleanup;
    }
    
    obj_ledger = PyObject_NEW(PyDapChainLedgerObject, &DapChainLedgerObjectType);
    if (!obj_ledger) {
        log_it(L_ERROR, "Failed to create ledger object in notifier");
        goto cleanup;
    }
    // Initialize the object properly
    obj_ledger->ledger = l_args->ledger;
    
    obj_tx = PyObject_NEW(PyDapChainDatumTxObject, &DapChainDatumTxObjectType);
    if (!obj_tx) {
        log_it(L_ERROR, "Failed to create datum tx object in notifier");
        goto cleanup;
    }
    // Initialize the object properly
    obj_tx->datum_tx = l_args->datum_tx;
    obj_tx->original = true;  // Set to true to prevent destructor from freeing datum_tx
    
    // Handle notify_arg more carefully
    if (l_args->callback_data.argv && l_args->callback_data.argv != Py_None) {
        notify_arg = l_args->callback_data.argv;
        Py_INCREF(notify_arg);
    } else {
        notify_arg = Py_None;
        Py_INCREF(notify_arg);
    }
    
    argv = Py_BuildValue("OOO", (PyObject*)obj_ledger, (PyObject*)obj_tx, notify_arg);
    if (!argv) {
        log_it(L_ERROR, "Failed to build arguments for notifier callback");
        goto cleanup;
    }
    
    log_it(L_DEBUG, "Call tx added ledger notifier for net %s", l_args->ledger->net->pub.name);
    
    // Make sure the function is still valid before calling it
    if (PyCallable_Check(l_args->callback_data.func)) {
        result = PyObject_CallObject(l_args->callback_data.func, argv);
        if (!result) {
            log_it(L_ERROR, "Python callback failed in ledger notifier");
            python_error_in_log_it(LOG_TAG);
        }
    } else {
        log_it(L_ERROR, "Callback function became invalid during execution");
    }
    
cleanup:
    // Proper cleanup of all references
    Py_XDECREF(result);
    Py_XDECREF(argv);
    Py_XDECREF(obj_tx);      // Clean up created tx object
    Py_XDECREF(obj_ledger);  // Clean up created ledger object
    Py_XDECREF(notify_arg);  // Clean up notify arg (Py_None or callback arg)
    Py_XDECREF(l_args->callback_data.func);
    Py_XDECREF(l_args->callback_data.argv);
    
    log_it(L_DEBUG, "[GIL-DEBUG] Ledger notifier proc release thread=%lu", (unsigned long)pthread_self());
    PyGILState_Release(state);
    
    // Clean up allocated memory - only once with safety checks
    log_it(L_DEBUG, "Freeing datum_tx=%p and args=%p", l_args->datum_tx, l_args);
    if (l_args->datum_tx) {
        DAP_DELETE(l_args->datum_tx);
        l_args->datum_tx = NULL;  // Prevent double free
    }
    if (l_args) {
        DAP_DELETE(l_args);
        l_args = NULL;  // Prevent double free (though not accessible after)
    }
    return false;
}

static void pvt_wrapping_dap_chain_ledger_tx_add_notify(void *a_arg, dap_ledger_t *a_ledger,
                                                        dap_chain_datum_tx_t *a_tx, dap_ledger_notify_opcodes_t a_opcode){
    if (!a_arg)
        return;
    if (a_opcode == DAP_LEDGER_NOTIFY_OPCODE_ADDED){
        pvt_ledger_notify_t *notifier = (pvt_ledger_notify_t*)a_arg;
        
        // Validate notifier data first
        if (!notifier->func || !PyCallable_Check(notifier->func)) {
            log_it(L_ERROR, "Invalid notifier function");
            return;
        }
        
        // Create thread-safe copy of arguments
        pvt_ledger_notify_thread_args_t *l_args = DAP_NEW_Z(pvt_ledger_notify_thread_args_t);
        if (!l_args) {
            log_it(L_ERROR, "Memory allocation failed for ledger notifier args");
            return;
        }
        
        size_t l_tx_size = dap_chain_datum_tx_get_size(a_tx);
        l_args->ledger = a_ledger;
        l_args->datum_tx = DAP_DUP_SIZE(a_tx, l_tx_size);
        if (!l_args->datum_tx) {
            log_it(L_ERROR, "Failed to duplicate transaction data");
            DAP_DELETE(l_args);
            return;
        }
        l_args->datum_tx_size = l_tx_size;
        l_args->opcode = a_opcode;
        
        // Copy callback data with proper reference counting - ensure we have a copy
        l_args->callback_data.func = notifier->func;
        l_args->callback_data.argv = notifier->argv;
        Py_XINCREF(l_args->callback_data.func);
        Py_XINCREF(l_args->callback_data.argv);
        
        log_it(L_DEBUG, "Creating ledger notifier args=%p datum_tx=%p size=%zu for net %s", 
               l_args, l_args->datum_tx, l_tx_size, a_ledger->net->pub.name);
        
        // Schedule callback in proc thread
        dap_proc_thread_callback_add(NULL, pvt_ledger_notify_proc_callback, l_args);
    }
}

PyObject *dap_chain_ledger_tx_add_notify_py(PyObject *self, PyObject *args) {
    PyObject *obj_func, *obj_argv = NULL;
    if (!PyArg_ParseTuple(args, "O|O", &obj_func, &obj_argv)) {
        return NULL;
    }
    if (!PyCallable_Check(obj_func)) {
        PyErr_SetString(PyExc_AttributeError, "This function, as the first argument, must take the"
                                              " function called by the callback.");
        return NULL;
    }
    pvt_ledger_notify_t *notifier = DAP_NEW(pvt_ledger_notify_t);
    if(!notifier) {
        log_it(L_CRITICAL, "Memory allocation error");
        return NULL;
    }
    notifier->func = obj_func;
    notifier->argv = obj_argv;
    Py_INCREF(obj_func);
    Py_XINCREF(obj_argv);
    dap_ledger_tx_add_notify(((PyDapChainLedgerObject*)self)->ledger, pvt_wrapping_dap_chain_ledger_tx_add_notify, notifier);
    Py_RETURN_NONE;
}

struct py_notifier_callback_args {
    dap_ledger_t *ledger;
    dap_chain_datum_tx_t *tx;
    dap_hash_fast_t tx_hash;
    void *arg;
};

static bool s_python_obj_notifier(void *a_arg)
{
    struct py_notifier_callback_args *l_args = a_arg;
    pvt_ledger_notify_t *l_notifier = l_args->arg;
    PyDapChainLedgerObject *obj_ledger = PyObject_NEW(PyDapChainLedgerObject, &DapChainLedgerObjectType);
    obj_ledger->ledger = l_args->ledger;
    PyDapChainDatumTxObject *obj_tx = PyObject_NEW(PyDapChainDatumTxObject, &DapChainDatumTxObjectType);
    obj_tx->datum_tx = l_args->tx;
    obj_tx->original = true;
    PyObject *l_notify_arg = !l_notifier->argv ? Py_None : l_notifier->argv;
    Py_INCREF(l_notify_arg);
    log_it(L_DEBUG, "Call bridged tx ledger notifier for net %s", l_args->ledger->net->pub.name);
    PyGILState_STATE state = PyGILState_Ensure();
    PyObject *obj_argv = Py_BuildValue("OOO", obj_ledger, obj_tx, l_notify_arg);
    Py_INCREF(l_notifier->func);
    PyObject *result = PyEval_CallObject(l_notifier->func, obj_argv);
    Py_XDECREF(l_notifier->func);
    Py_DECREF(obj_argv);
    Py_DECREF(obj_ledger);
    Py_DECREF(obj_tx);
    Py_DECREF(l_notify_arg);
    if (!result)
        python_error_in_log_it(LOG_TAG);
    Py_XDECREF(result);
    DAP_DELETE(l_args->tx);
    DAP_DELETE(l_args);
    PyGILState_Release(state);
    return false;
}

static void s_python_proc_notifier(dap_ledger_t *a_ledger, dap_chain_datum_tx_t *a_tx, dap_hash_fast_t *a_tx_hash, void *a_arg, dap_ledger_notify_opcodes_t a_opcode)
{
    if (!a_arg)
        return;
    
    struct py_notifier_callback_args *l_args = DAP_NEW_Z(struct py_notifier_callback_args);
    l_args->ledger = a_ledger;
    l_args->tx = DAP_DUP_SIZE(a_tx, dap_chain_datum_tx_get_size(a_tx));
    l_args->tx_hash = *a_tx_hash;
    l_args->arg = a_arg;
    dap_proc_thread_callback_add(NULL, s_python_obj_notifier, l_args);
}

static PyObject *s_bridged_tx_notify_add(PyObject *self, PyObject *args)
{
    PyObject *obj_func, *obj_argv = NULL;
    if (!PyArg_ParseTuple(args, "O|O", &obj_func, &obj_argv)) {
        return NULL;
    }
    if (!PyCallable_Check(obj_func)) {
        PyErr_SetString(PyExc_AttributeError, "This function, as the first argument, must take the"
                                              " function called by the callback.");
        return NULL;
    }
    pvt_ledger_notify_t *l_notifier = DAP_NEW(pvt_ledger_notify_t);
    if (!l_notifier) {
        log_it(L_CRITICAL, "Memory allocation error");
        return NULL;
    }
    l_notifier->func = obj_func;
    l_notifier->argv = obj_argv;
    Py_INCREF(obj_func);
    Py_XINCREF(obj_argv);
    dap_ledger_bridged_tx_notify_add(((PyDapChainLedgerObject*)self)->ledger, s_python_proc_notifier, l_notifier);
    Py_RETURN_NONE;
}

PyObject *dap_chain_ledger_tx_get_main_ticker_py(PyObject *self, PyObject *args)
{
    PyObject *l_obj_tx = NULL;
    if (!PyArg_ParseTuple(args, "O", &l_obj_tx)) {
         PyErr_SetString(PyExc_AttributeError, "This function, as the first argument, accepts DatumTx.");
        return NULL;
    }
    
    PyDapChainDatumTxObject *obj_tx = (PyDapChainDatumTxObject *)l_obj_tx;

    int l_ledger_rc = DAP_LEDGER_CHECK_INVALID_ARGS;
    const char * ticker = dap_ledger_tx_calculate_main_ticker(((PyDapChainLedgerObject*)self)->ledger, obj_tx->datum_tx, &l_ledger_rc);
    return Py_BuildValue("(s,i)", ticker ? ticker : "UNKWNOWN", l_ledger_rc);
}

PyObject *dap_chain_ledger_tx_hash_is_used_out_item_hash_py(PyObject *self, PyObject *args){
    PyObject *tx_hash;
    uint64_t idx;
    if (!PyArg_ParseTuple(args, "OK", &tx_hash, &idx)) {
        return NULL;
    }
    dap_hash_fast_t l_spender_hash = {0};
    if (dap_ledger_tx_hash_is_used_out_item(((PyDapChainLedgerObject*)self)->ledger, ((PyDapHashFastObject*)tx_hash)->hash_fast, idx, &l_spender_hash)){
        PyDapHashFastObject *obj_hf = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
        obj_hf->hash_fast = DAP_NEW(dap_hash_fast_t);
        memcpy(obj_hf->hash_fast, &l_spender_hash, sizeof(dap_hash_fast_t));
        obj_hf->origin = true;
        return (PyObject*)obj_hf;
    }
    Py_RETURN_NONE;
}


static PyObject *dap_chain_ledger_get_final_multi_wallet_tx_hash_py(PyObject *self, PyObject *args)
{
    PyObject *py_base_hash = NULL;
    int       subtype      = 0; 
    int       with_refills = 0; 

    if (!PyArg_ParseTuple(args, "Oi|p", &py_base_hash, &subtype, &with_refills))
        return NULL;

    if (!PyObject_TypeCheck(py_base_hash, &DapChainHashFastObjectType)) {
        PyErr_SetString(PyExc_TypeError,
            "First arg must be CellFrame.HashFast object");
        return NULL;
    }

    dap_hash_fast_t *base =
        ((PyDapHashFastObject *)py_base_hash)->hash_fast;

    dap_hash_fast_t final =
        dap_ledger_get_final_chain_tx_hash(
            ((PyDapChainLedgerObject *)self)->ledger,
            (dap_chain_tx_out_cond_subtype_t)subtype,
            base,
            with_refills);

    PyDapHashFastObject *py_final =
        PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    if (!py_final)
        return PyErr_NoMemory();

    py_final->hash_fast = DAP_NEW(dap_hash_fast_t);
    memcpy(py_final->hash_fast, &final, sizeof(final));
    py_final->origin = true;

    return (PyObject *)py_final;
}

PyObject *dap_chain_ledger_get_unspent_outputs_for_amount_py(PyObject *self, PyObject *args) {
    PyObject   *py_addr            = NULL;
    const char *token_ticker       = NULL;
    PyObject   *py_required_amount = NULL;

    if (!PyArg_ParseTuple(args, "Os|O",
                          &py_addr,
                          &token_ticker,
                          &py_required_amount))
        return NULL;

    uint256_t required_amount = {0};
    if (py_required_amount && py_required_amount != Py_None) {
        if (!PyLong_Check(py_required_amount)) {
            PyErr_SetString(PyExc_TypeError,
                            "required_amount must be int or None");
            return NULL;
        }
        PyObject *py_str = PyObject_Str(py_required_amount);
        if (!py_str)
            return NULL;
        required_amount = dap_chain_balance_scan(
            PyUnicode_AsUTF8(py_str));
        Py_DECREF(py_str);
    }

    dap_list_t *used_out_list = dap_chain_wallet_get_list_tx_outs_with_val(
        ((PyDapChainLedgerObject*)self)->ledger,
        token_ticker,
        ((PyDapChainAddrObject*)py_addr)->addr,
        required_amount,
        NULL);

    if (!used_out_list)
        return PyList_New(0);

    const size_t list_length = dap_list_length(used_out_list);
    PyObject *py_result_list = PyList_New(list_length);
    if (!py_result_list) {
        dap_list_free_full(used_out_list, free);
        return NULL;
    }

    size_t            py_index = 0;
    dap_list_t       *el;
    LL_FOREACH(used_out_list, el) {
        dap_chain_tx_used_out_item_t *out_item =
            (dap_chain_tx_used_out_item_t *)el->data;

        PyDapHashFastObject *py_hash =
            PyObject_New(PyDapHashFastObject,
                         &DapChainHashFastObjectType);
        if (!py_hash) {
            Py_DECREF(py_result_list);
            dap_list_free_full(used_out_list, free);
            return NULL;
        }
        py_hash->hash_fast = DAP_DUP(&out_item->tx_hash_fast);
        py_hash->origin    = true;

        const char *value_str = dap_uint256_to_char(out_item->value, NULL);
        PyObject   *py_value  = PyLong_FromString((char*)value_str,
                                                  NULL, 10);
        if (!py_value) {
            Py_DECREF(py_hash);
            Py_DECREF(py_result_list);
            dap_list_free_full(used_out_list, free);
            return NULL;
        }

        PyObject *py_tuple = Py_BuildValue("OIO",
                                           py_hash,
                                           out_item->num_idx_out,
                                           py_value);
        Py_DECREF(py_hash);
        Py_DECREF(py_value);
        if (!py_tuple) {
            Py_DECREF(py_result_list);
            dap_list_free_full(used_out_list, free);
            return NULL;
        }

        PyList_SET_ITEM(py_result_list, py_index++, py_tuple);
    }

    dap_list_free_full(used_out_list, free);
    return py_result_list;
}

// Event functions
PyObject *dap_chain_ledger_event_find_py(PyObject *self, PyObject *args){
    PyObject *obj_hash;
    if (!PyArg_ParseTuple(args, "O", &obj_hash))
        return NULL;
    
    dap_hash_fast_t *l_hash = ((PyDapHashFastObject *)obj_hash)->hash_fast;
    dap_chain_tx_event_t *l_event = dap_ledger_event_find(((PyDapChainLedgerObject *)self)->ledger, l_hash);
    
    if (!l_event) {
        Py_RETURN_NONE;
    }
    
    PyDapChainLedgerEventObject *obj_event = PyObject_New(PyDapChainLedgerEventObject, &DapChainLedgerEventObjectType);
    obj_event->event = l_event;
    obj_event->origin = true;
    return (PyObject *)obj_event;
}

PyObject *dap_chain_ledger_event_get_list_py(PyObject *self, PyObject *args){
    const char *group_name = NULL;
    if (!PyArg_ParseTuple(args, "|s", &group_name))
        return NULL;
    
    dap_list_t *l_list = dap_ledger_event_get_list(((PyDapChainLedgerObject *)self)->ledger, group_name);
    if (!l_list) {
        PyObject *l_list_empty = PyList_New(0);
        return l_list_empty;
    }
    
    PyObject *l_list_obj = PyList_New(0);
    for (dap_list_t *l_iter = l_list; l_iter; l_iter = l_iter->next) {
        dap_chain_tx_event_t *l_event = (dap_chain_tx_event_t *)l_iter->data;
        PyDapChainLedgerEventObject *obj_event = PyObject_New(PyDapChainLedgerEventObject, &DapChainLedgerEventObjectType);
        obj_event->event = l_event;
        obj_event->origin = false; // The events will be freed by dap_list_free_full
        PyList_Append(l_list_obj, (PyObject *)obj_event);
        Py_DECREF(obj_event);
    }
    
    // Free the list but not the events (they will be freed by the Python objects)
    dap_list_free(l_list);
    
    return l_list_obj;
}

PyObject *dap_chain_ledger_event_pkey_list_py(PyObject *self, PyObject *args){
    (void)args;
    dap_list_t *l_list = dap_ledger_event_pkey_list(((PyDapChainLedgerObject *)self)->ledger);
    if (!l_list) {
        PyObject *l_list_empty = PyList_New(0);
        return l_list_empty;
    }
    
    PyObject *l_list_obj = PyList_New(0);
    for (dap_list_t *l_iter = l_list; l_iter; l_iter = l_iter->next) {
        dap_hash_fast_t *l_hash = (dap_hash_fast_t *)l_iter->data;
        PyDapHashFastObject *obj_hash = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
        obj_hash->hash_fast = DAP_DUP(l_hash);
        obj_hash->origin = true;
        PyList_Append(l_list_obj, (PyObject *)obj_hash);
        Py_DECREF(obj_hash);
    }
    
    // Free the list and the hashes
    dap_list_free_full(l_list, free);
    
    return l_list_obj;
}

// Event object methods
PyObject *dap_chain_ledger_event_get_group_name_py(PyObject *self, void *closure) {
    (void)closure;
    PyDapChainLedgerEventObject *obj = (PyDapChainLedgerEventObject *)self;
    if (!obj->event) {
        Py_RETURN_NONE;
    }
    return PyUnicode_FromString(obj->event->group_name);
}

PyObject *dap_chain_ledger_event_get_type_py(PyObject *self, void *closure) {
    (void)closure;
    PyDapChainLedgerEventObject *obj = (PyDapChainLedgerEventObject *)self;
    if (!obj->event) {
        Py_RETURN_NONE;
    }
    return PyLong_FromLong(obj->event->event_type);
}

PyObject *dap_chain_ledger_event_get_data_py(PyObject *self, void *closure) {
    (void)closure;
    PyDapChainLedgerEventObject *obj = (PyDapChainLedgerEventObject *)self;
    if (!obj->event || !obj->event->event_data_size) {
        Py_RETURN_NONE;
    }
    return PyBytes_FromStringAndSize((char *)obj->event->event_data, obj->event->event_data_size);
}

PyObject *dap_chain_ledger_event_get_timestamp_py(PyObject *self, void *closure) {
    (void)closure;
    PyDapChainLedgerEventObject *obj = (PyDapChainLedgerEventObject *)self;
    if (!obj->event) {
        Py_RETURN_NONE;
    }
    return PyLong_FromLong(obj->event->timestamp);
}

// Структура для хранения Python-колбэка и его аргументов
typedef struct dap_chain_ledger_event_py_callback {
    PyObject *py_callback;
    PyObject *py_arg;
} dap_chain_ledger_event_py_callback_t;

// Функция-обёртка для вызова Python-колбэка из C
static void s_ledger_event_py_callback(void *a_arg, dap_ledger_t *a_ledger, dap_chain_tx_event_t *a_event, 
                                      dap_hash_fast_t *a_tx_hash, dap_ledger_notify_opcodes_t a_opcode)
{
    dap_chain_ledger_event_py_callback_t *l_py_cb = (dap_chain_ledger_event_py_callback_t *)a_arg;
    PyGILState_STATE l_gil_state;
    l_gil_state = PyGILState_Ensure();

    // Создаем объект события для передачи в Python-колбэк
    PyDapChainLedgerEventObject *l_event_obj = PyObject_New(PyDapChainLedgerEventObject, &DapChainLedgerEventObjectType);
    if (!l_event_obj) {
        log_it(L_ERROR, "Can't create Python event object in callback");
        PyGILState_Release(l_gil_state);
        return;
    }
    
    // Создаем копию события, так как оно будет освобождено после вызова колбэка
    l_event_obj->event = DAP_NEW_Z(dap_chain_tx_event_t);
    if (!l_event_obj->event) {
        log_it(L_ERROR, "Memory allocation error in Python event callback");
        Py_DECREF(l_event_obj);
        PyGILState_Release(l_gil_state);
        return;
    }
    
    // Копируем данные события
    *l_event_obj->event = *a_event;
    l_event_obj->event->group_name = dap_strdup(a_event->group_name);
    if (a_event->event_data_size > 0 && a_event->event_data)
        l_event_obj->event->event_data = DAP_DUP_SIZE(a_event->event_data, a_event->event_data_size);
    else {
        l_event_obj->event->event_data = NULL;
        l_event_obj->event->event_data_size = 0;
    }
    
    l_event_obj->origin = true;
    
    // Создаем объект хэша транзакции
    PyDapHashFastObject *l_hash_obj = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    if (!l_hash_obj) {
        log_it(L_ERROR, "Can't create Python hash object in callback");
        Py_DECREF(l_event_obj);
        PyGILState_Release(l_gil_state);
        return;
    }
    l_hash_obj->hash_fast = DAP_NEW(dap_hash_fast_t);
    if (!l_hash_obj->hash_fast) {
        log_it(L_ERROR, "Memory allocation error in Python event callback");
        Py_DECREF(l_event_obj);
        Py_DECREF(l_hash_obj);
        PyGILState_Release(l_gil_state);
        return;
    }
    memcpy(l_hash_obj->hash_fast, a_tx_hash, sizeof(dap_hash_fast_t));
    l_hash_obj->origin = true;
    
    // Создаем объект леджера
    PyDapChainLedgerObject *l_ledger_obj = PyObject_New(PyDapChainLedgerObject, &DapChainLedgerObjectType);
    if (!l_ledger_obj) {
        log_it(L_ERROR, "Can't create Python ledger object in callback");
        Py_DECREF(l_event_obj);
        Py_DECREF(l_hash_obj);
        PyGILState_Release(l_gil_state);
        return;
    }
    l_ledger_obj->ledger = a_ledger;
    
    // Создаем аргументы для вызова Python-колбэка
    PyObject *l_args = Py_BuildValue("OOOci", l_ledger_obj, l_event_obj, l_hash_obj, a_opcode, l_py_cb->py_arg);
    if (!l_args) {
        log_it(L_ERROR, "Can't build arguments for Python callback");
        Py_DECREF(l_event_obj);
        Py_DECREF(l_hash_obj);
        Py_DECREF(l_ledger_obj);
        PyGILState_Release(l_gil_state);
        return;
    }
    
    // Вызываем Python-колбэк
    PyObject *l_result = PyObject_CallObject(l_py_cb->py_callback, l_args);
    if (!l_result) {
        PyErr_Print();
        log_it(L_ERROR, "Error calling Python callback for ledger event");
    } else {
        Py_DECREF(l_result);
    }
    
    Py_DECREF(l_args);
    Py_DECREF(l_ledger_obj);
    Py_DECREF(l_hash_obj);
    Py_DECREF(l_event_obj);
    
    PyGILState_Release(l_gil_state);
}

// Функция для освобождения ресурсов Python-колбэка
static void s_ledger_event_py_callback_delete(void *a_arg)
{
    if (!a_arg)
        return;
        
    dap_chain_ledger_event_py_callback_t *l_py_cb = (dap_chain_ledger_event_py_callback_t *)a_arg;
    
    PyGILState_STATE l_gil_state;
    l_gil_state = PyGILState_Ensure();
    
    Py_XDECREF(l_py_cb->py_callback);
    Py_XDECREF(l_py_cb->py_arg);
    
    PyGILState_Release(l_gil_state);
    
    DAP_DELETE(l_py_cb);
}

// Python-функция для регистрации колбэка событий
PyObject *dap_chain_ledger_event_notify_add_py(PyObject *self, PyObject *args)
{
    PyObject *py_callback;
    PyObject *py_arg = Py_None;
    
    if (!PyArg_ParseTuple(args, "O|O", &py_callback, &py_arg))
        return NULL;
    
    if (!PyCallable_Check(py_callback)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be callable");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = ((PyDapChainLedgerObject *)self)->ledger;
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger object");
        return NULL;
    }
    
    // Создаем структуру для хранения Python-колбэка и его аргументов
    dap_chain_ledger_event_py_callback_t *l_py_cb = DAP_NEW_Z(dap_chain_ledger_event_py_callback_t);
    if (!l_py_cb) {
        PyErr_SetString(PyExc_MemoryError, "Memory allocation error");
        return NULL;
    }
    
    // Сохраняем ссылки на Python-объекты
    l_py_cb->py_callback = py_callback;
    Py_INCREF(py_callback);
    
    l_py_cb->py_arg = py_arg;
    Py_INCREF(py_arg);
    
    // Регистрируем C-колбэк, который будет вызывать Python-колбэк
    dap_ledger_event_notify_add(l_ledger, s_ledger_event_py_callback, l_py_cb);
    
    Py_RETURN_NONE;
}
