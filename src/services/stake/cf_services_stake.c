#include "cellframe.h"
#include "cf_services_ext.h"
#include "cf_services_ext_helpers.h"
#include "dap_chain_net_srv_order.h"
#include "dap_chain_net_srv_stake.h"
#include "dap_chain_net_srv_stake_decree.h"
#include "dap_chain_net_srv_stake_pos_delegate.h"
#include "dap_chain_net_srv_stake_tx_builder.h"
#include "../../datum/cf_datum_internal.h"

// =============================================================================
// STAKE CORE + POS DELEGATE + TX BUILDERS
// =============================================================================

PyObject* dap_chain_net_srv_stake_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    if (!PyArg_ParseTuple(a_args, ""))
        return NULL;
    int l_rc = dap_chain_net_srv_stake_init();
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_net_srv_stake_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    if (!PyArg_ParseTuple(a_args, ""))
        return NULL;
    dap_chain_net_srv_stake_deinit();
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_srv_stake_pos_delegate_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    if (!PyArg_ParseTuple(a_args, ""))
        return NULL;
    int l_rc = dap_chain_net_srv_stake_pos_delegate_init();
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_net_srv_stake_pos_delegate_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    if (!PyArg_ParseTuple(a_args, ""))
        return NULL;
    dap_chain_net_srv_stake_pos_delegate_deinit();
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_srv_stake_decree_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    if (!PyArg_ParseTuple(a_args, ""))
        return NULL;
    int l_rc = dap_chain_net_srv_stake_decree_init();
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_net_srv_stake_decree_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    if (!PyArg_ParseTuple(a_args, ""))
        return NULL;
    dap_chain_net_srv_stake_decree_deinit();
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_srv_stake_key_delegate_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj = NULL;
    PyObject *a_signing_addr_obj = NULL;
    PyObject *a_decree_hash_obj = NULL;
    PyObject *a_tx_hash_obj = NULL;
    PyObject *a_value_obj = NULL;
    PyObject *a_node_addr_obj = NULL;
    PyObject *a_pkey_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OOOOOOO", &a_net_obj, &a_signing_addr_obj, &a_decree_hash_obj,
                          &a_tx_hash_obj, &a_value_obj, &a_node_addr_obj, &a_pkey_obj))
        return NULL;

    dap_chain_net_t *l_net = cf_services_get_net(a_net_obj);
    if (!l_net) {
        return NULL;
    }

    dap_chain_addr_t l_signing_addr = {};
    if (cf_datum_parse_addr(a_signing_addr_obj, &l_signing_addr) != 0) {
        return NULL;
    }

    dap_hash_fast_t l_decree_hash = {};
    dap_hash_fast_t *l_decree_ptr = NULL;
    if (a_decree_hash_obj && a_decree_hash_obj != Py_None) {
        if (cf_datum_parse_hash_fast(a_decree_hash_obj, &l_decree_hash) != 0) {
            return NULL;
        }
        l_decree_ptr = &l_decree_hash;
    }

    dap_hash_fast_t l_tx_hash = {};
    dap_hash_fast_t *l_tx_ptr = NULL;
    if (a_tx_hash_obj && a_tx_hash_obj != Py_None) {
        if (cf_datum_parse_hash_fast(a_tx_hash_obj, &l_tx_hash) != 0) {
            return NULL;
        }
        l_tx_ptr = &l_tx_hash;
    }

    uint256_t l_value = {};
    if (cf_datum_parse_uint256(a_value_obj, &l_value) != 0) {
        return NULL;
    }

    dap_chain_node_addr_t l_node_addr = {};
    if (cf_datum_parse_node_addr(a_node_addr_obj, &l_node_addr) != 0) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(a_pkey_obj)) {
        PyErr_SetString(PyExc_TypeError, "pkey must be a dap_pkey_t capsule");
        return NULL;
    }
    dap_pkey_t *l_pkey = (dap_pkey_t *)PyCapsule_GetPointer(a_pkey_obj, "dap_pkey_t");
    if (!l_pkey) {
        PyErr_SetString(PyExc_ValueError, "Invalid dap_pkey_t capsule");
        return NULL;
    }

    dap_chain_net_srv_stake_key_delegate(
        l_net, &l_signing_addr, l_decree_ptr, l_tx_ptr, l_value, &l_node_addr, l_pkey);
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_srv_stake_key_invalidate_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_signing_addr_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "O", &a_signing_addr_obj))
        return NULL;

    dap_chain_addr_t l_signing_addr = {};
    if (cf_datum_parse_addr(a_signing_addr_obj, &l_signing_addr) != 0) {
        return NULL;
    }

    dap_chain_net_srv_stake_key_invalidate(&l_signing_addr);
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_srv_stake_key_update_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_signing_addr_obj = NULL;
    PyObject *a_value_obj = NULL;
    PyObject *a_tx_hash_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "OOO", &a_signing_addr_obj, &a_value_obj, &a_tx_hash_obj))
        return NULL;

    dap_chain_addr_t l_signing_addr = {};
    if (cf_datum_parse_addr(a_signing_addr_obj, &l_signing_addr) != 0) {
        return NULL;
    }

    uint256_t l_value = {};
    if (cf_datum_parse_uint256(a_value_obj, &l_value) != 0) {
        return NULL;
    }

    dap_hash_fast_t l_tx_hash = {};
    if (cf_datum_parse_hash_fast(a_tx_hash_obj, &l_tx_hash) != 0) {
        return NULL;
    }

    dap_chain_net_srv_stake_key_update(&l_signing_addr, l_value, &l_tx_hash);
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_srv_stake_pkey_update_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj = NULL;
    PyObject *a_pkey_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "OO", &a_net_obj, &a_pkey_obj))
        return NULL;

    dap_chain_net_t *l_net = cf_services_get_net(a_net_obj);
    if (!l_net) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(a_pkey_obj)) {
        PyErr_SetString(PyExc_TypeError, "pkey must be a dap_pkey_t capsule");
        return NULL;
    }
    dap_pkey_t *l_pkey = (dap_pkey_t *)PyCapsule_GetPointer(a_pkey_obj, "dap_pkey_t");
    if (!l_pkey) {
        PyErr_SetString(PyExc_ValueError, "Invalid dap_pkey_t capsule");
        return NULL;
    }

    dap_chain_net_srv_stake_pkey_update(l_net, l_pkey);
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_srv_stake_set_allowed_min_value_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj = NULL;
    PyObject *a_value_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "OO", &a_net_obj, &a_value_obj))
        return NULL;

    dap_chain_net_t *l_net = cf_services_get_net(a_net_obj);
    if (!l_net) {
        return NULL;
    }

    uint256_t l_value = {};
    if (cf_datum_parse_uint256(a_value_obj, &l_value) != 0) {
        return NULL;
    }

    dap_chain_net_srv_stake_set_allowed_min_value(l_net->pub.id, l_value);
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_srv_stake_get_allowed_min_value_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "O", &a_net_obj))
        return NULL;

    dap_chain_net_t *l_net = cf_services_get_net(a_net_obj);
    if (!l_net) {
        return NULL;
    }

    uint256_t l_value = dap_chain_net_srv_stake_get_allowed_min_value(l_net->pub.id);
    return cf_services_uint256_to_py_str(l_value);
}

PyObject* dap_chain_net_srv_stake_set_percent_max_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj = NULL;
    PyObject *a_value_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "OO", &a_net_obj, &a_value_obj))
        return NULL;

    dap_chain_net_t *l_net = cf_services_get_net(a_net_obj);
    if (!l_net) {
        return NULL;
    }

    uint256_t l_value = {};
    if (cf_datum_parse_uint256(a_value_obj, &l_value) != 0) {
        return NULL;
    }

    dap_chain_net_srv_stake_set_percent_max(l_net->pub.id, l_value);
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_srv_stake_get_percent_max_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "O", &a_net_obj))
        return NULL;

    dap_chain_net_t *l_net = cf_services_get_net(a_net_obj);
    if (!l_net) {
        return NULL;
    }

    uint256_t l_value = dap_chain_net_srv_stake_get_percent_max(l_net->pub.id);
    return cf_services_uint256_to_py_str(l_value);
}

PyObject* dap_chain_net_srv_stake_key_delegated_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_signing_addr_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "O", &a_signing_addr_obj))
        return NULL;

    dap_chain_addr_t l_signing_addr = {};
    if (cf_datum_parse_addr(a_signing_addr_obj, &l_signing_addr) != 0) {
        return NULL;
    }

    int l_rc = dap_chain_net_srv_stake_key_delegated(&l_signing_addr);
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_net_srv_stake_verify_key_and_node_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_signing_addr_obj = NULL;
    PyObject *a_node_addr_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "OO", &a_signing_addr_obj, &a_node_addr_obj))
        return NULL;

    dap_chain_addr_t l_signing_addr = {};
    if (cf_datum_parse_addr(a_signing_addr_obj, &l_signing_addr) != 0) {
        return NULL;
    }

    dap_chain_node_addr_t l_node_addr = {};
    if (cf_datum_parse_node_addr(a_node_addr_obj, &l_node_addr) != 0) {
        return NULL;
    }

    int l_rc = dap_chain_net_srv_stake_verify_key_and_node(&l_signing_addr, &l_node_addr);
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_net_srv_stake_get_validators_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj = NULL;
    int a_only_active = 1;
    if (!PyArg_ParseTuple(a_args, "O|p", &a_net_obj, &a_only_active))
        return NULL;

    dap_chain_net_t *l_net = cf_services_get_net(a_net_obj);
    if (!l_net) {
        return NULL;
    }

    dap_list_t *l_list = dap_chain_net_srv_stake_get_validators(
        l_net->pub.id, a_only_active ? true : false, NULL);

    PyObject *l_py_list = PyList_New(0);
    if (!l_list) {
        return l_py_list;
    }

    for (dap_list_t *l_ptr = l_list; l_ptr; l_ptr = l_ptr->next) {
        dap_chain_net_srv_stake_item_t *l_item = (dap_chain_net_srv_stake_item_t *)l_ptr->data;
        PyObject *l_dict = cf_services_stake_item_to_dict(l_item);
        if (l_dict) {
            PyList_Append(l_py_list, l_dict);
            Py_DECREF(l_dict);
        }
        DAP_DELETE(l_item);
    }
    dap_list_free(l_list);
    return l_py_list;
}

PyObject* dap_chain_net_srv_stake_get_fee_validators_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "O", &a_net_obj))
        return NULL;

    dap_chain_net_t *l_net = cf_services_get_net(a_net_obj);
    if (!l_net) {
        return NULL;
    }

    uint256_t l_max = {};
    uint256_t l_avg = {};
    uint256_t l_min = {};
    uint256_t l_median = {};
    bool l_has_fee = dap_chain_net_srv_stake_get_fee_validators(l_net, &l_max, &l_avg, &l_min, &l_median);
    if (!l_has_fee) {
        Py_RETURN_NONE;
    }

    PyObject *l_dict = PyDict_New();
    PyObject *l_max_obj = cf_services_uint256_to_py_str(l_max);
    PyObject *l_avg_obj = cf_services_uint256_to_py_str(l_avg);
    PyObject *l_min_obj = cf_services_uint256_to_py_str(l_min);
    PyObject *l_median_obj = cf_services_uint256_to_py_str(l_median);
    PyDict_SetItemString(l_dict, "max_fee", l_max_obj);
    PyDict_SetItemString(l_dict, "average_fee", l_avg_obj);
    PyDict_SetItemString(l_dict, "min_fee", l_min_obj);
    PyDict_SetItemString(l_dict, "median_fee", l_median_obj);
    Py_DECREF(l_max_obj);
    Py_DECREF(l_avg_obj);
    Py_DECREF(l_min_obj);
    Py_DECREF(l_median_obj);
    return l_dict;
}

PyObject* dap_chain_net_srv_stake_load_cache_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "O", &a_net_obj))
        return NULL;

    dap_chain_net_t *l_net = cf_services_get_net(a_net_obj);
    if (!l_net) {
        return NULL;
    }

    int l_rc = dap_chain_net_srv_stake_load_cache(l_net);
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_net_srv_stake_check_validator_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj = NULL;
    PyObject *a_tx_hash_obj = NULL;
    int a_time_connect = 0;
    int a_time_response = 0;

    if (!PyArg_ParseTuple(a_args, "OOii", &a_net_obj, &a_tx_hash_obj, &a_time_connect, &a_time_response))
        return NULL;

    dap_chain_net_t *l_net = cf_services_get_net(a_net_obj);
    if (!l_net) {
        return NULL;
    }

    dap_hash_fast_t l_hash = {};
    if (cf_datum_parse_hash_fast(a_tx_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    dap_chain_ch_validator_test_t l_out = {};
    int l_rc = dap_chain_net_srv_stake_check_validator(
        l_net, &l_hash, &l_out, a_time_connect, a_time_response);

    PyObject *l_header = PyDict_New();
    PyObject *l_version = PyBytes_FromStringAndSize((const char *)l_out.header.version, sizeof(l_out.header.version));
    PyObject *l_flags = PyLong_FromUnsignedLong((unsigned long)l_out.header.flags);
    PyObject *l_sign_size = PyLong_FromUnsignedLong((unsigned long)l_out.header.sign_size);
    PyObject *l_sign_correct = PyLong_FromUnsignedLong((unsigned long)l_out.header.sign_correct);
    PyObject *l_overall_correct = PyLong_FromUnsignedLong((unsigned long)l_out.header.overall_correct);
    PyDict_SetItemString(l_header, "version", l_version);
    PyDict_SetItemString(l_header, "flags", l_flags);
    PyDict_SetItemString(l_header, "sign_size", l_sign_size);
    PyDict_SetItemString(l_header, "sign_correct", l_sign_correct);
    PyDict_SetItemString(l_header, "overall_correct", l_overall_correct);
    Py_DECREF(l_version);
    Py_DECREF(l_flags);
    Py_DECREF(l_sign_size);
    Py_DECREF(l_sign_correct);
    Py_DECREF(l_overall_correct);

    PyObject *l_tuple = PyTuple_New(2);
    PyTuple_SetItem(l_tuple, 0, PyLong_FromLong(l_rc));
    PyTuple_SetItem(l_tuple, 1, l_header);
    return l_tuple;
}

PyObject* dap_chain_net_srv_stake_decree_approve_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj = NULL;
    PyObject *a_tx_hash_obj = NULL;
    PyObject *a_cert_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "OOO", &a_net_obj, &a_tx_hash_obj, &a_cert_obj))
        return NULL;

    dap_chain_net_t *l_net = cf_services_get_net(a_net_obj);
    if (!l_net) {
        return NULL;
    }

    dap_hash_fast_t l_hash = {};
    if (cf_datum_parse_hash_fast(a_tx_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(a_cert_obj)) {
        PyErr_SetString(PyExc_TypeError, "cert must be a certificate capsule");
        return NULL;
    }
    dap_cert_t *l_cert = (dap_cert_t *)PyCapsule_GetPointer(a_cert_obj, "dap_cert_t");
    if (!l_cert) {
        PyErr_SetString(PyExc_ValueError, "Invalid certificate capsule");
        return NULL;
    }

    dap_chain_datum_decree_t *l_decree = dap_chain_net_srv_stake_decree_approve(l_net, &l_hash, l_cert);
    if (!l_decree) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(l_decree, "dap_chain_datum_decree_t", NULL);
}

PyObject* dap_chain_net_srv_stake_mark_validator_active_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_signing_addr_obj = NULL;
    int a_on_off = 0;
    if (!PyArg_ParseTuple(a_args, "Op", &a_signing_addr_obj, &a_on_off))
        return NULL;

    dap_chain_addr_t l_signing_addr = {};
    if (cf_datum_parse_addr(a_signing_addr_obj, &l_signing_addr) != 0) {
        return NULL;
    }

    int l_rc = dap_chain_net_srv_stake_mark_validator_active(&l_signing_addr, (bool)a_on_off);
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_net_srv_stake_check_pkey_hash_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj = NULL;
    PyObject *a_hash_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "OO", &a_net_obj, &a_hash_obj))
        return NULL;

    dap_chain_net_t *l_net = cf_services_get_net(a_net_obj);
    if (!l_net) {
        return NULL;
    }

    dap_hash_fast_t l_hash = {};
    if (cf_datum_parse_hash_fast(a_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    dap_chain_net_srv_stake_item_t *l_item = dap_chain_net_srv_stake_check_pkey_hash(l_net->pub.id, &l_hash);
    if (!l_item) {
        Py_RETURN_NONE;
    }
    return cf_services_stake_item_to_dict(l_item);
}

PyObject* dap_chain_net_srv_stake_get_total_weight_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "O", &a_net_obj))
        return NULL;

    dap_chain_net_t *l_net = cf_services_get_net(a_net_obj);
    if (!l_net) {
        return NULL;
    }

    uint256_t l_locked = {};
    uint256_t l_total = dap_chain_net_srv_stake_get_total_weight(l_net->pub.id, &l_locked);

    PyObject *l_dict = PyDict_New();
    PyObject *l_total_obj = cf_services_uint256_to_py_str(l_total);
    PyObject *l_locked_obj = cf_services_uint256_to_py_str(l_locked);
    PyDict_SetItemString(l_dict, "total_weight", l_total_obj);
    PyDict_SetItemString(l_dict, "locked_weight", l_locked_obj);
    Py_DECREF(l_total_obj);
    Py_DECREF(l_locked_obj);
    return l_dict;
}

PyObject* dap_chain_net_srv_stake_get_total_keys_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "O", &a_net_obj))
        return NULL;

    dap_chain_net_t *l_net = cf_services_get_net(a_net_obj);
    if (!l_net) {
        return NULL;
    }

    size_t l_inactive = 0;
    size_t l_total = dap_chain_net_srv_stake_get_total_keys(l_net->pub.id, &l_inactive);

    PyObject *l_dict = PyDict_New();
    PyObject *l_total_obj = PyLong_FromSize_t(l_total);
    PyObject *l_inactive_obj = PyLong_FromSize_t(l_inactive);
    PyDict_SetItemString(l_dict, "total", l_total_obj);
    PyDict_SetItemString(l_dict, "inactive", l_inactive_obj);
    Py_DECREF(l_total_obj);
    Py_DECREF(l_inactive_obj);
    return l_dict;
}

PyObject* dap_chain_net_srv_stake_add_approving_decree_info_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_decree_obj = NULL;
    PyObject *a_net_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "OO", &a_decree_obj, &a_net_obj))
        return NULL;

    if (!PyCapsule_CheckExact(a_decree_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a decree capsule");
        return NULL;
    }
    dap_chain_datum_decree_t *l_decree = (dap_chain_datum_decree_t *)PyCapsule_GetPointer(
        a_decree_obj, "dap_chain_datum_decree_t");
    if (!l_decree) {
        PyErr_SetString(PyExc_ValueError, "Invalid decree capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = cf_services_get_net(a_net_obj);
    if (!l_net) {
        return NULL;
    }

    dap_chain_net_srv_stake_add_approving_decree_info(l_decree, l_net);
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_srv_stake_remove_approving_decree_info_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj = NULL;
    PyObject *a_signing_addr_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "OO", &a_net_obj, &a_signing_addr_obj))
        return NULL;

    dap_chain_net_t *l_net = cf_services_get_net(a_net_obj);
    if (!l_net) {
        return NULL;
    }

    dap_chain_addr_t l_signing_addr = {};
    if (cf_datum_parse_addr(a_signing_addr_obj, &l_signing_addr) != 0) {
        return NULL;
    }

    dap_chain_net_srv_stake_remove_approving_decree_info(l_net, &l_signing_addr);
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_srv_stake_hardfork_data_export_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "O", &a_net_obj))
        return NULL;

    dap_chain_net_t *l_net = cf_services_get_net(a_net_obj);
    if (!l_net) {
        return NULL;
    }

    dap_list_t *l_list = NULL;
    int l_rc = dap_chain_net_srv_stake_hardfork_data_export(l_net, &l_list);
    if (l_rc != 0) {
        if (l_list) {
            for (dap_list_t *l_ptr = l_list; l_ptr; l_ptr = l_ptr->next) {
                DAP_DELETE(l_ptr->data);
            }
            dap_list_free(l_list);
        }
        return Py_BuildValue("iO", l_rc, Py_None);
    }

    PyObject *l_py_list = PyList_New(0);
    for (dap_list_t *l_ptr = l_list; l_ptr; l_ptr = l_ptr->next) {
        dap_tsd_t *l_tsd = (dap_tsd_t *)l_ptr->data;
        if (l_tsd && l_tsd->type == DAP_CHAIN_DATUM_DECREE_TSD_TYPE_HASH && l_tsd->size >= sizeof(dap_hash_fast_t)) {
            dap_hash_fast_t *l_hash = (dap_hash_fast_t *)l_tsd->data;
            char *l_hash_str = dap_chain_hash_fast_to_str_new(l_hash);
            if (l_hash_str) {
                PyObject *l_hash_obj = PyUnicode_FromString(l_hash_str);
                PyList_Append(l_py_list, l_hash_obj);
                Py_DECREF(l_hash_obj);
                DAP_DELETE(l_hash_str);
            }
        }
        DAP_DELETE(l_tsd);
    }
    dap_list_free(l_list);
    return Py_BuildValue("iN", l_rc, l_py_list);
}

PyObject* dap_chain_net_srv_stake_hardfork_data_import_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj = NULL;
    PyObject *a_hash_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "OO", &a_net_obj, &a_hash_obj))
        return NULL;

    dap_chain_net_t *l_net = cf_services_get_net(a_net_obj);
    if (!l_net) {
        return NULL;
    }

    dap_hash_fast_t l_hash = {};
    if (cf_datum_parse_hash_fast(a_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    int l_rc = dap_chain_net_srv_stake_hardfork_data_import(l_net->pub.id, &l_hash);
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_net_srv_stake_hardfork_data_verify_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj = NULL;
    PyObject *a_hash_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "OO", &a_net_obj, &a_hash_obj))
        return NULL;

    dap_chain_net_t *l_net = cf_services_get_net(a_net_obj);
    if (!l_net) {
        return NULL;
    }

    dap_hash_fast_t l_hash = {};
    if (cf_datum_parse_hash_fast(a_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    int l_rc = dap_chain_net_srv_stake_hardfork_data_verify(l_net, &l_hash);
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_net_srv_stake_switch_table_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj = NULL;
    int a_to_sandbox = 0;
    if (!PyArg_ParseTuple(a_args, "Op", &a_net_obj, &a_to_sandbox))
        return NULL;

    dap_chain_net_t *l_net = cf_services_get_net(a_net_obj);
    if (!l_net) {
        return NULL;
    }

    int l_rc = dap_chain_net_srv_stake_switch_table(l_net->pub.id, (bool)a_to_sandbox);
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_net_srv_stake_get_pkey_by_hash_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj = NULL;
    PyObject *a_hash_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "OO", &a_net_obj, &a_hash_obj))
        return NULL;

    dap_chain_net_t *l_net = cf_services_get_net(a_net_obj);
    if (!l_net) {
        return NULL;
    }

    dap_hash_fast_t l_hash = {};
    if (cf_datum_parse_hash_fast(a_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    dap_pkey_t *l_pkey = dap_chain_net_srv_stake_get_pkey_by_hash(l_net->pub.id, &l_hash);
    if (!l_pkey) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(l_pkey, "dap_pkey_t", NULL);
}

PyObject* dap_chain_net_srv_stake_hardfork_tx_update_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "O", &a_net_obj))
        return NULL;

    dap_chain_net_t *l_net = cf_services_get_net(a_net_obj);
    if (!l_net) {
        return NULL;
    }

    dap_chain_net_srv_stake_hardfork_tx_update(l_net);
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_srv_stake_get_validator_ext_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_order_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "O", &a_order_obj))
        return NULL;

    if (!PyCapsule_CheckExact(a_order_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be an order capsule");
        return NULL;
    }
    dap_chain_net_srv_order_t *l_order = (dap_chain_net_srv_order_t *)PyCapsule_GetPointer(
        a_order_obj, "dap_chain_net_srv_order_t");
    if (!l_order) {
        PyErr_SetString(PyExc_ValueError, "Invalid order capsule");
        return NULL;
    }

    uint256_t l_tax = {};
    uint256_t l_value_max = {};
    int l_rc = dap_chain_net_srv_stake_get_validator_ext(l_order, &l_tax, &l_value_max);

    PyObject *l_tax_str = cf_services_uint256_to_py_str(l_tax);
    PyObject *l_value_max_str = cf_services_uint256_to_py_str(l_value_max);
    PyObject *l_tuple = PyTuple_New(3);
    PyTuple_SetItem(l_tuple, 0, PyLong_FromLong(l_rc));
    PyTuple_SetItem(l_tuple, 1, l_tax_str);
    PyTuple_SetItem(l_tuple, 2, l_value_max_str);
    return l_tuple;
}

PyObject* dap_chain_net_srv_stake_tx_builders_register_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    if (!PyArg_ParseTuple(a_args, ""))
        return NULL;
    int l_rc = dap_stake_tx_builders_register();
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_net_srv_stake_tx_builders_unregister_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    if (!PyArg_ParseTuple(a_args, ""))
        return NULL;
    dap_stake_tx_builders_unregister();
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_srv_stake_tx_create_lock_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_used_outs_obj = Py_None;
    PyObject *a_wallet_addr_obj = NULL;
    const char *a_main_ticker = NULL;
    PyObject *a_value_obj = NULL;
    PyObject *a_fee_obj = NULL;
    unsigned long long a_time_staking = 0;
    PyObject *a_reinvest_obj = NULL;
    PyObject *a_delegated_ticker_obj = NULL;
    PyObject *a_delegated_value_obj = NULL;
    unsigned long long a_chain_id = 0;
    unsigned long long a_srv_uid = 0;

    if (!PyArg_ParseTuple(a_args, "OOsOOKOOOKK", &a_used_outs_obj, &a_wallet_addr_obj, &a_main_ticker,
                          &a_value_obj, &a_fee_obj, &a_time_staking, &a_reinvest_obj,
                          &a_delegated_ticker_obj, &a_delegated_value_obj, &a_chain_id, &a_srv_uid))
        return NULL;

    dap_list_t *l_used_outs = NULL;
    if (a_used_outs_obj && a_used_outs_obj != Py_None) {
        if (!PyCapsule_CheckExact(a_used_outs_obj)) {
            PyErr_SetString(PyExc_TypeError, "First argument must be a dap_list_t capsule or None");
            return NULL;
        }
        l_used_outs = (dap_list_t *)PyCapsule_GetPointer(a_used_outs_obj, "dap_list_t");
        if (!l_used_outs) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_list_t capsule");
            return NULL;
        }
    }

    dap_chain_addr_t l_wallet_addr = {};
    if (cf_datum_parse_addr(a_wallet_addr_obj, &l_wallet_addr) != 0) {
        return NULL;
    }

    uint256_t l_value = {};
    if (cf_datum_parse_uint256(a_value_obj, &l_value) != 0) {
        return NULL;
    }

    uint256_t l_fee = {};
    if (cf_datum_parse_uint256(a_fee_obj, &l_fee) != 0) {
        return NULL;
    }

    uint256_t l_reinvest = {};
    if (cf_datum_parse_uint256(a_reinvest_obj, &l_reinvest) != 0) {
        return NULL;
    }

    const char *l_delegated_ticker = NULL;
    if (a_delegated_ticker_obj && a_delegated_ticker_obj != Py_None) {
        l_delegated_ticker = cf_datum_get_cstring(a_delegated_ticker_obj, "delegated_ticker must be str or bytes");
        if (!l_delegated_ticker) {
            return NULL;
        }
    }

    uint256_t l_delegated_value = {};
    if (a_delegated_value_obj && a_delegated_value_obj != Py_None) {
        if (cf_datum_parse_uint256(a_delegated_value_obj, &l_delegated_value) != 0) {
            return NULL;
        }
    }

    dap_chain_id_t l_chain_id = {.uint64 = a_chain_id};
    dap_chain_srv_uid_t l_srv_uid = {.uint64 = a_srv_uid};
    dap_chain_datum_tx_t *l_tx = dap_stake_tx_create_lock(
        l_used_outs, &l_wallet_addr, a_main_ticker, l_value, l_fee, (dap_time_t)a_time_staking,
        l_reinvest, l_delegated_ticker, l_delegated_value, l_chain_id, l_srv_uid);
    if (!l_tx) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}

PyObject* dap_chain_net_srv_stake_tx_create_unlock_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_used_outs_obj = Py_None;
    PyObject *a_stake_hash_obj = NULL;
    unsigned int a_prev_cond_idx = 0;
    const char *a_main_ticker = NULL;
    PyObject *a_value_obj = NULL;
    PyObject *a_fee_obj = NULL;
    PyObject *a_delegated_ticker_obj = NULL;
    PyObject *a_delegated_value_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OOIsOOOO", &a_used_outs_obj, &a_stake_hash_obj, &a_prev_cond_idx,
                          &a_main_ticker, &a_value_obj, &a_fee_obj,
                          &a_delegated_ticker_obj, &a_delegated_value_obj))
        return NULL;

    dap_list_t *l_used_outs = NULL;
    if (a_used_outs_obj && a_used_outs_obj != Py_None) {
        if (!PyCapsule_CheckExact(a_used_outs_obj)) {
            PyErr_SetString(PyExc_TypeError, "First argument must be a dap_list_t capsule or None");
            return NULL;
        }
        l_used_outs = (dap_list_t *)PyCapsule_GetPointer(a_used_outs_obj, "dap_list_t");
        if (!l_used_outs) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_list_t capsule");
            return NULL;
        }
    }

    dap_hash_fast_t l_stake_hash = {};
    if (cf_datum_parse_hash_fast(a_stake_hash_obj, &l_stake_hash) != 0) {
        return NULL;
    }

    uint256_t l_value = {};
    if (cf_datum_parse_uint256(a_value_obj, &l_value) != 0) {
        return NULL;
    }

    uint256_t l_fee = {};
    if (cf_datum_parse_uint256(a_fee_obj, &l_fee) != 0) {
        return NULL;
    }

    const char *l_delegated_ticker = NULL;
    if (a_delegated_ticker_obj && a_delegated_ticker_obj != Py_None) {
        l_delegated_ticker = cf_datum_get_cstring(a_delegated_ticker_obj, "delegated_ticker must be str or bytes");
        if (!l_delegated_ticker) {
            return NULL;
        }
    }

    uint256_t l_delegated_value = {};
    if (a_delegated_value_obj && a_delegated_value_obj != Py_None) {
        if (cf_datum_parse_uint256(a_delegated_value_obj, &l_delegated_value) != 0) {
            return NULL;
        }
    }

    dap_chain_datum_tx_t *l_tx = dap_stake_tx_create_unlock(
        l_used_outs, &l_stake_hash, a_prev_cond_idx, a_main_ticker, l_value, l_fee,
        l_delegated_ticker, l_delegated_value);
    if (!l_tx) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}

PyObject* dap_chain_net_srv_stake_tx_create_delegate_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_used_outs_obj = Py_None;
    PyObject *a_value_obj = NULL;
    PyObject *a_fee_obj = NULL;
    PyObject *a_signing_addr_obj = NULL;
    PyObject *a_node_addr_obj = NULL;
    PyObject *a_sovereign_addr_obj = Py_None;
    PyObject *a_sovereign_tax_obj = NULL;
    PyObject *a_prev_tx_obj = Py_None;
    unsigned long long a_srv_uid = 0;

    if (!PyArg_ParseTuple(a_args, "OOOOOOOOK", &a_used_outs_obj, &a_value_obj, &a_fee_obj,
                          &a_signing_addr_obj, &a_node_addr_obj, &a_sovereign_addr_obj,
                          &a_sovereign_tax_obj, &a_prev_tx_obj, &a_srv_uid))
        return NULL;

    dap_list_t *l_used_outs = NULL;
    if (a_used_outs_obj && a_used_outs_obj != Py_None) {
        if (!PyCapsule_CheckExact(a_used_outs_obj)) {
            PyErr_SetString(PyExc_TypeError, "First argument must be a dap_list_t capsule or None");
            return NULL;
        }
        l_used_outs = (dap_list_t *)PyCapsule_GetPointer(a_used_outs_obj, "dap_list_t");
        if (!l_used_outs) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_list_t capsule");
            return NULL;
        }
    }

    uint256_t l_value = {};
    if (cf_datum_parse_uint256(a_value_obj, &l_value) != 0) {
        return NULL;
    }

    uint256_t l_fee = {};
    if (cf_datum_parse_uint256(a_fee_obj, &l_fee) != 0) {
        return NULL;
    }

    dap_chain_addr_t l_signing_addr = {};
    if (cf_datum_parse_addr(a_signing_addr_obj, &l_signing_addr) != 0) {
        return NULL;
    }

    dap_chain_node_addr_t l_node_addr = {};
    if (cf_datum_parse_node_addr(a_node_addr_obj, &l_node_addr) != 0) {
        return NULL;
    }

    dap_chain_addr_t l_sovereign_addr = {};
    dap_chain_addr_t *l_sovereign_ptr = NULL;
    if (a_sovereign_addr_obj && a_sovereign_addr_obj != Py_None) {
        if (cf_datum_parse_addr(a_sovereign_addr_obj, &l_sovereign_addr) != 0) {
            return NULL;
        }
        l_sovereign_ptr = &l_sovereign_addr;
    }

    uint256_t l_sovereign_tax = {};
    if (cf_datum_parse_uint256(a_sovereign_tax_obj, &l_sovereign_tax) != 0) {
        return NULL;
    }

    dap_chain_datum_tx_t *l_prev_tx = NULL;
    if (a_prev_tx_obj && a_prev_tx_obj != Py_None) {
        if (!PyCapsule_CheckExact(a_prev_tx_obj)) {
            PyErr_SetString(PyExc_TypeError, "prev_tx must be a tx capsule or None");
            return NULL;
        }
        l_prev_tx = (dap_chain_datum_tx_t *)PyCapsule_GetPointer(a_prev_tx_obj, "dap_chain_datum_tx_t");
        if (!l_prev_tx) {
            PyErr_SetString(PyExc_ValueError, "Invalid tx capsule");
            return NULL;
        }
    }

    dap_chain_srv_uid_t l_srv_uid = {.uint64 = a_srv_uid};
    dap_chain_datum_tx_t *l_tx = dap_stake_tx_create_delegate(
        l_used_outs, l_value, l_fee, &l_signing_addr, &l_node_addr, l_sovereign_ptr,
        l_sovereign_tax, l_prev_tx, l_srv_uid);
    if (!l_tx) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}

PyObject* dap_chain_net_srv_stake_tx_create_invalidate_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_used_outs_obj = Py_None;
    PyObject *a_tx_hash_obj = NULL;
    PyObject *a_fee_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OOO", &a_used_outs_obj, &a_tx_hash_obj, &a_fee_obj))
        return NULL;

    dap_list_t *l_used_outs = NULL;
    if (a_used_outs_obj && a_used_outs_obj != Py_None) {
        if (!PyCapsule_CheckExact(a_used_outs_obj)) {
            PyErr_SetString(PyExc_TypeError, "First argument must be a dap_list_t capsule or None");
            return NULL;
        }
        l_used_outs = (dap_list_t *)PyCapsule_GetPointer(a_used_outs_obj, "dap_list_t");
        if (!l_used_outs) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_list_t capsule");
            return NULL;
        }
    }

    dap_hash_fast_t l_tx_hash = {};
    if (cf_datum_parse_hash_fast(a_tx_hash_obj, &l_tx_hash) != 0) {
        return NULL;
    }

    uint256_t l_fee = {};
    if (cf_datum_parse_uint256(a_fee_obj, &l_fee) != 0) {
        return NULL;
    }

    dap_chain_datum_tx_t *l_tx = dap_stake_tx_create_invalidate(l_used_outs, &l_tx_hash, l_fee);
    if (!l_tx) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}


// =============================================================================
