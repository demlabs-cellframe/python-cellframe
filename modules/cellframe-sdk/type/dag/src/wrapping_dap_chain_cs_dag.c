#include "wrapping_dap_chain_cs_dag.h"
#include "dap_chain_cell.h"

PyObject *dap_chain_cs_dag_find_event_by_hash_py(PyObject *self, PyObject *args){
    PyObject *obj_hash;
    PyObject *obj_atom_iter;
    if (!PyArg_ParseTuple(args, "O", &obj_hash)){
        PyErr_SetString(PyExc_AttributeError, "function takes exactly one arguments");
        return NULL;
    }
    bool isCheck = PyObject_TypeCheck(obj_hash, &DapHashFastObject_DapHashFastObjectType);
    dap_chain_cell_t *l_cell = ((PyDapChainCsDagObject*)self)->dag->chain->cells;
    size_t size_atom = 0;
    dap_chain_atom_iter_t *l_iter = ((PyDapChainCsDagObject*)self)->dag->chain->callback_atom_iter_create(
            ((PyDapChainCsDagObject*)self)->dag->chain);
    dap_chain_atom_ptr_t l_ptr = ((PyDapChainCsDagObject*)self)->dag->chain->callback_atom_find_by_hash(
            l_iter,
            ((PyDapHashFastObject*)obj_hash)->hash_fast,
            &size_atom);
    if (l_ptr == NULL){
        return Py_None;
    }
    PyDapChainCsDagEventObject *obj_event = PyObject_New(PyDapChainCsDagEventObject,
                                                         &DapChainCsDagEvent_DapChainCsDagEventType);
    PyObject_Dir((PyObject*)obj_event);
    obj_event->event = l_iter->cur;
    obj_event->event_size = l_iter->cur_size;
    return (PyObject*)obj_event;
}