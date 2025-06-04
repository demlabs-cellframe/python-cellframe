#include "libdap-python.h"
#include "libdap_chain_atom_iter_python.h"

#define LOG_TAG "chain_atom_iter"

static PyMethodDef DapChainAtomIterMethods[] = {
        {}
};

void PyDapChainAtomIter_dealloc(PyChainAtomIterObject *self) {
    if (self->atom_iter && self->atom_iter->chain && self->atom_iter->chain->callback_atom_iter_delete) {
        self->atom_iter->chain->callback_atom_iter_delete(self->atom_iter);
        self->atom_iter = NULL;
    }
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyTypeObject DapChainAtomIterObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.ChainAtomIter", sizeof(PyChainAtomIterObject),
        "Chain atom iter objects",
        .tp_dealloc = (destructor)PyDapChainAtomIter_dealloc,
        .tp_methods = DapChainAtomIterMethods);

bool PyDapChainAtomIter_Check(PyObject *obj){
    return PyObject_TypeCheck(obj, &DapChainAtomIterObjectType);
}
