#include "libdap_crypto_key_type_python.h"

PyMethodDef PyCryptoKeyTypeObjectMethods[] = {
        {"DAP_ENC_KEY_TYPE_IAES", (PyCFunction)get_ENC_KEY_TYPE_IAES, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_OAES", (PyCFunction)get_ENC_KEY_TYPE_OAES, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_RLWE_NEWHOPE", (PyCFunction)get_ENC_KEY_TYPE_RLWE_NEWHOPE, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_SIDH_CLN16", (PyCFunction)get_ENC_KEY_TYPE_SIDH_CLN16, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_DEFEO", (PyCFunction)get_ENC_KEY_TYPE_DEFEO, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_MSRLN", (PyCFunction)get_ENC_KEY_TYPE_MSRLN, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_MSRLN16", (PyCFunction)get_ENC_KEY_TYPE_MSRLN16, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_RLWE_BCNS15", (PyCFunction)get_ENC_KEY_TYPE_RLWE_BCNS15, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_LWE_FRODO", (PyCFunction)get_ENC_KEY_TYPE_LWE_FRODO, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_SIDH_IQC_REF", (PyCFunction)get_ENC_KEY_TYPE_SIDH_IQC_REF, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_CODE_MCBITS", (PyCFunction)get_ENC_KEY_TYPE_CODE_MCBITS, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_NTRU", (PyCFunction)get_ENC_KEY_TYPE_NTRU, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_MLWE_KYBER", (PyCFunction)get_ENC_KEY_TYPE_MLWE_KYBER, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_SIG_PICNIC", (PyCFunction)get_ENC_KEY_TYPE_SIG_PICNIC, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_SIG_BLISS", (PyCFunction)get_ENC_KEY_TYPE_SIG_BLISS, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_SIG_TESLA", (PyCFunction)get_ENC_KEY_TYPE_SIG_TESLA, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_SIG_DILITHIUM", (PyCFunction)get_ENC_KEY_TYPE_SIG_DILITHIUM, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_NULL", (PyCFunction)get_ENC_KEY_TYPE_NULL, METH_NOARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject CryptoKeyTypeObject_CryptoKeyTypeObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.CryptoKeyType",           /* tp_name */
        sizeof(PyCryptoKeyTypeObject),       /* tp_basicsize */
        0,		                         /* tp_itemsize */
        0,		   			 /* tp_dealloc */
        0,            		         /* tp_print */
        0,           		         /* tp_getattr */
        0,                         		 /* tp_setattr */
        0,                         		 /* tp_reserved */
        0,                         		 /* tp_repr */
        0,                                   /* tp_as_number */
        0,                                   /* tp_as_sequence */
        0,                                   /* tp_as_mapping */
        0,                                   /* tp_hash  */
        0,                                   /* tp_call */
        PyCryptoKeyType_str,                 /* tp_str */
        0,                                   /* tp_getattro */
        0,                                   /* tp_setattro */
        0,                                   /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,             /* tp_flags */
        "Crypto keys type object",          /* tp_doc */
        0,                                   /* tp_traverse */
        0,		                         /* tp_clear */
        0,		               		 /* tp_richcompare */
        0,                                   /* tp_weaklistoffset */
        0,		                         /* tp_iter */
        0,		                         /* tp_iternext */
        PyCryptoKeyTypeObjectMethods,        /* tp_methods */
        0,                                   /* tp_members */
        0,                                   /* tp_getset */
        0,                                   /* tp_base */
        0,                                   /* tp_dict */
        0,                                   /* tp_descr_get */
        0,                                   /* tp_descr_set */
        0,                                   /* tp_dictoffset */
        (initproc)dap_crypto_key_type_py_create, /* tp_init */
        0,                                   /* tp_alloc */
        PyType_GenericNew,                   /* tp_new */
};

PyObject *PyCryptoKeyType_str(PyObject *self){
    char *str;
    switch (((PyCryptoKeyTypeObject*)self)->type) {
        case DAP_ENC_KEY_TYPE_SIG_DILITHIUM:
            str = "DAP_ENC_KEY_TYPE_SIG_DILITHIUM";
            break;
        case DAP_ENC_KEY_TYPE_SIG_TESLA:
            str = "DAP_ENC_KEY_TYPE_SIG_TESLA";
            break;
        case DAP_ENC_KEY_TYPE_SIG_BLISS:
            str = "DAP_ENC_KEY_TYPE_SIG_BLISS";
            break;
        case DAP_ENC_KEY_TYPE_SIG_PICNIC:
            str = "DAP_ENC_KEY_TYPE_SIG_PICNIC";
            break;
        case DAP_ENC_KEY_TYPE_MLWE_KYBER:
            str = "DAP_ENC_KEY_TYPE_MLWE_KYBER";
            break;
        case DAP_ENC_KEY_TYPE_NTRU:
            str = "DAP_ENC_KEY_TYPE_NTRU";
            break;
        case DAP_ENC_KEY_TYPE_CODE_MCBITS:
            str = "DAP_ENC_KEY_TYPE_CODE_MCBITS";
            break;
        case DAP_ENC_KEY_TYPE_SIDH_IQC_REF:
            str = "DAP_ENC_KEY_TYPE_SIDH_IQC_REF";
            break;
        case  DAP_ENC_KEY_TYPE_LWE_FRODO:
            str = "DAP_ENC_KEY_TYPE_LWE_FRODO";
            break;
        case DAP_ENC_KEY_TYPE_RLWE_BCNS15:
            str = "DAP_ENC_KEY_TYPE_RLWE_BCNS15";
            break;
        case DAP_ENC_KEY_TYPE_RLWE_MSRLN16:
            str = "DAP_ENC_KEY_TYPE_RLWE_MSRLN16";
            break;
        case DAP_ENC_KEY_TYPE_MSRLN:
            str = "DAP_ENC_KEY_TYPE_MSRLN";
            break;
        case DAP_ENC_KEY_TYPE_DEFEO:
            str = "DAP_ENC_KEY_TYPE_DEFEO";
            break;
        case DAP_ENC_KEY_TYPE_SIDH_CLN16:
            str = "DAP_ENC_KEY_TYPE_SIDH_CLN16";
            break;
        case DAP_ENC_KEY_TYPE_RLWE_NEWHOPE_CPA_KEM:
            str = "DAP_ENC_KEY_TYPE_RLWE_NEWHOPE_CPA_KEM";
            break;
        case DAP_ENC_KEY_TYPE_OAES:
            str = "DAP_ENC_KEY_TYPE_OAES";
            break;
        case DAP_ENC_KEY_TYPE_IAES:
            str = "DAP_ENC_KEY_TYPE_IAES";
            break;
        case DAP_ENC_KEY_TYPE_SIG_RINGCT20:
            str = "DAP_ENC_KEY_TYPE_SIG_RINGCT20";
            break;
        case DAP_ENC_KEY_TYPE_SEED_OFB:
            str = "DAP_ENC_KEY_TYPE_SEED_OFB";
            break;
        case DAP_ENC_KEY_TYPE_SALSA2012:
            str = "DAP_ENC_KEY_TYPE_SALSA2012";
            break;
        case DAP_ENC_KEY_TYPE_KUZN_OFB:
            str = "DAP_ENC_KEY_TYPE_KUZN_OFB";
            break;
        case DAP_ENC_KEY_TYPE_GOST_OFB:
            str = "DAP_ENC_KEY_TYPE_GOST_OFB";
            break;
        case DAP_ENC_KEY_TYPE_BF_OFB:
            str = "DAP_ENC_KEY_TYPE_BF_OFB";
            break;
        case DAP_ENC_KEY_TYPE_BF_CBC:
            str = "DAP_ENC_KEY_TYPE_BF_CBC";
            break;
        case DAP_ENC_KEY_TYPE_INVALID:
            str = "DAP_ENC_KEY_TYPE_INVALID";
            break;
    }
    return Py_BuildValue("s", str);
}

int dap_crypto_key_type_py_create(PyCryptoKeyTypeObject *self, PyObject *args, PyObject *kwds){
    const char *kwlist[] = {
            "type",
            NULL
    };
    PyObject *obj_type = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", (char**)kwlist, &obj_type)){
        return -1;
    }
    if (PyUnicode_Check(obj_type)) {
        const char *l_str = PyUnicode_AsUTF8(obj_type);
        if (dap_strcmp(l_str, "IAES") == 0) {
            self->type = DAP_ENC_KEY_TYPE_IAES;
        } else if (dap_strcmp(l_str, "OAES") == 0) {
            self->type = DAP_ENC_KEY_TYPE_OAES;
        } else if (dap_strcmp(l_str, "BF_CBC") == 0) {
            self->type = DAP_ENC_KEY_TYPE_BF_CBC;
        } else if (dap_strcmp(l_str, "BF_OFB") == 0) {
            self->type = DAP_ENC_KEY_TYPE_BF_OFB;
        } else if (dap_strcmp(l_str, "GOST_OFB") == 0) {
            self->type = DAP_ENC_KEY_TYPE_GOST_OFB;
        } else if (dap_strcmp(l_str, "KUZN_OFB") == 0) {
            self->type = DAP_ENC_KEY_TYPE_KUZN_OFB;
        } else if (dap_strcmp(l_str, "SALSA2012") == 0) {
            self->type = DAP_ENC_KEY_TYPE_SALSA2012;
        } else if (dap_strcmp(l_str, "SEED_OFB") == 0) {
            self->type = DAP_ENC_KEY_TYPE_SEED_OFB;
        } else if (dap_strcmp(l_str, "RLWE_NEWHOPE_CPA_KEM") == 0) {
            self->type = DAP_ENC_KEY_TYPE_RLWE_NEWHOPE_CPA_KEM;
        } else if (dap_strcmp(l_str, "SIDH_CLN16") == 0) {
            self->type = DAP_ENC_KEY_TYPE_SIDH_CLN16;
        } else if (dap_strcmp(l_str, "DEFEO") == 0) {
            self->type = DAP_ENC_KEY_TYPE_DEFEO;
        } else if (dap_strcmp(l_str, "MSRLN") == 0) {
            self->type = DAP_ENC_KEY_TYPE_MSRLN;
        } else if (dap_strcmp(l_str, "RLWE_MSRLN16") == 0) {
            self->type = DAP_ENC_KEY_TYPE_RLWE_MSRLN16;
        } else if (dap_strcmp(l_str, "RLWE_BCNS15") == 0) {
            self->type = DAP_ENC_KEY_TYPE_RLWE_BCNS15;
        } else if (dap_strcmp(l_str, "LWE_FRODO") == 0) {
            self->type = DAP_ENC_KEY_TYPE_LWE_FRODO;
        } else if (dap_strcmp(l_str, "SIDH_IQC_REF") == 0) {
            self->type = DAP_ENC_KEY_TYPE_SIDH_IQC_REF;
        } else if (dap_strcmp(l_str, "CODE_MCBITS") == 0) {
            self->type = DAP_ENC_KEY_TYPE_CODE_MCBITS;
        } else if (dap_strcmp(l_str, "NTRU") == 0) {
            self->type = DAP_ENC_KEY_TYPE_NTRU;
        } else if (dap_strcmp(l_str, "MLWE_KYBER") == 0) {
            self->type = DAP_ENC_KEY_TYPE_MLWE_KYBER;
        } else if (dap_strcmp(l_str, "SIG_PICNIC") == 0) {
            self->type = DAP_ENC_KEY_TYPE_SIG_PICNIC;
        } else if (dap_strcmp(l_str, "SIG_BLISS") == 0) {
            self->type = DAP_ENC_KEY_TYPE_SIG_BLISS;
        } else if (dap_strcmp(l_str, "SIG_TESLA") == 0) {
            self->type = DAP_ENC_KEY_TYPE_SIG_TESLA;
        } else if (dap_strcmp(l_str, "SIG_DILITHIUM") == 0) {
            self->type = DAP_ENC_KEY_TYPE_SIG_DILITHIUM;
        } else if (dap_strcmp(l_str, "SIG_RINGCT20") == 0) {
            self->type = DAP_ENC_KEY_TYPE_SIG_RINGCT20;
        } else if (dap_strcmp(l_str, "LAST") == 0) {
            self->type = DAP_ENC_KEY_TYPE_LAST;
        } else {
            return -1;
        }
        return 0;
    }
    return -1;
}

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
