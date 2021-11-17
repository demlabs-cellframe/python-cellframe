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
    return PyDateTime_FromTimestamp(((PyDapChainDatumObject*)self)->datum->header.ts_create);
}
