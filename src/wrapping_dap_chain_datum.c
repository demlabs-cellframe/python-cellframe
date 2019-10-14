#include "wrapping_dap_chain_datum.h"

//void PyDapChainDatumObject_dealloc(PyDapChainDatumObject* object){
//}

PyObject *PyDapChainDatumObject_new(PyTypeObject *type_object, PyObject *args, PyObject *kwds){
    uint16_t type_id;
    PyBytesObject *bytes;
    size_t data_size;
    if (!PyArg_ParseTuple(args, "h|S|n", &type_id, &bytes, &data_size))
        return NULL;
    PyDapChainDatumObject *obj = (PyDapChainDatumObject*)PyType_GenericNew(type_object, args, kwds);
    obj->datum = dap_chain_datum_create(type_id, bytes, data_size);
    return (PyObject *)obj;
}
