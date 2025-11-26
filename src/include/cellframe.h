#ifndef CELLFRAME_H
#define CELLFRAME_H

// =========================================
// VERSION AND BUILD CONFIGURATION
// =========================================
#define PYTHON_CELLFRAME_VERSION "6.0.0"
#define PYTHON_CELLFRAME_MODULE_NAME "python_cellframe"

// Include Python headers first to avoid macro conflicts
#include <Python.h>

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// =========================================
// CONDITIONAL CELLFRAME SDK INTEGRATION
// =========================================
// When SDK is embedded, include DAP SDK and Cellframe headers
// DAP SDK headers (through external include paths)
#include "dap_common.h"
#include "dap_config.h"
#include "dap_chain_common.h"
// Cellframe SDK module headers  
#include "dap_chain.h"
#include "dap_chain_net.h"
#include "dap_chain_wallet.h"
#define CELLFRAME_AVAILABLE 1

// =========================================
// EXCEPTION OBJECTS (declared before includes)
// =========================================
extern PyObject *CellframeError;
extern PyObject *CellframeChainError;
extern PyObject *CellframeNetworkError;
extern PyObject *CellframeWalletError;
extern PyObject *CellframeNodeError;

// =========================================
// INCLUDE TYPE DEFINITIONS
// =========================================
// Include specialized headers after basic definitions
#include "cellframe_node.h"
#include "cellframe_wallet.h"
#include "cellframe_chain.h"
#include "cf_ledger.h"
#include "cellframe_services.h"
#include "cellframe_network.h"
#include "cf_consensus.h"
#include "cf_stake.h"
#include "cf_mempool.h"

// =========================================
// FUNCTION DECLARATIONS
// =========================================
// Module initialization
PyMODINIT_FUNC PyInit_python_cellframe(void);

// Specific function declarations are now in respective header files:
// - cellframe_node.h: Node functions
// - cellframe_chain.h: Chain functions  
// - cellframe_wallet.h: Wallet functions
// - cellframe_network.h: Network functions

// =========================================
// UTILITY FUNCTIONS
// =========================================
bool cellframe_sdk_is_available(void);
PyObject* cellframe_sdk_is_available_wrapper(PyObject *self);
PyObject* cellframe_get_version(PyObject *self);
PyObject* cellframe_initialize(PyObject *self, PyObject *args, PyObject *kwds);
PyObject* cellframe_deinitialize(PyObject *self);

#ifdef __cplusplus
}
#endif

#endif /* CELLFRAME_H */

