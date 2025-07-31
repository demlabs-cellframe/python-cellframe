#include "python_cellframe.h"

// Wallet type stub - будет реализован позже
PyTypeObject PyCellframeWalletType = {0};

PyObject* PyCellframeWallet_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    (void)type; (void)args; (void)kwds;
    PyErr_SetString(PyExc_NotImplementedError, "Wallet type not yet implemented");
    return NULL;
}

int PyCellframeWallet_init(PyCellframeWallet *self, PyObject *args, PyObject *kwds) {
    (void)self; (void)args; (void)kwds;
    return -1;
}

void PyCellframeWallet_dealloc(PyCellframeWallet *self) {
    (void)self;
}

PyObject* PyCellframeWallet_create(PyObject *self, PyObject *args) {
    (void)self; (void)args;
    PyErr_SetString(PyExc_NotImplementedError, "Wallet create not yet implemented");
    return NULL;
}

PyObject* PyCellframeWallet_get_balance(PyCellframeWallet *self, PyObject *args) {
    (void)self; (void)args;
    PyErr_SetString(PyExc_NotImplementedError, "Wallet get_balance not yet implemented");
    return NULL;
} 