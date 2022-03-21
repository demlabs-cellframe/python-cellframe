#ifndef _LIBDAP_CHAIN_TYPE_PYTHON_
#define _LIBDAP_CHAIN_TYPE_PYTHON_
#include "Python.h"
#include "dap_chain.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyChainType{
    PyObject_HEAD
    dap_chain_type_t chain_type;
} PyChainTypeObject;

PyObject* CHAIN_TYPE_FIRST_PY();
PyObject* CHAIN_TYPE_TOKEN_PY();
PyObject* CHAIN_TYPE_EMISSION_PY();
PyObject* CHAIN_TYPE_TX_PY();
PyObject* CHAIN_TYPE_LAST_PY();

extern PyTypeObject DapChainTypeObjectType;

#ifdef __cplusplus
}
#endif

#endif //_LIBDAP_CHAIN_TYPE_PYTHON_
