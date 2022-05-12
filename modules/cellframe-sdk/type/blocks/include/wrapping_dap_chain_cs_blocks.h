#pragma once

#include "Python.h"
#include "dap_chain_cs_blocks.h"

typedef struct PyDapChainCSBlocks{
    PyObject_HEAD
    dap_chain_cs_blocks_t *blocks;
}PyDapChainCSBlocksObject;

PyObject *wrapping_dap_chain_cs_block_get_block(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_cs_block_get_list_blocks(PyObject *self, PyObject *args);


