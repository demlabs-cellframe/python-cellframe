#pragma once

#include <Python.h>
#include "dap_chain.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyChainAtomPtr{
    PyObject_HEAD
    dap_chain_atom_ptr_t ptr;
}PyChainAtomPtrObject;

extern PyTypeObject DapChainAtomPtrObjectType;

bool PyDapChainAtomPtr_Check(PyObject *obj);

#ifdef __cplusplus
}
#endif