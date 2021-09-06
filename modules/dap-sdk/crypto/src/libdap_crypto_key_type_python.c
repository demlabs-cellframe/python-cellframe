#include "libdap_crypto_key_type_python.h"

PyObject *get_ENC_KEY_TYPE_IAES(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyCryptoKeyTypeObject *new_obj = (PyCryptoKeyTypeObject*)_PyObject_New(&CryptoKeyTypeObjecy_CryptoKeyTypeObjecyType);
    new_obj->key_type = DAP_ENC_KEY_TYPE_IAES;
    return (PyObject*)new_obj;
}
PyObject *get_ENC_KEY_TYPE_OAES(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyCryptoKeyTypeObject *new_obj = (PyCryptoKeyTypeObject*)_PyObject_New(&CryptoKeyTypeObjecy_CryptoKeyTypeObjecyType);
    new_obj->key_type = DAP_ENC_KEY_TYPE_OAES;
    return (PyObject*)new_obj;
}
PyObject *get_ENC_KEY_TYPE_RLWE_NEWHOPE(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyCryptoKeyTypeObject *new_obj = (PyCryptoKeyTypeObject*)_PyObject_New(&CryptoKeyTypeObjecy_CryptoKeyTypeObjecyType);
    new_obj->key_type = DAP_ENC_KEY_TYPE_RLWE_NEWHOPE_CPA_KEM;
    return (PyObject*)new_obj;
}
PyObject *get_ENC_KEY_TYPE_SIDH_CLN16(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyCryptoKeyTypeObject *new_obj = (PyCryptoKeyTypeObject*)_PyObject_New(&CryptoKeyTypeObjecy_CryptoKeyTypeObjecyType);
    new_obj->key_type = DAP_ENC_KEY_TYPE_SIDH_CLN16;
    return (PyObject*)new_obj;
}
PyObject *get_ENC_KEY_TYPE_DEFEO(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyCryptoKeyTypeObject *new_obj = (PyCryptoKeyTypeObject*)_PyObject_New(&CryptoKeyTypeObjecy_CryptoKeyTypeObjecyType);
    new_obj->key_type = DAP_ENC_KEY_TYPE_DEFEO;
    return (PyObject*)new_obj;
}
PyObject *get_ENC_KEY_TYPE_MSRLN(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyCryptoKeyTypeObject *new_obj = (PyCryptoKeyTypeObject*)_PyObject_New(&CryptoKeyTypeObjecy_CryptoKeyTypeObjecyType);
    new_obj->key_type = DAP_ENC_KEY_TYPE_MSRLN;
    return (PyObject*)new_obj;
}
PyObject *get_ENC_KEY_TYPE_MSRLN16(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyCryptoKeyTypeObject *new_obj = (PyCryptoKeyTypeObject*)_PyObject_New(&CryptoKeyTypeObjecy_CryptoKeyTypeObjecyType);
    new_obj->key_type = DAP_ENC_KEY_TYPE_RLWE_MSRLN16;
    return (PyObject*)new_obj;
}
PyObject *get_ENC_KEY_TYPE_RLWE_BCNS15(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyCryptoKeyTypeObject *new_obj = (PyCryptoKeyTypeObject*)_PyObject_New(&CryptoKeyTypeObjecy_CryptoKeyTypeObjecyType);
    new_obj->key_type = DAP_ENC_KEY_TYPE_RLWE_BCNS15;
    return (PyObject*)new_obj;
}
PyObject *get_ENC_KEY_TYPE_LWE_FRODO(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyCryptoKeyTypeObject *new_obj = (PyCryptoKeyTypeObject*)_PyObject_New(&CryptoKeyTypeObjecy_CryptoKeyTypeObjecyType);
    new_obj->key_type = DAP_ENC_KEY_TYPE_LWE_FRODO;
    return (PyObject*)new_obj;
}
PyObject *get_ENC_KEY_TYPE_SIDH_IQC_REF(PyObject *self, PyObject *args){
    (void)args;(void)self;
    (void)args;
    PyCryptoKeyTypeObject *new_obj = (PyCryptoKeyTypeObject*)_PyObject_New(&CryptoKeyTypeObjecy_CryptoKeyTypeObjecyType);
    new_obj->key_type = DAP_ENC_KEY_TYPE_SIDH_IQC_REF;
    return (PyObject*)new_obj;
}
PyObject *get_ENC_KEY_TYPE_CODE_MCBITS(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyCryptoKeyTypeObject *new_obj = (PyCryptoKeyTypeObject*)_PyObject_New(&CryptoKeyTypeObjecy_CryptoKeyTypeObjecyType);
    new_obj->key_type = DAP_ENC_KEY_TYPE_CODE_MCBITS;
    return (PyObject*)new_obj;
}
PyObject *get_ENC_KEY_TYPE_NTRU(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyCryptoKeyTypeObject *new_obj = (PyCryptoKeyTypeObject*)_PyObject_New(&CryptoKeyTypeObjecy_CryptoKeyTypeObjecyType);
    new_obj->key_type = DAP_ENC_KEY_TYPE_NTRU;
    return (PyObject*)new_obj;
}
PyObject *get_ENC_KEY_TYPE_MLWE_KYBER(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyCryptoKeyTypeObject *new_obj = (PyCryptoKeyTypeObject*)_PyObject_New(&CryptoKeyTypeObjecy_CryptoKeyTypeObjecyType);
    new_obj->key_type = DAP_ENC_KEY_TYPE_MLWE_KYBER;
    return (PyObject*)new_obj;
}
PyObject *get_ENC_KEY_TYPE_SIG_PICNIC(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyCryptoKeyTypeObject *new_obj = (PyCryptoKeyTypeObject*)_PyObject_New(&CryptoKeyTypeObjecy_CryptoKeyTypeObjecyType);
    new_obj->key_type = DAP_ENC_KEY_TYPE_SIG_PICNIC;
    return (PyObject*)new_obj;
}
PyObject *get_ENC_KEY_TYPE_SIG_BLISS(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyCryptoKeyTypeObject *new_obj = (PyCryptoKeyTypeObject*)_PyObject_New(&CryptoKeyTypeObjecy_CryptoKeyTypeObjecyType);
    new_obj->key_type = DAP_ENC_KEY_TYPE_SIG_BLISS;
    return (PyObject*)new_obj;
}
PyObject *get_ENC_KEY_TYPE_SIG_TESLA(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyCryptoKeyTypeObject *new_obj = (PyCryptoKeyTypeObject*)_PyObject_New(&CryptoKeyTypeObjecy_CryptoKeyTypeObjecyType);
    new_obj->key_type = DAP_ENC_KEY_TYPE_SIG_TESLA;
    return (PyObject*)new_obj;
}
PyObject *get_ENC_KEY_TYPE_SIG_DILITHIUM(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyCryptoKeyTypeObject *new_obj = (PyCryptoKeyTypeObject*)_PyObject_New(&CryptoKeyTypeObjecy_CryptoKeyTypeObjecyType);
    new_obj->key_type = DAP_ENC_KEY_TYPE_SIG_DILITHIUM;
    return (PyObject*)new_obj;
}
PyObject *get_ENC_KEY_TYPE_NULL(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyCryptoKeyTypeObject *new_obj = (PyCryptoKeyTypeObject*)_PyObject_New(&CryptoKeyTypeObjecy_CryptoKeyTypeObjecyType);
    new_obj->key_type = DAP_ENC_KEY_TYPE_NULL;
    return (PyObject*)new_obj;
}
