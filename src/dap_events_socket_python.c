#include "dap_events_socket_python.h"

#define LOG_TAG "dap_events_socket_python"

PyObject *dap_events_socket_create_after_py(PyDapEventsSocketObject *self){
    if (self->t_events_socket == NULL){
        return NULL;
    }
    dap_events_socket_create_after(self->t_events_socket);
    return  PyLong_FromLong(0);
}

PyObject *dap_events_socket_find_py(PyDapEventsSocketObject *self, PyObject *args){
    if (self->t_events_socket != NULL){
        return NULL;
    }
    int socket;
    PyObject *in_object;
    if (!PyArg_ParseTuple(args, "i|O", &socket, &in_object)){
        return NULL;
    }
    self->t_events_socket = dap_events_socket_find(socket, ((PyDapEventsObject*)in_object)->t_events);
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
    dap_events_socket_set_readable(self->t_events_socket, is_ready);
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
    dap_events_socket_set_writable(self->t_events_socket, is_ready);
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
    size_t res_size = dap_events_socket_write(self->t_events_socket,
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
    size_t res_size = dap_events_socket_write_f(self->t_events_socket, format, ap);
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
    size_t res_size = dap_events_socket_read(self->t_events_socket, res, in_size);
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
    dap_events_socket_delete(self->t_events_socket, boolean);
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

