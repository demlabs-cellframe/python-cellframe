#ifndef CELLFRAME_NETWORK_H
#define CELLFRAME_NETWORK_H

#include <Python.h>

#ifdef __cplusplus
extern "C" {
#endif

// =========================================
// NETWORK TYPE DEFINITION
// =========================================

typedef struct {
    PyObject_HEAD
    void *network_ptr;
    char *network_name;
} PyCellframeNetwork;

// =========================================
// NETWORK FUNCTION DECLARATIONS
// =========================================

extern PyTypeObject PyCellframeNetworkType;
PyObject* PyCellframeNetwork_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCellframeNetwork_init(PyCellframeNetwork *self, PyObject *args, PyObject *kwds);
void PyCellframeNetwork_dealloc(PyCellframeNetwork *self);
PyObject* PyCellframeNetwork_connect(PyCellframeNetwork *self, PyObject *args);

#ifdef __cplusplus
}
#endif


#endif // CELLFRAME_NETWORK_H 