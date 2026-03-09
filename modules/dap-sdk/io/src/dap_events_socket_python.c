#include "libdap-python.h"
#include "dap_events_socket_python.h"

#define LOG_TAG "dap_events_socket_python"

static PyMethodDef PyDapEventsObjectSocket_methods[] = {
        {"createAfter", (PyCFunction)dap_events_socket_create_after_py, METH_NOARGS, ""},
        {"setReadable", (PyCFunction)dap_events_socket_set_readable_py, METH_VARARGS, ""},
        {"setWritable", (PyCFunction)dap_events_socket_set_writable_py, METH_VARARGS, ""},
        {"write", (PyCFunction)dap_events_socket_write_py, METH_VARARGS, ""},
        {"writeF", (PyCFunction)dap_events_socket_write_f_py, METH_VARARGS, ""},
        {"read", (PyCFunction)dap_events_socket_read_py, METH_VARARGS, ""},
        {"delete", (PyCFunction)dap_events_socket_delete_py, METH_VARARGS, ""},
        {"shringBufIn", (PyCFunction)dap_events_socket_shrink_buf_in_py, METH_VARARGS, ""},
        {}  /* Sentinel */
};

PyTypeObject DapEventsSocketObjectType = DAP_PY_TYPE_OBJECT(
        "DAP.Net.EventsSocket", sizeof(PyDapEventsSocketObject),
        "DapEventsSocket objects",
        .tp_methods = PyDapEventsObjectSocket_methods);

PyObject *dap_events_socket_create_after_py(PyDapEventsSocketObject *self, __attribute__((unused)) PyObject *args)
{
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
    PyObject *py_format = NULL, *fmt_args = NULL;
    if (!PyArg_ParseTuple(args, "U|O", &py_format, &fmt_args)){
        return NULL;
    }
    PyObject *formatted = fmt_args ? PyUnicode_Format(py_format, fmt_args) : (Py_INCREF(py_format), py_format);
    if (!formatted)
        return NULL;
    const char *data = PyUnicode_AsUTF8(formatted);
    if (!data) {
        Py_DECREF(formatted);
        return NULL;
    }
    size_t res_size = dap_events_socket_write_unsafe(self->t_events_socket, data, strlen(data));
    Py_DECREF(formatted);
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
    bool preserve_inheritor = true;
    if (in_bool == Py_False)
        preserve_inheritor = false;
    UNUSED(preserve_inheritor); // Is it really need for?
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

