#ifndef CELLFRAME_RPC_H
#define CELLFRAME_RPC_H

#include <Python.h>

#ifdef __cplusplus
extern "C" {
#endif

PyMethodDef* cellframe_rpc_get_methods(void);
int cellframe_rpc_init(PyObject *module);

#ifdef __cplusplus
}
#endif

#endif // CELLFRAME_RPC_H
