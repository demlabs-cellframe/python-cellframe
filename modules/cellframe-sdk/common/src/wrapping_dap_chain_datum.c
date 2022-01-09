#include "wrapping_dap_chain_datum.h"

//void PyDapChainDatumObject_dealloc(PyDapChainDatumObject* object){
//}

PyObject *PyDapChainDatumObject_new(PyTypeObject *type_object, PyObject *args, PyObject *kwds){
    uint16_t type_id;
    PyBytesObject *bytes;
    size_t data_size;
    if (!PyArg_ParseTuple(args, "H|S|n", &type_id, &bytes, &data_size))
        return NULL;
    PyDapChainDatumObject *obj = (PyDapChainDatumObject*)PyType_GenericNew(type_object, args, kwds);
    void* bytes_v = (void *)PyBytes_AsString((PyObject*)bytes);
    obj->datum = dap_chain_datum_create(type_id, bytes_v, data_size);
    return (PyObject *)obj;
}

PyObject *dap_chain_datum_size_py(PyObject *self, PyObject *args){
    (void)args;
    size_t size = dap_chain_datum_size(((PyDapChainDatumObject*)self)->datum);
    return PyLong_FromSize_t(size);
}

PyObject *dap_chain_datum_get_ts_created_py(PyObject *self, void* closure){
    (void)closure;
    PyDateTime_IMPORT;
    PyObject *obj_ts_long =  Py_BuildValue("(k)",((PyDapChainDatumObject*)self)->datum->header.ts_create);
    PyObject *obj_ts = PyDateTime_FromTimestamp(obj_ts_long);
    return obj_ts;
}

PyObject *dap_chain_datum_is_type_tx(PyObject *self, PyObject *args){
    (void)args;
    if (((PyDapChainDatumObject*)self)->datum->header.type_id == DAP_CHAIN_DATUM_TX){
        return Py_BuildValue("O", Py_True);
    } else {
        return Py_BuildValue("O", Py_False);
    }
}

PyObject *dap_chain_datum_is_type_token(PyObject *self, PyObject *args){
    (void)args;
    if (((PyDapChainDatumObject*)self)->datum->header.type_id == DAP_CHAIN_DATUM_TOKEN_DECL ){
        return Py_BuildValue("O", Py_True);
    } else {
        return Py_BuildValue("O", Py_False);
    }
}

PyObject *wrapping_dap_chain_datum_get_datum_token(PyObject *self, PyObject *args){
    (void)args;
    if (((PyDapChainDatumObject*)self)->datum->header.type_id == DAP_CHAIN_DATUM_TOKEN_DECL ){
        PyDapChainDatumTokenObject *obj_token = PyObject_New(PyDapChainDatumTokenObject,
                                                             &DapChainDatumToken_DapChainDatumTokenObjectType);
        PyObject_Dir((PyObject*)obj_token);
        size_t l_size_token = ((PyDapChainDatumObject*)self)->datum->header.data_size;
        obj_token->token = dap_chain_datum_token_read(((PyDapChainDatumObject*)self)->datum->data,
                                                      &l_size_token);
    }else{
        PyErr_SetString(PyExc_Exception, "Due to the type of this datum, it is not possible to get the token datum.");
        return NULL;
    }
}

PyObject *dap_chain_datum_is_type_emission(PyObject *self, PyObject *args){
    (void)args;
    if (((PyDapChainDatumObject*)self)->datum->header.type_id == DAP_CHAIN_DATUM_TOKEN_EMISSION){
        return Py_BuildValue("O", Py_True);
    }else{
        return Py_BuildValue("O", Py_False);
    }
}

PyObject *wrapping_dap_chain_datum_get_datum_token_emission(PyObject *self, PyObject *args){
    (void)args;
    if (((PyDapChainDatumObject*)self)->datum->header.type_id == DAP_CHAIN_DATUM_TOKEN_EMISSION ){
        PyDapChainDatumTokenEmissionObject *obj_emission = PyObject_New(
                PyDapChainDatumTokenEmissionObject,
                &DapChainDatumTokenEmission_DapChainDatumTokenEmissionObjectType
                );
        PyObject_Dir((PyObject*)obj_emission);
        size_t l_token_emission_size = 0;
        obj_emission->token_emission = dap_chain_datum_emission_read(((PyDapChainDatumObject*)self)->datum->data,
                                                                     &l_token_emission_size);
        return (PyObject*)obj_emission;

    }else{
        PyErr_SetString(PyExc_Exception, "Due to the type of this datum, it is not possible to get the token datum.");
        return NULL;
    }
}

PyObject *wrapping_dap_chain_datum_get_datum_tx(PyObject *self, PyObject *args){
    (void)args;
    if(((PyDapChainDatumObject *)self)->datum->header.type_id == DAP_CHAIN_DATUM_TX){
        PyObject *obj_datum_tx = _PyObject_New(&DapChainDatumTx_DapChainDatumTxObjectType);
        obj_datum_tx = PyObject_Init(obj_datum_tx, &DapChainDatumTx_DapChainDatumTxObjectType);
        PyObject_Dir(obj_datum_tx);
        ((PyDapChainDatumTxObject*)obj_datum_tx)->datum_tx = ((PyDapChainDatumObject*)self)->datum->data;
        return obj_datum_tx;
    }else{
        PyErr_SetString(PyExc_Exception, "Due to the type of this datum, it is not possible to get the transaction datum.");
        return NULL;
    }
}

PyObject *dap_chain_datum_get_type_str_py(PyObject *self, PyObject *args){
    (void)args;
    const char *l_ret;
    DAP_DATUM_TYPE_STR(((PyDapChainDatumObject*)self)->datum->header.type_id, l_ret);
    if (l_ret == NULL)
        return Py_None;
    return Py_BuildValue("s", l_ret);
}

PyObject *wrapping_dap_chain_datum_get_version_str_py(PyObject *self, void* closure){
    (void)closure;
    return Py_BuildValue("s", dap_strdup_printf("0x%02X",((PyDapChainDatumObject*)self)->datum->header.version_id));
}
