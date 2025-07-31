#ifndef PYTHON_CELLFRAME_H
#define PYTHON_CELLFRAME_H

#include <Python.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// =========================================
// CONDITIONAL CELLFRAME SDK INTEGRATION
// =========================================
#ifdef CELLFRAME_SDK_EMBEDDED
    // When SDK is embedded, include Cellframe headers
    #include "cellframe-sdk/modules/common/include/dap_common.h"
    #include "cellframe-sdk/modules/chain/include/dap_chain.h"
    #include "cellframe-sdk/modules/net/include/dap_chain_net.h"
    #include "cellframe-sdk/modules/wallet/include/dap_chain_wallet.h"
    #define CELLFRAME_AVAILABLE 1
#else
    // When SDK not embedded, define stub types
    typedef void* dap_chain_t;
    typedef void* dap_chain_net_t;
    typedef void* dap_chain_wallet_t;
    #define CELLFRAME_AVAILABLE 0
#endif

// =========================================
// MODULE INFORMATION
// =========================================
#define PYTHON_CELLFRAME_VERSION "2.0.0"
#define PYTHON_CELLFRAME_MODULE_NAME "python_cellframe"

// =========================================
// PYTHON TYPE STRUCTURES
// =========================================
typedef struct {
    PyObject_HEAD
    dap_chain_t *chain;
    char *chain_name;
    bool is_valid;
} PyCellframeChain;

typedef struct {
    PyObject_HEAD
    dap_chain_net_t *net;
    char *net_name;
    bool is_valid;
} PyCellframeNetwork;

typedef struct {
    PyObject_HEAD
    dap_chain_wallet_t *wallet;
    char *wallet_name;
    bool is_valid;
} PyCellframeWallet;

typedef struct {
    PyObject_HEAD
    char *node_addr;
    uint16_t node_port;
    bool is_initialized;
} PyCellframeNode;

// =========================================
// FUNCTION DECLARATIONS
// =========================================
// Module initialization
PyMODINIT_FUNC PyInit_python_cellframe(void);

// Chain functions
extern PyTypeObject PyCellframeChainType;
PyObject* PyCellframeChain_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCellframeChain_init(PyCellframeChain *self, PyObject *args, PyObject *kwds);
void PyCellframeChain_dealloc(PyCellframeChain *self);
PyObject* PyCellframeChain_create(PyObject *self, PyObject *args);
PyObject* PyCellframeChain_get_stats(PyCellframeChain *self, PyObject *args);

// Network functions
extern PyTypeObject PyCellframeNetworkType;
PyObject* PyCellframeNetwork_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCellframeNetwork_init(PyCellframeNetwork *self, PyObject *args, PyObject *kwds);
void PyCellframeNetwork_dealloc(PyCellframeNetwork *self);
PyObject* PyCellframeNetwork_connect(PyCellframeNetwork *self, PyObject *args);

// Wallet functions
extern PyTypeObject PyCellframeWalletType;
PyObject* PyCellframeWallet_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCellframeWallet_init(PyCellframeWallet *self, PyObject *args, PyObject *kwds);
void PyCellframeWallet_dealloc(PyCellframeWallet *self);
PyObject* PyCellframeWallet_create(PyObject *self, PyObject *args);
PyObject* PyCellframeWallet_get_balance(PyCellframeWallet *self, PyObject *args);

// Node functions
extern PyTypeObject PyCellframeNodeType;
PyObject* PyCellframeNode_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCellframeNode_init(PyCellframeNode *self, PyObject *args, PyObject *kwds);
void PyCellframeNode_dealloc(PyCellframeNode *self);
PyObject* PyCellframeNode_start(PyCellframeNode *self, PyObject *args);
PyObject* PyCellframeNode_stop(PyCellframeNode *self, PyObject *args);
PyObject* PyCellframeNode_get_status(PyCellframeNode *self, PyObject *args);

// =========================================
// UTILITY FUNCTIONS
// =========================================
bool cellframe_sdk_is_available(void);
PyObject* cellframe_get_version(PyObject *self, PyObject *args);
PyObject* cellframe_initialize(PyObject *self, PyObject *args);
PyObject* cellframe_deinitialize(PyObject *self, PyObject *args);

// =========================================
// ERROR HANDLING
// =========================================
extern PyObject *CellframeError;
extern PyObject *CellframeChainError;
extern PyObject *CellframeNetworkError;
extern PyObject *CellframeWalletError;
extern PyObject *CellframeNodeError;

#ifdef __cplusplus
}
#endif

#endif // PYTHON_CELLFRAME_H 