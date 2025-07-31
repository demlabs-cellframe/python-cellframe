#include "python_cellframe.h"

// Node type stub - будет реализован позже
PyTypeObject PyCellframeNodeType = {0};

PyObject* PyCellframeNode_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    (void)type; (void)args; (void)kwds;
    PyErr_SetString(PyExc_NotImplementedError, "Node type not yet implemented");
    return NULL;
}

int PyCellframeNode_init(PyCellframeNode *self, PyObject *args, PyObject *kwds) {
    (void)self; (void)args; (void)kwds;
    return -1;
}

void PyCellframeNode_dealloc(PyCellframeNode *self) {
    (void)self;
}

PyObject* PyCellframeNode_start(PyCellframeNode *self, PyObject *args) {
    (void)self; (void)args;
    PyErr_SetString(PyExc_NotImplementedError, "Node start not yet implemented");
    return NULL;
}

PyObject* PyCellframeNode_stop(PyCellframeNode *self, PyObject *args) {
    (void)self; (void)args;
    PyErr_SetString(PyExc_NotImplementedError, "Node stop not yet implemented");
    return NULL;
}

PyObject* PyCellframeNode_get_status(PyCellframeNode *self, PyObject *args) {
    (void)self; (void)args;
    PyErr_SetString(PyExc_NotImplementedError, "Node get_status not yet implemented");
    return NULL;
} 