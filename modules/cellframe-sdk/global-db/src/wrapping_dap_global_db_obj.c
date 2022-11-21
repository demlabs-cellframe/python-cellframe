#include "libdap-python.h"
#include "wrapping_dap_global_db_obj.h"

static PyGetSetDef DapChainGlobalDBContainerGetSet[] = {
        {"id", (getter)wrapping_dap_global_db_obj_get_id, NULL, NULL, NULL},
        {"key", (getter)wrapping_dap_global_db_obj_get_key, NULL, NULL, NULL},
        {"value", (getter)wrapping_dap_global_db_obj_get_value, NULL, NULL, NULL},
        {NULL}
};

PyTypeObject DapChainGlobalDBContainerObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.ChainGlobalDBContainer", sizeof(PyDapChainGlobalDBContainerObject),
        "Chain GlobalDB container object",
        .tp_getset = DapChainGlobalDBContainerGetSet);

PyObject *wrapping_dap_global_db_obj_get_id(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", ((PyDapChainGlobalDBContainerObject*)self)->obj.id);
}
PyObject *wrapping_dap_global_db_obj_get_key(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("s", ((PyDapChainGlobalDBContainerObject*)self)->obj.key);
}
PyObject *wrapping_dap_global_db_obj_get_value(PyObject *self, void *closure){
    (void)closure;
    size_t l_size_value = ((PyDapChainGlobalDBContainerObject*)self)->obj.value_len;
    if (l_size_value == 0 || !((PyDapChainGlobalDBContainerObject*)self)->obj.value){
        Py_RETURN_NONE;
    }
    PyObject *obj_bytes = PyBytes_FromStringAndSize((char *)((PyDapChainGlobalDBContainerObject*)self)->obj.value, l_size_value);
    return obj_bytes;
}

