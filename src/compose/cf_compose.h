/**
 * @file cf_compose.h
 * @brief Python bindings for TX compose API
 * @details Transaction composition interface with callback registry
 * @author Cellframe Development Team
 * @date 2025-01-11
 */

#pragma once

#include <Python.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Cleanup all TX compose callbacks
 * @details CRITICAL: Must be called on module unload to prevent memory leaks
 *          Releases all Py_INCREF'd Python callback objects
 */
void dap_chain_tx_compose_cleanup_callbacks_py(void);

#ifdef __cplusplus
}
#endif
