#include "wrapping_dap_chain_ledger.h"
#include "wrapping_dap_chain_atom_ptr.h"
#include "python-cellframe_common.h"
#include "dap_proc_thread.h"
#include "dap_events.h"
#include "dap_chain_datum_tx_in.h"
#include "dap_chain_net.h"
#include "dap_hash.h"
#include "utlist.h"

#define LOG_TAG "ledger wrapper"

static PyObject *s_bridged_tx_notify_add(PyObject *self, PyObject *args);
static PyObject *dap_chain_ledger_get_final_multi_wallet_tx_hash_py(PyObject *self, PyObject *args);

static PyMethodDef DapChainLedgerMethods[] = {
        {"setLocalCellId", (PyCFunction)dap_chain_ledger_set_local_cell_id_py, METH_VARARGS, ""},
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
        {}
};

PyTypeObject DapChainLedgerObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.ChainLedger", sizeof(PyDapChainLedgerObject),
        "Chain ledger objects",
        .tp_methods = DapChainLedgerMethods);

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

PyObject *dap_chain_ledger_set_local_cell_id_py(PyObject *self, PyObject *args){
    PyObject *obj_local_cell_id;
    if (!PyArg_ParseTuple(args, "O", &obj_local_cell_id))
        return NULL;
    dap_ledger_set_local_cell_id(((PyDapChainLedgerObject*)self)->ledger,
                                       ((PyDapChainCellIDObject*)obj_local_cell_id)->cell_id);
    return  PyLong_FromLong(0);
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
        l_addr = ((PyDapChainAddrObject*)obj_addr)->addr;
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
            ((PyDapChainAddrObject*)addr)->addr,
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
        ((PyDapChainAddrObject*)addr)->addr, token_ticker);
    return (PyObject*)l_obj_balance;
}
PyObject *dap_chain_ledger_tx_find_by_hash_py(PyObject *self, PyObject *args){
    PyObject *h_fast;
    if (!PyArg_ParseTuple(args, "O", &h_fast))
        return NULL;
    
    dap_ledger_t *ledger = ((PyDapChainLedgerObject*)self)->ledger;
    dap_hash_fast_t *hash_fast = ((PyDapHashFastObject*)h_fast)->hash_fast;

    dap_chain_datum_t *full_datum = NULL;
    dap_chain_hash_fast_t block_hash = {0};
    int ret_code = 0;
    dap_chain_atom_ptr_t atom = NULL;
    size_t atom_size = 0;
    
    dap_chain_t *main_chain = dap_chain_net_get_chain_by_name(ledger->net, "main");
    
    if (ledger->net && main_chain && main_chain->callback_datum_find_by_hash) {
        full_datum = main_chain->callback_datum_find_by_hash(
            main_chain, hash_fast, &block_hash, &ret_code);
        
        if (full_datum && !dap_hash_fast_is_blank(&block_hash)) {
            atom = dap_chain_get_atom_by_hash(main_chain, &block_hash, &atom_size);
        }
    }
    
    PyObject *obj_datum = NULL;
    PyObject *obj_atom = NULL;
    
    if (full_datum) {
        PyDapChainDatumObject *obj_datum_full = PyObject_NEW(PyDapChainDatumObject, &DapChainDatumObjectType);
        if (!obj_datum_full) {
            PyErr_SetString(PyExc_MemoryError, "Failed to create datum object");
            return NULL;
        }
        obj_datum_full->datum = full_datum;
        obj_datum_full->origin = false;
        obj_datum = (PyObject*)obj_datum_full;
        
        if (atom) {
            PyChainAtomObject *obj_atom_ptr = PyObject_NEW(PyChainAtomObject, &DapChainAtomPtrObjectType);
            if (!obj_atom_ptr) {
                Py_DECREF(obj_datum);
                PyErr_SetString(PyExc_MemoryError, "Failed to create atom object");
                return NULL;
            }
            obj_atom_ptr->atom = atom;
            obj_atom_ptr->atom_size = atom_size;
            obj_atom = (PyObject*)obj_atom_ptr;
        } else {
            obj_atom = Py_None;
            Py_INCREF(Py_None);
        }
        
        return Py_BuildValue("OO", obj_datum, obj_atom);
    } else {
        PyDapChainDatumTxObject *res = PyObject_NEW(PyDapChainDatumTxObject, &DapChainDatumTxObjectType);
        if (!res) {
            PyErr_SetString(PyExc_MemoryError, "Failed to create tx object");
            return NULL;
        }
        
        res->datum_tx = dap_ledger_tx_find_by_hash(ledger, hash_fast);
        res->original = false;
        
        if (res->datum_tx == NULL) {
            PyObject_DEL(res);
            Py_RETURN_NONE;
        }
        
        obj_datum = (PyObject*)res;
        obj_atom = Py_None;
        Py_INCREF(Py_None);
        
        return Py_BuildValue("OO", obj_datum, obj_atom);
    }
}
PyObject *dap_chain_ledger_tx_find_by_addr_py(PyObject *self, PyObject *args){
    const char *token;
    PyObject *addr;
    PyObject *first_hash;
    if (!PyArg_ParseTuple(args, "s|O|O", &token, &addr, &first_hash))
        return NULL;
    PyDapChainDatumTxObject *res = PyObject_New(PyDapChainDatumTxObject, &DapChainDatumTxObjectType);
    res->datum_tx = dap_ledger_tx_find_by_addr(((PyDapChainLedgerObject*)self)->ledger, token, ((PyDapChainAddrObject*)addr)->addr, ((PyDapHashFastObject*)first_hash)->hash_fast, false);
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

static void pvt_wrapping_dap_chain_ledger_tx_add_notify(void *a_arg, dap_ledger_t *a_ledger, dap_chain_datum_tx_t *a_tx, 
                                                        dap_hash_fast_t *a_tx_hash, dap_chan_ledger_notify_opcodes_t a_opcode){
    if (!a_arg)
        return;
    if (a_opcode == DAP_LEDGER_NOTIFY_OPCODE_ADDED){
        char tx_hash_str[DAP_CHAIN_HASH_FAST_STR_SIZE];
        dap_chain_hash_fast_to_str(a_tx_hash, tx_hash_str, sizeof(tx_hash_str));
        log_it(L_INFO, "TX Notificator called for tx hash: %s, net: %s", 
               tx_hash_str, a_ledger->net->pub.name);
        
        pvt_ledger_notify_t *notifier = (pvt_ledger_notify_t*)a_arg;
        PyGILState_STATE state = PyGILState_Ensure();
        
        dap_chain_datum_t *full_datum = NULL;
        dap_chain_hash_fast_t block_hash = {0};
        int ret_code = 0;
        dap_chain_atom_ptr_t atom = NULL;
        size_t atom_size = 0;
        
        dap_chain_t *main_chain = dap_chain_net_get_chain_by_name(a_ledger->net, "main");
        log_it(L_INFO, "DIAGNOSTIC: Retrieved main chain: %p for net: %s", main_chain, a_ledger->net->pub.name);
        
        // TIMING ANALYSIS: Check if this is a race condition
        uint64_t timing_start = dap_nanotime_now();
        
        // DIAGNOSTIC: Check why callback might be NULL
        if (!a_ledger->net) {
            log_it(L_WARNING, "DIAGNOSTIC: a_ledger->net is NULL for tx %s", tx_hash_str);
        } else if (!main_chain) {
            log_it(L_WARNING, "DIAGNOSTIC: main_chain is NULL for net %s, tx %s", a_ledger->net->pub.name, tx_hash_str);
        } else if (!main_chain->callback_datum_find_by_hash) {
            log_it(L_WARNING, "DIAGNOSTIC: main_chain->callback_datum_find_by_hash is NULL for chain %s, tx %s", 
                   main_chain->name, tx_hash_str);
        }
        
        if (a_ledger->net && main_chain && main_chain->callback_datum_find_by_hash) {
            log_it(L_INFO, "DIAGNOSTIC: Chain callback available, searching for full datum in chain for tx %s", tx_hash_str);
                   
            full_datum = main_chain->callback_datum_find_by_hash(
                main_chain, a_tx_hash, &block_hash, &ret_code);
            
            uint64_t timing_after_search = dap_nanotime_now();
            log_it(L_INFO, "DIAGNOSTIC: callback_datum_find_by_hash completed in %lu ns: datum=%p, ret_code=%d for tx %s", 
                   timing_after_search - timing_start, full_datum, ret_code, tx_hash_str);
            
            if (full_datum) {
                char block_hash_str[DAP_CHAIN_HASH_FAST_STR_SIZE];
                dap_chain_hash_fast_to_str(&block_hash, block_hash_str, sizeof(block_hash_str));
                log_it(L_INFO, "DIAGNOSTIC: Found full datum for tx %s, ret_code: %d, block_hash: %s", 
                       tx_hash_str, ret_code, block_hash_str);
                
                if (!dap_hash_fast_is_blank(&block_hash)) {
                    atom = dap_chain_get_atom_by_hash(main_chain, &block_hash, &atom_size);
                    log_it(L_INFO, "DIAGNOSTIC: dap_chain_get_atom_by_hash returned: atom=%p, atom_size=%zu for tx %s", 
                           atom, atom_size, tx_hash_str);
                }
            } else {
                log_it(L_INFO, "DIAGNOSTIC: Full datum NOT found for tx %s, using fallback. Possible race condition - notificator called before datum indexing", tx_hash_str);
                
                // Additional timing info
                log_it(L_INFO, "DIAGNOSTIC: Time since notificator start: %lu ns for tx %s", 
                       timing_after_search - timing_start, tx_hash_str);
            }
        } else {
            log_it(L_WARNING, "DIAGNOSTIC: Main chain callback not available: net=%p, main_chain=%p, callback=%p for tx %s", 
                   a_ledger->net, main_chain, 
                   main_chain ? main_chain->callback_datum_find_by_hash : NULL, tx_hash_str);
        }
        
        PyObject *obj_datum = NULL;
        PyObject *obj_atom = NULL;
        
        if (full_datum) {
            log_it(L_INFO, "Creating PyDapChainDatumObject for tx %s", tx_hash_str);
            PyDapChainDatumObject *obj_datum_full = PyObject_NEW(PyDapChainDatumObject, &DapChainDatumObjectType);
            if (!obj_datum_full) {
                log_it(L_ERROR, "Failed to create PyDapChainDatumObject for tx %s", tx_hash_str);
                Py_DECREF(obj_atom);
                PyGILState_Release(state);
                return;
            }
            obj_datum_full->datum = full_datum;
            obj_datum_full->origin = false;
            obj_datum = (PyObject*)obj_datum_full;
            
            if (atom) {
                log_it(L_INFO, "Creating PyChainAtomObject for tx %s", tx_hash_str);
                PyChainAtomObject *obj_atom_ptr = PyObject_NEW(PyChainAtomObject, &DapChainAtomPtrObjectType);
                if (!obj_atom_ptr) {
                    log_it(L_ERROR, "Failed to create PyChainAtomObject for tx %s", tx_hash_str);
                    Py_DECREF(obj_datum);
                    Py_DECREF(obj_atom);
                    PyGILState_Release(state);
                    return;
                }
                obj_atom_ptr->atom = atom;
                obj_atom_ptr->atom_size = atom_size;
                Py_DECREF(obj_atom);
                obj_atom = (PyObject*)obj_atom_ptr;
            } else {
                log_it(L_INFO, "No atom found for tx %s, using None", tx_hash_str);
            }
        } else {
            // Fallback: create partial datum from tx to maintain consistent object type
            log_it(L_INFO, "Creating partial PyDapChainDatumObject from tx %s", tx_hash_str);
            size_t tx_size = dap_chain_datum_tx_get_size(a_tx);
            size_t datum_size = sizeof(dap_chain_datum_t) + tx_size;
            
            dap_chain_datum_t *partial_datum = DAP_NEW_SIZE(dap_chain_datum_t, datum_size);
            if (!partial_datum) {
                log_it(L_ERROR, "Failed to allocate partial datum for tx %s", tx_hash_str);
                PyGILState_Release(state);
                return;
            }
            
            // Fill datum header
            partial_datum->header.type_id = DAP_CHAIN_DATUM_TX;
            partial_datum->header.data_size = tx_size;
            partial_datum->header.version_id = 1;
            partial_datum->header.ts_create = time(NULL);
            
            // Copy transaction data
            memcpy(partial_datum->data, a_tx, tx_size);
            
            // Create consistent PyDapChainDatumObject type
            PyDapChainDatumObject *obj_datum_partial = PyObject_NEW(PyDapChainDatumObject, &DapChainDatumObjectType);
            if (!obj_datum_partial) {
                log_it(L_ERROR, "Failed to create partial PyDapChainDatumObject for tx %s", tx_hash_str);
                DAP_DELETE(partial_datum);
                PyGILState_Release(state);
                return;
            }
            obj_datum_partial->datum = partial_datum;
            obj_datum_partial->origin = true; // We own this memory
            obj_datum = (PyObject*)obj_datum_partial;
            
            log_it(L_INFO, "Created partial PyDapChainDatumObject from tx %s successfully", tx_hash_str);
        }
        
        // Create tuple and call Python callback
        PyObject *tuple = PyTuple_New(2);
        if (!tuple) {
            log_it(L_ERROR, "Failed to create tuple for tx %s", tx_hash_str);
            Py_XDECREF(obj_datum);
            Py_XDECREF(obj_atom);
            PyGILState_Release(state);
            return;
        }
        
        PyTuple_SetItem(tuple, 0, obj_datum ? obj_datum : Py_None);
        PyTuple_SetItem(tuple, 1, obj_atom ? obj_atom : Py_None);
        if (!obj_datum) Py_INCREF(Py_None);
        if (!obj_atom) Py_INCREF(Py_None);
        
        PyObject *l_argv = Py_BuildValue("OO", tuple, notifier->argv);
        PyObject *l_result = PyObject_CallObject(notifier->func, l_argv);
        
        uint64_t timing_end = dap_nanotime_now();
        log_it(L_INFO, "Notificator complete for tx %s, total time: %lu ns", 
               tx_hash_str, timing_end - timing_start);
        
        if (!l_result) {
            log_it(L_ERROR, "Error in tx notificator for datum %s", tx_hash_str);
            PyErr_Print();
        }
        
        Py_XDECREF(l_result);
        Py_XDECREF(l_argv);
        Py_XDECREF(tuple);
        PyGILState_Release(state);
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
    
        // Try to get full datum with atom info
    dap_chain_datum_t *full_datum = NULL;
    dap_chain_hash_fast_t block_hash = {0};
    int ret_code = 0;
    dap_chain_atom_ptr_t atom = NULL;
    size_t atom_size = 0;
    
    // Get main chain
    dap_chain_t *main_chain = dap_chain_net_get_chain_by_name(l_args->ledger->net, "main");
    if (l_args->ledger->net && main_chain && main_chain->callback_datum_find_by_hash) {
        full_datum = main_chain->callback_datum_find_by_hash(
            main_chain, &l_args->tx_hash, &block_hash, &ret_code);
        
        // If we got full datum and block hash, try to find the atom
        if (full_datum && !dap_hash_fast_is_blank(&block_hash)) {
            atom = dap_chain_get_atom_by_hash(main_chain, &block_hash, &atom_size);
        }
    }
    
    PyObject *obj_datum;
    PyObject *obj_atom = Py_None;
    
    if (full_datum) {
        // Create full datum object with atom info
        PyDapChainDatumObject *obj_datum_full = PyObject_NEW(PyDapChainDatumObject, &DapChainDatumObjectType);
        obj_datum_full->datum = full_datum;
        obj_datum_full->origin = false; // Don't delete the datum, it's from chain
        obj_datum = (PyObject*)obj_datum_full;
        
        // If we found the atom, create Python atom object
        if (atom) {
            // TODO: Create proper Python atom object
            // For now, we'll pass atom pointer and size as tuple
            obj_atom = Py_BuildValue("(K)", (unsigned long)atom);
        }
    } else {
        // Fallback to original behavior - create partial datum
        PyDapChainDatumTxObject *obj_tx_partial = PyObject_NEW(PyDapChainDatumTxObject, &DapChainDatumTxObjectType);
        obj_tx_partial->datum_tx = l_args->tx;
        obj_datum = (PyObject*)obj_tx_partial;
    }
    
            ((PyDapChainDatumObject*)obj_datum)->origin = false;
    PyObject *l_notify_arg = !l_notifier->argv ? Py_None : l_notifier->argv;
    Py_INCREF(l_notify_arg);
    log_it(L_DEBUG, "Call bridged tx ledger notifier for net %s", l_args->ledger->net->pub.name);
    PyGILState_STATE state = PyGILState_Ensure();
    PyObject *obj_argv = Py_BuildValue("OOOO", obj_ledger, obj_datum, obj_atom, l_notify_arg);
    Py_INCREF(l_notifier->func);
    PyObject *result = PyEval_CallObject(l_notifier->func, obj_argv);
    Py_XDECREF(l_notifier->func);
    Py_DECREF(obj_argv);
    Py_DECREF(obj_ledger);
    Py_DECREF(obj_datum);
    Py_DECREF(l_notify_arg);
    if (!result)
        python_error_in_log_it(LOG_TAG);
    Py_XDECREF(result);
    DAP_DELETE(l_args->tx);
    DAP_DELETE(l_args);
    PyGILState_Release(state);
    return false;
}

static void s_python_proc_notifier(dap_ledger_t *a_ledger, dap_chain_datum_tx_t *a_tx, dap_hash_fast_t *a_tx_hash, void *a_arg, dap_chan_ledger_notify_opcodes_t a_opcode)
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

    dap_list_t *used_out_list = dap_ledger_get_list_tx_outs_with_val(
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