#include "wrapping_dap_chain_net_srv_auctions.h"
#include "libdap_chain_net_python.h"
#include "wrapping_dap_hash.h"
#include "math_python.h"
#include "dap_chain_net_srv_auctions.h"
#include "dap_chain_wallet.h"
#include "dap_enc_key.h"
#include "dap_hash.h"
#include "dap_math_ops.h"
#include "dap_math_convert.h"
#include "dap_string.h"
#include "dap_json_rpc.h"
#include <json-c/json.h>

int DapChainNetSrvAuctionsObject_init(PyObject *self, PyObject *args, PyObject *kwds){
    const char *kwlist[] = {
            "net",
            NULL
    };
    PyObject* obj_net;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", (char**)kwlist, &obj_net))
        return -1;
    ((PyDapChainNetSrvAuctionsObject*)self)->net = ((PyDapChainNetObject*)obj_net)->chain_net;
    return 0;
}

PyObject *wrapping_dap_chain_net_srv_auctions_bid_tx_create(PyObject *self, PyObject *argv){
    (void)self;
    PyObject *obj_wallet_path, *obj_auction_hash, *obj_amount, *obj_fee;
    uint64_t lock_time;
    
    if (!PyArg_ParseTuple(argv, "OSSS", &obj_wallet_path, &obj_auction_hash, &obj_amount, &lock_time, &obj_fee)) {
        return NULL;
    }
    
    // Validate lock_time (convert days to seconds)
    if (lock_time < 3 || lock_time > 24) {
        PyErr_SetString(PyExc_ValueError, "lock_time must be between 3 and 24 days");
        return NULL;
    }
    dap_time_t lock_time_seconds = lock_time * 24 * 60 * 60;
    
    // Get wallet path
    const char *wallet_path = PyUnicode_AsUTF8(obj_wallet_path);
    if (!wallet_path) {
        PyErr_SetString(PyExc_ValueError, "Invalid wallet path");
        return NULL;
    }
    
    // Open wallet
    unsigned int wallet_status = 0;
    dap_chain_wallet_t *wallet = dap_chain_wallet_open(wallet_path, NULL, &wallet_status);
    if (!wallet) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to open wallet");
        return NULL;
    }
    
    // Get encryption key  
    dap_enc_key_t *enc_key = dap_chain_wallet_get_key(wallet, 0);
    if (!enc_key) {
        dap_chain_wallet_close(wallet);
        PyErr_SetString(PyExc_RuntimeError, "Failed to get encryption key from wallet");
        return NULL;
    }
    
    // Parse auction hash
    dap_hash_fast_t auction_hash;
    const char *auction_hash_str = PyUnicode_AsUTF8(obj_auction_hash);
    if (dap_chain_hash_fast_from_str(auction_hash_str, &auction_hash) != 0) {
        DAP_DELETE(enc_key);
        dap_chain_wallet_close(wallet);
        PyErr_SetString(PyExc_ValueError, "Invalid auction hash format");
        return NULL;
    }
    
    // Parse amount
    const char *amount_str = PyUnicode_AsUTF8(obj_amount);
    uint256_t amount = dap_uint256_scan_uninteger(amount_str);
    if (IS_ZERO_256(amount)) {
        DAP_DELETE(enc_key);
        dap_chain_wallet_close(wallet);
        PyErr_SetString(PyExc_ValueError, "Invalid amount format");
        return NULL;
    }
    
    // Parse fee
    const char *fee_str = PyUnicode_AsUTF8(obj_fee);
    uint256_t fee = dap_uint256_scan_uninteger(fee_str);
    if (IS_ZERO_256(fee)) {
        DAP_DELETE(enc_key);
        dap_chain_wallet_close(wallet);
        PyErr_SetString(PyExc_ValueError, "Invalid fee format");
        return NULL;
    }
    
    // Create transaction
    int ret_code = 0;
    char *tx_hash_str = dap_auction_bid_tx_create(((PyDapChainNetSrvAuctionsObject *)self)->net, enc_key, &auction_hash,
                                     amount, lock_time_seconds, 0, fee, &ret_code);
    
    // Cleanup
    DAP_DELETE(enc_key);
    dap_chain_wallet_close(wallet);
    
    if (ret_code != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create auction bid transaction");
        return NULL;
    }
    
    if (tx_hash_str) {
        PyObject *result = PyUnicode_FromString(tx_hash_str);
        DAP_DELETE(tx_hash_str);
        return result;
    }
    
    Py_RETURN_NONE;
}

PyObject *wrapping_dap_chain_net_srv_auctions_bid_withdraw_tx_create(PyObject *self, PyObject *argv){
    (void)self;
    PyObject *obj_wallet_path, *obj_bid_tx_hash, *obj_fee;
    
    if (!PyArg_ParseTuple(argv, "OSS", &obj_wallet_path, &obj_bid_tx_hash, &obj_fee)) {
        return NULL;
    }
    
    // Get wallet path
    const char *wallet_path = PyUnicode_AsUTF8(obj_wallet_path);
    if (!wallet_path) {
        PyErr_SetString(PyExc_ValueError, "Invalid wallet path");
        return NULL;
    }
    
    // Open wallet
    dap_chain_wallet_t *wallet = dap_chain_wallet_open(wallet_path, NULL, NULL);
    if (!wallet) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to open wallet");
        return NULL;
    }
    
    // Get encryption key
    dap_enc_key_t *enc_key = dap_chain_wallet_get_key(wallet, 0);
    if (!enc_key) {
        dap_chain_wallet_close(wallet);
        PyErr_SetString(PyExc_RuntimeError, "Failed to get encryption key from wallet");
        return NULL;
    }
    
    // Parse bid transaction hash
    dap_hash_fast_t bid_tx_hash;
    const char *bid_tx_hash_str = PyUnicode_AsUTF8(obj_bid_tx_hash);
    if (dap_chain_hash_fast_from_str(bid_tx_hash_str, &bid_tx_hash) != 0) {
        DAP_DELETE(enc_key);
        dap_chain_wallet_close(wallet);
        PyErr_SetString(PyExc_ValueError, "Invalid bid transaction hash format");
        return NULL;
    }
    
    // Parse fee
    uint256_t fee;
    const char *fee_str = PyUnicode_AsUTF8(obj_fee);
    if (IS_ZERO_256(fee = dap_uint256_scan_uninteger(fee_str))) {
        DAP_DELETE(enc_key);
        dap_chain_wallet_close(wallet);
        PyErr_SetString(PyExc_ValueError, "Invalid fee format");
        return NULL;
    }
    
    // Create withdrawal transaction
    int ret_code = 0;
    char *tx_hash_str = dap_auction_bid_withdraw_tx_create(((PyDapChainNetSrvAuctionsObject *)self)->net, enc_key, &bid_tx_hash, fee, &ret_code);
    
    // Cleanup
    DAP_DELETE(enc_key);
    dap_chain_wallet_close(wallet);
    
    if (ret_code != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create auction bid withdrawal transaction");
        return NULL;
    }
    
    if (tx_hash_str) {
        PyObject *result = PyUnicode_FromString(tx_hash_str);
        DAP_DELETE(tx_hash_str);
        return result;
    }
    
    Py_RETURN_NONE;
}

PyObject *wrapping_dap_chain_net_srv_auctions_get_info(PyObject *self, PyObject *argv){
    (void)self;
    PyObject *obj_auction_hash;
    
    if (!PyArg_ParseTuple(argv, "O", &obj_auction_hash)) {
        return NULL;
    }
    
    // Parse auction hash
    dap_hash_fast_t auction_hash;
    const char *auction_hash_str = PyUnicode_AsUTF8(obj_auction_hash);
    if (dap_chain_hash_fast_from_str(auction_hash_str, &auction_hash) != 0) {
        PyErr_SetString(PyExc_ValueError, "Invalid auction hash format");
        return NULL;
    }
    
    // Get auction information
    dap_chain_net_srv_auction_t *auction = dap_chain_net_srv_auctions_get_detailed(((PyDapChainNetSrvAuctionsObject *)self)->net, &auction_hash);
    if (!auction) {
        PyErr_SetString(PyExc_RuntimeError, "Auction not found");
        return NULL;
    }
    
    // Convert to Python dictionary
    PyObject *result = PyDict_New();
    
    PyDict_SetItemString(result, "auction_hash", PyUnicode_FromString(auction_hash_str));
    PyDict_SetItemString(result, "status", PyUnicode_FromString(dap_auction_status_to_str(auction->status)));
    PyDict_SetItemString(result, "created_time", PyLong_FromUnsignedLongLong(auction->created_time));
    PyDict_SetItemString(result, "start_time", PyLong_FromUnsignedLongLong(auction->start_time));
    PyDict_SetItemString(result, "end_time", PyLong_FromUnsignedLongLong(auction->end_time));
    
    if (auction->description) {
        PyDict_SetItemString(result, "description", PyUnicode_FromString(auction->description));
    }
    
    PyDict_SetItemString(result, "bids_count", PyLong_FromUnsignedLong(auction->bids_count));
    PyDict_SetItemString(result, "projects_count", PyLong_FromUnsignedLong(auction->projects_count));
    PyDict_SetItemString(result, "has_winner", PyBool_FromLong(auction->has_winner));
    
    if (auction->has_winner) {
        PyDict_SetItemString(result, "winners_count", PyLong_FromUnsignedLong(auction->winners_cnt));
        
        PyObject *winners_array = PyList_New(auction->winners_cnt);
        for (uint8_t i = 0; i < auction->winners_cnt; i++) {
            PyList_SetItem(winners_array, i, PyLong_FromUnsignedLong(auction->winners_ids[i]));
        }
        PyDict_SetItemString(result, "winners_ids", winners_array);
    }
    
    // Add projects if available
    if (auction->projects) {
        PyObject *projects_array = PyList_New(auction->projects_count);
        for (uint32_t i = 0; i < auction->projects_count; i++) {
            PyObject *project_obj = PyDict_New();
            
            if (auction->projects[i].project_name) {
                PyDict_SetItemString(project_obj, "project_name", PyUnicode_FromString(auction->projects[i].project_name));
            }
            
            char *total_amount_str = dap_uint256_uninteger_to_char(auction->projects[i].total_amount);
            PyDict_SetItemString(project_obj, "total_amount", PyUnicode_FromString(total_amount_str));
            DAP_DELETE(total_amount_str);
            
            // Total amount in CELL
            char *total_amount_coin_str = dap_uint256_decimal_to_char(auction->projects[i].total_amount);
            if (total_amount_coin_str) {
                PyDict_SetItemString(project_obj, "total_amount_coin", PyUnicode_FromString(total_amount_coin_str));
                DAP_DELETE(total_amount_coin_str);
            } else {
                PyDict_SetItemString(project_obj, "total_amount_coin", PyUnicode_FromString("0.0"));
            }
            
            PyDict_SetItemString(project_obj, "bids_count", PyLong_FromUnsignedLong(auction->projects[i].bids_count));
            PyDict_SetItemString(project_obj, "active_bids_count", PyLong_FromUnsignedLong(auction->projects[i].active_bids_count));
            
            PyList_SetItem(projects_array, i, project_obj);
        }
        PyDict_SetItemString(result, "projects", projects_array);
    }
    
    dap_chain_net_srv_auction_delete(auction);
    return result;
}

PyObject *wrapping_dap_chain_net_srv_auctions_get_list(PyObject *self, PyObject *argv){
    (void)self;
    int active_only = 0;
    int include_projects = 0;
    
    if (!PyArg_ParseTuple(argv, "|ii", &active_only, &include_projects)) {
        return NULL;
    }
    
    // Get auction status filter
    dap_auction_status_t status_filter = active_only ? DAP_AUCTION_STATUS_ACTIVE : DAP_AUCTION_STATUS_UNKNOWN;
    
    // Get list of auctions
    dap_list_t *auctions_list = dap_chain_net_srv_auctions_get_list(((PyDapChainNetSrvAuctionsObject *)self)->net, status_filter, include_projects);
    if (!auctions_list) {
        return PyList_New(0); // Return empty list
    }
    
    // Convert to Python list
    PyObject *result = PyList_New(0);
    
    for (dap_list_t *item = auctions_list; item; item = dap_list_next(item)) {
        dap_chain_net_srv_auction_t *auction = (dap_chain_net_srv_auction_t *)item->data;
        
        PyObject *auction_obj = PyDict_New();
        
        char auction_hash_str[65];
        dap_chain_hash_fast_to_str(&auction->auction_hash, auction_hash_str, sizeof(auction_hash_str));
        PyDict_SetItemString(auction_obj, "auction_hash", PyUnicode_FromString(auction_hash_str));
        
        if (auction->auction_name) {
            PyDict_SetItemString(auction_obj, "auction_name", PyUnicode_FromString(auction->auction_name));
        }
        
        PyDict_SetItemString(auction_obj, "status", PyUnicode_FromString(dap_auction_status_to_str(auction->status)));
        PyDict_SetItemString(auction_obj, "created_time", PyLong_FromUnsignedLongLong(auction->created_time));
        PyDict_SetItemString(auction_obj, "start_time", PyLong_FromUnsignedLongLong(auction->start_time));
        PyDict_SetItemString(auction_obj, "end_time", PyLong_FromUnsignedLongLong(auction->end_time));
        
        if (auction->description) {
            PyDict_SetItemString(auction_obj, "description", PyUnicode_FromString(auction->description));
        }
        
        PyDict_SetItemString(auction_obj, "bids_count", PyLong_FromUnsignedLong(auction->bids_count));
        PyDict_SetItemString(auction_obj, "projects_count", PyLong_FromUnsignedLong(auction->projects_count));
        PyDict_SetItemString(auction_obj, "has_winner", PyBool_FromLong(auction->has_winner));
        
        if (auction->has_winner) {
            PyDict_SetItemString(auction_obj, "winners_count", PyLong_FromUnsignedLong(auction->winners_cnt));
            
            PyObject *winners_array = PyList_New(auction->winners_cnt);
            for (uint8_t i = 0; i < auction->winners_cnt; i++) {
                PyList_SetItem(winners_array, i, PyLong_FromUnsignedLong(auction->winners_ids[i]));
            }
            PyDict_SetItemString(auction_obj, "winners_ids", winners_array);
        }
        
        // Add projects if requested and available
        if (include_projects && auction->projects) {
            PyObject *projects_array = PyList_New(auction->projects_count);
            for (uint32_t i = 0; i < auction->projects_count; i++) {
                PyObject *project_obj = PyDict_New();
                
                if (auction->projects[i].project_name) {
                    PyDict_SetItemString(project_obj, "project_name", PyUnicode_FromString(auction->projects[i].project_name));
                } else {
                    PyDict_SetItemString(project_obj, "project_name", PyUnicode_FromString("Unknown"));
                }
                
                char *total_amount_str = dap_uint256_uninteger_to_char(auction->projects[i].total_amount);
                if (total_amount_str) {
                    PyDict_SetItemString(project_obj, "total_amount", PyUnicode_FromString(total_amount_str));
                    DAP_DELETE(total_amount_str);
                } else {
                    PyDict_SetItemString(project_obj, "total_amount", PyUnicode_FromString("0"));
                }
                
                // Total amount in CELL
                char *total_amount_coin_str = dap_uint256_decimal_to_char(auction->projects[i].total_amount);
                if (total_amount_coin_str) {
                    PyDict_SetItemString(project_obj, "total_amount_coin", PyUnicode_FromString(total_amount_coin_str));
                    DAP_DELETE(total_amount_coin_str);
                } else {
                    PyDict_SetItemString(project_obj, "total_amount_coin", PyUnicode_FromString("0.0"));
                }
                
                PyDict_SetItemString(project_obj, "bids_count", PyLong_FromUnsignedLong(auction->projects[i].bids_count));
                PyDict_SetItemString(project_obj, "active_bids_count", PyLong_FromUnsignedLong(auction->projects[i].active_bids_count));
                
                PyList_SetItem(projects_array, i, project_obj);
            }
            PyDict_SetItemString(auction_obj, "projects", projects_array);
        }
        
        PyList_Append(result, auction_obj);
    }
    
    // Clean up list
    for (dap_list_t *item = auctions_list; item; item = dap_list_next(item)) {
        dap_chain_net_srv_auction_delete((dap_chain_net_srv_auction_t *)item->data);
    }
    dap_list_free(auctions_list);
    
    return result;
}

PyObject *wrapping_dap_chain_net_srv_auctions_get_stats(PyObject *self, PyObject *argv){
    (void)self;
    (void)argv;
    
    // Get auction statistics
    dap_auction_stats_t *stats = dap_chain_net_srv_auctions_get_stats(((PyDapChainNetSrvAuctionsObject *)self)->net);
    if (!stats) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to get auction statistics");
        return NULL;
    }
    
    // Convert to Python dictionary
    PyObject *result = PyDict_New();
    PyDict_SetItemString(result, "total_auctions", PyLong_FromUnsignedLong(stats->total_auctions));
    PyDict_SetItemString(result, "active_auctions", PyLong_FromUnsignedLong(stats->active_auctions));
    PyDict_SetItemString(result, "ended_auctions", PyLong_FromUnsignedLong(stats->ended_auctions));
    PyDict_SetItemString(result, "cancelled_auctions", PyLong_FromUnsignedLong(stats->cancelled_auctions));
    PyDict_SetItemString(result, "total_bids", PyLong_FromUnsignedLong(stats->total_bids));
    PyDict_SetItemString(result, "total_projects", PyLong_FromUnsignedLong(stats->total_projects));
    
    DAP_DELETE(stats);
    return result;
}

PyObject *wrapping_dap_chain_net_srv_auctions_get_events(PyObject *self, PyObject *argv){
    (void)self;
    PyObject *obj_auction_hash = NULL;
    PyObject *obj_event_type = NULL;
    uint32_t limit = 100;
    
    if (!PyArg_ParseTuple(argv, "|OOk", &obj_auction_hash, &obj_event_type, &limit)) {
        return NULL;
    }
    
    // TODO: Implement event retrieval from ledger
    // This would require access to the ledger's event system
    // For now, return empty list
    PyObject *result = PyList_New(0);
    
    return result;
}

static PyMethodDef PyDapChainNetSrvAuctionsMethods[] = {
        {
            "bidTxCreate",
            wrapping_dap_chain_net_srv_auctions_bid_tx_create,
            METH_VARARGS,
            "Create a bid transaction for an auction"
        },
        {
            "bidWithdrawTxCreate",
            wrapping_dap_chain_net_srv_auctions_bid_withdraw_tx_create,
            METH_VARARGS,
            "Create a withdrawal transaction for a bid"
        },
        {
            "getInfo",
            wrapping_dap_chain_net_srv_auctions_get_info,
            METH_VARARGS,
            "Get detailed information about an auction"
        },
        {
            "getList",
            wrapping_dap_chain_net_srv_auctions_get_list,
            METH_VARARGS,
            "Get list of auctions"
        },
        {
            "getStats",
            wrapping_dap_chain_net_srv_auctions_get_stats,
            METH_VARARGS,
            "Get auction statistics"
        },
        {
            "getEvents",
            wrapping_dap_chain_net_srv_auctions_get_events,
            METH_VARARGS,
            "Get auction events"
        },
        {NULL, NULL, 0, NULL}
};

PyTypeObject PyDapChainNetSrvAuctionsObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "DapChainNetSrvAuctions",
        .tp_basicsize = sizeof(PyDapChainNetSrvAuctionsObject),
        .tp_itemsize = 0,
        .tp_dealloc = (destructor)PyObject_Del,
        .tp_getattr = 0,
        .tp_setattr = 0,
        .tp_as_async = 0,
        .tp_repr = 0,
        .tp_as_number = 0,
        .tp_as_sequence = 0,
        .tp_as_mapping = 0,
        .tp_hash = 0,
        .tp_call = 0,
        .tp_str = 0,
        .tp_getattro = 0,
        .tp_setattro = 0,
        .tp_as_buffer = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT,
        .tp_doc = "DapChainNetSrvAuctions object",
        .tp_traverse = 0,
        .tp_clear = 0,
        .tp_richcompare = 0,
        .tp_weaklistoffset = 0,
        .tp_iter = 0,
        .tp_iternext = 0,
        .tp_methods = PyDapChainNetSrvAuctionsMethods,
        .tp_members = 0,
        .tp_getset = 0,
        .tp_base = 0,
        .tp_dict = 0,
        .tp_descr_get = 0,
        .tp_descr_set = 0,
        .tp_dictoffset = 0,
        .tp_init = (initproc)DapChainNetSrvAuctionsObject_init,
        .tp_alloc = 0,
        .tp_new = PyType_GenericNew,
        .tp_free = 0,
        .tp_is_gc = 0,
        .tp_bases = 0,
        .tp_mro = 0,
        .tp_cache = 0,
        .tp_subclasses = 0,
        .tp_weaklist = 0,
        .tp_del = 0,
        .tp_version_tag = 0,
        .tp_finalize = 0,
}; 