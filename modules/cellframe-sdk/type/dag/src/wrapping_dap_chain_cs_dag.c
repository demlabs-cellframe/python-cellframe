#include "wrapping_dap_chain_cs_dag.h"
#include "dap_chain_cell.h"

PyMethodDef DapChainCsDagMethods[] = {
        {"findByHash", (PyCFunction)dap_chain_cs_dag_find_event_by_hash_py, METH_VARARGS, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainCsDagType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.Consensus.ChainCSDag",                                            /* tp_name */
        sizeof(PyDapChainCsDagObject),                                     /* tp_basicsize */
        0,                                                            /* tp_itemsize */
        0,                         /* tp_dealloc */
        0,                                                            /* tp_print */
        0,                                                            /* tp_getattr */
        0,                                                            /* tp_setattr */
        0,                                                            /* tp_reserved */
        0,                                                            /* tp_repr */
        0,                                                            /* tp_as_number */
        0,                                                            /* tp_as_sequence */
        0,                                                            /* tp_as_mapping */
        0,                                                            /* tp_hash  */
        0,                                                            /* tp_call */
        0,                                                            /* tp_str */
        0,                                                            /* tp_getattro */
        0,                                                            /* tp_setattro */
        0,                                                            /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                                      /* tp_flags */
        "Chain cs dag objects",                                              /* tp_doc */
        0,                                                              /* tp_traverse */
        0,                                                              /* tp_clear */
        0,                                                              /* tp_richcompare */
        0,                                                              /* tp_weaklistoffset */
        0,                                                              /* tp_iter */
        0,                                                            /* tp_iternext */
        DapChainCsDagMethods,                                        /* tp_methods */
        0,                                                          /* tp_members */
        0,                                                           /* tp_getset */
        0,                                                            /* tp_base */
        0,                                                            /* tp_dict */
        0,                                                            /* tp_descr_get */
        0,                                                            /* tp_descr_set */
        0,                                                            /* tp_dictoffset */
        0,                                                            /* tp_init */
        0,                                                            /* tp_alloc */
        PyType_GenericNew,                                            /* tp_new */
};

PyObject *dap_chain_cs_dag_find_event_by_hash_py(PyObject *self, PyObject *args){
    PyObject *obj_hash;
    PyObject *obj_atom_iter;
    if (!PyArg_ParseTuple(args, "O", &obj_hash)){
        PyErr_SetString(PyExc_AttributeError, "Function takes exactly two arguments. ");
        return NULL;
    }
    bool isCheck = PyObject_TypeCheck(obj_hash, &DapChainHashFastObjectType);
    if (!isCheck){
        PyErr_SetString(PyExc_AttributeError, "The first argument to this function is not a DapHash object.");
        return NULL;
    }
//    if (!PyDapChainAtomIter_Check(obj_atom_iter)){
//        PyErr_SetString(PyExc_AttributeError, "The second argument received to this function is not an object of type ChainAtomIter.");
//        return NULL;
//    }
    dap_chain_cell_t *l_cell = ((PyDapChainCsDagObject*)self)->dag->chain->cells;
    size_t size_atom = 0;
    dap_chain_atom_iter_t *l_iter = ((PyDapChainCsDagObject*)self)->dag->chain->callback_atom_iter_create(
            ((PyDapChainCsDagObject*)self)->dag->chain,
            l_cell->id, 0
            );
    dap_chain_atom_ptr_t l_ptr = ((PyDapChainCsDagObject*)self)->dag->chain->callback_atom_find_by_hash(
            l_iter,
            ((PyDapHashFastObject*)obj_hash)->hash_fast,
            &size_atom);
    if (l_ptr == NULL){
        Py_RETURN_NONE;
    }
    PyDapChainCsDagEventObject *obj_event = PyObject_New(PyDapChainCsDagEventObject,
                                                         &DapChainCsDagEventType);
    obj_event->event = (dap_chain_cs_dag_event_t *)l_iter->cur;
    obj_event->event_size = l_iter->cur_size;
    return (PyObject*)obj_event;
}
