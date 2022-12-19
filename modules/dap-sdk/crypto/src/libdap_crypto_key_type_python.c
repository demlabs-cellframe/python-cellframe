#include "libdap-python.h"
#include "libdap_crypto_key_type_python.h"

static PyMethodDef PyCryptoKeyTypeObjectMethods[] = {
        {"DAP_ENC_KEY_TYPE_IAES", (PyCFunction)get_ENC_KEY_TYPE_IAES, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_OAES", (PyCFunction)get_ENC_KEY_TYPE_OAES, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_RLWE_NEWHOPE", (PyCFunction)get_ENC_KEY_TYPE_RLWE_NEWHOPE, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_MSRLN", (PyCFunction)get_ENC_KEY_TYPE_MSRLN, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_MSRLN16", (PyCFunction)get_ENC_KEY_TYPE_MSRLN16, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_RLWE_BCNS15", (PyCFunction)get_ENC_KEY_TYPE_RLWE_BCNS15, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_LWE_FRODO", (PyCFunction)get_ENC_KEY_TYPE_LWE_FRODO, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_CODE_MCBITS", (PyCFunction)get_ENC_KEY_TYPE_CODE_MCBITS, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_NTRU", (PyCFunction)get_ENC_KEY_TYPE_NTRU, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_MLWE_KYBER", (PyCFunction)get_ENC_KEY_TYPE_MLWE_KYBER, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_SIG_PICNIC", (PyCFunction)get_ENC_KEY_TYPE_SIG_PICNIC, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_SIG_BLISS", (PyCFunction)get_ENC_KEY_TYPE_SIG_BLISS, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_SIG_TESLA", (PyCFunction)get_ENC_KEY_TYPE_SIG_TESLA, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_SIG_DILITHIUM", (PyCFunction)get_ENC_KEY_TYPE_SIG_DILITHIUM, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_NULL", (PyCFunction)get_ENC_KEY_TYPE_NULL, METH_NOARGS | METH_STATIC, ""},
        {}
};

PyTypeObject DapCryptoKeyTypeObjectType = DAP_PY_TYPE_OBJECT(
        "DAP.Crypto.KeyType", sizeof(PyCryptoKeyTypeObject),
        "Crypto keys type objects",
        .tp_methods = PyCryptoKeyTypeObjectMethods);

PyObject *get_ENC_KEY_TYPE_IAES(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_IAES);
}
PyObject *get_ENC_KEY_TYPE_OAES(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_OAES);
}
PyObject *get_ENC_KEY_TYPE_RLWE_NEWHOPE(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_RLWE_NEWHOPE_CPA_KEM );
}
PyObject *get_ENC_KEY_TYPE_MSRLN(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_MSRLN);
}
PyObject *get_ENC_KEY_TYPE_MSRLN16(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_RLWE_MSRLN16);
}
PyObject *get_ENC_KEY_TYPE_RLWE_BCNS15(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_RLWE_BCNS15);
}
PyObject *get_ENC_KEY_TYPE_LWE_FRODO(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_LWE_FRODO);
}
PyObject *get_ENC_KEY_TYPE_CODE_MCBITS(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_CODE_MCBITS);
}
PyObject *get_ENC_KEY_TYPE_NTRU(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_NTRU);
}
PyObject *get_ENC_KEY_TYPE_MLWE_KYBER(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_MLWE_KYBER);
}
PyObject *get_ENC_KEY_TYPE_SIG_PICNIC(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_SIG_PICNIC);
}
PyObject *get_ENC_KEY_TYPE_SIG_BLISS(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_SIG_BLISS);
}
PyObject *get_ENC_KEY_TYPE_SIG_TESLA(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_SIG_TESLA);
}
PyObject *get_ENC_KEY_TYPE_SIG_DILITHIUM(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_SIG_DILITHIUM);
}
PyObject *get_ENC_KEY_TYPE_NULL(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_NULL);
}
