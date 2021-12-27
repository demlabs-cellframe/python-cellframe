#include "wrapping_dap_sign.h"

PyObject *PyDapSignType_to_str(PyObject *self){
    return Py_BuildValue("s", dap_sign_type_to_str(*((PyDapSignTypeObject*)self)->sign_type));
}

PyObject *wrapping_dap_sign_get_type(PyObject *self, void *closure){
    (void)closure;
    PyDapSignTypeObject *obj_type_sign = PyObject_New(PyDapSignTypeObject, &DapSignTypeObject_DapSignTypeObjectType);
    PyObject_Dir((PyObject*)obj_type_sign);
    obj_type_sign->sign_type = &((PyDapSignObject*)self)->sign->header.type;
    return (PyObject*)obj_type_sign;
}
PyObject *wrapping_dap_sign_get_pkey(PyObject *self, void *closure){
    (void)closure;
    PyDapPkeyObject *obj_pkey = PyObject_New(PyDapPkeyObject, &DapPkeyObject_DapPkeyObjectType);
    PyObject_Dir((PyObject*)obj_pkey);
    obj_pkey->pkey = (dap_pkey_t*)((PyDapSignObject*)self)->sign->pkey_n_sign;
    return (PyObject*)obj_pkey;
}
PyObject *wrapping_dap_sign_get_size(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("I", ((PyDapSignObject*)self)->sign->header.sign_size);
}