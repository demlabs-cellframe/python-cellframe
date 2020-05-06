#include "libdap_crypto_key_type_python.h"

PyObject *get_ENC_KEY_TYPE_IAES(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_IAES);
}
PyObject *get_ENC_KEY_TYPE_OAES(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_OAES);
}
PyObject *get_ENC_KEY_TYPE_RLWE_NEWHOPE(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_RLWE_NEWHOPE_CPA_KEM );
}
PyObject *get_ENC_KEY_TYPE_SIDH_CLN16(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_SIDH_CLN16);
}
PyObject *get_ENC_KEY_TYPE_DEFEO(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_DEFEO);
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
PyObject *get_ENC_KEY_TYPE_SIDH_IQC_REF(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_SIDH_IQC_REF);
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
