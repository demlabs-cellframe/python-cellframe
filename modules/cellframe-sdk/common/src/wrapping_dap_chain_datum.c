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
    size_t size = dap_chain_datum_size(((PyDapChainDatumObject*)self)->datum);
    return PyLong_FromSize_t(size);
}

PyObject *dap_chain_datum_get_ts_created(PyObject *self, void* closure){
    (void)closure;
    PyObject *timestamp = Py_BuildValue("k", ((PyDapChainDatumObject*)self)->datum->header.ts_create);
    return PyDateTime_FromTimestamp(timestamp);
}

PyObject *dap_chain_datum_is_type_tx(PyObject *self, PyObject *args){
    (void)args;
    if (((PyDapChainDatumObject*)self)->datum->header.type_id == DAP_CHAIN_DATUM_TX){
        return Py_BuildValue("O", Py_True);
    } else {
        return Py_BuildValue("O", Py_False);
    }
}

PyObject *wrapping_dap_chain_datum_get_datum_tx(PyObject *self, PyObject *args){
    (void)args;
    if(((PyDapChainDatumObject *)self)->datum->header.type_id == DAP_CHAIN_DATUM_TX){
        PyObject *obj_datum_tx = _PyObject_New(&DapChainDatumTx_DapChainDatumTxObjectType);
        obj_datum_tx = PyObject_Init(obj_datum_tx, &DapChainDatumTx_DapChainDatumTxObjectType);
        PyObject_Dir(obj_datum_tx);
        ((PyDapChainDatumTxObject*)obj_datum_tx)->datum_tx = (dap_chain_datum_tx_t *)((PyDapChainDatumObject*)self)->datum->data;
        return obj_datum_tx;
    }else{
        PyErr_SetString(PyExc_Exception, "Due to the type of this datum, it is not possible to get the transaction datum.");
        return NULL;
    }
}
