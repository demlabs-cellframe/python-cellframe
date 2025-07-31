#include "python_cellframe.h"

// Chain type stub - будет реализован позже
PyTypeObject PyCellframeChainType = {0};

PyObject* PyCellframeChain_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    (void)type; (void)args; (void)kwds;
    PyErr_SetString(PyExc_NotImplementedError, "Chain type not yet implemented");
    return NULL;
}

int PyCellframeChain_init(PyCellframeChain *self, PyObject *args, PyObject *kwds) {
    (void)self; (void)args; (void)kwds;
    return -1;
}

void PyCellframeChain_dealloc(PyCellframeChain *self) {
    (void)self;
}

PyObject* PyCellframeChain_create(PyObject *self, PyObject *args) {
    (void)self; (void)args;
    PyErr_SetString(PyExc_NotImplementedError, "Chain create not yet implemented");
    return NULL;
}

PyObject* PyCellframeChain_get_stats(PyCellframeChain *self, PyObject *args) {
    (void)self; (void)args;
    PyErr_SetString(PyExc_NotImplementedError, "Chain get_stats not yet implemented");
    return NULL;
} 