#include "wrapping_dap_chain_net_srv_stake_ext.h"
#include "libdap_chain_net_python.h"
#include "wrapping_dap_hash.h"
#include "math_python.h"
#include "dap_chain_net_srv_stake_ext.h"
#include "dap_chain_wallet.h"
#include "dap_enc_key.h"
#include "dap_hash.h"
#include "dap_math_ops.h"
#include "dap_math_convert.h"
#include "dap_string.h"
#include "dap_json_rpc.h"
#include <json-c/json.h>

// Transaction creation functions
static PyObject *wrapping_dap_chain_net_srv_stake_ext_lock_tx_create(PyObject *self, PyObject *argv);
static PyObject *wrapping_dap_chain_net_srv_stake_ext_unlock_tx_create(PyObject *self, PyObject *argv);

// Information retrieval functions
static PyObject *wrapping_dap_chain_net_srv_stake_ext_get_info(PyObject *self, PyObject *argv);
static PyObject *wrapping_dap_chain_net_srv_stake_ext_get_list(PyObject *self, PyObject *argv);
static PyObject *wrapping_dap_chain_net_srv_stake_ext_get_stats(PyObject *self, PyObject *argv);
static PyObject *wrapping_dap_chain_net_srv_stake_ext_get_events(PyObject *self, PyObject *argv);
static PyObject *wrapping_dap_chain_net_srv_stake_ext_started_tx_event_create_py(PyObject *self, PyObject *argv);
static PyObject *wrapping_dap_chain_net_srv_stake_ext_ended_tx_event_create_py(PyObject *self, PyObject *argv);


int DapChainNetSrvStakeExtObject_init(PyObject *self, PyObject *args, PyObject *kwds){
    const char *kwlist[] = {
            "net",
            NULL
    };
    PyObject* obj_net;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", (char**)kwlist, &obj_net))
        return -1;
    ((PyDapChainNetSrvStakeExtObject*)self)->net = ((PyDapChainNetObject*)obj_net)->chain_net;
    return 0;
}

static PyObject *wrapping_dap_chain_net_srv_stake_ext_lock_tx_create(PyObject *self, PyObject *argv){
    (void)self;
    PyObject *obj_wallet_path, *obj_stake_ext_hash, *obj_amount, *obj_fee;
    uint32_t position_id;
    uint64_t lock_time;
    
    if (!PyArg_ParseTuple(argv, "OSSLLO", &obj_wallet_path, &obj_stake_ext_hash, 
                        &obj_amount, &lock_time, &position_id, &obj_fee)) {
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
    
    // Parse stake_ext hash
    dap_hash_fast_t stake_ext_hash;
    const char *stake_ext_hash_str = PyUnicode_AsUTF8(obj_stake_ext_hash);
    if (dap_chain_hash_fast_from_str(stake_ext_hash_str, &stake_ext_hash) != 0) {
        DAP_DELETE(enc_key);
        dap_chain_wallet_close(wallet);
        PyErr_SetString(PyExc_ValueError, "Invalid stake_ext hash format");
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
    char *tx_hash_str = dap_chain_net_srv_stake_ext_lock_create(((PyDapChainNetSrvStakeExtObject *)self)->net, enc_key, &stake_ext_hash,
                                     amount, lock_time_seconds, position_id, fee, &ret_code);
    
    // Cleanup
    DAP_DELETE(enc_key);
    dap_chain_wallet_close(wallet);
    
    if (ret_code != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create stake_ext lock transaction");
        return NULL;
    }
    
    if (tx_hash_str) {
        PyObject *result = PyUnicode_FromString(tx_hash_str);
        DAP_DELETE(tx_hash_str);
        return result;
    }
    
    Py_RETURN_NONE;
}

static PyObject *wrapping_dap_chain_net_srv_stake_ext_unlock_tx_create(PyObject *self, PyObject *argv){
    (void)self;
    PyObject *obj_wallet_path, *obj_lock_tx_hash, *obj_fee;
    
    if (!PyArg_ParseTuple(argv, "OSS", &obj_wallet_path, &obj_lock_tx_hash, &obj_fee)) {
        return NULL;
    }
    
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
    
    // Parse lock transaction hash
    dap_hash_fast_t lock_tx_hash;
    const char *lock_tx_hash_str = PyUnicode_AsUTF8(obj_lock_tx_hash);
    if (dap_chain_hash_fast_from_str(lock_tx_hash_str, &lock_tx_hash) != 0) {
        DAP_DELETE(enc_key);
        dap_chain_wallet_close(wallet);
        PyErr_SetString(PyExc_ValueError, "Invalid lock transaction hash format");
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
    
    // Create unlockal transaction
    int ret_code = 0;
    char *tx_hash_str = dap_chain_net_srv_stake_ext_unlock_create(((PyDapChainNetSrvStakeExtObject *)self)->net, enc_key, &lock_tx_hash, fee, NULL, &ret_code);
    
    // Cleanup
    DAP_DELETE(enc_key);
    dap_chain_wallet_close(wallet);
    
    if (ret_code != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create stake_ext lock unlockal transaction");
        return NULL;
    }
    
    if (tx_hash_str) {
        PyObject *result = PyUnicode_FromString(tx_hash_str);
        DAP_DELETE(tx_hash_str);
        return result;
    }
    
    Py_RETURN_NONE;
}

static PyObject *wrapping_dap_chain_net_srv_stake_ext_get_info(PyObject *self, PyObject *argv){
    (void)self;
    PyObject *obj_stake_ext_hash;
    
    if (!PyArg_ParseTuple(argv, "O", &obj_stake_ext_hash)) {
        return NULL;
    }
    
    // Parse stake_ext hash
    dap_hash_fast_t stake_ext_hash;
    const char *stake_ext_hash_str = PyUnicode_AsUTF8(obj_stake_ext_hash);
    if (dap_chain_hash_fast_from_str(stake_ext_hash_str, &stake_ext_hash) != 0) {
        PyErr_SetString(PyExc_ValueError, "Invalid stake_ext hash format");
        return NULL;
    }
    
    // Get stake_ext information
    dap_chain_net_srv_stake_ext_t *stake_ext = dap_chain_net_srv_stake_ext_get_detailed(((PyDapChainNetSrvStakeExtObject *)self)->net, &stake_ext_hash);
    if (!stake_ext) {
        PyErr_SetString(PyExc_RuntimeError, "stake_ext not found");
        return NULL;
    }
    
    // Convert to Python dictionary
    PyObject *result = PyDict_New();
    
    PyDict_SetItemString(result, "stake_ext_hash", PyUnicode_FromString(stake_ext_hash_str));
    PyDict_SetItemString(result, "status", PyUnicode_FromString(dap_stake_ext_status_to_str(stake_ext->status)));
    PyDict_SetItemString(result, "start_time", PyLong_FromUnsignedLongLong(stake_ext->start_time));
    PyDict_SetItemString(result, "end_time", PyLong_FromUnsignedLongLong(stake_ext->end_time));
    
    if (stake_ext->description) {
        PyDict_SetItemString(result, "description", PyUnicode_FromString(stake_ext->description));
    }
    
    PyDict_SetItemString(result, "locks_count", PyLong_FromUnsignedLong(stake_ext->locks_count));
    PyDict_SetItemString(result, "positions_count", PyLong_FromUnsignedLong(stake_ext->positions_count));
    PyDict_SetItemString(result, "has_winner", PyBool_FromLong(stake_ext->winners_cnt));
    
    if (stake_ext->winners_cnt) {
        PyDict_SetItemString(result, "winners_count", PyLong_FromUnsignedLong(stake_ext->winners_cnt));
        
        PyObject *winners_array = PyList_New(stake_ext->winners_cnt);
        for (uint8_t i = 0; i < stake_ext->winners_cnt; i++) {
            PyList_SetItem(winners_array, i, PyLong_FromUnsignedLong(stake_ext->winners_ids[i]));
        }
        PyDict_SetItemString(result, "winners_ids", winners_array);
    }
    
    // Add positions if available
    if (stake_ext->positions) {
        PyObject *positions_array = PyList_New(stake_ext->positions_count);
        for (uint32_t i = 0; i < stake_ext->positions_count; i++) {
            PyObject *position_obj = PyDict_New();
                       
            char *total_amount_str = dap_uint256_uninteger_to_char(stake_ext->positions[i].total_amount);
            PyDict_SetItemString(position_obj, "total_amount", PyUnicode_FromString(total_amount_str));
            DAP_DELETE(total_amount_str);
            
            // Total amount in CELL
            char *total_amount_coin_str = dap_uint256_decimal_to_char(stake_ext->positions[i].total_amount);
            if (total_amount_coin_str) {
                PyDict_SetItemString(position_obj, "total_amount_coin", PyUnicode_FromString(total_amount_coin_str));
                DAP_DELETE(total_amount_coin_str);
            } else {
                PyDict_SetItemString(position_obj, "total_amount_coin", PyUnicode_FromString("0.0"));
            }
            
            PyDict_SetItemString(position_obj, "locks_count", PyLong_FromUnsignedLong(stake_ext->positions[i].locks_count));
            PyDict_SetItemString(position_obj, "active_locks_count", PyLong_FromUnsignedLong(stake_ext->positions[i].active_locks_count));
            
            PyList_SetItem(positions_array, i, position_obj);
        }
        PyDict_SetItemString(result, "positions", positions_array);
    }
    
    dap_chain_net_srv_stake_ext_delete(stake_ext);
    return result;
}

static PyObject *wrapping_dap_chain_net_srv_stake_ext_get_list(PyObject *self, PyObject *argv){
    (void)self;
    int active_only = 0;
    int include_positions = 0;
    
    if (!PyArg_ParseTuple(argv, "|ii", &active_only, &include_positions)) {
        return NULL;
    }
    
    // Get stake_ext status filter
    dap_stake_ext_status_t status_filter = active_only ? DAP_STAKE_EXT_STATUS_ACTIVE : DAP_STAKE_EXT_STATUS_UNKNOWN;
    
    // Get list of stake_ext
    dap_list_t *stake_ext_list = dap_chain_net_srv_stake_ext_get_list(((PyDapChainNetSrvStakeExtObject *)self)->net, status_filter, include_positions);
    if (!stake_ext_list) {
        return PyList_New(0); // Return empty list
    }
    
    // Convert to Python list
    PyObject *result = PyList_New(0);
    
    for (dap_list_t *item = stake_ext_list; item; item = dap_list_next(item)) {
        dap_chain_net_srv_stake_ext_t *stake_ext = (dap_chain_net_srv_stake_ext_t *)item->data;
        
        // Safety check: verify stake_ext structure is valid
        if (!stake_ext) {
            continue; // Skip NULL stake_ext
        }
        
        PyObject *stake_ext_obj = PyDict_New();
        if (!stake_ext_obj) {
            continue; // Skip if dict creation failed
        }
        
        // Convert stake_ext hash to string with proper error handling
        char stake_ext_hash_str[DAP_HASH_FAST_STR_SIZE] = {};      
        int ret = dap_chain_hash_fast_to_str(&stake_ext->stake_ext_hash, stake_ext_hash_str, sizeof(stake_ext_hash_str));
        if (ret <= 0 || stake_ext_hash_str[0] == '\0') {
            // If hash conversion failed, use a placeholder
            strncpy(stake_ext_hash_str, "INVALID_HASH", sizeof(stake_ext_hash_str) - 1);
            stake_ext_hash_str[sizeof(stake_ext_hash_str) - 1] = '\0';
        }
        
        // Create Python unicode object with error checking
        PyObject *hash_obj = PyUnicode_FromString(stake_ext_hash_str);
        if (hash_obj) {
            PyDict_SetItemString(stake_ext_obj, "stake_ext_hash", hash_obj);
            Py_DECREF(hash_obj);
        } else {
            // If string creation failed, set a safe placeholder
            PyDict_SetItemString(stake_ext_obj, "stake_ext_hash", PyUnicode_FromString("ERROR"));
        }
        
        if (stake_ext->guuid) {
            PyObject *name_obj = PyUnicode_FromString(stake_ext->guuid);
            if (name_obj) {
                PyDict_SetItemString(stake_ext_obj, "stake_ext_name", name_obj);
                Py_DECREF(name_obj);
            }
        }
        
        // Safe status conversion with fallback
        const char *status_str = dap_stake_ext_status_to_str(stake_ext->status);
        if (!status_str) {
            status_str = "UNKNOWN";
        }
        PyObject *status_obj = PyUnicode_FromString(status_str);
        if (status_obj) {
            PyDict_SetItemString(stake_ext_obj, "status", status_obj);
            Py_DECREF(status_obj);
        }
        PyDict_SetItemString(stake_ext_obj, "start_time", PyLong_FromUnsignedLongLong(stake_ext->start_time));
        PyDict_SetItemString(stake_ext_obj, "end_time", PyLong_FromUnsignedLongLong(stake_ext->end_time));
        
        if (stake_ext->description) {
            PyObject *desc_obj = PyUnicode_FromString(stake_ext->description);
            if (desc_obj) {
                PyDict_SetItemString(stake_ext_obj, "description", desc_obj);
                Py_DECREF(desc_obj);
            }
        }
        
        PyDict_SetItemString(stake_ext_obj, "locks_count", PyLong_FromUnsignedLong(stake_ext->locks_count));
        PyDict_SetItemString(stake_ext_obj, "positions_count", PyLong_FromUnsignedLong(stake_ext->positions_count));
        PyDict_SetItemString(stake_ext_obj, "has_winner", PyBool_FromLong(stake_ext->winners_cnt));
        
        if (stake_ext->winners_cnt) {
            PyDict_SetItemString(stake_ext_obj, "winners_count", PyLong_FromUnsignedLong(stake_ext->winners_cnt));
            
            PyObject *winners_array = PyList_New(stake_ext->winners_cnt);
            for (uint8_t i = 0; i < stake_ext->winners_cnt; i++) {
                PyList_SetItem(winners_array, i, PyLong_FromUnsignedLong(stake_ext->winners_ids[i]));
            }
            PyDict_SetItemString(stake_ext_obj, "winners_ids", winners_array);
        }
        
        // Add positions if requested and available
        if (include_positions && stake_ext->positions) {
            PyObject *positions_array = PyList_New(stake_ext->positions_count);
            for (uint32_t i = 0; i < stake_ext->positions_count; i++) {
                PyObject *position_obj = PyDict_New();
                
                PyObject *position_id_obj = PyLong_FromUnsignedLong(stake_ext->positions[i].position_id);
                if (position_id_obj) {
                    PyDict_SetItemString(position_obj, "position_id", position_id_obj);
                    Py_DECREF(position_id_obj);
                }
                
                char *total_amount_str = dap_uint256_uninteger_to_char(stake_ext->positions[i].total_amount);
                if (total_amount_str) {
                    PyObject *amount_obj = PyUnicode_FromString(total_amount_str);
                    if (amount_obj) {
                        PyDict_SetItemString(position_obj, "total_amount", amount_obj);
                        Py_DECREF(amount_obj);
                    } else {
                        PyDict_SetItemString(position_obj, "total_amount", PyUnicode_FromString("0"));
                    }
                    DAP_DELETE(total_amount_str);
                } else {
                    PyDict_SetItemString(position_obj, "total_amount", PyUnicode_FromString("0"));
                }
                
                // Total amount in CELL
                char *total_amount_coin_str = dap_uint256_decimal_to_char(stake_ext->positions[i].total_amount);
                if (total_amount_coin_str) {
                    PyObject *amount_coin_obj = PyUnicode_FromString(total_amount_coin_str);
                    if (amount_coin_obj) {
                        PyDict_SetItemString(position_obj, "total_amount_coin", amount_coin_obj);
                        Py_DECREF(amount_coin_obj);
                    } else {
                        PyDict_SetItemString(position_obj, "total_amount_coin", PyUnicode_FromString("0.0"));
                    }
                    DAP_DELETE(total_amount_coin_str);
                } else {
                    PyDict_SetItemString(position_obj, "total_amount_coin", PyUnicode_FromString("0.0"));
                }
                
                PyDict_SetItemString(position_obj, "locks_count", PyLong_FromUnsignedLong(stake_ext->positions[i].locks_count));
                PyDict_SetItemString(position_obj, "active_locks_count", PyLong_FromUnsignedLong(stake_ext->positions[i].active_locks_count));
                
                PyList_SetItem(positions_array, i, position_obj);
            }
            PyDict_SetItemString(stake_ext_obj, "positions", positions_array);
        }
        
        PyList_Append(result, stake_ext_obj);
    }
    
    // Clean up list
    for (dap_list_t *item = stake_ext_list; item; item = dap_list_next(item)) {
        dap_chain_net_srv_stake_ext_delete((dap_chain_net_srv_stake_ext_t *)item->data);
    }
    dap_list_free(stake_ext_list);
    
    return result;
}

PyObject *wrapping_dap_chain_net_srv_stake_ext_get_stats(PyObject *self, PyObject *argv){
    (void)self;
    (void)argv;
    
    // Get stake_ext statistics
    dap_stake_ext_stats_t *stats = dap_chain_net_srv_stake_ext_get_stats(((PyDapChainNetSrvStakeExtObject *)self)->net);
    if (!stats) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to get stake_ext statistics");
        return NULL;
    }
    
    // Convert to Python dictionary
    PyObject *result = PyDict_New();
    PyDict_SetItemString(result, "total_stake_ext", PyLong_FromUnsignedLong(stats->total_stake_ext));
    PyDict_SetItemString(result, "active_stake_ext", PyLong_FromUnsignedLong(stats->active_stake_ext));
    PyDict_SetItemString(result, "ended_stake_ext", PyLong_FromUnsignedLong(stats->ended_stake_ext));
    PyDict_SetItemString(result, "cancelled_stake_ext", PyLong_FromUnsignedLong(stats->cancelled_stake_ext));
    PyDict_SetItemString(result, "total_locks", PyLong_FromUnsignedLong(stats->total_locks));
    PyDict_SetItemString(result, "total_positions", PyLong_FromUnsignedLong(stats->total_positions));
    
    DAP_DELETE(stats);
    return result;
}

PyObject *wrapping_dap_chain_net_srv_stake_ext_get_events(PyObject *self, PyObject *argv){
    (void)self;
    PyObject *obj_stake_ext_hash = NULL;
    PyObject *obj_event_type = NULL;
    uint32_t limit = 100;
    
    if (!PyArg_ParseTuple(argv, "|OOk", &obj_stake_ext_hash, &obj_event_type, &limit)) {
        return NULL;
    }
    
    // TODO: Implement event retrieval from ledger
    // This would require access to the ledger's event system
    // For now, return empty list
    PyObject *result = PyList_New(0);
    
    return result;
}

static PyMethodDef PyDapChainNetSrvStakeExtMethods[] = {
        {
            "lockTxCreate",
            wrapping_dap_chain_net_srv_stake_ext_lock_tx_create,
            METH_VARARGS,
            "Create a lock transaction for an stake_ext"
        },
        {
            "lockunlockTxCreate",
            wrapping_dap_chain_net_srv_stake_ext_unlock_tx_create,
            METH_VARARGS,
            "Create a unlockal transaction for a lock"
        },
        {
            "getInfo",
            wrapping_dap_chain_net_srv_stake_ext_get_info,
            METH_VARARGS,
            "Get detailed information about an stake_ext"
        },
        {
            "getList",
            wrapping_dap_chain_net_srv_stake_ext_get_list,
            METH_VARARGS,
            "Get list of stake_ext"
        },
        {
            "getStats",
            wrapping_dap_chain_net_srv_stake_ext_get_stats,
            METH_VARARGS,
            "Get stake_ext statistics"
        },
        {
            "getEvents",
            wrapping_dap_chain_net_srv_stake_ext_get_events,
            METH_VARARGS,
            "Get stake_ext events"
        },
        {
            "stakeExtStartedTxEventCreate",
            wrapping_dap_chain_net_srv_stake_ext_started_tx_event_create_py,
            METH_VARARGS,
            "Create stake_ext started tx event"
        },
        {
            "stakeExtEndedTxEventCreate",
            wrapping_dap_chain_net_srv_stake_ext_ended_tx_event_create_py,
            METH_VARARGS,
            "Create stake_ext ended tx event"
        },
        {NULL, NULL, 0, NULL}
};

PyTypeObject PyDapChainNetSrvStakeExtObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "DapChainNetSrvStakeExt",
        .tp_basicsize = sizeof(PyDapChainNetSrvStakeExtObject),
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
        .tp_doc = "DapChainNetSrvStakeExt object",
        .tp_traverse = 0,
        .tp_clear = 0,
        .tp_richcompare = 0,
        .tp_weaklistoffset = 0,
        .tp_iter = 0,
        .tp_iternext = 0,
        .tp_methods = PyDapChainNetSrvStakeExtMethods,
        .tp_members = 0,
        .tp_getset = 0,
        .tp_base = 0,
        .tp_dict = 0,
        .tp_descr_get = 0,
        .tp_descr_set = 0,
        .tp_dictoffset = 0,
        .tp_init = (initproc)DapChainNetSrvStakeExtObject_init,
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

static PyObject *wrapping_dap_chain_net_srv_stake_ext_started_tx_event_create_py(PyObject *self, PyObject *argv)
{
    (void)self;
    unsigned int multiplier;
    unsigned long long duration; // dap_time_t
    unsigned int time_unit; // dap_chain_tx_event_data_time_unit_t
    unsigned int calculation_rule_id;
    PyObject *py_positions = NULL;
    if (!PyArg_ParseTuple(argv, "IKII|O", &multiplier, &duration, &time_unit, &calculation_rule_id, &py_positions))
        return NULL;

    uint8_t total_positions = 0;
    uint32_t *position_ids = NULL;

    if (py_positions && py_positions != Py_None) {
        if (!PyList_Check(py_positions) && !PyTuple_Check(py_positions)) {
            PyErr_SetString(PyExc_TypeError, "position_ids must be a list or tuple of integers");
            return NULL;
        }
        Py_ssize_t n = PySequence_Size(py_positions);
        if (n < 0)
            return NULL;
        if (n > 255) {
            PyErr_SetString(PyExc_ValueError, "position_ids length must be <= 255");
            return NULL;
        }
        total_positions = (uint8_t)n;
        if (total_positions) {
            position_ids = (uint32_t *)DAP_NEW_Z_COUNT(uint32_t, total_positions);
            if (!position_ids) {
                PyErr_SetString(PyExc_MemoryError, "Allocation failed");
                return NULL;
            }
            for (Py_ssize_t i = 0; i < n; i++) {
                PyObject *item = PySequence_GetItem(py_positions, i);
                if (!item) { DAP_DELETE(position_ids); return NULL; }
                unsigned long val = PyLong_AsUnsignedLong(item);
                Py_DECREF(item);
                if (PyErr_Occurred()) { DAP_DELETE(position_ids); return NULL; }
                position_ids[i] = (uint32_t)val;
            }
        }
    }

    size_t data_size = 0;
    byte_t *data = dap_chain_srv_stake_ext_started_tx_event_create(
        &data_size, (uint32_t)multiplier, (dap_time_t)duration, (dap_chain_tx_event_data_time_unit_t)time_unit,
        (uint32_t)calculation_rule_id, total_positions, (uint32_t *)position_ids);

    if (position_ids)
        DAP_DELETE(position_ids);

    if (!data) {
        Py_RETURN_NONE;
    }

    PyObject *py_bytes = PyBytes_FromStringAndSize((const char *)data, (Py_ssize_t)data_size);
    DAP_DELETE(data);
    return py_bytes;
}

static PyObject *wrapping_dap_chain_net_srv_stake_ext_ended_tx_event_create_py(PyObject *self, PyObject *argv)
{
    (void)self;
    dap_time_t end_time;
    unsigned int winners_cnt;
    PyObject *py_winners_ids = NULL;
    if (!PyArg_ParseTuple(argv, "LI|O", &end_time, &winners_cnt, &py_winners_ids))
        return NULL;

    uint32_t *winners_ids = NULL;
    if (py_winners_ids && py_winners_ids != Py_None) {
        if (!PyList_Check(py_winners_ids) && !PyTuple_Check(py_winners_ids)) {
            PyErr_SetString(PyExc_TypeError, "winners_ids must be a list or tuple of integers");
            return NULL;
        }
        Py_ssize_t n = PySequence_Size(py_winners_ids);
        if (n < 0)
            return NULL;
        if (n > 255) {
            PyErr_SetString(PyExc_ValueError, "winners_ids length must be <= 255");
            return NULL;
        }
        winners_cnt = (uint8_t)n;
        if (winners_cnt) {
            winners_ids = (uint32_t *)DAP_NEW_Z_COUNT(uint32_t, winners_cnt);
            if (!winners_ids) {
                PyErr_SetString(PyExc_MemoryError, "Allocation failed");
                return NULL;
            }
        }
        for (Py_ssize_t i = 0; i < n; i++) {
            PyObject *item = PySequence_GetItem(py_winners_ids, i);
            if (!item) { DAP_DELETE(winners_ids); return NULL; }
            unsigned long val = PyLong_AsUnsignedLong(item);
            Py_DECREF(item);
            if (PyErr_Occurred()) { DAP_DELETE(winners_ids); return NULL; }
            winners_ids[i] = (uint32_t)val;
        }
    }

    size_t data_size = 0;
    byte_t *data = dap_chain_srv_stake_ext_ended_tx_event_create(
        &data_size, end_time, winners_cnt, (uint32_t *)winners_ids);

    if (winners_ids)
        DAP_DELETE(winners_ids);

    if (!data) {
        Py_RETURN_NONE;
    }

    PyObject *py_bytes = PyBytes_FromStringAndSize((const char *)data, (Py_ssize_t)data_size);
    DAP_DELETE(data);
    return py_bytes;
}
