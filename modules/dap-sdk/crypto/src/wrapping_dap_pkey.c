#include "wrapping_dap_pkey.h"

PyObject *PyDapPkeyType_str(PyObject *self){
    const char *str = dap_pkey_type_to_str(*((PyDapPkeyTypeObject*)self)->pkey_type);
    return Py_BuildValue("s", str);
}

PyObject *wrapping_dap_pkey_get_type(PyObject *self, void *closure){
    (void)closure;
    PyDapPkeyTypeObject *obj_type_pkey = PyObject_New(PyDapPkeyTypeObject, &DapPkeyTypeObject_DapPkeyTypeObjectType);
    PyObject_Dir((PyObject*)obj_type_pkey);
    obj_type_pkey->pkey_type = &((PyDapPkeyObject*)self)->pkey->header.type;
    return (PyObject*)obj_type_pkey;
}
PyObject *wrapping_dap_pkey_get_hash(PyObject *self, void *closure){
    (void)closure;
    PyDapHashFastObject *obj_hash = PyObject_New(PyDapHashFastObject, &DapHashFastObject_DapHashFastObjectType);
    PyObject_Dir((PyObject*)obj_hash);
    dap_hash_fast(((PyDapPkeyObject*)self)->pkey->pkey, ((PyDapPkeyObject*)self)->pkey->header.size, obj_hash->hash_fast);
    return (PyObject*)obj_hash;
}
PyObject *wrapping_dap_pkey_get_size(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("I", ((PyDapPkeyObject*)self)->pkey->header.size);
}
