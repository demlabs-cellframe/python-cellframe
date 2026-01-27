#ifndef CELLFRAME_NODE_H
#define CELLFRAME_NODE_H

#include <Python.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

PyMethodDef* cellframe_node_get_methods(void);
int cellframe_node_init(PyObject *module);

#ifdef __cplusplus
}
#endif

#endif // CELLFRAME_NODE_H
