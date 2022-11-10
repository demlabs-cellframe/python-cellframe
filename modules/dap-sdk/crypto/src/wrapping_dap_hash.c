#include "libdap-python.h"
#include "wrapping_dap_hash.h"

/* Hash type */
static PyMethodDef DapHashTypeMethods[] = {
        {"DAP_HASH_TYPE_KECCAK", (PyCFunction)DAP_HASH_TYPE_KECCAK_PY, METH_NOARGS | METH_STATIC, ""},
        {"DAP_HASH_TYPE_SLOW_0", (PyCFunction)DAP_HASH_TYPE_SLOW_0_PY, METH_NOARGS | METH_STATIC, ""},
        {}
};

PyTypeObject DapHashTypeObject_DapChainHashTypeObjectType = DAP_PY_TYPE_OBJECT(
        "DAP.Crypto.HashType", sizeof(PyDapHashTypeObject),
        "Hash type object",
        .tp_methods = DapHashTypeMethods);

PyObject *DAP_HASH_TYPE_KECCAK_PY(){
    PyObject *obj = _PyObject_New(&DapHashTypeObject_DapChainHashTypeObjectType);
    ((PyDapHashTypeObject*)obj)->hash_type = DAP_HASH_TYPE_KECCAK;
    return Py_BuildValue("O", obj);
}
PyObject *DAP_HASH_TYPE_SLOW_0_PY(){
    PyObject *obj = _PyObject_New(&DapHashTypeObject_DapChainHashTypeObjectType);
    ((PyDapHashTypeObject*)obj)->hash_type = DAP_HASH_TYPE_SLOW_0;
    return Py_BuildValue("O", obj);
}

/* Chain hash fast */
static PyMethodDef DapHashFastMethods[] = {
        {"fromString", (PyCFunction)dap_chain_str_to_hash_fast_py, METH_VARARGS | METH_STATIC, ""},
        {"hashFast", (PyCFunction)dap_hash_fast_py, METH_VARARGS, ""},
        {"compare", (PyCFunction)dap_hash_fast_compare_py, METH_VARARGS | METH_STATIC, ""},
        {"isBlank", (PyCFunction)dap_hash_fast_is_blank_py, METH_VARARGS, ""},
        {"toStr", (PyCFunction)dap_chain_hash_fast_to_str_py, METH_VARARGS, ""},
        {"toStrNew", (PyCFunction)dap_chain_hash_fast_to_str_new_py, METH_VARARGS, ""},
//        {"__str__", (PyCFunction)wrapping_dap_hash_to_str, METH_VARARGS, ""},
        {}
};

PyTypeObject DapChainHashFastObjectType = DAP_PY_TYPE_OBJECT(
        "DAP.Crypto.HashFast", sizeof(PyDapHashFastObject),
        "Hash fast object",
        .tp_str = wrapping_dap_hash_to_str,
        .tp_richcompare = PyDapHashFast_compare,
        .tp_methods = DapHashFastMethods,
        .tp_init = PyDapHashFast_init);

PyObject* PyDapHashFast_compare(PyObject *self, PyObject *other, int op){
    if (!PyDapHashFast_Check((PyDapHashFastObject*)other)){
        PyErr_SetString(PyExc_NotImplementedError, "The comparison works for instances of the HashFast "
                                                   "object.");
        return NULL;
    }
    if (op == Py_EQ){
        bool res = dap_hash_fast_compare(
                ((PyDapHashFastObject *)self)->hash_fast,
                ((PyDapHashFastObject*)other)->hash_fast
                );
        if (res) {
            Py_RETURN_TRUE;
        } else {
            Py_RETURN_FALSE;
        }
    }
    if (op == Py_NE){
        bool res = dap_hash_fast_compare(
                ((PyDapHashFastObject*)self)->hash_fast,
                ((PyDapHashFastObject*)other)->hash_fast
                );
        if (!res){
            Py_RETURN_TRUE;
        } else {
            Py_RETURN_FALSE;
        }
    }
    PyErr_SetString(PyExc_NotImplementedError, "Two instances of an object of type HashFast can only be "
                                               "tested for equality and not equality relative to each other.");
    return NULL;
}

int PyDapHashFast_init(PyObject *self, PyObject *args, PyObject *kwds){
    const char *kwlist[] = {
            "data",
            NULL
    };
    PyObject *obj_data;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", (char**)kwlist, &obj_data)){
        return -1;
    }
    void *l_data = NULL;
    size_t l_data_size = 0;
    if (PyBytes_Check(obj_data)){
        l_data = PyBytes_AsString(obj_data);
        l_data_size = PyBytes_Size(obj_data);
    }
    if (DapChainDatumToken_Check(obj_data)){
        l_data = ((PyDapChainDatumTokenObject*)obj_data)->token;
        l_data_size = ((PyDapChainDatumTokenObject*)obj_data)->token_size;
    }
    if (PyDapChainDatumTokenEmissionObject_check(obj_data)){
        l_data = ((PyDapChainDatumTokenEmissionObject*)obj_data)->token_emission;
        l_data_size = ((PyDapChainDatumTokenEmissionObject*)obj_data)->token_size;
    }
    if (PyDapChainDatum_Check(obj_data)){
        l_data = ((PyDapChainDatumObject*)obj_data)->datum;
        l_data_size = dap_chain_datum_size(((PyDapChainDatumObject*)obj_data)->datum);
    }
    if (DapChainDatumTx_Check(obj_data)){
        l_data = ((PyDapChainDatumTxObject*)obj_data)->datum_tx;
        l_data_size = dap_chain_datum_tx_get_size(((PyDapChainDatumTxObject*)obj_data)->datum_tx);
    }
    if (!l_data || l_data_size == 0){
        PyErr_SetString(PyExc_AttributeError, "The attribute of this function was passed incorrectly, "
                                              "the function accepts the attribute Datum, Datum Token, "
                                              "DatumTokenEmission, DatumTx or byte.");
        return -1;
    }
    ((PyDapHashFastObject*)self)->hash_fast = DAP_NEW(dap_hash_fast_t);
    dap_hash_fast(l_data, l_data_size, ((PyDapHashFastObject*)self)->hash_fast);
    return 0;
}

PyObject *dap_chain_str_to_hash_fast_py(PyObject *self, PyObject *args){
    const char *hash_str;
    if (!PyArg_ParseTuple(args, "s", &hash_str))
        return NULL;
    PyDapHashFastObject *obj_hash_fast = self ? (PyDapHashFastObject *)self :
                                                 PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    obj_hash_fast->hash_fast = DAP_NEW(dap_hash_fast_t);
    if (dap_chain_hash_fast_from_str(hash_str, obj_hash_fast->hash_fast)) {
        DAP_DEL_Z(obj_hash_fast->hash_fast);
        Py_RETURN_NONE;
    }
    return (PyObject*)obj_hash_fast;
}

PyObject *dap_hash_fast_py(PyObject *self, PyObject *args){
    PyObject *obj_bytes;
    size_t data_in_size;
    if (!PyArg_ParseTuple(args, "O|n", &obj_bytes, &data_in_size))
        return NULL;
    const void *bytes = (void*)PyBytes_AsString(obj_bytes);
    bool res = dap_hash_fast(bytes, data_in_size, ((PyDapHashFastObject*)self)->hash_fast);
    if (res)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject *dap_hash_fast_compare_py(PyObject *self, PyObject *args){
    PyObject *hash1;
    PyObject *hash2;
    if (!PyArg_ParseTuple(args, "O|O", &hash1, &hash2))
        return NULL;
    bool res = dap_hash_fast_compare(((PyDapHashFastObject*)hash1)->hash_fast, ((PyDapHashFastObject*)hash2)->hash_fast);
    if (res)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject *dap_hash_fast_is_blank_py(PyObject *self, PyObject *args){
    bool res = dap_hash_fast_is_blank(((PyDapHashFastObject*)self)->hash_fast);
    if (res)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject *dap_chain_hash_fast_to_str_py(PyObject *self, PyObject *args){
    char *str;
    size_t str_max;
    if (!PyArg_ParseTuple(args, "s|n", &str, &str_max))
        return NULL;
    dap_chain_hash_fast_to_str(((PyDapHashFastObject*)self)->hash_fast, str, str_max);
    return Py_BuildValue("sn", &str, &str_max);
}

PyObject *dap_chain_hash_fast_to_str_new_py(PyObject *self, PyObject *args){
    char *res = dap_chain_hash_fast_to_str_new(((PyDapHashFastObject*)self)->hash_fast);
    return Py_BuildValue("s", res);
}

PyObject *wrapping_dap_hash_to_str(PyObject *self)
{
    char str[70];
    if (((PyDapHashFastObject*)self)->hash_fast == NULL){
        Py_BuildValue("s", "Hash is missing.");
    }
    dap_chain_hash_fast_to_str(((PyDapHashFastObject*)self)->hash_fast, str, 70);
    PyObject *obj_str = PyUnicode_FromString(str);
    return obj_str;
}
