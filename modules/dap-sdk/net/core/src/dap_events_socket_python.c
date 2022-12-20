#include "dap_events_socket_python.h"

#define LOG_TAG "dap_events_socket_python"

PyMethodDef PyDapEventsObjectSocket_methods[] = {
        {"createAfter", (PyCFunction)dap_events_socket_create_after_py, METH_NOARGS, ""},
        {"setReadable", (PyCFunction)dap_events_socket_set_readable_py, METH_VARARGS, ""},
        {"setWritable", (PyCFunction)dap_events_socket_set_writable_py, METH_VARARGS, ""},
        {"write", (PyCFunction)dap_events_socket_write_py, METH_VARARGS, ""},
        {"writeF", (PyCFunction)dap_events_socket_write_f_py, METH_VARARGS, ""},
        {"read", (PyCFunction)dap_events_socket_read_py, METH_VARARGS, ""},
        {"delete", (PyCFunction)dap_events_socket_delete_py, METH_VARARGS, ""},
        {"shringBufIn", (PyCFunction)dap_events_socket_shrink_buf_in_py, METH_VARARGS, ""},
        {NULL, NULL, 0, NULL}  /* Sentinel */
};

PyTypeObject DapEventsSocketObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "DAP.Net.EventsSocket",             /* tp_name */
        sizeof(PyDapEventsSocketObject),                   /* tp_basicsize */
        0,                                                 /* tp_itemsize */
        0,                                                 /* tp_dealloc */
        0,                                                 /* tp_print */
        0,                                                 /* tp_getattr */
        0,                                                 /* tp_setattr */
        0,                                                 /* tp_reserved */
        0,                                                 /* tp_repr */
        0,                                                 /* tp_as_number */
        0,                                                 /* tp_as_sequence */
        0,                                                 /* tp_as_mapping */
        0,                                                 /* tp_hash  */
        0,                                                 /* tp_call */
        0,                                                 /* tp_str */
        0,                                                 /* tp_getattro */
        0,                                                 /* tp_setattro */
        0,                                                 /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                           /* tp_flags */
        "DapEventsSocket objects",                         /* tp_doc */
        0,		                                       /* tp_traverse */
        0,                        		               /* tp_clear */
        0,		                                       /* tp_richcompare */
        0,                        		               /* tp_weaklistoffset */
        0,		                                       /* tp_iter */
        0,                        		               /* tp_iternext */
        PyDapEventsObjectSocket_methods,                   /* tp_methods */
        0,                                                 /* tp_members */
        0,                                                 /* tp_getset */
        0,                                                 /* tp_base */
        0,                                                 /* tp_dict */
        0,                                                 /* tp_descr_get */
        0,                                                 /* tp_descr_set */
        0,                                                 /* tp_dictoffset */
        0,                                                 /* tp_init */
        0,                                                 /* tp_alloc */
        PyType_GenericNew,                                 /* tp_new */
};

PyObject *dap_events_socket_create_after_py(PyDapEventsSocketObject *self, PyObject *args){
    if (self->t_events_socket == NULL){
        return NULL;
    }
    dap_worker_add_events_socket_auto(self->t_events_socket);
    return  PyLong_FromLong(0);
}


PyObject *dap_events_socket_set_readable_py(PyDapEventsSocketObject *self, PyObject *args){
    if (self->t_events_socket == NULL){
        return NULL;
    }
    PyObject *l_is_ready;
    if (!PyArg_ParseTuple(args, "O",  &l_is_ready)){
        return NULL;
    }
    bool is_ready = true;
    if (l_is_ready == Py_False)
        is_ready = false;
    dap_events_socket_set_readable_unsafe(self->t_events_socket, is_ready);
    return  PyLong_FromLong(0);
}
PyObject *dap_events_socket_set_writable_py(PyDapEventsSocketObject *self, PyObject *args){
    if (self->t_events_socket == NULL){
        return NULL;
    }
    PyObject *l_is_ready;
    if (!PyArg_ParseTuple(args, "O", &l_is_ready)){
        return NULL;
    }
    bool is_ready = true;
    if (l_is_ready == Py_False)
        is_ready = false;
    dap_events_socket_set_writable_unsafe(self->t_events_socket, is_ready);
    return  PyLong_FromLong(0);
}

PyObject *dap_events_socket_write_py(PyDapEventsSocketObject *self, PyObject *args){
    if (self->t_events_socket == NULL){
        return NULL;
    }
    PyBytesObject *in_bytes;
    size_t in_size;
    if (!PyArg_ParseTuple(args, "S|n", &in_bytes, &in_size)){
        return NULL;
    }
    char *in_v_bytes = PyBytes_AsString((PyObject*)in_bytes);
    size_t res_size = dap_events_socket_write_unsafe(self->t_events_socket,
                                              in_v_bytes, in_size);
    return Py_BuildValue("n", res_size);
}
PyObject *dap_events_socket_write_f_py(PyDapEventsSocketObject *self, PyObject *args){
    if (self->t_events_socket == NULL){
        return NULL;
    }
    const char *format;
    PyTupleObject *pto;
    if (!PyArg_ParseTuple(args, "s|O", &format, &pto)){
        return  NULL;
    }
    va_list ap;
    if (!PyArg_VaParse(args, format, ap)){
        return NULL;
    }
    size_t res_size = dap_events_socket_write_f_unsafe(self->t_events_socket, format, ap);
    return Py_BuildValue("n", res_size);
}
PyObject *dap_events_socket_read_py(PyDapEventsSocketObject *self, PyObject *args){
    if (self->t_events_socket == NULL){
        return NULL;
    }
    size_t in_size;
    if (!PyArg_ParseTuple(args, "n", &in_size)){
        return NULL;
    }
    void *res = DAP_NEW_SIZE(void*, in_size);
    size_t res_size = dap_events_socket_pop_from_buf_in(self->t_events_socket, res, in_size);
    PyBytesObject *res_obj = (PyBytesObject*)PyBytes_FromString(res);
    return  Py_BuildValue("nS", res_size, res_obj);
}

PyObject *dap_events_socket_delete_py(PyDapEventsSocketObject *self, PyObject *args){
    if (self->t_events_socket == NULL){
        return NULL;
    }
    PyObject *in_bool;
    if (!PyArg_ParseTuple(args, "O", &in_bool)){
        return NULL;
    }
    bool boolean = true;
    if (in_bool == Py_False)
        boolean = false;
    dap_events_socket_remove_and_delete_mt(self->t_events_socket->worker, self->t_events_socket->uuid);
    return  PyLong_FromLong(0);
}

PyObject *dap_events_socket_shrink_buf_in_py(PyDapEventsSocketObject *self, PyObject *args){
    if (self->t_events_socket == NULL){
        return NULL;
    }
    size_t in_size;
    if(!PyArg_ParseTuple(args, "n", &in_size)){
        return NULL;
    }
    dap_events_socket_shrink_buf_in(self->t_events_socket, in_size);
    return  PyLong_FromLong(0);
}

