#pragma once

#include "Python.h"
#include "dap_events_socket.h"
#include "dap_events_python.h"
#include "dap_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyDapEventsSocket{
    PyObject_HEAD
    dap_events_socket_t *t_events_socket;
}PyDapEventsSocketObject;

PyObject *dap_events_socket_create_after_py(PyDapEventsSocketObject *self);//dap_events_socket_t * a_es);

//PyObject *dap_events_socket_wrap_no_add_py(PyObject *self, PyObject *args);//struct dap_events * a_events,
                                           // int s, dap_events_socket_callbacks_t * a_callbacks); // Create new client and add it to the list


PyObject *dap_events_socket_find_py(PyDapEventsSocketObject *self, PyObject *args);//int sock, struct dap_events * sh); // Find client by socket
//PyObject *dap_events_socket_is_ready_to_read_py(PyDapEventsSocketObject *self);//dap_events_socket_t * sc);
//PyObject *dap_events_socket_is_ready_to_write_py(PyDapEventsSocketObject *self);//dap_events_socket_t * sc);
PyObject *dap_events_socket_set_readable_py(PyDapEventsSocketObject *self, PyObject *args);//dap_events_socket_t * sc,bool is_ready);
PyObject *dap_events_socket_set_writable_py(PyDapEventsSocketObject *self, PyObject *args);//dap_events_socket_t * sc,bool is_ready);

PyObject *dap_events_socket_write_py(PyDapEventsSocketObject *self, PyObject *args);//dap_events_socket_t *sc, const void * data, size_t data_size);
PyObject *dap_events_socket_write_f_py(PyDapEventsSocketObject *self, PyObject *args);//dap_events_socket_t *sc, const char * format,...);
PyObject *dap_events_socket_read_py(PyDapEventsSocketObject *self, PyObject *args);//dap_events_socket_t *sc, void * data, size_t data_size);

PyObject *dap_events_socket_delete_py(PyDapEventsSocketObject *self, PyObject *args);//dap_events_socket_t *sc,bool preserve_inheritor); // Removes the client from the list

PyObject *dap_events_socket_shrink_buf_in_py(PyDapEventsSocketObject *self, PyObject *args);//dap_events_socket_t * cl, size_t shrink_size);

static PyMethodDef PyDapEventsObjectSocket_methods[] = {
    {"createAfter", (PyCFunction)dap_events_socket_create_after_py, METH_NOARGS, ""},
    {"find", (PyCFunction)dap_events_socket_find_py, METH_VARARGS, ""},
//    {"isReadyToRead", (PyCFunction)dap_events_socket_is_ready_to_read_py, METH_NOARGS, ""},
//    {"isReadyToWrite", (PyCFunction)dap_events_socket_is_ready_to_write_py, METH_NOARGS, ""},
    {"setReadable", (PyCFunction)dap_events_socket_set_readable_py, METH_VARARGS, ""},
    {"setWritable", (PyCFunction)dap_events_socket_set_writable_py, METH_VARARGS, ""},
    {"write", (PyCFunction)dap_events_socket_write_py, METH_VARARGS, ""},
    {"writeF", (PyCFunction)dap_events_socket_write_f_py, METH_VARARGS, ""},
    {"read", (PyCFunction)dap_events_socket_read_py, METH_VARARGS, ""},
    {"delete", (PyCFunction)dap_events_socket_delete_py, METH_VARARGS, ""},
    {"shringBufIn", (PyCFunction)dap_events_socket_shrink_buf_in_py, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}  /* Sentinel */
};

static PyTypeObject dapEventsSocket_dapEventsSocketType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libCellFrame.ServerCore.DapEventsSocket",             /* tp_name */
    sizeof(PyDapEventsSocketObject),             /* tp_basicsize */
    0,                         /* tp_itemsize */
    0,//(destructor)Noddy_dealloc, /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,   /* tp_flags */
    "DapEventsSocket objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    PyDapEventsObjectSocket_methods,             /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,//(initproc)PyDapEventsObject_init,//(initproc)Noddy_init,      /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,//PyDapEventsObject_new,//Noddy_new,                 /* tp_new */
};

#ifdef __cplusplus
}
#endif
