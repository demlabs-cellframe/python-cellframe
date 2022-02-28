#include "libdap-chain-python.h"

#define LOG_TAG "libdap-chain-python"


int init_chain_py(){
    return  dap_chain_init();
}

void deinit_chain_py(){
    dap_chain_deinit();
}

PyObject *dap_chain_find_by_id_py(PyObject *self, PyObject *args){
    PyObject *obj_net_id;
    PyObject *obj_chain_id;
    if (!PyArg_ParseTuple(args, "O|O", &obj_net_id, &obj_chain_id))
        return NULL;
    PyObject *new_obj = _PyObject_New(&dapChainObject_dapChainType);
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
        return  Py_BuildValue("O", Py_True);
    else
        return Py_BuildValue("O", Py_False);
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
    PyObject *res_obj = _PyObject_New(&dapChainObject_dapChainType);
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
    dap_chain_delete(chain->chain_t);
    Py_TYPE(chain)->tp_free((PyObject*)chain);
}

PyObject *dap_chain_python_create_atom_iter(PyObject *self, PyObject *args){
    (void)args;
    PyObject *obj_atom_iter = _PyObject_New(&DapChainAtomIter_DapChainAtomIterType);
    PyObject_Init(obj_atom_iter, &DapChainAtomIter_DapChainAtomIterType);
    PyObject_Dir(obj_atom_iter);
    ((PyChainAtomIterObject*)obj_atom_iter)->atom_iter =
            ((PyDapChainObject*)self)->chain_t->callback_atom_iter_create(((PyDapChainObject*)self)->chain_t);
    return obj_atom_iter;
}

PyObject *dap_chain_python_atom_iter_get_first(PyObject *self, PyObject *args){
    PyObject *obj_iter;
    if (!PyArg_ParseTuple(args, "O", &obj_iter)){
        PyErr_SetString(PyExc_AttributeError, "function takes exactly one argument");
        return NULL;
    }
    if (!PyDapChainAtomIter_Check(obj_iter)){
        PyErr_SetString(PyExc_TypeError, "argument must be ChainAtomIter object");
        return NULL;
    }
    PyObject *obj_atom_ptr = _PyObject_New(&DapChainAtomPtr_DapChainAtomPtrType);
    obj_atom_ptr = PyObject_Init(obj_atom_ptr, &DapChainAtomPtr_DapChainAtomPtrType);
    PyObject_Dir(obj_atom_ptr);
    size_t l_atom_size = 0;
    ((PyChainAtomPtrObject*)obj_atom_ptr)->ptr = ((PyDapChainObject*)self)->chain_t->callback_atom_iter_get_first(
            ((PyChainAtomIterObject*)obj_iter)->atom_iter, &l_atom_size
            );
    if (((PyChainAtomPtrObject*)obj_atom_ptr)->ptr == NULL){
        return Py_None;
    }
    return Py_BuildValue("On", obj_atom_ptr, l_atom_size);
}

PyObject *dap_chain_python_atom_get_datums(PyObject *self, PyObject *args){
    PyObject *obj_atom = NULL;
    size_t atom_size = 0;
    if(!PyArg_ParseTuple(args, "On", &obj_atom, &atom_size)){
        PyErr_SetString(PyExc_AttributeError, "the second argument must be an integer");
        return NULL;
    }
    size_t datums_count = 0;
    dap_chain_datum_t **l_datums = ((PyDapChainObject*)self)->chain_t->callback_atom_get_datums(((PyChainAtomPtrObject*)obj_atom)->ptr, atom_size, &datums_count);
    PyObject *list_datums = PyList_New(datums_count);
    for (int i=0; i < datums_count; i++){
        PyObject *obj_datum = _PyObject_New(&DapChainDatumObject_DapChainDatumObjectType);
        obj_datum = PyObject_Init(obj_datum, &DapChainDatumObject_DapChainDatumObjectType);
        PyObject_Dir(obj_datum);
        ((PyDapChainDatumObject*)obj_datum)->datum = l_datums[i];
        PyList_SetItem(list_datums, i, obj_datum);
    }
    return list_datums;
}

PyObject *dap_chain_python_atom_iter_get_next(PyObject *self, PyObject *args){
    //
    size_t atom_size = 0;
    PyObject *atom_iter = NULL;
    if(!PyArg_ParseTuple(args, "O", &atom_iter)){
        PyErr_SetString(PyExc_AttributeError, "function takes exactly one argument");
        return NULL;
    }
    if (!PyDapChainAtomIter_Check(atom_iter)){
        PyErr_SetString(PyExc_AttributeError, "the first argument must be ChainAtomIter object.");
        return NULL;
    }
    PyObject *obj_atom_ptr = _PyObject_New(&DapChainAtomPtr_DapChainAtomPtrType);
    obj_atom_ptr = PyObject_Init(obj_atom_ptr, &DapChainAtomPtr_DapChainAtomPtrType);
    PyObject_Dir(obj_atom_ptr);
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
    PyDapChainCsDagObject *obj_dag = PyObject_New(PyDapChainCsDagObject, &DapChainCsDag_DapChainCsDagType);
    PyObject_Dir((PyObject*)obj_dag);
    obj_dag->dag = DAP_CHAIN_CS_DAG(((PyDapChainObject*)self)->chain_t);
    return (PyObject*)obj_dag;
}
