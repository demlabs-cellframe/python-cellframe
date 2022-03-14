#ifndef _LIBDAP_CHAIN_ATOM_ITER_PYTHON_
#define _LIBDAP_CHAIN_ATOM_ITER_PYTHON_
#include "Python.h"
#include "dap_chain.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyChainAtomIter{
    PyObject_HEAD
    dap_chain_atom_iter_t *atom_iter;
} PyChainAtomIterObject;

extern PyTypeObject DapChainAtomIter_DapChainAtomIterType;

static bool PyDapChainAtomIter_Check(PyObject *obj){
    return PyObject_TypeCheck(obj, &DapChainAtomIter_DapChainAtomIterType);
}

#ifdef __cplusplus
}
#endif

#endif //_LIBDAP_CHAIN_ATOM_ITER_PYTHON_
