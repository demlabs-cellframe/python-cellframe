#ifndef _WRAPPING_DAP_MEMPOOL_
#define _WRAPPING_DAP_MEMPOOL_

#include <Python.h>
#include "dap_chain_mempool.h"
#include "dap_server_python.h"
#include "dap_http.h"
#include "libdap-chain-python.h"
#include "wrapping_dap_pkey.h"
#include "wrapping_dap_chain_common.h"
#include "libdap_chain_net_python.h"
#include "wrapping_dap_hash.h"

typedef struct PyDapMempool{
    PyObject_HEAD
}PyDapMempoolObject;

PyObject *dap_chain_mempool_add_proc_py(PyObject *self, PyObject *args);
PyObject *dap_chain_mempool_tx_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_mempool_tx_create_cond_py(PyObject *self, PyObject *args);
PyObject *dap_chain_mempool_tx_create_cond_input_py(PyObject *self, PyObject *args);

extern PyTypeObject DapChainMempoolObjectType;


#endif //_WRAPPING_DAP_MEMPOOL_
