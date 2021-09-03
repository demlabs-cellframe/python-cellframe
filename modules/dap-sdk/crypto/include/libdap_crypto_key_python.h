//#ifndef LIBDAP_CRYPTO_KEY_PYTHON_H_
//#define LIBDAP_CRYPTO_KEY_PYTHON_H_

//#include "Python.h"
//#include "dap_enc_key.h"
//#ifdef __cplusplus
//extern "C" {
//#endif

//typedef struct PyCryptoKey{
//    PyObject_HEAD
//    dap_enc_key_t *key;
//}PyCryptoKeyObject;

//static void PyCryptoKeyObject_dealloc(PyCryptoKeyObject *cryptoObject){
//    dap_enc_key_delete(cryptoObject->key);
//    Py_TYPE(cryptoObject)->tp_free((PyObject*)cryptoObject);
//}

//static PyTypeObject PyCryptoKeyObject_PyCryptoKeyType = {
//    PyVarObject_HEAD_INIT(NULL, 0)
//    "DAP.Crypto.Key",                            /* tp_name */
//    sizeof(PyCryptoKeyObject),                         /* tp_basicsize */
//    0,                                                 /* tp_itemsize */
//    (destructor)PyCryptoKeyObject_dealloc,             /* tp_dealloc */
//    0,                                                 /* tp_print */
//    0,                                                 /* tp_getattr */
//    0,                                                 /* tp_setattr */
//    0,                                                 /* tp_compare */
//    0,                                                 /* tp_repr */
//    0,                                                 /* tp_as_number */
//    0,                                                 /* tp_as_sequence */
//    0,                                                 /* tp_as_mapping */
//    0,                                                 /* tp_hash */
//    0,                                                 /* tp_call */
//    0,                                                 /* tp_str */
//    0,                                                 /* tp_getattro */
//    0,                                                 /* tp_setattro */
//    0,                                                 /* tp_as_buffer */
//    Py_TPFLAGS_DEFAULT,                                /* tp_flags */
//    "Crypto key objects",                              /* tp_doc */
//};

//#ifdef __cplusplus
//}
//#endif

//#endif //LIBDAP_CRYPTO_KEY_PYTHON_H_
