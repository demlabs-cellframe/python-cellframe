/*
 * Authors:
 * Dmitriy Gerasimov <naeper@demlabs.net>
 * DeM Labs Inc.   https://demlabs.net
 * Copyright  (c) 2025
 * All rights reserved.

 Phase 7.1: Governance C Bindings - Decree Object
 
 This file implements low-level Python bindings for dap_chain_datum_decree_t.
 It provides thin wrappers over C API with proper memory management.
*/

#include "cf_governance.h"
#include "dap_chain_datum_decree.h"
#include "dap_chain_decree_callbacks.h"
#include "dap_chain_ledger_decree.h"
#include "dap_chain_common.h"
#include "dap_chain_policy.h"
#include "dap_sign.h"
#include "dap_list.h"
#include <Python.h>
#include <structmember.h>

#define LOG_TAG "cf_governance"

// ============================================================================
// Python DapDecree Type Definition
// ============================================================================

typedef struct {
    PyObject_HEAD
    dap_chain_datum_decree_t *decree;
    size_t decree_size;
    bool owned;  // True if we own the decree and should free it
} PyDapDecreeObject;

static PyTypeObject PyDapDecreeType;

// ============================================================================
// Helper Functions
// ============================================================================

/**
 * @brief Create a new Python DapDecree object wrapping a C decree
 * @param a_decree The C decree pointer (can be borrowed or owned)
 * @param a_size Size of the decree in bytes
 * @param a_owned If true, the Python object will own and free the decree on dealloc
 * @return New Python DapDecree object or NULL on error
 */
PyObject* PyDapDecree_from_decree(dap_chain_datum_decree_t *a_decree, size_t a_size, bool a_owned) {
    if (!a_decree) {
        Py_RETURN_NONE;
    }
    
    PyDapDecreeObject *self = (PyDapDecreeObject *)PyDapDecreeType.tp_alloc(&PyDapDecreeType, 0);
    if (!self) {
        if (a_owned) {
            DAP_DELETE(a_decree);
        }
        return NULL;
    }
    
    self->decree = a_decree;
    self->decree_size = a_size;
    self->owned = a_owned;
    return (PyObject *)self;
}

// ============================================================================
// Python DapDecree Object Methods
// ============================================================================

/**
 * @brief Constructor for DapDecree object
 * Creates a new empty decree or parses from bytes
 */
static PyObject *PyDapDecree_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    // Parse arguments: either (net_id, chain_id, cell_id) or (bytes_data)
    PyObject *bytes_data = NULL;
    unsigned long long net_id_raw = 0, chain_id_raw = 0, cell_id_raw = 0;
    
    static char *kwlist[] = {"data", "net_id", "chain_id", "cell_id", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|OKKK", kwlist,
                                     &bytes_data, &net_id_raw, &chain_id_raw, &cell_id_raw)) {
        return NULL;
    }
    
    dap_chain_datum_decree_t *decree = NULL;
    size_t decree_size = 0;
    
    if (bytes_data && PyBytes_Check(bytes_data)) {
        // Parse from bytes
        char *data = PyBytes_AS_STRING(bytes_data);
        decree_size = PyBytes_GET_SIZE(bytes_data);
        
        if (decree_size < sizeof(dap_chain_datum_decree_t)) {
            PyErr_SetString(PyExc_ValueError, "Decree data too small");
            return NULL;
        }
        
        // Create copy of decree data
        decree = DAP_NEW_Z_SIZE(dap_chain_datum_decree_t, decree_size);
        if (!decree) {
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate decree");
            return NULL;
        }
        
        memcpy(decree, data, decree_size);
        
    } else if (net_id_raw != 0 || chain_id_raw != 0 || cell_id_raw != 0) {
        // Create new decree
        dap_chain_net_id_t net_id = {.uint64 = net_id_raw};
        dap_chain_id_t chain_id = {.uint64 = chain_id_raw};
        dap_chain_cell_id_t cell_id = {.uint64 = cell_id_raw};
        
        decree = dap_chain_datum_decree_new(net_id, chain_id, cell_id, 0);
        if (!decree) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to create decree");
            return NULL;
        }
        
        decree_size = dap_chain_datum_decree_get_size(decree);
        
    } else {
        PyErr_SetString(PyExc_TypeError, "Either data or (net_id, chain_id, cell_id) must be provided");
        return NULL;
    }
    
    return PyDapDecree_from_decree(decree, decree_size, true);
}

/**
 * @brief Destructor for DapDecree object
 */
static void PyDapDecree_dealloc(PyDapDecreeObject *self) {
    if (self->decree && self->owned) {
        DAP_DELETE(self->decree);
    }
    Py_TYPE(self)->tp_free((PyObject *)self);
}

/**
 * @brief Get decree type
 * @return int - Decree type (COMMON=1, SERVICE=2)
 */
static PyObject *PyDapDecree_get_type(PyDapDecreeObject *self, PyObject *args) {
    return Py_BuildValue("H", self->decree->header.type);
}

/**
 * @brief Get decree subtype
 * @return int - Decree subtype
 */
static PyObject *PyDapDecree_get_subtype(PyDapDecreeObject *self, PyObject *args) {
    return Py_BuildValue("H", self->decree->header.sub_type);
}

/**
 * @brief Get creation timestamp
 * @return int - Unix timestamp
 */
static PyObject *PyDapDecree_get_timestamp(PyDapDecreeObject *self, PyObject *args) {
    return Py_BuildValue("K", (unsigned long long)self->decree->header.ts_created);
}

/**
 * @brief Get network ID
 * @return int - Network ID as uint64
 */
static PyObject *PyDapDecree_get_net_id(PyDapDecreeObject *self, PyObject *args) {
    return Py_BuildValue("K", (unsigned long long)self->decree->header.common_decree_params.net_id.uint64);
}

/**
 * @brief Get chain ID
 * @return int - Chain ID as uint64
 */
static PyObject *PyDapDecree_get_chain_id(PyDapDecreeObject *self, PyObject *args) {
    return Py_BuildValue("K", (unsigned long long)self->decree->header.common_decree_params.chain_id.uint64);
}

/**
 * @brief Get cell ID
 * @return int - Cell ID as uint64
 */
static PyObject *PyDapDecree_get_cell_id(PyDapDecreeObject *self, PyObject *args) {
    return Py_BuildValue("K", (unsigned long long)self->decree->header.common_decree_params.cell_id.uint64);
}

/**
 * @brief Get fee value from decree
 * @return int - Fee as uint256 (Python long) or None if not present
 */
static PyObject *PyDapDecree_get_fee(PyDapDecreeObject *self, PyObject *args) {
    uint256_t fee = uint256_0;
    
    if (dap_chain_datum_decree_get_fee(self->decree, &fee) != 0) {
        Py_RETURN_NONE;
    }
    
    // Convert uint256 to Python integer
    char *fee_str = dap_uint256_to_char(fee, NULL);
    if (!fee_str) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to convert fee to string");
        return NULL;
    }
    
    PyObject *result = PyLong_FromString(fee_str, NULL, 10);
    DAP_DELETE(fee_str);
    return result;
}

/**
 * @brief Get fee wallet address
 * @return str - Fee wallet address or None if not present
 */
static PyObject *PyDapDecree_get_fee_addr(PyDapDecreeObject *self, PyObject *args) {
    dap_chain_addr_t addr;
    
    if (dap_chain_datum_decree_get_fee_addr(self->decree, &addr) != 0) {
        Py_RETURN_NONE;
    }
    
    // Convert address to string
    const char *addr_str = dap_chain_addr_to_str(&addr);
    if (!addr_str) {
        Py_RETURN_NONE;
    }
    
    return Py_BuildValue("s", addr_str);
}

/**
 * @brief Get list of owner public keys
 * @return list - List of owner keys as bytes objects, or empty list
 */
static PyObject *PyDapDecree_get_owners(PyDapDecreeObject *self, PyObject *args) {
    uint16_t owners_count = 0;
    dap_list_t *owners_list = dap_chain_datum_decree_get_owners(self->decree, &owners_count);
    
    PyObject *result = PyList_New(0);
    if (!result) {
        dap_list_free(owners_list);
        return NULL;
    }
    
    if (!owners_list || owners_count == 0) {
        dap_list_free(owners_list);
        return result;  // Return empty list
    }
    
    // Convert each owner pkey to bytes
    for (dap_list_t *it = owners_list; it; it = it->next) {
        dap_pkey_t *pkey = (dap_pkey_t *)it->data;
        if (!pkey) continue;
        
        // Serialize pkey to bytes
        size_t pkey_size = dap_pkey_get_size(pkey);
        PyObject *pkey_bytes = PyBytes_FromStringAndSize((char *)pkey, pkey_size);
        
        if (pkey_bytes) {
            PyList_Append(result, pkey_bytes);
            Py_DECREF(pkey_bytes);
        }
    }
    
    dap_list_free(owners_list);
    return result;
}

/**
 * @brief Get minimum number of owners required
 * @return int - Minimum owners count or None if not present
 */
static PyObject *PyDapDecree_get_min_owners(PyDapDecreeObject *self, PyObject *args) {
    uint256_t min_owners = uint256_0;
    
    if (dap_chain_datum_decree_get_min_owners(self->decree, &min_owners) != 0) {
        Py_RETURN_NONE;
    }
    
    // Convert to Python int (assuming it fits in uint64)
    return Py_BuildValue("K", (unsigned long long)min_owners);
}

/**
 * @brief Get stake transaction hash
 * @return bytes - Transaction hash (32 bytes) or None if not present
 */
static PyObject *PyDapDecree_get_hash(PyDapDecreeObject *self, PyObject *args) {
    dap_hash_fast_t hash;
    
    if (dap_chain_datum_decree_get_hash(self->decree, &hash) != 0) {
        Py_RETURN_NONE;
    }
    
    return PyBytes_FromStringAndSize((char *)&hash, sizeof(dap_hash_fast_t));
}

/**
 * @brief Get stake value
 * @return int - Stake value as uint256 (Python long) or None if not present
 */
static PyObject *PyDapDecree_get_stake_value(PyDapDecreeObject *self, PyObject *args) {
    uint256_t stake_value = uint256_0;
    
    if (dap_chain_datum_decree_get_stake_value(self->decree, &stake_value) != 0) {
        Py_RETURN_NONE;
    }
    
    char *value_str = dap_uint256_to_char(stake_value, NULL);
    if (!value_str) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to convert stake value to string");
        return NULL;
    }
    
    PyObject *result = PyLong_FromString(value_str, NULL, 10);
    DAP_DELETE(value_str);
    return result;
}

/**
 * @brief Get signing address
 * @return str - Signing address or None if not present
 */
static PyObject *PyDapDecree_get_signing_addr(PyDapDecreeObject *self, PyObject *args) {
    dap_chain_addr_t addr;
    
    if (dap_chain_datum_decree_get_stake_signing_addr(self->decree, &addr) != 0) {
        Py_RETURN_NONE;
    }
    
    const char *addr_str = dap_chain_addr_to_str(&addr);
    if (!addr_str) {
        Py_RETURN_NONE;
    }
    
    return Py_BuildValue("s", addr_str);
}

/**
 * @brief Get list of signatures
 * @return list - List of signatures as bytes objects
 */
static PyObject *PyDapDecree_get_signs(PyDapDecreeObject *self, PyObject *args) {
    size_t signs_size = 0;
    dap_sign_t *signs = dap_chain_datum_decree_get_signs(self->decree, &signs_size);
    
    PyObject *result = PyList_New(0);
    if (!result) {
        return NULL;
    }
    
    if (!signs || signs_size == 0) {
        return result;  // Return empty list
    }
    
    // Iterate through signatures
    size_t offset = 0;
    while (offset < signs_size) {
        dap_sign_t *sign = (dap_sign_t *)((byte_t *)signs + offset);
        size_t sign_size = dap_sign_get_size(sign);
        
        if (offset + sign_size > signs_size) {
            break;  // Invalid signature
        }
        
        PyObject *sign_bytes = PyBytes_FromStringAndSize((char *)sign, sign_size);
        if (sign_bytes) {
            PyList_Append(result, sign_bytes);
            Py_DECREF(sign_bytes);
        }
        
        offset += sign_size;
    }
    
    return result;
}

/**
 * @brief Get signer node address
 * @return bytes - Node address (sizeof(dap_chain_node_addr_t)) or None if not present
 */
static PyObject *PyDapDecree_get_node_addr(PyDapDecreeObject *self, PyObject *args) {
    dap_chain_node_addr_t node_addr;
    
    if (dap_chain_datum_decree_get_node_addr(self->decree, &node_addr) != 0) {
        Py_RETURN_NONE;
    }
    
    return PyBytes_FromStringAndSize((char *)&node_addr, sizeof(dap_chain_node_addr_t));
}

/**
 * @brief Get minimum stake value
 * @return int - Minimum stake value as uint256 (Python long) or None if not present
 */
static PyObject *PyDapDecree_get_stake_min_value(PyDapDecreeObject *self, PyObject *args) {
    uint256_t min_value = uint256_0;
    
    if (dap_chain_datum_decree_get_stake_min_value(self->decree, &min_value) != 0) {
        Py_RETURN_NONE;
    }
    
    char *value_str = dap_uint256_to_char(min_value, NULL);
    if (!value_str) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to convert min stake value to string");
        return NULL;
    }
    
    PyObject *result = PyLong_FromString(value_str, NULL, 10);
    DAP_DELETE(value_str);
    return result;
}

/**
 * @brief Get minimum signers count
 * @return int - Minimum signers count as uint256 (Python long) or None if not present
 */
static PyObject *PyDapDecree_get_stake_min_signers_count(PyDapDecreeObject *self, PyObject *args) {
    uint256_t min_signers = uint256_0;
    
    if (dap_chain_datum_decree_get_stake_min_signers_count(self->decree, &min_signers) != 0) {
        Py_RETURN_NONE;
    }
    
    char *value_str = dap_uint256_to_char(min_signers, NULL);
    if (!value_str) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to convert min signers count to string");
        return NULL;
    }
    
    PyObject *result = PyLong_FromString(value_str, NULL, 10);
    DAP_DELETE(value_str);
    return result;
}

/**
 * @brief Get action code
 * @return int - Action code (uint8) or None if not present
 */
static PyObject *PyDapDecree_get_action(PyDapDecreeObject *self, PyObject *args) {
    uint8_t action = 0;
    
    if (dap_chain_datum_decree_get_action(self->decree, &action) != 0) {
        Py_RETURN_NONE;
    }
    
    return Py_BuildValue("B", action);
}

/**
 * @brief Get signature type
 * @return int - Signature type (uint32) or None if not present
 */
static PyObject *PyDapDecree_get_signature_type(PyDapDecreeObject *self, PyObject *args) {
    uint32_t sig_type = 0;
    
    if (dap_chain_datum_decree_get_signature_type(self->decree, &sig_type) != 0) {
        Py_RETURN_NONE;
    }
    
    return Py_BuildValue("I", sig_type);
}

/**
 * @brief Get banned address
 * @return str - Banned address (host or string) or None if not present
 */
static PyObject *PyDapDecree_get_ban_addr(PyDapDecreeObject *self, PyObject *args) {
    const char *ban_addr = NULL;
    
    if (dap_chain_datum_decree_get_ban_addr(self->decree, &ban_addr) != 0 || !ban_addr) {
        Py_RETURN_NONE;
    }
    
    return Py_BuildValue("s", ban_addr);
}

/**
 * @brief Get public key from decree TSD
 * @return bytes - Public key as bytes or None if not present
 */
static PyObject *PyDapDecree_get_pkey(PyDapDecreeObject *self, PyObject *args) {
    dap_pkey_t *pkey = dap_chain_datum_decree_get_pkey(self->decree);
    
    if (!pkey) {
        Py_RETURN_NONE;
    }
    
    size_t pkey_size = dap_pkey_get_size(pkey);
    return PyBytes_FromStringAndSize((char *)pkey, pkey_size);
}

/**
 * @brief Get policy from decree TSD
 * @return bytes - Policy object as bytes or None if not present
 */
static PyObject *PyDapDecree_get_policy(PyDapDecreeObject *self, PyObject *args) {
    dap_chain_policy_t *policy = dap_chain_datum_decree_get_policy(self->decree);
    
    if (!policy) {
        Py_RETURN_NONE;
    }
    
    // Serialize policy to bytes
    size_t policy_size = dap_chain_policy_get_size(policy);
    return PyBytes_FromStringAndSize((char *)policy, policy_size);
}

/**
 * @brief Get atom/block number
 * @return int - Atom number (uint64) or None if not present
 */
static PyObject *PyDapDecree_get_atom_num(PyDapDecreeObject *self, PyObject *args) {
    uint64_t atom_num = 0;
    
    if (dap_chain_datum_decree_get_atom_num(self->decree, &atom_num) != 0) {
        Py_RETURN_NONE;
    }
    
    return Py_BuildValue("K", (unsigned long long)atom_num);
}

/**
 * @brief Get empty block generation period
 * @return int - Block generation period (uint16) or None if not present
 */
static PyObject *PyDapDecree_get_blockgen_period(PyDapDecreeObject *self, PyObject *args) {
    uint16_t period = 0;
    
    if (dap_chain_datum_decree_get_empty_block_every_times(self->decree, &period) != 0) {
        Py_RETURN_NONE;
    }
    
    return Py_BuildValue("H", period);
}

/**
 * @brief Dump decree to human-readable string
 * @return str - Formatted decree information
 */
static PyObject *PyDapDecree_dump(PyDapDecreeObject *self, PyObject *args) {
    const char *hash_out_type = "hex";  // Default to hex
    if (!PyArg_ParseTuple(args, "|s", &hash_out_type)) {
        return NULL;
    }
    
    dap_string_t *str_out = dap_string_new(NULL);
    if (!str_out) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate string");
        return NULL;
    }
    
    dap_chain_datum_decree_dump(str_out, self->decree, self->decree_size, hash_out_type);
    
    PyObject *result = Py_BuildValue("s", str_out->str);
    dap_string_free(str_out, true);
    return result;
}

/**
 * @brief Convert decree to JSON
 * @return str - JSON representation of the decree
 */
static PyObject *PyDapDecree_to_json(PyDapDecreeObject *self, PyObject *args) {
    const char *hash_out_type = "hex";
    if (!PyArg_ParseTuple(args, "|s", &hash_out_type)) {
        return NULL;
    }
    
    dap_json_t *json_obj = dap_json_object_new();
    if (!json_obj) {
        PyErr_SetString(PyExc_MemoryError, "Failed to create JSON object");
        return NULL;
    }
    
    dap_chain_datum_decree_dump_json(json_obj, self->decree, self->decree_size, hash_out_type);
    
    char *json_str = dap_json_to_string_pretty(json_obj);
    dap_json_object_free(json_obj);
    
    if (!json_str) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to convert JSON to string");
        return NULL;
    }
    
    PyObject *result = Py_BuildValue("s", json_str);
    DAP_DELETE(json_str);
    return result;
}

/**
 * @brief Get decree as raw bytes
 * @return bytes - Raw decree data
 */
static PyObject *PyDapDecree_to_bytes(PyDapDecreeObject *self, PyObject *args) {
    return PyBytes_FromStringAndSize((char *)self->decree, self->decree_size);
}

// ============================================================================
// Python DapDecree Method Definitions
// ============================================================================

static PyMethodDef PyDapDecree_methods[] = {
    {"get_type", (PyCFunction)PyDapDecree_get_type, METH_NOARGS, "Get decree type"},
    {"get_subtype", (PyCFunction)PyDapDecree_get_subtype, METH_NOARGS, "Get decree subtype"},
    {"get_timestamp", (PyCFunction)PyDapDecree_get_timestamp, METH_NOARGS, "Get creation timestamp"},
    {"get_net_id", (PyCFunction)PyDapDecree_get_net_id, METH_NOARGS, "Get network ID"},
    {"get_chain_id", (PyCFunction)PyDapDecree_get_chain_id, METH_NOARGS, "Get chain ID"},
    {"get_cell_id", (PyCFunction)PyDapDecree_get_cell_id, METH_NOARGS, "Get cell ID"},
    {"get_fee", (PyCFunction)PyDapDecree_get_fee, METH_NOARGS, "Get fee value"},
    {"get_fee_addr", (PyCFunction)PyDapDecree_get_fee_addr, METH_NOARGS, "Get fee wallet address"},
    {"get_owners", (PyCFunction)PyDapDecree_get_owners, METH_NOARGS, "Get list of owner public keys"},
    {"get_min_owners", (PyCFunction)PyDapDecree_get_min_owners, METH_NOARGS, "Get minimum owners count"},
    {"get_hash", (PyCFunction)PyDapDecree_get_hash, METH_NOARGS, "Get stake transaction hash"},
    {"get_stake_value", (PyCFunction)PyDapDecree_get_stake_value, METH_NOARGS, "Get stake value"},
    {"get_signing_addr", (PyCFunction)PyDapDecree_get_signing_addr, METH_NOARGS, "Get signing address"},
    {"get_signs", (PyCFunction)PyDapDecree_get_signs, METH_NOARGS, "Get list of signatures"},
    {"get_node_addr", (PyCFunction)PyDapDecree_get_node_addr, METH_NOARGS, "Get signer node address"},
    {"get_stake_min_value", (PyCFunction)PyDapDecree_get_stake_min_value, METH_NOARGS, "Get minimum stake value"},
    {"get_stake_min_signers_count", (PyCFunction)PyDapDecree_get_stake_min_signers_count, METH_NOARGS, "Get minimum signers count"},
    {"get_action", (PyCFunction)PyDapDecree_get_action, METH_NOARGS, "Get action code"},
    {"get_signature_type", (PyCFunction)PyDapDecree_get_signature_type, METH_NOARGS, "Get signature type"},
    {"get_ban_addr", (PyCFunction)PyDapDecree_get_ban_addr, METH_NOARGS, "Get banned address"},
    {"get_pkey", (PyCFunction)PyDapDecree_get_pkey, METH_NOARGS, "Get public key from TSD"},
    {"get_policy", (PyCFunction)PyDapDecree_get_policy, METH_NOARGS, "Get policy from TSD"},
    {"get_atom_num", (PyCFunction)PyDapDecree_get_atom_num, METH_NOARGS, "Get atom/block number"},
    {"get_blockgen_period", (PyCFunction)PyDapDecree_get_blockgen_period, METH_NOARGS, "Get empty block generation period"},
    {"dump", (PyCFunction)PyDapDecree_dump, METH_VARARGS, "Dump decree to string"},
    {"to_json", (PyCFunction)PyDapDecree_to_json, METH_VARARGS, "Convert decree to JSON"},
    {"to_bytes", (PyCFunction)PyDapDecree_to_bytes, METH_NOARGS, "Get decree as raw bytes"},
    {NULL, NULL, 0, NULL}
};

// ============================================================================
// Python DapDecree Type Definition
// ============================================================================

static PyTypeObject PyDapDecreeType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "CellFrame.Decree",
    .tp_doc = "Governance Decree object",
    .tp_basicsize = sizeof(PyDapDecreeObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = PyDapDecree_new,
    .tp_dealloc = (destructor)PyDapDecree_dealloc,
    .tp_methods = PyDapDecree_methods,
};

// ============================================================================
// Module Initialization
// ============================================================================

int cellframe_governance_init(PyObject *module) {
    // Initialize Decree type
    if (PyType_Ready(&PyDapDecreeType) < 0) {
        return -1;
    }
    
    Py_INCREF(&PyDapDecreeType);
    if (PyModule_AddObject(module, "Decree", (PyObject *)&PyDapDecreeType) < 0) {
        Py_DECREF(&PyDapDecreeType);
        return -1;
    }
    
    // Initialize Governance Manager
    if (cellframe_governance_manager_init(module) < 0) {
        return -1;
    }
    
    // Add decree type constants
    PyModule_AddIntConstant(module, "DECREE_TYPE_COMMON", DAP_CHAIN_DATUM_DECREE_TYPE_COMMON);
    PyModule_AddIntConstant(module, "DECREE_TYPE_SERVICE", DAP_CHAIN_DATUM_DECREE_TYPE_SERVICE);
    
    // Add decree subtype constants (common)
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_FEE", DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_FEE);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_OWNERS", DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_OWNERS);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_OWNERS_MIN", DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_OWNERS_MIN);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_STAKE_APPROVE", DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_APPROVE);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_STAKE_INVALIDATE", DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_INVALIDATE);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_STAKE_MIN_VALUE", DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_MIN_VALUE);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_STAKE_MIN_VALIDATORS_COUNT", DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_MIN_VALIDATORS_COUNT);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_BAN", DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_BAN);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_UNBAN", DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_UNBAN);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_REWARD", DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_REWARD);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_MAX_WEIGHT", DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_MAX_WEIGHT);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_EMERGENCY_VALIDATORS", DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_EMERGENCY_VALIDATORS);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_POLICY", DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_POLICY);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_EVENT_PKEY_ADD", DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_EVENT_PKEY_ADD);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_EVENT_PKEY_REMOVE", DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_EVENT_PKEY_REMOVE);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_HARDFORK", DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_HARDFORK);
    
    log_it(L_INFO, "Governance module initialized (Phase 7.1: Decree object)");
    return 0;
}
