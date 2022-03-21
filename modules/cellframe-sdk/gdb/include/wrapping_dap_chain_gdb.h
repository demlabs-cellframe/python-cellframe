#ifndef _WRAPPING_DAP_CHAIN_GDB_
#define _WRAPPING_DAP_CHAIN_GDB_
#include <Python.h>
#include "dap_chain_cs_none.h"
#include "libdap-chain-python.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyDapChainGDB{
    PyObject_HEAD
}PyDapChainGDBObject;

int dap_chain_gdb_init_py(void);

PyObject* dap_chain_gdb_new_py(PyObject* self, PyObject *args);

PyObject *dap_chain_gdb_delete_py(PyObject *self, PyObject *args);
PyObject *dap_chain_gdb_get_group_py(PyObject *self, PyObject *args);

extern PyTypeObject DapChainGdbObjectType;

#ifdef __cplusplus
}
#endif
#endif // _WRAPPING_DAP_CHAIN_GDB_
