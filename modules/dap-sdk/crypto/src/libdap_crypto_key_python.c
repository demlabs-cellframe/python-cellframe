#include "libdap_crypto_key_python.h"

PyTypeObject PyCryptoKeyObject_PyCryptoKeyType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.Crypto.Key",                            /* tp_name */
        sizeof(PyCryptoKeyObject),                         /* tp_basicsize */
        0,                                                 /* tp_itemsize */
        (destructor)PyCryptoKeyObject_dealloc,             /* tp_dealloc */
        0,                                                 /* tp_print */
        0,                                                 /* tp_getattr */
        0,                                                 /* tp_setattr */
        0,                                                 /* tp_compare */
        0,                                                 /* tp_repr */
        0,                                                 /* tp_as_number */
        0,                                                 /* tp_as_sequence */
        0,                                                 /* tp_as_mapping */
        0,                                                 /* tp_hash */
        0,                                                 /* tp_call */
        0,                                                 /* tp_str */
        0,                                                 /* tp_getattro */
        0,                                                 /* tp_setattro */
        0,                                                 /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                /* tp_flags */
        "Crypto key objects",                              /* tp_doc */
};

bool PyCryptoKey_check(PyObject *obj_key){
    return PyObject_TypeCheck(obj_key, &PyCryptoKeyObject_PyCryptoKeyType);
}

void PyCryptoKeyObject_dealloc(PyCryptoKeyObject *cryptoObject){
    dap_enc_key_delete(cryptoObject->key);
    Py_TYPE(cryptoObject)->tp_free((PyObject*)cryptoObject);
}
