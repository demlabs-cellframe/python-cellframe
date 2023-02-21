#include "libdap-chain-python.h"
#include "python-cellframe_common.h"

#define LOG_TAG "libdap-chain-python"


int init_chain_py(){
    return  dap_chain_init();
}

void deinit_chain_py(){
    dap_chain_deinit();
}

static PyMethodDef DapChainMethods[] = {
        {"findById", (PyCFunction)dap_chain_find_by_id_py, METH_VARARGS|METH_STATIC, ""},
        {"loadFromCfg", (PyCFunction)dap_chain_load_from_cfg_py, METH_VARARGS|METH_STATIC, ""},
        {"hasFileStore", (PyCFunction)dap_chain_has_file_store_py, METH_NOARGS, ""},
        {"saveAll", (PyCFunction) dap_chain_save_all_py, METH_NOARGS, ""},
        {"loadAll", (PyCFunction)dap_chain_load_all_py, METH_NOARGS, ""},
        {"createAtomItem", (PyCFunction) dap_chain_python_create_atom_iter, METH_VARARGS, ""},
        {"atomIterGetFirst", (PyCFunction) dap_chain_python_atom_iter_get_first, METH_VARARGS, ""},
        {"atomGetDatums", (PyCFunction) dap_chain_python_atom_get_datums, METH_VARARGS, ""},
        {"atomIterGetNext", (PyCFunction)dap_chain_python_atom_iter_get_next, METH_VARARGS, ""},
        {"getDag", (PyCFunction)dap_chain_python_atom_iter_get_dag, METH_NOARGS},
        {"addMempoolNotify", (PyCFunction)dap_chain_python_add_mempool_notify_callback, METH_VARARGS, ""},
        {"addAtomNotify", (PyCFunction)dap_chain_net_add_atom_notify_callback, METH_VARARGS, ""},
        {"atomFindByHash", (PyCFunction)dap_chain_python_atom_find_by_hash, METH_VARARGS, ""},
        {"countAtom", (PyCFunction)dap_chain_python_get_atom_count, METH_NOARGS, ""},
        {"getAtoms", (PyCFunction)dap_chain_python_get_atoms, METH_VARARGS, ""},
        {"countTx", (PyCFunction)dap_chain_python_get_count_tx, METH_NOARGS, ""},
        {"getTransactions", (PyCFunction)dap_chain_python_get_txs, METH_VARARGS, ""},
        {"getCSName", (PyCFunction)dap_chain_python_get_cs_name, METH_NOARGS, ""},
        //{"close", (PyCFunction)dap_chain_close_py, METH_NOARGS, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.Chain",                                            /* tp_name */
        sizeof(PyDapChainObject),                                     /* tp_basicsize */
        0,                                                            /* tp_itemsize */
        (destructor)PyDapChainObject_dealloc,                         /* tp_dealloc */
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
        PyDapChain_str,                                                            /* tp_str */
        0,                                                            /* tp_getattro */
        0,                                                            /* tp_setattro */
        0,                                                            /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                                      /* tp_flags */
        "Chain objects",                                              /* tp_doc */
        0,		                                                      /* tp_traverse */
        0,		                                                      /* tp_clear */
        0,		                                                      /* tp_richcompare */
        0,		                                                      /* tp_weaklistoffset */
        0,		                                                      /* tp_iter */
        0,		                                                      /* tp_iternext */
        DapChainMethods,                                              /* tp_methods */
        0,                                                            /* tp_members */
        0,                                                            /* tp_getset */
        0,                                                            /* tp_base */
        0,                                                            /* tp_dict */
        0,                                                            /* tp_descr_get */
        0,                                                            /* tp_descr_set */
        0,                                                            /* tp_dictoffset */
        0,                                                            /* tp_init */
        0,                                                            /* tp_alloc */
        PyDapChainObject_new,                                            /* tp_new */
};

PyObject *dap_chain_find_by_id_py(PyObject *self, PyObject *args){
    PyObject *obj_net_id;
    PyObject *obj_chain_id;
    if (!PyArg_ParseTuple(args, "O|O", &obj_net_id, &obj_chain_id))
        return NULL;
    PyObject *new_obj = _PyObject_New(&DapChainObjectType);
    ((PyDapChainObject*)new_obj)->chain_t = dap_chain_find_by_id(((PyDapChainNetIdObject*)obj_net_id)->net_id,
                                                                 *(((PyDapChainIDObject*)obj_chain_id)->chain_id));
    return Py_BuildValue("O", &new_obj);
}

//PyObject *dap_chain_load_from_cfg_py(PyObject *self, PyObject *args){
//return NULL;
//}

PyObject *dap_chain_has_file_store_py(PyObject *self, PyObject *args){
    bool res = dap_chain_has_file_store(((PyDapChainObject*)self)->chain_t);
    if (res)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject *dap_chain_save_all_py(PyObject *self, PyObject *args){
    return PyLong_FromLong( dap_chain_save_all(((PyDapChainObject*)self)->chain_t) );
}

PyObject *dap_chain_load_all_py(PyObject *self, PyObject *args){
    return  PyLong_FromLong(dap_chain_load_all(((PyDapChainObject*)self)->chain_t));
}

PyObject *dap_chain_load_from_cfg_py(PyObject *self, PyObject *args){
    PyObject *obj_ledger;
    const char *chain_net_name;
    PyObject *obj_net_id;
    const char *cfg_name;
    if (!PyArg_ParseTuple(args, "O|s|O|s", &obj_ledger, &chain_net_name, &obj_net_id, &cfg_name))
        return NULL;
    PyObject *res_obj = _PyObject_New(&DapChainObjectType);
    ((PyDapChainObject*)res_obj)->chain_t = dap_chain_load_from_cfg(((PyDapChainLedgerObject*)obj_ledger)->ledger, chain_net_name, ((PyDapChainNetIdObject*)obj_net_id)->net_id, cfg_name);
    return Py_BuildValue("O", &res_obj);
}

//PyObject *dap_chain_init_net_cfg_name_py(PyObject *self, PyObject *args){
//return NULL;
//}

//PyObject *dap_chain_close_py(PyObject *self, PyObject *args){
//    dap_chain_close(((PyDapChainObject*)self)->chain_t);
//    return PyLong_FromLong(0);
//}

//PyObject *dap_chain_info_dump_log_py(PyObject *self, PyObject *args){
//return NULL;
//}


PyObject *PyDapChainObject_new(PyTypeObject *type_object, PyObject *args, PyObject *kwds){
    PyDapChainObject *obj = (PyDapChainObject*)PyType_GenericNew(type_object, args, kwds);
    return (PyObject *)obj;
}

void PyDapChainObject_dealloc(PyDapChainObject* chain){
    if (chain->chain_t) {
        dap_chain_delete(chain->chain_t);
    }
    Py_TYPE(chain)->tp_free((PyObject*)chain);
}

PyObject *dap_chain_python_create_atom_iter(PyObject *self, PyObject *args){
//    PyObject *obj_cell_id;
    PyObject *obj_boolean;
    if (!PyArg_ParseTuple(args, "O", &obj_boolean)){
        PyErr_SetString(PyExc_AttributeError, "This function takes two arguments. ");
        return NULL;
    }
//    if (!PyDapChainCell_Check(obj_cell_id)){
//        PyErr_SetString(PyExc_AttributeError, "The first argument to this function must be of type ChainCell.");
//        return NULL;
//    }
    if (!PyBool_Check(obj_boolean)){
        PyErr_SetString(PyExc_AttributeError, "The second argument accepted by this function is not a boolean value. ");
        return NULL;
    }
    bool with_treshold = (obj_boolean == Py_True) ? 1 : 0;
    PyObject *obj_atom_iter = _PyObject_New(&DapChainAtomIterObjectType);
    PyObject_Init(obj_atom_iter, &DapChainAtomIterObjectType);
//    ((PyDapChainObject*)self)->chain_t->callback_atom_iter_create()
    ((PyChainAtomIterObject*)obj_atom_iter)->atom_iter =
            ((PyDapChainObject*)self)->chain_t->callback_atom_iter_create(
                    ((PyDapChainObject*)self)->chain_t,
                    ((PyDapChainObject*)self)->chain_t->cells->id, with_treshold);
    return obj_atom_iter;
}

PyObject *dap_chain_python_atom_iter_get_first(PyObject *self, PyObject *args){
    PyObject *obj_iter;
    if (!PyArg_ParseTuple(args, "O", &obj_iter)){
        PyErr_SetString(PyExc_AttributeError, "Function takes exactly one argument");
        return NULL;
    }
    if (!PyDapChainAtomIter_Check(obj_iter)){
        PyErr_SetString(PyExc_ValueError, "Argument must be ChainAtomIter object");
        return NULL;
    }
    PyObject *obj_atom_ptr = _PyObject_New(&DapChainAtomPtrObjectType);
    obj_atom_ptr = PyObject_Init(obj_atom_ptr, &DapChainAtomPtrObjectType);
    size_t l_atom_size = 0;
    ((PyChainAtomPtrObject*)obj_atom_ptr)->ptr = ((PyDapChainObject*)self)->chain_t->callback_atom_iter_get_first(
            ((PyChainAtomIterObject*)obj_iter)->atom_iter, &l_atom_size
            );
    if (((PyChainAtomPtrObject*)obj_atom_ptr)->ptr == NULL){
        Py_RETURN_NONE;
    }
    return Py_BuildValue("On", obj_atom_ptr, l_atom_size);
}

PyObject *dap_chain_python_atom_get_datums(PyObject *self, PyObject *args){
    PyObject *obj_atom = NULL;
    size_t atom_size = 0;
    if(!PyArg_ParseTuple(args, "On", &obj_atom, &atom_size)){
        PyErr_SetString(PyExc_AttributeError, "The second argument must be an integer");
        return NULL;
    }
    size_t datums_count = 0;
    dap_chain_datum_t **l_datums = ((PyDapChainObject*)self)->chain_t->callback_atom_get_datums(((PyChainAtomPtrObject*)obj_atom)->ptr, atom_size, &datums_count);
    PyObject *list_datums = PyList_New(datums_count);
    for (size_t i=0; i < datums_count; i++){
        PyObject *obj_datum = _PyObject_New(&DapChainDatumObjectType);
        obj_datum = PyObject_Init(obj_datum, &DapChainDatumObjectType);
        ((PyDapChainDatumObject*)obj_datum)->datum = l_datums[i];
        ((PyDapChainDatumObject*)obj_datum)->origin = false;
        PyList_SetItem(list_datums, i, obj_datum);
    }
    return list_datums;
}

PyObject *dap_chain_python_atom_iter_get_next(PyObject *self, PyObject *args){
    //
    size_t atom_size = 0;
    PyObject *atom_iter = NULL;
    if(!PyArg_ParseTuple(args, "O", &atom_iter)){
        PyErr_SetString(PyExc_AttributeError, "Function takes exactly one argument");
        return NULL;
    }
    if (!PyDapChainAtomIter_Check(atom_iter)){
        PyErr_SetString(PyExc_AttributeError, "The first argument must be ChainAtomIter object.");
        return NULL;
    }
    PyObject *obj_atom_ptr = _PyObject_New(&DapChainAtomPtrObjectType);
    obj_atom_ptr = PyObject_Init(obj_atom_ptr, &DapChainAtomPtrObjectType);
    ((PyChainAtomPtrObject*)obj_atom_ptr)->ptr = ((PyDapChainObject*)self)->chain_t->callback_atom_iter_get_next(
            ((PyChainAtomIterObject*)atom_iter)->atom_iter,
            &atom_size);
    if (((PyChainAtomPtrObject*)obj_atom_ptr)->ptr == NULL){
        return Py_BuildValue("On", Py_None, 0);
    }
    return Py_BuildValue("On", obj_atom_ptr, atom_size);
}

PyObject *dap_chain_python_atom_iter_get_dag(PyObject *self, PyObject *args){
    (void)args;
    PyDapChainCsDagObject *obj_dag = PyObject_New(PyDapChainCsDagObject, &DapChainCsDagType);
    obj_dag->dag = DAP_CHAIN_CS_DAG(((PyDapChainObject*)self)->chain_t);
    return (PyObject*)obj_dag;
}

typedef struct _wrapping_chain_mempool_notify_callback{
    PyObject *func;
    PyObject *arg;
}_wrapping_chain_mempool_notify_callback_t;

void _wrapping_dap_chain_mempool_notify_handler(void * a_arg, const char a_op_code, const char * a_group,
                                                const char * a_key, const void * a_value, const size_t a_value_len){
    if (!a_arg){
        return;
    }
    _wrapping_chain_mempool_notify_callback_t *l_callback = (_wrapping_chain_mempool_notify_callback_t*)a_arg;
    char l_op_code[2];
    l_op_code[0] = a_op_code;
    l_op_code[1] = '\0';
    PyObject *l_args;
    PyGILState_STATE state = PyGILState_Ensure();
    if (a_op_code == DAP_DB$K_OPTYPE_ADD) {
        PyObject *l_value = PyBytes_FromStringAndSize(a_value, (Py_ssize_t)a_value_len);
        l_args = Py_BuildValue("sssOO", l_op_code, a_group, a_key, l_value, l_callback->arg);
        Py_DECREF(l_value);
    } else
        l_args = Py_BuildValue("sssOO", l_op_code, a_group, a_key, Py_None, l_callback->arg);
    log_it(L_DEBUG, "Call mempool notifier with key '%s'", a_key ? a_key : "null");
    PyEval_CallObject(l_callback->func, l_args);
    Py_DECREF(l_args);
    PyGILState_Release(state);
}

PyObject *dap_chain_python_add_mempool_notify_callback(PyObject *self, PyObject *args){
    dap_chain_t *l_chain = ((PyDapChainObject*)self)->chain_t;
    PyObject *obj_func;
    PyObject *obj_arg;
    if (!PyArg_ParseTuple(args, "OO", &obj_func, &obj_arg)){
        PyErr_SetString(PyExc_AttributeError, "Argument must be callable");
        return NULL;
    }
    if (!PyCallable_Check(obj_func)){
        PyErr_SetString(PyExc_AttributeError, "Invalid first parameter passed to function. The first "
                                              "argument must be an instance of an object of type Chain. ");
        return NULL;
    }
    _wrapping_chain_mempool_notify_callback_t *l_callback = DAP_NEW(_wrapping_chain_mempool_notify_callback_t);
    l_callback->func = obj_func;
    l_callback->arg = obj_arg;
    Py_INCREF(obj_func);
    Py_INCREF(obj_arg);
    dap_chain_add_mempool_notify_callback(l_chain, _wrapping_dap_chain_mempool_notify_handler, l_callback);
    Py_RETURN_NONE;
}

//typedef struct _wrapping_chain_atom_notify_callback{
//    PyObject *func;
//    PyObject *arg;
//}_wrapping_chain_atom_notify_callback_t;

static void _wrapping_dap_chain_atom_notify_handler(void * a_arg, dap_chain_t *a_chain, dap_chain_cell_id_t a_id, void* a_atom, size_t a_atom_size){
    if (!a_arg){
        return;
    }
    _wrapping_chain_mempool_notify_callback_t *l_callback = (_wrapping_chain_mempool_notify_callback_t*)a_arg;

    PyObject *l_args;
    PyGILState_STATE state = PyGILState_Ensure();

    dap_chain_atom_ptr_t l_atom = (dap_chain_atom_ptr_t) a_atom;
    PyChainAtomPtrObject *l_atom_ptr_obj = NULL;
    if(l_atom){
        l_atom_ptr_obj= PyObject_New(PyChainAtomPtrObject, &DapChainAtomPtrObjectType);
        l_atom_ptr_obj->ptr = l_atom;
        l_args = Py_BuildValue("OnO", l_atom_ptr_obj, a_atom_size, l_callback->arg);
    }else{
        l_args = Py_BuildValue("OnO", Py_None, 0, l_callback->arg);
    }

    log_it(L_DEBUG, "Call atom notifier for chain %s with atom size %zd", a_chain->name, a_atom_size );
    PyObject *result = PyEval_CallObject(l_callback->func, l_args);
    if (!result) {
        python_error_in_log_it(LOG_TAG);
    }
    Py_XDECREF(result);
    Py_DECREF(l_args);
    PyGILState_Release(state);
}


PyObject *dap_chain_net_add_atom_notify_callback(PyObject *self, PyObject *args){
    dap_chain_t *l_chain = ((PyDapChainObject*)self)->chain_t;
    PyObject *obj_func;
    PyObject *obj_arg;
    if (!PyArg_ParseTuple(args, "OO", &obj_func, &obj_arg)){
        PyErr_SetString(PyExc_AttributeError, "Argument must be callable");
        return NULL;
    }
    if (!PyCallable_Check(obj_func)){
        PyErr_SetString(PyExc_AttributeError, "Invalid first parameter passed to function. The first "
                                              "argument must be a function ");
        return NULL;
    }

    _wrapping_chain_mempool_notify_callback_t *l_callback = DAP_NEW(_wrapping_chain_mempool_notify_callback_t);
    l_callback->func = obj_func;
    l_callback->arg = obj_arg;
    Py_INCREF(obj_func);
    Py_INCREF(obj_arg);
    dap_chain_add_callback_notify(l_chain, _wrapping_dap_chain_atom_notify_handler, l_callback);
    Py_RETURN_NONE;
}

PyObject *dap_chain_python_atom_find_by_hash(PyObject *self, PyObject* args){
    PyObject *obj_iter;
    PyDapHashFastObject *obj_hf;
    if (!PyArg_ParseTuple(args, "OO", &obj_iter, &obj_hf)){
        return NULL;
    }
    if (!PyDapChainAtomIter_Check(obj_iter)){
        PyErr_SetString(PyExc_AttributeError, "The first argument to the function was not correctly "
                                              "passed, the argument must be an instance of a class of type ChainAtomPtr.");
        return NULL;
    }
    if(!PyDapHashFast_Check(obj_hf)){
        PyErr_SetString(PyExc_AttributeError, "The second argument to the function was not correctly passed, the "
                                              "argument must be an instance of a class of type HashFast.");
        return NULL;
    }
    size_t l_size_atom = 0;
    dap_chain_atom_ptr_t l_ptr = ((PyDapChainObject*)self)->chain_t->callback_atom_find_by_hash(
            ((PyChainAtomIterObject*)obj_iter)->atom_iter,
            obj_hf->hash_fast,
            &l_size_atom);
    if (l_ptr == NULL) {
        return Py_BuildValue("On", Py_None, 0);
    } else {
        PyChainAtomPtrObject *l_obj_ptr = PyObject_New(PyChainAtomPtrObject, &DapChainAtomPtrObjectType);
        l_obj_ptr->ptr = l_ptr;
        return Py_BuildValue("On", l_obj_ptr, l_size_atom);
    }
}

PyObject *dap_chain_python_get_atom_count(PyObject *self, PyObject *args){
    (void)args;
    size_t l_count = ((PyDapChainObject*)self)->chain_t->callback_count_atom(((PyDapChainObject*)self)->chain_t);
    return Py_BuildValue("n", l_count);
}
PyObject *dap_chain_python_get_atoms(PyObject *self, PyObject *args){
    size_t count, page;
    PyObject *obj_reverse;
    if (!PyArg_ParseTuple(args, "nnO",&count, &page, &obj_reverse)){
        return NULL;
    }
    if (!PyBool_Check(obj_reverse)){
        PyErr_SetString(PyExc_AttributeError, "");
        return NULL;
    }
    bool reverse = (obj_reverse == Py_True) ? true : false;
    dap_chain_t *l_chain = ((PyDapChainObject*)self)->chain_t;
    dap_list_t *l_atoms = l_chain->callback_get_atoms(l_chain, count, page, reverse);
    if (!l_atoms){
        Py_RETURN_NONE;
    }
    PyObject *obj_list = PyList_New(0);
    for (dap_list_t *l_iter = l_atoms; l_iter != NULL; l_iter = l_iter->next){
        PyChainAtomPtrObject *obj_atom = PyObject_New(PyChainAtomPtrObject, &DapChainAtomPtrObjectType);
        obj_atom->ptr = l_iter->data;
        l_iter = l_iter->next;
        size_t l_ptr_size = *((size_t*)l_iter->data);
        PyObject *obj_ptr = Py_BuildValue("On", (PyObject*)obj_atom, l_ptr_size);
        PyList_Append(obj_list, obj_ptr);
        Py_XDECREF(obj_ptr);
    }
    return obj_list;
}

PyObject *dap_chain_python_get_count_tx(PyObject *self, PyObject *args){
    (void)args;
    dap_chain_t *l_chain = ((PyDapChainObject*)self)->chain_t;
    size_t cnt = l_chain->callback_count_tx(l_chain);
    return Py_BuildValue("n", cnt);
}
PyObject *dap_chain_python_get_txs(PyObject *self, PyObject *args){
    dap_chain_t *l_chain = ((PyDapChainObject*)self)->chain_t;
    size_t count = 0, page = 0;
    PyObject *obj_reverse;
    if (!PyArg_ParseTuple(args, "nnO", &count, &page, &obj_reverse)){
        return NULL;
    }
    if (!PyBool_Check(obj_reverse)){
        PyErr_SetString(PyExc_AttributeError, "");
        return NULL;
    }
    bool l_reverse = (obj_reverse == Py_True) ? true : false;
    dap_list_t *l_list = l_chain->callback_get_txs(l_chain, count, page, l_reverse);
    if (l_list != NULL){
        PyObject *l_obj_list = PyList_New(0);
        for (dap_list_t *l_ptr = l_list; l_ptr != NULL; l_ptr = l_ptr->next){
            PyDapChainDatumTxObject *l_obj_tx = PyObject_New(PyDapChainDatumTxObject, &DapChainDatumTxObjectType);
            l_obj_tx->datum_tx = l_ptr->data;
            l_obj_tx->original = false;
            PyList_Append(l_obj_list, (PyObject *)l_obj_tx);
            Py_XDECREF((PyObject *)l_obj_tx);
        }
        dap_list_free(l_list);
        return l_obj_list;
    }
    Py_RETURN_NONE;
}

PyObject *dap_chain_python_get_cs_name(PyObject *self, PyObject *args){
    (void)args;
    dap_chain_t* l_chain = ((PyDapChainObject*)self)->chain_t;
    dap_chain_pvt_t *l_chain_pvt = DAP_CHAIN_PVT(l_chain);
    return Py_BuildValue("s", l_chain_pvt->cs_name);
}

PyObject *PyDapChain_str(PyObject *self){
    return Py_BuildValue("s", ((PyDapChainObject*)self)->chain_t->name);
}
