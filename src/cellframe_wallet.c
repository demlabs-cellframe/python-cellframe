#include "python_cellframe.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef CELLFRAME_SDK_EMBEDDED
// Include signature types for wallet creation
#include "dap_sign.h"
#include "dap_chain_ledger.h"
#include "dap_math_ops.h"
#endif

// Wallet type stub - будет реализован позже
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
    
#ifdef CELLFRAME_SDK_EMBEDDED
    // Get wallets path from global config
    const char *wallets_path = dap_chain_wallet_get_path(g_config);
    if (!wallets_path) {
        PyErr_SetString(CellframeWalletError, "Wallet path not configured");
        return NULL;
    }
    
    // Create wallet with default signature type
    dap_sign_type_t sig_type = {.type = SIG_TYPE_DILITHIUM};
    dap_chain_wallet_t *wallet = dap_chain_wallet_create_with_seed(
        wallet_name, wallets_path, sig_type, NULL, 0, pass);
    
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
#else
    PyErr_SetString(CellframeWalletError, "Cellframe SDK not embedded");
    return NULL;
#endif
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
    
#ifdef CELLFRAME_SDK_EMBEDDED
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
    dap_chain_wallet_t *wallet = dap_chain_wallet_create_with_seed(
        wallet_name, wallets_path, sig_type, seed_data, seed_size, pass);
    
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
#else
    PyErr_SetString(CellframeWalletError, "Cellframe SDK not embedded");
    return NULL;
#endif
}

// Wallet creation with multiple signatures
PyObject* py_dap_chain_wallet_create_with_seed_multi(PyObject *self, PyObject *args) {
    (void)self;
    const char *wallet_name;
    const char *pass;
    const char *net_name;
    const char *seed_hex;
    int sig_type;
    int count;
    
    if (!PyArg_ParseTuple(args, "ssssii", &wallet_name, &pass, &net_name, &seed_hex, &sig_type, &count)) {
        return NULL;
    }
    
#ifdef CELLFRAME_SDK_EMBEDDED
    // TODO: Call actual dap_chain_wallet_create_with_seed_multi when SDK is linked
    PyErr_SetString(PyExc_NotImplementedError, "Wallet create with seed multi not yet implemented in SDK");
    return NULL;
#else
    PyErr_SetString(CellframeWalletError, "Cellframe SDK not embedded");
    return NULL;
#endif
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
    
#ifdef CELLFRAME_SDK_EMBEDDED
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
#else
    PyErr_SetString(CellframeWalletError, "Cellframe SDK not embedded");
    return NULL;
#endif
}

// Open wallet extended
PyObject* py_dap_chain_wallet_open_ext(PyObject *self, PyObject *args) {
    (void)self;
    const char *wallet_name;
    const char *pass;
    const char *net_name;
    int sig_type;
    
    if (!PyArg_ParseTuple(args, "sssi", &wallet_name, &pass, &net_name, &sig_type)) {
        return NULL;
    }
    
#ifdef CELLFRAME_SDK_EMBEDDED
    // TODO: Call actual dap_chain_wallet_open_ext when SDK is linked
    PyErr_SetString(PyExc_NotImplementedError, "Wallet open ext not yet implemented in SDK");
    return NULL;
#else
    PyErr_SetString(CellframeWalletError, "Cellframe SDK not embedded");
    return NULL;
#endif
}

// Close wallet
PyObject* py_dap_chain_wallet_close(PyObject *self, PyObject *args) {
    (void)self;
    const char *wallet_name;
    
    if (!PyArg_ParseTuple(args, "s", &wallet_name)) {
        return NULL;
    }
    
#ifdef CELLFRAME_SDK_EMBEDDED
    // TODO: Call actual dap_chain_wallet_close when SDK is linked
    PyErr_SetString(PyExc_NotImplementedError, "Wallet close not yet implemented in SDK");
    return NULL;
#else
    PyErr_SetString(CellframeWalletError, "Cellframe SDK not embedded");
    return NULL;
#endif
}

// Save wallet
PyObject* py_dap_chain_wallet_save(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_obj;
    const char *pass;
    
    if (!PyArg_ParseTuple(args, "Os", &wallet_obj, &pass)) {
        return NULL;
    }
    
#ifdef CELLFRAME_SDK_EMBEDDED
    // TODO: Call actual dap_chain_wallet_save when SDK is linked
    PyErr_SetString(PyExc_NotImplementedError, "Wallet save not yet implemented in SDK");
    return NULL;
#else
    PyErr_SetString(CellframeWalletError, "Cellframe SDK not embedded");
    return NULL;
#endif
}

// Get wallet address
PyObject* py_dap_chain_wallet_get_addr(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_obj;
    const char *net_name;
    
    if (!PyArg_ParseTuple(args, "Os", &wallet_obj, &net_name)) {
        return NULL;
    }
    
#ifdef CELLFRAME_SDK_EMBEDDED
    // TODO: Call actual dap_chain_wallet_get_addr when SDK is linked
    PyErr_SetString(PyExc_NotImplementedError, "Wallet get addr not yet implemented in SDK");
    return NULL;
#else
    PyErr_SetString(CellframeWalletError, "Cellframe SDK not embedded");
    return NULL;
#endif
}

// Get wallet balance
PyObject* py_dap_chain_wallet_get_balance(PyObject *self, PyObject *args) {
    (void)self;
    const char *wallet_addr_str;
    const char *net_name;
    const char *token_ticker;
    
    if (!PyArg_ParseTuple(args, "sss", &wallet_addr_str, &net_name, &token_ticker)) {
        return NULL;
    }
    
#ifdef CELLFRAME_SDK_EMBEDDED
    // Find network by name
    dap_chain_net_t *l_net = dap_chain_net_by_name(net_name);
    if (!l_net) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Network '%s' not found", net_name);
        PyErr_SetString(CellframeWalletError, error_msg);
        return NULL;
    }
    
    // Parse wallet address
    dap_chain_addr_t *l_addr = dap_chain_addr_from_str(wallet_addr_str);
    if (!l_addr) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Invalid wallet address '%s'", wallet_addr_str);
        PyErr_SetString(CellframeWalletError, error_msg);
        return NULL;
    }
    
    // Calculate balance
    uint256_t balance = dap_ledger_calc_balance(l_net->pub.ledger, l_addr, token_ticker);
    
    // Clean up
    DAP_DELETE(l_addr);
    
    // Convert uint256 to string for Python
    const char *balance_coins = NULL;
    const char *balance_datoshi = dap_uint256_to_char(balance, &balance_coins);
    
    if (!balance_datoshi) {
        PyErr_SetString(PyExc_MemoryError, "Failed to convert balance to string");
        return NULL;
    }
    
    // Create result dictionary with coins and datoshi
    PyObject *result = PyDict_New();
    if (!result) {
        return NULL;
    }
    
    PyObject *coins_str = PyUnicode_FromString(balance_coins ? balance_coins : "0");
    PyObject *datoshi_str = PyUnicode_FromString(balance_datoshi);
    
    if (!coins_str || !datoshi_str) {
        Py_XDECREF(coins_str);
        Py_XDECREF(datoshi_str);
        Py_DECREF(result);
        return NULL;
    }
    
    PyDict_SetItemString(result, "coins", coins_str);
    PyDict_SetItemString(result, "datoshi", datoshi_str);
    
    Py_DECREF(coins_str);
    Py_DECREF(datoshi_str);
    
    return result;
#else
    PyErr_SetString(CellframeWalletError, "Cellframe SDK not embedded");
    return NULL;
#endif
}

// Get wallet key
PyObject* py_dap_chain_wallet_get_key(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_obj;
    int idx;
    
    if (!PyArg_ParseTuple(args, "Oi", &wallet_obj, &idx)) {
        return NULL;
    }
    
#ifdef CELLFRAME_SDK_EMBEDDED
    // TODO: Call actual dap_chain_wallet_get_key when SDK is linked
    PyErr_SetString(PyExc_NotImplementedError, "Wallet get key not yet implemented in SDK");
    return NULL;
#else
    PyErr_SetString(CellframeWalletError, "Cellframe SDK not embedded");
    return NULL;
#endif
}

// Get wallet public key
PyObject* py_dap_chain_wallet_get_pkey(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_obj;
    int idx;
    
    if (!PyArg_ParseTuple(args, "Oi", &wallet_obj, &idx)) {
        return NULL;
    }
    
#ifdef CELLFRAME_SDK_EMBEDDED
    // TODO: Call actual dap_chain_wallet_get_pkey when SDK is linked
    PyErr_SetString(PyExc_NotImplementedError, "Wallet get pkey not yet implemented in SDK");
    return NULL;
#else
    PyErr_SetString(CellframeWalletError, "Cellframe SDK not embedded");
    return NULL;
#endif
}

// Activate wallet
PyObject* py_dap_chain_wallet_activate(PyObject *self, PyObject *args) {
    (void)self;
    const char *wallet_name;
    const char *net_name;
    
    if (!PyArg_ParseTuple(args, "ss", &wallet_name, &net_name)) {
        return NULL;
    }
    
#ifdef CELLFRAME_SDK_EMBEDDED
    // TODO: Call actual dap_chain_wallet_activate when SDK is linked
    PyErr_SetString(PyExc_NotImplementedError, "Wallet activate not yet implemented in SDK");
    return NULL;
#else
    PyErr_SetString(CellframeWalletError, "Cellframe SDK not embedded");
    return NULL;
#endif
}

// Deactivate wallet
PyObject* py_dap_chain_wallet_deactivate(PyObject *self, PyObject *args) {
    (void)self;
    const char *wallet_name;
    const char *net_name;
    
    if (!PyArg_ParseTuple(args, "ss", &wallet_name, &net_name)) {
        return NULL;
    }
    
#ifdef CELLFRAME_SDK_EMBEDDED
    // TODO: Call actual dap_chain_wallet_deactivate when SDK is linked
    PyErr_SetString(PyExc_NotImplementedError, "Wallet deactivate not yet implemented in SDK");
    return NULL;
#else
    PyErr_SetString(CellframeWalletError, "Cellframe SDK not embedded");
    return NULL;
#endif
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

PyObject* PyCellframeWallet_create(PyObject *self, PyObject *args) {
    (void)self; (void)args;
    PyErr_SetString(PyExc_NotImplementedError, "Wallet create not yet implemented");
    return NULL;
}

PyObject* PyCellframeWallet_get_balance_method(PyCellframeWallet *self, PyObject *args) {
    (void)self; (void)args;
    PyErr_SetString(PyExc_NotImplementedError, "Wallet get_balance not yet implemented");
    return NULL;
}