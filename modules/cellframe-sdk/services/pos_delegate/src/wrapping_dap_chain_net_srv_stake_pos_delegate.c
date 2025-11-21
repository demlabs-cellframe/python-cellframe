#include "wrapping_dap_chain_net_srv_stake_pos_delegate.h"
#include "libdap_chain_net_python.h"
#include "wrapping_dap_hash.h"
#include "wrapping_dap_stream_ch_chain_validator_test.h"
#include "dap_chain_net_srv_stake_pos_delegate.h"
#include "math_python.h"
#include "dap_chain_common.h"
#include "dap_net_common.h"

int DapChainNetSrvStakePosDelegateObject_init(PyObject *self, PyObject *args, PyObject *kwds){
    const char *kwlist[] = {
            "net",
            NULL
    };
    PyObject* obj_net;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", (char**)kwlist, &obj_net))
        return -1;
    ((PyDapChainNetSrvStakePosDelegateObject*)self)->net = ((PyDapChainNetObject*)obj_net)->chain_net;
    return 0;
}

PyObject *wrapping_dap_chain_net_srv_stake_check_validator(PyObject *self, PyObject *argv){
    (void)self;
    PyObject *obj_tx_hash;
    uint16_t time_connect;
    uint16_t time_response;
    if (!PyArg_ParseTuple(argv, "OHH", &obj_tx_hash, &time_connect, &time_response)) {
        return NULL;
    }
    if (!PyDapHashFast_Check((PyDapHashFastObject*)obj_tx_hash)) {
        PyErr_SetString(PyExc_AttributeError, "");
        return NULL;
    }
    dap_chain_ch_validator_test_t l_out = {0};
    bool res = dap_chain_net_srv_stake_check_validator(
            ((PyDapChainNetSrvStakePosDelegateObject *)self)->net,
            ((PyDapHashFastObject*)obj_tx_hash)->hash_fast,
            &l_out, time_connect, time_response);
    if (res)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject *wrapping_dap_chain_net_srv_stake_check_validator_full_info(PyObject *self, PyObject *argv){
    (void)self;
    PyObject *obj_tx_hash;
    uint16_t time_connect;
    uint16_t time_response;
    if (!PyArg_ParseTuple(argv, "OHH", &obj_tx_hash, &time_connect, &time_response)) {
        return NULL;
    }
    if (!PyDapHashFast_Check((PyDapHashFastObject*)obj_tx_hash)) {
        PyErr_SetString(PyExc_AttributeError, "");
        return NULL;
    }
    dap_chain_ch_validator_test_t l_out = {0};
    dap_chain_net_srv_stake_check_validator(
            ((PyDapChainNetSrvStakePosDelegateObject *)self)->net,
            ((PyDapHashFastObject*)obj_tx_hash)->hash_fast,
            &l_out, time_connect, time_response);
    PyDapStreamChChainValidatorTestObject *obj_rnd = PyObject_New(PyDapStreamChChainValidatorTestObject, &PyDapStreamChChainValidatorTestObjectType);
    obj_rnd->rnd = DAP_NEW_Z_SIZE(dap_chain_ch_validator_test_t, sizeof(dap_chain_ch_validator_test_t) + l_out.header.sign_size);
    memcpy(obj_rnd->rnd, &l_out, sizeof(dap_chain_ch_validator_test_t) + l_out.header.sign_size);
    return (PyObject*)obj_rnd;
}

PyObject *wrapping_dap_chain_net_srv_stake_get_count_validator(PyObject *self, void *closure) {
    (void)closure;
    size_t l_in_active_validators = 0;
    size_t l_total_validator = dap_chain_net_srv_stake_get_total_keys(
            ((PyDapChainNetSrvStakePosDelegateObject *)self)->net->pub.id, &l_in_active_validators);
    return Py_BuildValue("nn", l_total_validator, l_in_active_validators);
}

PyObject *wrapping_dap_chain_net_srv_stake_get_total_weight(PyObject *self, void *closure) {
    (void)closure;
    uint256_t total_weight = dap_chain_net_srv_stake_get_total_weight(
            ((PyDapChainNetSrvStakePosDelegateObject *)self)->net->pub.id, NULL);
    DapMathObject *obj_weight = PyObject_New(DapMathObject, &DapMathObjectType);
    obj_weight->value = total_weight;
    return (PyObject*)obj_weight;
}

PyObject *wrapping_dap_chain_net_srv_stake_get_validators_list(PyObject *self, PyObject *argv) {
    (void)self;
    int active_only = 0;
    
    if (!PyArg_ParseTuple(argv, "|i", &active_only)) {
        return NULL;
    }
    
    dap_chain_net_id_t net_id = ((PyDapChainNetSrvStakePosDelegateObject *)self)->net->pub.id;
    
    // Get total weight for calculating relative weight
    uint256_t total_weight = dap_chain_net_srv_stake_get_total_weight(net_id, NULL);
    
    // Get validators list
    dap_list_t *validators_list = dap_chain_net_srv_stake_get_validators(net_id, active_only ? true : false, NULL);
    if (!validators_list) {
        return PyList_New(0); // Return empty list
    }
    
    // Create Python list
    PyObject *result = PyList_New(0);
    if (!result) {
        dap_list_free_full(validators_list, DAP_DELETE);
        return NULL;
    }
    
    // Convert each validator to Python dictionary
    for (dap_list_t *item = validators_list; item; item = dap_list_next(item)) {
        dap_chain_net_srv_stake_item_t *stake = (dap_chain_net_srv_stake_item_t *)item->data;
        if (!stake) {
            continue;
        }
        
        PyObject *validator_dict = PyDict_New();
        if (!validator_dict) {
            Py_DECREF(result);
            dap_list_free_full(validators_list, DAP_DELETE);
            return NULL;
        }
        
        // pkey_hash
        char pkey_hash_str[DAP_CHAIN_HASH_FAST_STR_SIZE + 1];
        dap_chain_hash_fast_to_str(&stake->signing_addr.data.hash_fast, 
                                    &pkey_hash_str[stake->pkey ? 0 : 1], 
                                    sizeof(pkey_hash_str));
        if (!stake->pkey) {
            pkey_hash_str[0] = '.';
        }
        PyObject *py_pkey_hash = PyUnicode_FromString(pkey_hash_str);
        if (py_pkey_hash) {
            PyDict_SetItemString(validator_dict, "pkey_hash", py_pkey_hash);
            Py_DECREF(py_pkey_hash);
        }
        
        // stake_value (locked_value)
        char *stake_value_str = dap_chain_balance_coins_print(stake->locked_value);
        if (stake_value_str) {
            PyObject *py_stake_value = PyUnicode_FromString(stake_value_str);
            if (py_stake_value) {
                PyDict_SetItemString(validator_dict, "stake_value", py_stake_value);
                Py_DECREF(py_stake_value);
            }
            DAP_DELETE(stake_value_str);
        }
        
        // effective_value (value)
        char *effective_value_str = dap_chain_balance_coins_print(stake->value);
        if (effective_value_str) {
            PyObject *py_effective_value = PyUnicode_FromString(effective_value_str);
            if (py_effective_value) {
                PyDict_SetItemString(validator_dict, "effective_value", py_effective_value);
                Py_DECREF(py_effective_value);
            }
            DAP_DELETE(effective_value_str);
        }
        
        // related_weight (percentage)
        uint256_t rel_weight, tmp;
        MULT_256_256(stake->value, GET_256_FROM_64(100), &tmp);
        DIV_256_COIN(tmp, total_weight, &rel_weight);
        char *rel_weight_str = dap_chain_balance_coins_print(rel_weight);
        if (rel_weight_str) {
            PyObject *py_rel_weight = PyUnicode_FromString(rel_weight_str);
            if (py_rel_weight) {
                PyDict_SetItemString(validator_dict, "related_weight", py_rel_weight);
                Py_DECREF(py_rel_weight);
            }
            DAP_DELETE(rel_weight_str);
        }
        
        // tx_hash
        char tx_hash_str[DAP_CHAIN_HASH_FAST_STR_SIZE];
        dap_chain_hash_fast_to_str(&stake->tx_hash.hash, tx_hash_str, sizeof(tx_hash_str));
        PyObject *py_tx_hash = PyUnicode_FromString(tx_hash_str);
        if (py_tx_hash) {
            PyDict_SetItemString(validator_dict, "tx_hash", py_tx_hash);
            Py_DECREF(py_tx_hash);
        }
        
        // node_addr
        char node_addr[32];
        snprintf(node_addr, 32, ""NODE_ADDR_FP_STR"", NODE_ADDR_FP_ARGS_S(stake->node_addr));
        PyObject *py_node_addr = PyUnicode_FromString(node_addr);
        if (py_node_addr) {
            PyDict_SetItemString(validator_dict, "node_addr", py_node_addr);
            Py_DECREF(py_node_addr);
        }
        
        // sovereign_addr
        const char *sov_addr_str = dap_chain_addr_is_blank(&stake->sovereign_addr) ?
                    "null" : dap_chain_addr_to_str_static(&stake->sovereign_addr);
        PyObject *py_sov_addr = PyUnicode_FromString(sov_addr_str);
        if (py_sov_addr) {
            PyDict_SetItemString(validator_dict, "sovereign_addr", py_sov_addr);
            Py_DECREF(py_sov_addr);
        }
        
        // sovereign_tax (as percentage)
        uint256_t sov_tax_percent = uint256_0;
        MULT_256_256(stake->sovereign_tax, GET_256_FROM_64(100), &sov_tax_percent);
        char *sov_tax_str = dap_chain_balance_coins_print(sov_tax_percent);
        if (sov_tax_str) {
            PyObject *py_sov_tax = PyUnicode_FromString(sov_tax_str);
            if (py_sov_tax) {
                PyDict_SetItemString(validator_dict, "sovereign_tax", py_sov_tax);
                Py_DECREF(py_sov_tax);
            }
            DAP_DELETE(sov_tax_str);
        }
        
        // active status
        PyObject *py_active = stake->is_active ? Py_True : Py_False;
        Py_INCREF(py_active);
        PyDict_SetItemString(validator_dict, "active", py_active);
        Py_DECREF(py_active);
        
        // decree_hash
        char decree_hash_str[DAP_CHAIN_HASH_FAST_STR_SIZE];
        dap_chain_hash_fast_to_str(&stake->decree_hash.hash, decree_hash_str, sizeof(decree_hash_str));
        PyObject *py_decree_hash = PyUnicode_FromString(decree_hash_str);
        if (py_decree_hash) {
            PyDict_SetItemString(validator_dict, "decree_hash", py_decree_hash);
            Py_DECREF(py_decree_hash);
        }
        
        // Add validator dictionary to result list
        if (PyList_Append(result, validator_dict) < 0) {
            Py_DECREF(validator_dict);
            Py_DECREF(result);
            dap_list_free_full(validators_list, DAP_DELETE);
            return NULL;
        }
        Py_DECREF(validator_dict);
    }
    
    // Free validators list
    dap_list_free_full(validators_list, DAP_DELETE);
    
    return result;
}

static PyGetSetDef PyDapChainNetSrvStakePosDelegateGetsSets[] = {
        {"CountValidators", (getter)wrapping_dap_chain_net_srv_stake_get_count_validator, NULL, "", NULL},
        {"TotalWeight", (getter) wrapping_dap_chain_net_srv_stake_get_total_weight, NULL, "", NULL},
        {}
};

static PyMethodDef PyDapChainNetSrvStakePosDelegateMethods[] = {
        {
            "checkValidator",
            wrapping_dap_chain_net_srv_stake_check_validator,
            METH_VARARGS,
            ""
        },
        {
            "checkValidatorFullInfo",
            wrapping_dap_chain_net_srv_stake_check_validator_full_info,
            METH_VARARGS,
            ""
        },
        {
            "getValidatorsList",
            wrapping_dap_chain_net_srv_stake_get_validators_list,
            METH_VARARGS,
            "Get list of all validators with full information"
        },
        {}
};

PyTypeObject PyDapChainNetSrvStakePosDelegateObjectType = DAP_PY_TYPE_OBJECT(
            "CellFrame.Services.StakePosDelegate",
            sizeof(PyDapChainNetSrvStakePosDelegateObject),
            "CellFrame.Services.StakePosDelegate object",
            .tp_getset = PyDapChainNetSrvStakePosDelegateGetsSets,
            .tp_methods = PyDapChainNetSrvStakePosDelegateMethods,
            .tp_init = DapChainNetSrvStakePosDelegateObject_init
        );
