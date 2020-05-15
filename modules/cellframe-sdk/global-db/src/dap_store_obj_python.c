#include "dap_store_obj_python.h"

PyObject *dap_store_obj_copy_py(PyObject *self, PyObject *args){
    (void)self;
    PyObject *obj_story;
    size_t story_count;
    if (!PyArg_ParseTuple(args, "Oi", &obj_story, &story_count))
        return NULL;
    PyObject *copy_obj = _PyObject_New(&DapStoreObject_DapStoreType);
    ((PyDapStoreObj*)copy_obj)->store_obj = dap_store_obj_copy(((PyDapStoreObj*)copy_obj)->store_obj, story_count);
    return Py_BuildValue("(O)", copy_obj);
}
PyObject *dap_store_obj_free_py(PyObject *self, PyObject *args){
    size_t size_store;
    if (!PyArg_ParseTuple(args, "n", &size_store))
        return NULL;
    dap_store_obj_free(((PyDapStoreObj*)self)->store_obj, size_store);
    Py_TYPE(self)->tp_free(self);
    Py_XDECREF(self);
    Py_XINCREF(Py_None);
    return Py_None;
}

PyObject *dap_chain_global_db_driver_read_last_py(PyObject *self, PyObject *argv){
    (void)self;
    const char *group;
    if (!PyArg_ParseTuple(argv, "s", &group))
        return NULL;
    PyObject *store_obj = _PyObject_New(&DapStoreObject_DapStoreType);
    ((PyDapStoreObj*)store_obj)->store_obj = dap_chain_global_db_driver_read_last(group);
    return Py_BuildValue("(O)", store_obj);
}
//PyObject *dap_chain_global_db_driver_cond_read_py(PyObject *self, PyObject *argv){
//    (void)self;
//    const char *group;
//    uint64_t id;
//    size_t *a_count_out
//}
