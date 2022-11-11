#include "libdap-chain-python.h"
#include "dap_chain_pvt.h"

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
        {"createAtomIter", (PyCFunction) dap_chain_python_create_atom_iter, METH_VARARGS, ""},
        {"atomIterGetFirst", (PyCFunction) dap_chain_python_atom_iter_get_first, METH_VARARGS, ""},
        {"atomGetDatums", (PyCFunction) dap_chain_python_atom_get_datums, METH_VARARGS, ""},
        {"atomIterGetNext", (PyCFunction)dap_chain_python_atom_iter_get_next, METH_VARARGS, ""},
        {"getDag", (PyCFunction)dap_chain_python_atom_iter_get_dag, METH_NOARGS, ""},
        {"addMempoolNotify", (PyCFunction)dap_chain_python_add_mempool_notify_callback, METH_VARARGS, ""},
        {"addAtomNotify", (PyCFunction)dap_chain_net_add_atom_notify_callback, METH_VARARGS,"" },
        {"atomFindByHash", (PyCFunction)dap_chain_python_atom_find_by_hash, METH_VARARGS, ""},
        {"countAtom", (PyCFunction)dap_chain_python_get_atom_count, METH_NOARGS, ""},
        {"getAtoms", (PyCFunction)dap_chain_python_get_atoms, METH_VARARGS, ""},
        {"countTx", (PyCFunction)dap_chain_python_get_count_tx, METH_NOARGS, ""},
        {"getTransactions", (PyCFunction)dap_chain_python_get_txs, METH_VARARGS, ""},
        {"getCSName", (PyCFunction)dap_chain_python_get_cs_name, METH_NOARGS, ""},
        {}
};

PyTypeObject DapChainObjectType = {
        .ob_base = PyVarObject_HEAD_INIT(NULL,0)
        .tp_name = "CellFrame.Chain",
        .tp_basicsize = sizeof(PyDapChainObject),
        .tp_str = PyDapChain_str,
        .tp_dealloc = (destructor)PyDapChainObject_dealloc,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_methods = DapChainMethods,
        .tp_new = PyDapChainObject_new
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
    ((PyChainAtomObject*)obj_atom_ptr)->atom = ((PyDapChainObject*)self)->chain_t->callback_atom_iter_get_first(
            ((PyChainAtomIterObject*)obj_iter)->atom_iter, &l_atom_size
            );
    if (((PyChainAtomObject*)obj_atom_ptr)->atom == NULL){
        Py_RETURN_NONE;
    }
    return Py_BuildValue("On", obj_atom_ptr, l_atom_size);
}

/**
 * @brief dap_chain_python_atom_get_datums
 * @param self
 * @param args
 * @return
 */
PyObject *dap_chain_python_atom_get_datums(PyObject *self, PyObject *args){
    PyObject *l_obj_atom_py = NULL;
    if(!PyArg_ParseTuple(args, "O", &l_obj_atom_py)){
        PyErr_SetString(PyExc_AttributeError, "The second argument must be an integer");
        return NULL;
    }
    PyChainAtomObject *l_obj_atom = ((PyChainAtomObject*)l_obj_atom_py);
    PyDapChainObject* l_obj_chain = ((PyDapChainObject*)self);
    size_t datums_count = 0;
    dap_chain_datum_t **l_datums = l_obj_chain->chain_t->callback_atom_get_datums(
                l_obj_atom->atom, l_obj_atom->atom_size, &datums_count);

    PyObject *list_datums = PyList_New(datums_count);
    for (size_t i=0; i < datums_count; i++){
        PyDapChainDatumObject *l_obj_datum_py = PyObject_New(PyDapChainDatumObject, &DapChainDatumObjectType);
        l_obj_datum_py->datum = l_datums[i];
        l_obj_datum_py->origin = false;
        PyList_SetItem(list_datums, i, (PyObject*)l_obj_datum_py);
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
    ((PyChainAtomObject*)obj_atom_ptr)->atom = ((PyDapChainObject*)self)->chain_t->callback_atom_iter_get_next(
            ((PyChainAtomIterObject*)atom_iter)->atom_iter,
            &atom_size);
    if (((PyChainAtomObject*)obj_atom_ptr)->atom == NULL){
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

static void _wrapping_dap_chain_mempool_notify_handler(void * a_arg, const char a_op_code, const char * a_group,
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

/**
 * @brief _wrapping_dap_chain_atom_notify_handler
 * @param a_arg
 * @param a_chain
 * @param a_id
 * @param a_atom
 * @param a_atom_size
 */
static void _wrapping_dap_chain_atom_notify_handler(void * a_arg, dap_chain_t *a_chain, dap_chain_cell_id_t a_id, void* a_atom, size_t a_atom_size){
    if (!a_arg){
        return;
    }
    _wrapping_chain_mempool_notify_callback_t  *l_callback = (_wrapping_chain_mempool_notify_callback_t *)a_arg;

    PyObject *l_args;
    PyGILState_STATE state = PyGILState_Ensure();

    dap_chain_atom_ptr_t l_atom = (dap_chain_atom_ptr_t) a_atom;
    PyChainAtomObject *l_atom_obj = NULL;
    if(l_atom){
        l_atom_obj= PyObject_New(PyChainAtomObject, &DapChainAtomPtrObjectType);
        l_atom_obj->atom = l_atom;
        l_atom_obj->atom_size = a_atom_size;
        l_args = Py_BuildValue("OO", l_atom_obj, l_callback->arg);
    }else{
        l_args = Py_BuildValue("OO", Py_None, l_callback->arg);
    }

    log_it(L_DEBUG, "Call atom notifier for chain %s with atom size %zd", a_chain->name, a_atom_size );
    PyEval_CallObject(l_callback->func, l_args);
    Py_DECREF(l_args);
    PyGILState_Release(state);
}

/**
 * @brief dap_chain_python_add_mempool_notify_callback
 * @param self
 * @param args
 * @return
 */
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

/**
 * @brief dap_chain_net_add_atom_notify_py
 * @param self
 * @param args
 * @return
 */
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
    _wrapping_chain_mempool_notify_callback_t *l_callback = DAP_NEW_Z(_wrapping_chain_mempool_notify_callback_t);
    l_callback->func = obj_func;
    l_callback->arg = obj_arg;
    Py_INCREF(obj_func);
    Py_INCREF(obj_arg);
    dap_chain_add_callback_notify(l_chain, _wrapping_dap_chain_atom_notify_handler, l_callback);
    Py_RETURN_NONE;
}

/**
 * @brief dap_chain_python_atom_find_by_hash
 * @param self
 * @param args
 * @return
 */
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
    dap_chain_atom_ptr_t *l_ptr = ((PyDapChainObject*)self)->chain_t->callback_atom_find_by_hash(
            ((PyChainAtomIterObject*)obj_iter)->atom_iter,
            obj_hf->hash_fast,
            &l_size_atom);
    if (l_ptr == NULL) {
        return Py_BuildValue("On", Py_None, 0);
    } else {
        PyChainAtomObject *l_obj_ptr = PyObject_New(PyChainAtomObject, &DapChainAtomPtrObjectType);
        l_obj_ptr->atom = l_ptr;
        return Py_BuildValue("On", l_obj_ptr, l_size_atom);
    }
}

/**
 * @breif dap_chain_python_get_atom_count
 * @param self
 * @param args
 * @return
 */
PyObject *dap_chain_python_get_atom_count(PyObject *self, PyObject *args){
    (void)args;
    size_t l_count = ((PyDapChainObject*)self)->chain_t->callback_count_atom(((PyDapChainObject*)self)->chain_t);
    return Py_BuildValue("n", l_count);
}

/**
 * @breif dap_chain_python_get_atoms
 * @param self
 * @param args
 * @return
 */
PyObject *dap_chain_python_get_atoms(PyObject *self, PyObject *args) {
    size_t count, page;
    PyObject *obj_reverse;
    if (!PyArg_ParseTuple(args, "nnO", &count, &page, &obj_reverse)) {
        return NULL;
    }
    if (!PyBool_Check(obj_reverse)) {
        PyErr_SetString(PyExc_AttributeError, "");
        return NULL;
    }
    bool reverse = (obj_reverse == Py_True) ? true : false;
    dap_chain_t *l_chain = ((PyDapChainObject *) self)->chain_t;
    dap_list_t *l_atoms = l_chain->callback_get_atoms(l_chain, count, page, reverse);
    if (!l_atoms) {
        Py_RETURN_NONE;
    }
    PyObject *obj_list = PyList_New(0);
    for (dap_list_t *l_iter = l_atoms; l_iter != NULL; l_iter = l_iter->next) {
        PyChainAtomObject *obj_atom = PyObject_New(PyChainAtomObject, &DapChainAtomPtrObjectType);
        obj_atom->atom = l_iter->data;
        l_iter = l_iter->next;
        obj_atom->atom_size = *((size_t *) l_iter->data);
        PyObject *obj_ptr = Py_BuildValue("O", (PyObject *) obj_atom);
        PyList_Append(obj_list, obj_ptr);
    }
    return obj_list;
}

/**
 * @brief dap_chain_python_get_count_tx
 * @param self
 * @param args
 * @return
 */
PyObject *dap_chain_python_get_count_tx(PyObject *self, PyObject *args){
    (void)args;
    dap_chain_t *l_chain = ((PyDapChainObject*)self)->chain_t;
    size_t cnt = l_chain->callback_count_tx(l_chain);
    return Py_BuildValue("n", cnt);
}

/**
 * @brief dap_chain_python_get_txs
 * @param self
 * @param args
 * @return
 */
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
            PyObject_Dir((PyObject*)l_obj_tx);
            l_obj_tx->datum_tx = l_ptr->data;
            l_obj_tx->original = false;
            PyList_Append(l_obj_list, (PyObject *)l_obj_tx);
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
