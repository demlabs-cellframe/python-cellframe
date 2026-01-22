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

// TX Compose Registry API
PyObject* dap_chain_tx_compose_register_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_tx_compose_unregister_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_tx_compose_is_registered_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_tx_compose_create_py(PyObject *a_self, PyObject *a_args);

// High-level Compose API
PyObject* dap_compose_config_create_py(PyObject *a_self, PyObject *a_args, PyObject *a_kwargs);
PyObject* dap_compose_tx_create_py(PyObject *a_self, PyObject *a_args);

/**
 * @brief Cleanup all TX compose callbacks
 * @details CRITICAL: Must be called on module unload to prevent memory leaks
 *          Releases all Py_INCREF'd Python callback objects
 */
void dap_chain_tx_compose_cleanup_callbacks_py(void);

#ifdef __cplusplus
}
#endif
