#include "wrapping_dap_chain_gdb.h"

int dap_chain_gdb_init_py(void){
    return dap_chain_gdb_init();
}

PyObject* dap_chain_gdb_new_py(PyObject* self, PyObject *args){
    PyObject *obj_chain;
    if (!PyArg_ParseTuple(args, "O", &obj_chain))
        return NULL;
    int res = dap_chain_gdb_new(((PyDapChainObject*)obj_chain)->chain_t, g_config);
    return PyLong_FromLong(res);
}

PyObject *dap_chain_gdb_delete_py(PyObject *self, PyObject *args){
    PyObject *obj_chain;
    if (!PyArg_ParseTuple(args, "O", &obj_chain))
        return NULL;
    dap_chain_gdb_delete(((PyDapChainObject*)obj_chain)->chain_t);
    return PyLong_FromLong(0);
}

PyObject *dap_chain_gdb_get_group_py(PyObject *self, PyObject *args){
    PyObject *obj_chain;
    if (!PyArg_ParseTuple(args, "O", &obj_chain))
        return NULL;
    const char *res = dap_chain_gdb_get_group(((PyDapChainObject*)obj_chain)->chain_t);
    return Py_BuildValue("s", res);
}
