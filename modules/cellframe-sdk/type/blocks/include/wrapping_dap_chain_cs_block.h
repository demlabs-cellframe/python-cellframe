#pragma once

#include "Python.h"
#include "dap_chain_block.h"
#include "dap_chain_cs_blocks.h"
#include "wrapping_dap_chain_cell.h"
#include "libdap-chain-python.h"
#include "datetime.h"
#include "dap_chain_block_cache.h"

typedef struct PyDapChainCSBlock{
    PyObject_HEAD
    dap_chain_block_t *block;
}PyDapChainCSBlockObject;

PyObject *wrapping_dap_chain_block_get_version(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_block_get_cell_id(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_block_get_chain_id(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_block_get_ts_created(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_block_get_meta_data(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_block_get_datums(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_block_get_signs(PyObject *self, void *closure);
//PyObject *wrapping_dap_chain_block_get_block_cache(PyObject *self, void *closure);

extern PyTypeObject DapChainCsBlockType;
