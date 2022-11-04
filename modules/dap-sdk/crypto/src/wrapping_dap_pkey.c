#include "wrapping_dap_pkey.h"

PyTypeObject DapPkeyTypeObject_DapPkeyTypeObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.PkeyType", sizeof(PyDapPkeyTypeObject),
        "Pkey type object",
        .tp_str = PyDapPkeyType_str);

PyObject *PyDapPkeyType_str(PyObject *self){
    const char *str = dap_pkey_type_to_str(((PyDapPkeyTypeObject*)self)->pkey_type);
    return Py_BuildValue("s", str);
}

static PyGetSetDef PyDapPkeyGetsSetsDef[] = {
        {"hash", (getter)wrapping_dap_pkey_get_hash, NULL, NULL, NULL},
        {"type", (getter)wrapping_dap_pkey_get_type, NULL, NULL, NULL},
        {"size", (getter)wrapping_dap_pkey_get_size, NULL, NULL, NULL},
        {}
};

PyTypeObject DapPkeyObject_DapPkeyObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.Pkey", sizeof(PyDapPkeyObject),
        "Pkey object",
        .tp_getset = PyDapPkeyGetsSetsDef);

PyObject *wrapping_dap_pkey_get_type(PyObject *self, void *closure){
    (void)closure;
    PyDapPkeyTypeObject *obj_type_pkey = PyObject_New(PyDapPkeyTypeObject, &DapPkeyTypeObject_DapPkeyTypeObjectType);
    obj_type_pkey->pkey_type = ((PyDapPkeyObject*)self)->pkey->header.type;
    return (PyObject*)obj_type_pkey;
}
PyObject *wrapping_dap_pkey_get_hash(PyObject *self, void *closure){
    (void)closure;
    PyDapHashFastObject *obj_hash = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    dap_hash_fast(((PyDapPkeyObject*)self)->pkey->pkey, ((PyDapPkeyObject*)self)->pkey->header.size, obj_hash->hash_fast);
    return (PyObject*)obj_hash;
}
PyObject *wrapping_dap_pkey_get_size(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("I", ((PyDapPkeyObject*)self)->pkey->header.size);
}
