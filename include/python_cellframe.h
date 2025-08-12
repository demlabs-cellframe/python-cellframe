#ifndef PYTHON_CELLFRAME_H
#define PYTHON_CELLFRAME_H

// =========================================
// VERSION AND BUILD CONFIGURATION
// =========================================
#define PYTHON_CELLFRAME_VERSION "1.0.0"
#define PYTHON_CELLFRAME_MODULE_NAME "python_cellframe"

// Constants from DAP SDK
// Use larger ticker size value to avoid conflicts
#ifndef DAP_CHAIN_TICKER_SIZE_MAX
#define DAP_CHAIN_TICKER_SIZE_MAX 32
#else
#undef DAP_CHAIN_TICKER_SIZE_MAX
#define DAP_CHAIN_TICKER_SIZE_MAX 32
#endif

// Include Python headers first to avoid macro conflicts
#include <Python.h>

// Save important Python macro definitions before potential conflicts
#ifdef HAVE_DLFCN_H
#define PYTHON_HAVE_DLFCN_H HAVE_DLFCN_H
#undef HAVE_DLFCN_H
#endif
#ifdef HAVE_ENDIAN_H  
#define PYTHON_HAVE_ENDIAN_H HAVE_ENDIAN_H
#undef HAVE_ENDIAN_H
#endif
#ifdef HAVE_FCNTL_H
#define PYTHON_HAVE_FCNTL_H HAVE_FCNTL_H
#undef HAVE_FCNTL_H
#endif
#ifdef HAVE_INTTYPES_H
#define PYTHON_HAVE_INTTYPES_H HAVE_INTTYPES_H
#undef HAVE_INTTYPES_H
#endif
#ifdef HAVE_STDINT_H
#define PYTHON_HAVE_STDINT_H HAVE_STDINT_H
#undef HAVE_STDINT_H
#endif
#ifdef HAVE_STDLIB_H
#define PYTHON_HAVE_STDLIB_H HAVE_STDLIB_H
#undef HAVE_STDLIB_H
#endif
#ifdef HAVE_STRINGS_H
#define PYTHON_HAVE_STRINGS_H HAVE_STRINGS_H
#undef HAVE_STRINGS_H
#endif
#ifdef HAVE_STRING_H
#define PYTHON_HAVE_STRING_H HAVE_STRING_H
#undef HAVE_STRING_H
#endif
#ifdef HAVE_SYS_RANDOM_H
#define PYTHON_HAVE_SYS_RANDOM_H HAVE_SYS_RANDOM_H
#undef HAVE_SYS_RANDOM_H
#endif
#ifdef HAVE_SYS_RESOURCE_H
#define PYTHON_HAVE_SYS_RESOURCE_H HAVE_SYS_RESOURCE_H
#undef HAVE_SYS_RESOURCE_H
#endif
#ifdef HAVE_SYS_STAT_H
#define PYTHON_HAVE_SYS_STAT_H HAVE_SYS_STAT_H
#undef HAVE_SYS_STAT_H
#endif
#ifdef HAVE_SETLOCALE
#define PYTHON_HAVE_SETLOCALE HAVE_SETLOCALE
#undef HAVE_SETLOCALE
#endif
#ifdef HAVE_SNPRINTF
#define PYTHON_HAVE_SNPRINTF HAVE_SNPRINTF
#undef HAVE_SNPRINTF
#endif
#ifdef HAVE_GETRANDOM
#define PYTHON_HAVE_GETRANDOM HAVE_GETRANDOM
#undef HAVE_GETRANDOM
#endif
#ifdef HAVE_GETRUSAGE
#define PYTHON_HAVE_GETRUSAGE HAVE_GETRUSAGE
#undef HAVE_GETRUSAGE
#endif
#ifdef STDC_HEADERS
#define PYTHON_STDC_HEADERS STDC_HEADERS
#undef STDC_HEADERS
#endif

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// =========================================
// CONDITIONAL CELLFRAME SDK INTEGRATION
// =========================================
#ifdef CELLFRAME_SDK_EMBEDDED
    // When SDK is embedded, include DAP SDK and Cellframe headers
    // DAP SDK headers (through external include paths)
    #include "dap_common.h"
    #include "dap_chain_common.h"
    // Cellframe SDK module headers  
    #include "dap_chain.h"
    #include "dap_chain_net.h"
    #include "dap_chain_wallet.h"
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
#ifndef PYTHON_CELLFRAME_VERSION
#define PYTHON_CELLFRAME_VERSION "2.0.0"
#endif
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
PyObject* PyCellframeWallet_get_balance_method(PyCellframeWallet *self, PyObject *args);

// Wallet module-level functions (for import)
PyObject* py_dap_chain_wallet_create(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_create_with_seed(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_create_with_seed_multi(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_open(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_open_ext(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_close(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_save(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_get_addr(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_get_balance(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_get_key(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_get_pkey(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_activate(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_deactivate(PyObject *self, PyObject *args);

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
PyObject* cellframe_sdk_is_available_wrapper(PyObject *self);
PyObject* cellframe_get_version(PyObject *self);
PyObject* cellframe_initialize(PyObject *self, PyObject *args, PyObject *kwds);
PyObject* cellframe_deinitialize(PyObject *self);

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