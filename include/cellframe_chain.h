#ifndef CELLFRAME_CHAIN_H
#define CELLFRAME_CHAIN_H

#include <Python.h>

#ifdef __cplusplus
extern "C" {
#endif

// =========================================
// CHAIN TYPE DEFINITION
// =========================================

typedef struct {
    PyObject_HEAD
    void *chain_ptr;
    char *chain_name;
} PyCellframeChain;

// =========================================
// CHAIN FUNCTION DECLARATIONS
// =========================================

extern PyTypeObject PyCellframeChainType;
PyObject* PyCellframeChain_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCellframeChain_init(PyCellframeChain *self, PyObject *args, PyObject *kwds);
void PyCellframeChain_dealloc(PyCellframeChain *self);
PyObject* PyCellframeChain_create(PyObject *self, PyObject *args);
PyObject* PyCellframeChain_get_stats(PyCellframeChain *self, PyObject *args);

// =========================================
// MISSING FUNCTIONS DECLARATIONS
// =========================================

// Forward declarations for DAP types
typedef struct dap_chain_addr dap_chain_addr_t;
typedef struct dap_chain dap_chain_t;
typedef struct dap_chain_datum dap_chain_datum_t;

// Missing functions that Python requires
uint64_t dap_chain_addr_get_net_id(dap_chain_addr_t *a_addr);
void* dap_chain_atom_create(size_t a_size);
dap_chain_t* dap_chain_mempool_by_chain_name(const char *a_chain_name);
dap_chain_datum_t* dap_chain_mempool_tx_get_by_hash(dap_chain_t *a_chain, const char *a_tx_hash);

// Python wrapper functions for chain
PyObject* py_dap_chain_addr_from_str(PyObject *self, PyObject *args);
PyObject* py_dap_chain_addr_get_net_id(PyObject *self, PyObject *args);
PyObject* py_dap_chain_atom_create(PyObject *self, PyObject *args);
PyObject* py_dap_chain_mempool_by_chain_name(PyObject *self, PyObject *args);
PyObject* py_dap_chain_mempool_tx_get_by_hash(PyObject *self, PyObject *args);



// Module initialization function
int cellframe_chain_init(PyObject *module);

#ifdef __cplusplus
}
#endif

#endif // CELLFRAME_CHAIN_H 