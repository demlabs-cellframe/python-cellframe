#ifndef CELLFRAME_NODE_H
#define CELLFRAME_NODE_H

#include <Python.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// =========================================
// NODE TYPE DEFINITION
// =========================================

typedef struct {
    PyObject_HEAD
    void *node_ptr;
    char *node_addr;
    int node_port;
    bool is_initialized;
} PyCellframeNode;

// =========================================
// NODE FUNCTION DECLARATIONS
// =========================================

extern PyTypeObject PyCellframeNodeType;
PyObject* PyCellframeNode_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCellframeNode_init(PyCellframeNode *self, PyObject *args, PyObject *kwds);
void PyCellframeNode_dealloc(PyCellframeNode *self);
PyObject* PyCellframeNode_start(PyCellframeNode *self, PyObject *args);
PyObject* PyCellframeNode_stop(PyCellframeNode *self, PyObject *args);
PyObject* PyCellframeNode_get_status(PyCellframeNode *self, PyObject *args);

#ifdef __cplusplus
}
#endif

#endif // CELLFRAME_NODE_H