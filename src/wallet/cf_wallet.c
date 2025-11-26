#include "cellframe.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Include signature types for wallet creation
#include "dap_sign.h"
#include "dap_chain_ledger.h"
#include "dap_math_ops.h"
#include "dap_chain_wallet_shared.h"
#include "dap_json.h"
#include "dap_chain_common.h"  // Includes hash_fast definitions

// Wallet type - to be implemented
PyTypeObject PyCellframeWalletType = {0};

// =========================================
// WALLET FUNCTIONS - Module level functions
// =========================================

// Wallet creation
PyObject* py_dap_chain_wallet_create(PyObject *self, PyObject *args) {
    (void)self;
    const char *wallet_name;
    const char *pass;
    const char *net_name;
    
    if (!PyArg_ParseTuple(args, "sss", &wallet_name, &pass, &net_name)) {
        return NULL;
    }
    
    // Get wallets path from global config
    const char *wallets_path = dap_chain_wallet_get_path(g_config);
    if (!wallets_path) {
        PyErr_SetString(CellframeWalletError, "Wallet path not configured");
        return NULL;
    }
    
    // Create wallet with default signature type
    dap_sign_type_t sig_type = {.type = SIG_TYPE_DILITHIUM};
    dap_chain_wallet_t *wallet = dap_chain_wallet_create_with_seed_multi(
        wallet_name, wallets_path, &sig_type, 1, NULL, 0, pass);
    
    if (!wallet) {
        PyErr_SetString(CellframeWalletError, "Failed to create wallet");
        return NULL;
    }
    
    // Convert wallet pointer to Python capsule
    PyObject *wallet_capsule = PyCapsule_New(wallet, "dap_chain_wallet_t", NULL);
    if (!wallet_capsule) {
        dap_chain_wallet_close(wallet);
        return NULL;
    }
    
    return wallet_capsule;
}

// Wallet creation with seed
PyObject* py_dap_chain_wallet_create_with_seed(PyObject *self, PyObject *args) {
    (void)self;
    const char *wallet_name;
    const char *pass;
    const char *net_name;
    const char *seed_hex;
    
    if (!PyArg_ParseTuple(args, "ssss", &wallet_name, &pass, &net_name, &seed_hex)) {
        return NULL;
    }
    
    // Get wallets path from global config
    const char *wallets_path = dap_chain_wallet_get_path(g_config);
    if (!wallets_path) {
        PyErr_SetString(CellframeWalletError, "Wallet path not configured");
        return NULL;
    }
    
    // Convert hex seed to binary
    size_t seed_size = strlen(seed_hex) / 2;
    if (seed_size == 0) {
        PyErr_SetString(CellframeWalletError, "Invalid seed hex string");
        return NULL;
    }
    
    uint8_t *seed_data = malloc(seed_size);
    if (!seed_data) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for seed");
        return NULL;
    }
    
    // Simple hex to binary conversion
    for (size_t i = 0; i < seed_size; i++) {
        char hex_byte[3] = {seed_hex[i*2], seed_hex[i*2+1], '\0'};
        seed_data[i] = (uint8_t)strtol(hex_byte, NULL, 16);
    }
    
    // Create wallet with seed and default signature type
    dap_sign_type_t sig_type = {.type = SIG_TYPE_DILITHIUM};
    dap_chain_wallet_t *wallet = dap_chain_wallet_create_with_seed_multi(
        wallet_name, wallets_path, &sig_type, 1, seed_data, seed_size, pass);
    
    free(seed_data);
    
    if (!wallet) {
        PyErr_SetString(CellframeWalletError, "Failed to create wallet with seed");
        return NULL;
    }
    
    // Convert wallet pointer to Python capsule
    PyObject *wallet_capsule = PyCapsule_New(wallet, "dap_chain_wallet_t", NULL);
    if (!wallet_capsule) {
        dap_chain_wallet_close(wallet);
        return NULL;
    }
    
    return wallet_capsule;
}

// Wallet creation with multiple signatures
PyObject* py_dap_chain_wallet_create_with_seed_multi(PyObject *self, PyObject *args) {
    (void)self;
    const char *wallet_name;
    const char *pass;
    const char *seed_hex;
    PyObject *sig_types_list;
    
    if (!PyArg_ParseTuple(args, "sssO", &wallet_name, &pass, &seed_hex, &sig_types_list)) {
        return NULL;
    }
    
    // Get wallets path from global config
    const char *wallets_path = dap_chain_wallet_get_path(g_config);
    if (!wallets_path) {
        PyErr_SetString(CellframeWalletError, "Wallet path not configured");
        return NULL;
    }
    
    // Convert Python list to signature types array
    if (!PyList_Check(sig_types_list)) {
        PyErr_SetString(PyExc_TypeError, "sig_types must be a list");
        return NULL;
    }
    
    Py_ssize_t sig_count = PyList_Size(sig_types_list);
    if (sig_count <= 0) {
        PyErr_SetString(PyExc_ValueError, "sig_types list must not be empty");
        return NULL;
    }
    
    dap_sign_type_t *sig_types = malloc(sizeof(dap_sign_type_t) * sig_count);
    if (!sig_types) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for signature types");
        return NULL;
    }
    
    // Fill signature types array
    for (Py_ssize_t i = 0; i < sig_count; i++) {
        PyObject *item = PyList_GetItem(sig_types_list, i);
        if (!PyLong_Check(item)) {
            free(sig_types);
            PyErr_SetString(PyExc_TypeError, "All signature type items must be integers");
            return NULL;
        }
        sig_types[i].type = (dap_sign_type_enum_t)PyLong_AsLong(item);
    }
    
    // Convert hex seed to binary
    size_t seed_size = strlen(seed_hex) / 2;
    if (seed_size == 0) {
        free(sig_types);
        PyErr_SetString(CellframeWalletError, "Invalid seed hex string");
        return NULL;
    }
    
    uint8_t *seed_data = malloc(seed_size);
    if (!seed_data) {
        free(sig_types);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for seed");
        return NULL;
    }
    
    // Simple hex to binary conversion
    for (size_t i = 0; i < seed_size; i++) {
        char hex_byte[3] = {seed_hex[i*2], seed_hex[i*2+1], '\0'};
        seed_data[i] = (uint8_t)strtol(hex_byte, NULL, 16);
    }
    
    // Create wallet with multiple signatures
    dap_chain_wallet_t *wallet = dap_chain_wallet_create_with_seed_multi(
        wallet_name, wallets_path, sig_types, sig_count, seed_data, seed_size, pass);
    
    free(sig_types);
    free(seed_data);
    
    if (!wallet) {
        PyErr_SetString(CellframeWalletError, "Failed to create wallet with multiple signatures");
        return NULL;
    }
    
    // Convert wallet pointer to Python capsule
    PyObject *wallet_capsule = PyCapsule_New(wallet, "dap_chain_wallet_t", NULL);
    if (!wallet_capsule) {
        dap_chain_wallet_close(wallet);
        return NULL;
    }
    
    return wallet_capsule;
}

// Open wallet
PyObject* py_dap_chain_wallet_open(PyObject *self, PyObject *args) {
    (void)self;
    const char *wallet_name;
    const char *pass;
    const char *net_name;
    
    if (!PyArg_ParseTuple(args, "sss", &wallet_name, &pass, &net_name)) {
        return NULL;
    }
    
    // Get wallets path from global config
    const char *wallets_path = dap_chain_wallet_get_path(g_config);
    if (!wallets_path) {
        PyErr_SetString(CellframeWalletError, "Wallet path not configured");
        return NULL;
    }
    
    // Open existing wallet
    unsigned int status = 0;
    dap_chain_wallet_t *wallet = dap_chain_wallet_open(wallet_name, wallets_path, &status);
    
    if (!wallet) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to open wallet '%s' (status: %u)", wallet_name, status);
        PyErr_SetString(CellframeWalletError, error_msg);
        return NULL;
    }
    
    // Convert wallet pointer to Python capsule
    PyObject *wallet_capsule = PyCapsule_New(wallet, "dap_chain_wallet_t", NULL);
    if (!wallet_capsule) {
        dap_chain_wallet_close(wallet);
        return NULL;
    }
    
    return wallet_capsule;
}

// Open wallet extended
PyObject* py_dap_chain_wallet_open_ext(PyObject *self, PyObject *args) {
    (void)self;
    const char *wallet_name;
    const char *pass;
    
    if (!PyArg_ParseTuple(args, "ss", &wallet_name, &pass)) {
        return NULL;
    }
    
    // Get wallets path from global config
    const char *wallets_path = dap_chain_wallet_get_path(g_config);
    if (!wallets_path) {
        PyErr_SetString(CellframeWalletError, "Wallet path not configured");
        return NULL;
    }
    
    // NOTE: dap_chain_wallet_open_ext is not implemented in Cellframe SDK
    // Use regular open function (password parameter is currently not used)
    unsigned int status = 0;
    dap_chain_wallet_t *wallet = dap_chain_wallet_open(wallet_name, wallets_path, &status);
    
    if (!wallet) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to open wallet '%s' (status: %u)", wallet_name, status);
        PyErr_SetString(CellframeWalletError, error_msg);
        return NULL;
    }
    
    // Convert wallet pointer to Python capsule
    PyObject *wallet_capsule = PyCapsule_New(wallet, "dap_chain_wallet_t", NULL);
    if (!wallet_capsule) {
        dap_chain_wallet_close(wallet);
        return NULL;
    }
    
    return wallet_capsule;
}

// Close wallet
PyObject* py_dap_chain_wallet_close(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_capsule;
    
    if (!PyArg_ParseTuple(args, "O", &wallet_capsule)) {
        return NULL;
    }
    
    dap_chain_wallet_t *wallet = (dap_chain_wallet_t*)PyCapsule_GetPointer(wallet_capsule, "dap_chain_wallet_t");
    if (!wallet) {
        PyErr_SetString(CellframeWalletError, "Invalid wallet object");
        return NULL;
    }
    
    dap_chain_wallet_close(wallet);
    
    Py_RETURN_NONE;
}

// Save wallet
PyObject* py_dap_chain_wallet_save(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_capsule;
    const char *pass;
    
    if (!PyArg_ParseTuple(args, "Os", &wallet_capsule, &pass)) {
        return NULL;
    }
    
    dap_chain_wallet_t *wallet = (dap_chain_wallet_t*)PyCapsule_GetPointer(wallet_capsule, "dap_chain_wallet_t");
    if (!wallet) {
        PyErr_SetString(CellframeWalletError, "Invalid wallet object");
        return NULL;
    }
    
    int result = dap_chain_wallet_save(wallet, pass);
    if (result != 0) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to save wallet (error code: %d)", result);
        PyErr_SetString(CellframeWalletError, error_msg);
        return NULL;
    }
    
    Py_RETURN_NONE;
}

// Get wallet address
PyObject* py_dap_chain_wallet_get_addr(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_capsule;
    const char *net_name;
    
    if (!PyArg_ParseTuple(args, "Os", &wallet_capsule, &net_name)) {
        return NULL;
    }
    
    dap_chain_wallet_t *wallet = (dap_chain_wallet_t*)PyCapsule_GetPointer(wallet_capsule, "dap_chain_wallet_t");
    if (!wallet) {
        PyErr_SetString(CellframeWalletError, "Invalid wallet object");
        return NULL;
    }
    
    // Find network by name
    dap_chain_net_t *l_net = dap_chain_net_by_name(net_name);
    if (!l_net) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Network '%s' not found", net_name);
        PyErr_SetString(CellframeWalletError, error_msg);
        return NULL;
    }
    
    dap_chain_addr_t *addr = dap_chain_wallet_get_addr(wallet, l_net->pub.id);
    if (!addr) {
        PyErr_SetString(CellframeWalletError, "Failed to get wallet address");
        return NULL;
    }
    
    // Convert address to string (returns static buffer, no need to free)
    const char *addr_str = dap_chain_addr_to_str(addr);
    if (!addr_str) {
        PyErr_SetString(PyExc_MemoryError, "Failed to convert address to string");
        return NULL;
    }
    
    return PyUnicode_FromString(addr_str);
}

// Get wallet key
PyObject* py_dap_chain_wallet_get_key(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_capsule;
    unsigned int idx;
    
    if (!PyArg_ParseTuple(args, "OI", &wallet_capsule, &idx)) {
        return NULL;
    }
    
    dap_chain_wallet_t *wallet = (dap_chain_wallet_t*)PyCapsule_GetPointer(wallet_capsule, "dap_chain_wallet_t");
    if (!wallet) {
        PyErr_SetString(CellframeWalletError, "Invalid wallet object");
        return NULL;
    }
    
    dap_enc_key_t *key = dap_chain_wallet_get_key(wallet, (uint32_t)idx);
    if (!key) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get key at index %u", idx);
        PyErr_SetString(CellframeWalletError, error_msg);
        return NULL;
    }
    
    return PyCapsule_New(key, "dap_enc_key_t", NULL);
}

// Get wallet public key
PyObject* py_dap_chain_wallet_get_pkey(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_capsule;
    unsigned int idx;
    
    if (!PyArg_ParseTuple(args, "OI", &wallet_capsule, &idx)) {
        return NULL;
    }
    
    dap_chain_wallet_t *wallet = (dap_chain_wallet_t*)PyCapsule_GetPointer(wallet_capsule, "dap_chain_wallet_t");
    if (!wallet) {
        PyErr_SetString(CellframeWalletError, "Invalid wallet object");
        return NULL;
    }
    
    dap_pkey_t *pkey = dap_chain_wallet_get_pkey(wallet, (uint32_t)idx);
    if (!pkey) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get public key at index %u", idx);
        PyErr_SetString(CellframeWalletError, error_msg);
        return NULL;
    }
    
    return PyCapsule_New(pkey, "dap_pkey_t", NULL);
}

// Get wallet balance for token
PyObject* py_dap_chain_wallet_get_balance(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_capsule;
    PyObject *net_id_obj;
    const char *token_ticker;
    
    if (!PyArg_ParseTuple(args, "OOs", &wallet_capsule, &net_id_obj, &token_ticker)) {
        return NULL;
    }
    
    dap_chain_wallet_t *wallet = (dap_chain_wallet_t*)PyCapsule_GetPointer(wallet_capsule, "dap_chain_wallet_t");
    if (!wallet) {
        PyErr_SetString(CellframeWalletError, "Invalid wallet object");
        return NULL;
    }
    
    // Extract net_id from capsule or integer
    dap_chain_net_id_t net_id;
    if (PyLong_Check(net_id_obj)) {
        net_id.uint64 = PyLong_AsUnsignedLongLong(net_id_obj);
    } else if (PyCapsule_CheckExact(net_id_obj)) {
        dap_chain_net_id_t *net_id_ptr = (dap_chain_net_id_t*)PyCapsule_GetPointer(net_id_obj, "dap_chain_net_id_t");
        if (!net_id_ptr) {
            PyErr_SetString(CellframeWalletError, "Invalid net_id handle");
            return NULL;
        }
        net_id = *net_id_ptr;
    } else {
        PyErr_SetString(PyExc_TypeError, "net_id must be int or capsule");
        return NULL;
    }
    
    uint256_t balance = dap_chain_wallet_get_balance(wallet, net_id, token_ticker);
    
    // Convert uint256_t to string for Python
    const char *balance_coins = NULL;
    const char *balance_datoshi = dap_uint256_to_char(balance, &balance_coins);
    
    if (!balance_datoshi) {
        PyErr_SetString(CellframeWalletError, "Failed to convert balance");
        return NULL;
    }
    
    // Return the coin representation if available, otherwise datoshi
    return PyUnicode_FromString(balance_coins ? balance_coins : balance_datoshi);
}

// Activate wallet
PyObject* py_dap_chain_wallet_activate(PyObject *self, PyObject *args) {
    (void)self;
    const char *wallet_name;
    const char *wallet_pass;
    unsigned int ttl = 0; // Time-to-live in seconds, 0 = no expiration
    
    if (!PyArg_ParseTuple(args, "ss|I", &wallet_name, &wallet_pass, &ttl)) {
        return NULL;
    }
    
    // Get wallets path from global config
    const char *wallets_path = dap_chain_wallet_get_path(g_config);
    if (!wallets_path) {
        PyErr_SetString(CellframeWalletError, "Wallet path not configured");
        return NULL;
    }
    
    int result = dap_chain_wallet_activate(wallet_name, -1, wallets_path, wallet_pass, -1, ttl);
    if (result != 0) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to activate wallet '%s' (error code: %d)", wallet_name, result);
        PyErr_SetString(CellframeWalletError, error_msg);
        return NULL;
    }
    
    Py_RETURN_NONE;
}

// Deactivate wallet
PyObject* py_dap_chain_wallet_deactivate(PyObject *self, PyObject *args) {
    (void)self;
    const char *wallet_name;
    
    if (!PyArg_ParseTuple(args, "s", &wallet_name)) {
        return NULL;
    }
    
    int result = dap_chain_wallet_deactivate(wallet_name, -1);
    if (result != 0) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to deactivate wallet '%s' (error code: %d)", wallet_name, result);
        PyErr_SetString(CellframeWalletError, error_msg);
        return NULL;
    }
    
    Py_RETURN_NONE;
}

// Get public key hash from wallet
PyObject* py_dap_chain_wallet_get_pkey_hash(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_capsule;
    
    if (!PyArg_ParseTuple(args, "O", &wallet_capsule)) {
        return NULL;
    }
    
    dap_chain_wallet_t *wallet = (dap_chain_wallet_t*)PyCapsule_GetPointer(wallet_capsule, "dap_chain_wallet_t");
    if (!wallet) {
        PyErr_SetString(CellframeWalletError, "Invalid wallet object");
        return NULL;
    }
    
    dap_hash_fast_t pkey_hash = {};
    int result = dap_chain_wallet_get_pkey_hash(wallet, &pkey_hash);
    if (result != 0) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get public key hash (error code: %d)", result);
        PyErr_SetString(CellframeWalletError, error_msg);
        return NULL;
    }
    
    // Convert dap_hash_fast_t to hex string (0x...)
    char hash_str[DAP_CHAIN_HASH_FAST_STR_SIZE];
    dap_chain_hash_fast_to_str(&pkey_hash, hash_str, sizeof(hash_str));
    
    return PyUnicode_FromString(hash_str);
}

// =========================================
// SHARED WALLET FUNCTIONS
// =========================================

// Get shared wallet transaction hashes by public key hash
PyObject* py_dap_chain_wallet_shared_get_tx_hashes_json(PyObject *self, PyObject *args) {
    (void)self;
    const char *pkey_hash_str;
    const char *net_name;
    
    if (!PyArg_ParseTuple(args, "ss", &pkey_hash_str, &net_name)) {
        return NULL;
    }
    
    // Convert pkey_hash string to dap_hash_fast_t
    dap_hash_fast_t pkey_hash = {};
    if (dap_chain_hash_fast_from_str(pkey_hash_str, &pkey_hash) != 0) {
        PyErr_SetString(CellframeWalletError, "Invalid public key hash format");
        return NULL;
    }
    
    // Get shared wallet transaction hashes
    dap_json_t *json_result = dap_chain_wallet_shared_get_tx_hashes_json(&pkey_hash, net_name);
    if (!json_result) {
        Py_RETURN_NONE;
    }
    
    // Convert dap_json_t to Python string
    char *json_str = dap_json_to_string(json_result);
    dap_json_object_free(json_result);
    
    if (!json_str) {
        Py_RETURN_NONE;
    }
    
    PyObject *result = PyUnicode_FromString(json_str);
    DAP_DELETE(json_str);
    return result;
}

// Hold transaction in shared wallet
PyObject* py_dap_chain_wallet_shared_hold_tx_add(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule;
    const char *net_name;
    
    if (!PyArg_ParseTuple(args, "Os", &tx_capsule, &net_name)) {
        return NULL;
    }
    
    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t*)PyCapsule_GetPointer(tx_capsule, "dap_chain_datum_tx_t");
    if (!tx) {
        PyErr_SetString(CellframeWalletError, "Invalid transaction object");
        return NULL;
    }
    
    int result = dap_chain_wallet_shared_hold_tx_add(tx, net_name);
    if (result != 0) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to add hold transaction (error code: %d)", result);
        PyErr_SetString(CellframeWalletError, error_msg);
        return NULL;
    }
    
    Py_RETURN_NONE;
}

// =========================================
// WALLET TYPE METHODS - For Wallet object
// =========================================

PyObject* PyCellframeWallet_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    (void)type; (void)args; (void)kwds;
    PyErr_SetString(PyExc_NotImplementedError, "Wallet type not yet implemented");
    return NULL;
}

int PyCellframeWallet_init(PyCellframeWallet *self, PyObject *args, PyObject *kwds) {
    (void)self; (void)args; (void)kwds;
    return -1;
}

void PyCellframeWallet_dealloc(PyCellframeWallet *self) {
    (void)self;
}



// =========================================
// MODULE INITIALIZATION
// =========================================
// This function registers all wallet functions with the Python module
int cellframe_wallet_init(PyObject *module) {
    // Define wallet method definitions
    static PyMethodDef wallet_methods[] = {
        // Core wallet functions
        {"dap_chain_wallet_create", py_dap_chain_wallet_create, METH_VARARGS,
         "Create new wallet"},
        {"dap_chain_wallet_open", py_dap_chain_wallet_open, METH_VARARGS,
         "Open existing wallet"},
        {"dap_chain_wallet_close", py_dap_chain_wallet_close, METH_VARARGS,
         "Close wallet"},
        {"dap_chain_wallet_save", py_dap_chain_wallet_save, METH_VARARGS,
         "Save wallet to file"},
        
        // Wallet information
        {"dap_chain_wallet_get_addr", py_dap_chain_wallet_get_addr, METH_VARARGS,
         "Get wallet address"},
        {"dap_chain_wallet_get_key", py_dap_chain_wallet_get_key, METH_VARARGS,
         "Get wallet key"},
        {"dap_chain_wallet_get_pkey", py_dap_chain_wallet_get_pkey, METH_VARARGS,
         "Get wallet public key"},
        {"dap_chain_wallet_get_balance", py_dap_chain_wallet_get_balance, METH_VARARGS,
         "Get wallet balance for token"},
        
        // Wallet state management
        {"dap_chain_wallet_activate", py_dap_chain_wallet_activate, METH_VARARGS,
         "Activate wallet"},
        {"dap_chain_wallet_deactivate", py_dap_chain_wallet_deactivate, METH_VARARGS,
         "Deactivate wallet"},
        
        // Advanced wallet functions
        {"dap_chain_wallet_create_with_seed", py_dap_chain_wallet_create_with_seed, METH_VARARGS,
         "Create wallet with seed"},
        {"dap_chain_wallet_open_ext", py_dap_chain_wallet_open_ext, METH_VARARGS,
         "Open wallet with extended parameters"},
        {"dap_chain_wallet_create_with_seed_multi", py_dap_chain_wallet_create_with_seed_multi, METH_VARARGS,
         "Create wallet with seed (multi-signature support)"},
        
        // Shared wallet functions
        {"dap_chain_wallet_shared_get_tx_hashes_json", py_dap_chain_wallet_shared_get_tx_hashes_json, METH_VARARGS,
         "Get shared wallet transaction hashes by public key hash"},
        {"dap_chain_wallet_shared_hold_tx_add", py_dap_chain_wallet_shared_hold_tx_add, METH_VARARGS,
         "Add transaction to shared wallet hold list"},
        
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    
    // Add all methods to the module
    for (PyMethodDef *method = wallet_methods; method->ml_name != NULL; method++) {
        PyObject *func = PyCFunction_New(method, NULL);
        if (!func) {
            return -1;
        }
        
        if (PyModule_AddObject(module, method->ml_name, func) < 0) {
            Py_DECREF(func);
            return -1;
        }
    }
    
    return 0;
}