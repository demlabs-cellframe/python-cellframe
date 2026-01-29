#include "cellframe.h"
#include "cf_services_ext.h"
#include "cf_services_ext_helpers.h"
#include "dap_chain_net_srv_voting.h"
#include "dap_chain_net_srv_voting_compose.h"
#include "../../datum/cf_datum_internal.h"

// =============================================================================
// VOTING
// =============================================================================

PyObject* dap_chain_net_srv_voting_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    if (!PyArg_ParseTuple(a_args, ""))
        return NULL;
    int l_res = dap_chain_net_srv_voting_init();
    return PyLong_FromLong(l_res);
}

PyObject* dap_chain_net_srv_voting_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    if (!PyArg_ParseTuple(a_args, ""))
        return NULL;
    dap_chain_net_srv_voting_deinit();
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_srv_voting_create_py(PyObject *a_self, PyObject *a_args) {
    char *a_question;
    PyObject *a_options_list;
    long long a_expire_vote;
    long long a_max_vote;
    PyObject *a_fee_obj;
    int a_delegated_key_required;
    int a_vote_changing_allowed;
    PyObject *a_wallet_obj;
    PyObject *a_net_obj;
    char *a_token_ticker;
    char *a_hash_out_type = "hex";
    
    if (!PyArg_ParseTuple(a_args, "sOLLOiiOOs|s", 
        &a_question, &a_options_list, &a_expire_vote, &a_max_vote, &a_fee_obj, 
        &a_delegated_key_required, &a_vote_changing_allowed, &a_wallet_obj, &a_net_obj, 
        &a_token_ticker, &a_hash_out_type))
        return NULL;

    if (!PyList_Check(a_options_list)) {
        PyErr_SetString(PyExc_TypeError, "Options must be a list of strings");
        return NULL;
    }

    dap_list_t *l_options = NULL;
    Py_ssize_t l_size = PyList_Size(a_options_list);
    for (Py_ssize_t i = 0; i < l_size; i++) {
        PyObject *l_item = PyList_GetItem(a_options_list, i);
        if (!PyUnicode_Check(l_item)) {
            dap_list_free_full(l_options, free);
            PyErr_SetString(PyExc_TypeError, "All options must be strings");
            return NULL;
        }
        const char *l_item_str = PyUnicode_AsUTF8(l_item);
        if (!l_item_str) {
            dap_list_free_full(l_options, free);
            return NULL;
        }
        char *l_copy = strdup(l_item_str);
        if (!l_copy) {
            dap_list_free_full(l_options, free);
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate option string");
            return NULL;
        }
        l_options = dap_list_append(l_options, l_copy);
    }
    
    if (!PyCapsule_CheckExact(a_wallet_obj) || !PyCapsule_CheckExact(a_net_obj)) {
        dap_list_free_full(l_options, free);
        PyErr_SetString(PyExc_TypeError, "Invalid capsule arguments");
        return NULL;
    }

    dap_chain_wallet_t *l_wallet = (dap_chain_wallet_t *)PyCapsule_GetPointer(a_wallet_obj, "dap_chain_wallet_t");
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(a_net_obj, "dap_chain_net_t");
    uint256_t l_fee = py_obj_to_uint256(a_fee_obj);
    char *l_hash_output = NULL;

    int l_ret = dap_chain_net_srv_voting_create(
        a_question, l_options, (dap_time_t)a_expire_vote, (uint64_t)a_max_vote, l_fee,
        (bool)a_delegated_key_required, (bool)a_vote_changing_allowed,
        l_wallet, l_net, a_token_ticker, a_hash_out_type, &l_hash_output
    );

    // Free options list (and strings)
    dap_list_free_full(l_options, free);

    if (l_ret != 0) {
        return Py_BuildValue("is", l_ret, NULL);
    }
    
    PyObject *l_ret_obj = Py_BuildValue("is", l_ret, l_hash_output);
    DAP_DELETE(l_hash_output);
    return l_ret_obj;
}

PyObject* dap_chain_net_srv_voting_get_expiration_time_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_ledger_obj = NULL;
    char *a_hash_str = NULL;

    if (!PyArg_ParseTuple(a_args, "Os", &a_ledger_obj, &a_hash_str))
        return NULL;

    if (!PyCapsule_CheckExact(a_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(a_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }

    dap_chain_hash_fast_t l_hash;
    dap_chain_hash_fast_from_str(a_hash_str, &l_hash);

    dap_time_t l_expire_time = dap_chain_net_srv_voting_get_expiration_time(l_ledger, &l_hash);
    return PyLong_FromUnsignedLongLong((unsigned long long)l_expire_time);
}

PyObject* dap_chain_net_srv_voting_get_result_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_ledger_obj = NULL;
    char *a_hash_str = NULL;

    if (!PyArg_ParseTuple(a_args, "Os", &a_ledger_obj, &a_hash_str))
        return NULL;

    if (!PyCapsule_CheckExact(a_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(a_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }

    dap_chain_hash_fast_t l_hash;
    dap_chain_hash_fast_from_str(a_hash_str, &l_hash);

    dap_chain_net_t *l_net = dap_chain_net_by_id(l_ledger->net_id);
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Failed to resolve network for ledger");
        return NULL;
    }

    dap_chain_net_voting_info_t *l_info = dap_chain_net_voting_extract_info(l_net, &l_hash);
    if (!l_info) {
        Py_RETURN_NONE;
    }

    size_t l_count = l_info->options.count_option;
    if (l_count == 0) {
        dap_chain_net_voting_info_free(l_info);
        return PyList_New(0);
    }

    uint64_t *l_results = dap_chain_net_srv_voting_get_result(l_ledger, &l_hash);
    if (!l_results) {
        dap_chain_net_voting_info_free(l_info);
        Py_RETURN_NONE;
    }

    PyObject *l_list = PyList_New((Py_ssize_t)l_count);
    if (!l_list) {
        DAP_DELETE(l_results);
        dap_chain_net_voting_info_free(l_info);
        return NULL;
    }

    for (size_t i = 0; i < l_count; i++) {
        PyObject *l_val = PyLong_FromUnsignedLongLong((unsigned long long)l_results[i]);
        if (!l_val) {
            DAP_DELETE(l_results);
            dap_chain_net_voting_info_free(l_info);
            Py_DECREF(l_list);
            return NULL;
        }
        PyList_SetItem(l_list, (Py_ssize_t)i, l_val);
    }

    DAP_DELETE(l_results);
    dap_chain_net_voting_info_free(l_info);
    return l_list;
}

PyObject* dap_chain_net_srv_vote_create_py(PyObject *a_self, PyObject *a_args) {
    PyObject *a_cert_obj; // Optional?
    PyObject *a_fee_obj;
    PyObject *a_wallet_obj;
    char *a_voting_hash_str;
    long long a_option_idx;
    PyObject *a_net_obj;
    char *a_hash_out_type = "hex";

    if (!PyArg_ParseTuple(a_args, "OOOsLO|s", &a_cert_obj, &a_fee_obj, &a_wallet_obj, 
        &a_voting_hash_str, &a_option_idx, &a_net_obj, &a_hash_out_type))
        return NULL;

    dap_cert_t *l_cert = NULL;
    if (a_cert_obj != Py_None) {
        if (PyCapsule_CheckExact(a_cert_obj)) {
            l_cert = (dap_cert_t *)PyCapsule_GetPointer(a_cert_obj, "dap_cert_t");
        }
    }

    if (!PyCapsule_CheckExact(a_wallet_obj) || !PyCapsule_CheckExact(a_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "Invalid capsule arguments");
        return NULL;
    }

    dap_chain_wallet_t *l_wallet = (dap_chain_wallet_t *)PyCapsule_GetPointer(a_wallet_obj, "dap_chain_wallet_t");
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(a_net_obj, "dap_chain_net_t");
    uint256_t l_fee = py_obj_to_uint256(a_fee_obj);
    
    dap_chain_hash_fast_t l_voting_hash;
    dap_chain_hash_fast_from_str(a_voting_hash_str, &l_voting_hash);

    char *l_hash_tx_out = NULL;
    int l_ret = dap_chain_net_srv_vote_create(
        l_cert, l_fee, l_wallet, &l_voting_hash, (uint64_t)a_option_idx,
        l_net, a_hash_out_type, &l_hash_tx_out
    );

    if (l_ret != 0) {
        return Py_BuildValue("is", l_ret, NULL);
    }

    PyObject *l_ret_obj = Py_BuildValue("is", l_ret, l_hash_tx_out);
    DAP_DELETE(l_hash_tx_out);
    return l_ret_obj;
}

PyObject* dap_chain_net_voting_list_py(PyObject *a_self, PyObject *a_args) {
    PyObject *a_net_obj;
    if (!PyArg_ParseTuple(a_args, "O", &a_net_obj))
        return NULL;
        
    if (!PyCapsule_CheckExact(a_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(a_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }
    dap_list_t *l_list = dap_chain_net_voting_list(l_net);
    
    PyObject *l_py_list = PyList_New(0);
    for (dap_list_t *l_ptr = l_list; l_ptr; l_ptr = l_ptr->next) {
        dap_chain_net_voting_info_t *l_info = (dap_chain_net_voting_info_t*)l_ptr->data;
        PyObject *l_dict = PyDict_New();
        
        char *l_hash_str = dap_chain_hash_fast_to_str_new(&l_info->hash);
        PyDict_SetItemString(l_dict, "hash", PyUnicode_FromString(l_hash_str));
        DAP_DELETE(l_hash_str);
        
        PyDict_SetItemString(l_dict, "is_expired", PyBool_FromLong(l_info->is_expired));
        PyDict_SetItemString(l_dict, "is_cancelled", PyBool_FromLong(l_info->is_cancelled));
        
        PyList_Append(l_py_list, l_dict);
        Py_DECREF(l_dict);
    }
    // dap_chain_net_voting_list returns heap-allocated info items; free them here.
    dap_list_free_full(l_list, (dap_callback_destroyed_t)dap_chain_net_voting_info_free);
    return l_py_list;
}

PyObject* dap_chain_net_voting_extract_info_py(PyObject *a_self, PyObject *a_args) {
    PyObject *a_net_obj;
    char *a_hash_str;
    if (!PyArg_ParseTuple(a_args, "Os", &a_net_obj, &a_hash_str))
        return NULL;
        
    if (!PyCapsule_CheckExact(a_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(a_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }
    dap_chain_hash_fast_t l_hash;
    dap_chain_hash_fast_from_str(a_hash_str, &l_hash);
    dap_chain_net_voting_info_t *l_info = dap_chain_net_voting_extract_info(l_net, &l_hash);
    if (!l_info) Py_RETURN_NONE;
    
    // Return dict info
    PyObject *l_dict = PyDict_New();
    PyDict_SetItemString(l_dict, "is_expired", PyBool_FromLong(l_info->is_expired));
    dap_chain_net_voting_info_free(l_info);
    return l_dict;
}

PyObject* dap_chain_net_voting_info_free_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_info_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &a_info_obj))
        return NULL;

    if (a_info_obj == Py_None) {
        Py_RETURN_NONE;
    }

    if (!PyCapsule_CheckExact(a_info_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a voting info capsule");
        return NULL;
    }

    dap_chain_net_voting_info_t *l_info = (dap_chain_net_voting_info_t *)PyCapsule_GetPointer(
        a_info_obj, "dap_chain_net_voting_info_t");
    if (!l_info) {
        PyErr_SetString(PyExc_ValueError, "Invalid voting info capsule");
        return NULL;
    }

    dap_chain_net_voting_info_free(l_info);
    Py_RETURN_NONE;
}

PyObject* dap_get_options_list_from_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *a_str = NULL;

    if (!PyArg_ParseTuple(a_args, "s", &a_str))
        return NULL;

    dap_list_t *l_list = dap_get_options_list_from_str(a_str);
    if (!l_list) {
        Py_RETURN_NONE;
    }

    PyObject *l_py_list = PyList_New(0);
    if (!l_py_list) {
        dap_list_free_full(l_list, free);
        return NULL;
    }

    for (dap_list_t *l_ptr = l_list; l_ptr; l_ptr = l_ptr->next) {
        const char *l_opt = (const char *)l_ptr->data;
        PyObject *l_item = PyUnicode_FromString(l_opt ? l_opt : "");
        if (!l_item) {
            dap_list_free_full(l_list, free);
            Py_DECREF(l_py_list);
            return NULL;
        }
        PyList_Append(l_py_list, l_item);
        Py_DECREF(l_item);
    }

    dap_list_free_full(l_list, free);
    return l_py_list;
}

PyObject* dap_chain_net_vote_cancel_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_fee_obj = NULL;
    PyObject *a_wallet_obj = NULL;
    PyObject *a_net_obj = NULL;
    char *a_voting_hash_str = NULL;
    char *a_hash_out_type = "hex";

    if (!PyArg_ParseTuple(a_args, "OOsO|s", &a_fee_obj, &a_wallet_obj, &a_voting_hash_str,
        &a_net_obj, &a_hash_out_type))
        return NULL;

    if (!PyCapsule_CheckExact(a_wallet_obj) || !PyCapsule_CheckExact(a_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "Invalid capsule arguments");
        return NULL;
    }

    dap_chain_wallet_t *l_wallet = (dap_chain_wallet_t *)PyCapsule_GetPointer(a_wallet_obj, "dap_chain_wallet_t");
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(a_net_obj, "dap_chain_net_t");
    if (!l_wallet || !l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid wallet or network capsule");
        return NULL;
    }

    uint256_t l_fee = py_obj_to_uint256(a_fee_obj);
    dap_chain_hash_fast_t l_hash;
    dap_chain_hash_fast_from_str(a_voting_hash_str, &l_hash);

    dap_json_t *l_json = dap_json_array_new();
    if (!l_json) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate JSON reply");
        return NULL;
    }

    char *l_hash_tx_out = NULL;
    int l_ret = dap_chain_net_vote_cancel(
        l_json, l_fee, l_wallet, &l_hash, l_net, a_hash_out_type, &l_hash_tx_out);

    char *l_json_str = dap_json_to_string(l_json);
    dap_json_object_free(l_json);

    PyObject *l_json_obj = NULL;
    if (l_json_str) {
        l_json_obj = PyUnicode_FromString(l_json_str);
        DAP_DELETE(l_json_str);
        if (!l_json_obj) {
            DAP_DELETE(l_hash_tx_out);
            return NULL;
        }
    } else {
        Py_INCREF(Py_None);
        l_json_obj = Py_None;
    }

    PyObject *l_tuple = PyTuple_New(3);
    if (!l_tuple) {
        Py_DECREF(l_json_obj);
        DAP_DELETE(l_hash_tx_out);
        return NULL;
    }

    PyTuple_SetItem(l_tuple, 0, PyLong_FromLong(l_ret));
    if (l_hash_tx_out) {
        PyObject *l_hash_obj = PyUnicode_FromString(l_hash_tx_out);
        DAP_DELETE(l_hash_tx_out);
        if (!l_hash_obj) {
            Py_DECREF(l_tuple);
            Py_DECREF(l_json_obj);
            return NULL;
        }
        PyTuple_SetItem(l_tuple, 1, l_hash_obj);
    } else {
        Py_INCREF(Py_None);
        PyTuple_SetItem(l_tuple, 1, Py_None);
    }
    PyTuple_SetItem(l_tuple, 2, l_json_obj);
    return l_tuple;
}

// =============================================================================
// VOTING COMPOSE
// =============================================================================

PyObject* dap_chain_net_srv_voting_compose_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    if (!PyArg_ParseTuple(a_args, ""))
        return NULL;
    int l_res = dap_chain_net_srv_voting_compose_init();
    return PyLong_FromLong(l_res);
}

PyObject* dap_chain_net_srv_voting_compose_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    if (!PyArg_ParseTuple(a_args, ""))
        return NULL;
    dap_chain_net_srv_voting_compose_deinit();
    Py_RETURN_NONE;
}

PyObject* dap_chain_tx_compose_poll_create_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long a_net_id_uint = 0;
    const char *a_question_str = NULL;
    const char *a_options_list_str = NULL;
    const char *a_voting_expire_str = NULL;
    const char *a_max_votes_count_str = NULL;
    const char *a_fee_str = NULL;
    int a_is_delegated_key = 0;
    int a_is_vote_changing_allowed = 0;
    PyObject *a_wallet_addr_obj = NULL;
    const char *a_token_str = NULL;

    if (!PyArg_ParseTuple(a_args, "KsssssiiOs", &a_net_id_uint, &a_question_str, &a_options_list_str,
        &a_voting_expire_str, &a_max_votes_count_str, &a_fee_str, &a_is_delegated_key,
        &a_is_vote_changing_allowed, &a_wallet_addr_obj, &a_token_str))
        return NULL;

    dap_chain_addr_t l_wallet_addr = {};
    if (cf_datum_parse_addr(a_wallet_addr_obj, &l_wallet_addr) != 0) {
        return NULL;
    }

    dap_chain_net_id_t l_net_id = {.uint64 = a_net_id_uint};
    dap_json_t *l_json = dap_chain_tx_compose_poll_create(
        l_net_id, a_question_str, a_options_list_str, a_voting_expire_str, a_max_votes_count_str,
        a_fee_str, (bool)a_is_delegated_key, (bool)a_is_vote_changing_allowed, &l_wallet_addr,
        a_token_str);
    if (!l_json) {
        Py_RETURN_NONE;
    }

    char *l_json_str = dap_json_to_string(l_json);
    dap_json_object_free(l_json);
    if (!l_json_str) {
        Py_RETURN_NONE;
    }

    PyObject *l_result = PyUnicode_FromString(l_json_str);
    DAP_DELETE(l_json_str);
    return l_result;
}

PyObject* dap_chain_tx_compose_poll_vote_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long a_net_id_uint = 0;
    const char *a_hash_str = NULL;
    const char *a_cert_name = NULL;
    const char *a_fee_str = NULL;
    PyObject *a_wallet_addr_obj = NULL;
    const char *a_option_idx_str = NULL;

    if (!PyArg_ParseTuple(a_args, "KszsOs", &a_net_id_uint, &a_hash_str, &a_cert_name,
        &a_fee_str, &a_wallet_addr_obj, &a_option_idx_str))
        return NULL;

    dap_chain_addr_t l_wallet_addr = {};
    if (cf_datum_parse_addr(a_wallet_addr_obj, &l_wallet_addr) != 0) {
        return NULL;
    }

    dap_chain_net_id_t l_net_id = {.uint64 = a_net_id_uint};
    dap_json_t *l_json = dap_chain_tx_compose_poll_vote(
        l_net_id, a_hash_str, a_cert_name, a_fee_str, &l_wallet_addr, a_option_idx_str);
    if (!l_json) {
        Py_RETURN_NONE;
    }

    char *l_json_str = dap_json_to_string(l_json);
    dap_json_object_free(l_json);
    if (!l_json_str) {
        Py_RETURN_NONE;
    }

    PyObject *l_result = PyUnicode_FromString(l_json_str);
    DAP_DELETE(l_json_str);
    return l_result;
}

PyObject* dap_voting_tx_create_poll_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_ledger_obj = NULL;
    char *a_question = NULL;
    PyObject *a_options_list = NULL;
    long long a_expire_vote = 0;
    long long a_max_vote = 0;
    PyObject *a_fee_obj = NULL;
    int a_delegated_key_required = 0;
    int a_vote_changing_allowed = 0;
    PyObject *a_wallet_addr_obj = NULL;
    char *a_token_ticker = NULL;

    if (!PyArg_ParseTuple(a_args, "OsOLLOiiOs", &a_ledger_obj, &a_question, &a_options_list,
        &a_expire_vote, &a_max_vote, &a_fee_obj, &a_delegated_key_required,
        &a_vote_changing_allowed, &a_wallet_addr_obj, &a_token_ticker))
        return NULL;

    if (!PyCapsule_CheckExact(a_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(a_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }

    if (!PyList_Check(a_options_list)) {
        PyErr_SetString(PyExc_TypeError, "Options must be a list of strings");
        return NULL;
    }

    dap_list_t *l_options = NULL;
    Py_ssize_t l_size = PyList_Size(a_options_list);
    for (Py_ssize_t i = 0; i < l_size; i++) {
        PyObject *l_item = PyList_GetItem(a_options_list, i);
        if (!PyUnicode_Check(l_item)) {
            dap_list_free_full(l_options, free);
            PyErr_SetString(PyExc_TypeError, "All options must be strings");
            return NULL;
        }
        const char *l_item_str = PyUnicode_AsUTF8(l_item);
        if (!l_item_str) {
            dap_list_free_full(l_options, free);
            return NULL;
        }
        char *l_copy = strdup(l_item_str);
        if (!l_copy) {
            dap_list_free_full(l_options, free);
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate option string");
            return NULL;
        }
        l_options = dap_list_append(l_options, l_copy);
    }

    dap_chain_addr_t l_wallet_addr = {};
    if (cf_datum_parse_addr(a_wallet_addr_obj, &l_wallet_addr) != 0) {
        dap_list_free_full(l_options, free);
        return NULL;
    }

    uint256_t l_fee = py_obj_to_uint256(a_fee_obj);
    dap_chain_datum_tx_t *l_tx = dap_voting_tx_create_poll(
        l_ledger, a_question, l_options, (dap_time_t)a_expire_vote, (uint64_t)a_max_vote,
        l_fee, (bool)a_delegated_key_required, (bool)a_vote_changing_allowed, &l_wallet_addr,
        a_token_ticker);

    dap_list_free_full(l_options, free);

    if (!l_tx) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}

PyObject* dap_voting_tx_create_vote_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_ledger_obj = NULL;
    char *a_hash_str = NULL;
    long long a_option_idx = 0;
    PyObject *a_fee_obj = NULL;
    PyObject *a_wallet_addr_obj = NULL;
    PyObject *a_cert_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "OsLOO|O", &a_ledger_obj, &a_hash_str, &a_option_idx,
        &a_fee_obj, &a_wallet_addr_obj, &a_cert_obj))
        return NULL;

    if (!PyCapsule_CheckExact(a_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(a_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }

    dap_chain_addr_t l_wallet_addr = {};
    if (cf_datum_parse_addr(a_wallet_addr_obj, &l_wallet_addr) != 0) {
        return NULL;
    }

    dap_cert_t *l_cert = NULL;
    if (a_cert_obj != Py_None) {
        if (!PyCapsule_CheckExact(a_cert_obj)) {
            PyErr_SetString(PyExc_TypeError, "cert must be a certificate capsule or None");
            return NULL;
        }
        l_cert = (dap_cert_t *)PyCapsule_GetPointer(a_cert_obj, "dap_cert_t");
        if (!l_cert) {
            PyErr_SetString(PyExc_ValueError, "Invalid certificate capsule");
            return NULL;
        }
    }

    uint256_t l_fee = py_obj_to_uint256(a_fee_obj);
    dap_chain_hash_fast_t l_hash;
    dap_chain_hash_fast_from_str(a_hash_str, &l_hash);

    dap_chain_datum_tx_t *l_tx = dap_voting_tx_create_vote(
        l_ledger, &l_hash, (uint64_t)a_option_idx, l_fee, &l_wallet_addr, l_cert);
    if (!l_tx) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}

