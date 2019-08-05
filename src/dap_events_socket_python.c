#include "dap_events_socket_python.h"

PyObject *dap_events_socket_init_py(){
    int result = dap_events_socket_init();
    return  PyLong_FromLong(result);
}
PyObject *dap_events_socket_deinit_py(){
    dap_events_socket_deinit();
    return  PyLong_FromLong(0);
}

PyObject *dap_events_socket_create_after_py(PyObject *self, PyObject *args){
    PyObject *in_object;
    if(!PyArg_ParseTuple(args, "O", &in_object)){
        return NULL;
    }
    dap_events_socket_create_after(((PyDapEventsSocketObject*)in_object)->t_events_socket);
    return  PyLong_FromLong(0);
}

//PyObject *dap_events_socket_wrap_no_add_py(PyObject *self, PyObject *args){
//    return  PyLong_FromLong(0);
//}


PyObject *dap_events_socket_find_py(PyObject *self, PyObject *args){
    int socket;
    PyObject *in_object;
    if (!PyArg_ParseTuple(args, "i|O", &socket, &in_object)){
        return NULL;
    }
    dap_events_socket_t *events_socket = dap_events_socket_find(socket, ((PyDapEventsObject*)in_object)->t_events);
    PyObject *l_events_socket = _PyObject_New(&dapEventsSocket_dapEventsSocketType);
    ((PyDapEventsSocketObject*)l_events_socket)->t_events_socket = events_socket;
    return Py_BuildValue("O", l_events_socket);
}

PyObject *dap_events_socket_is_ready_to_read_py(PyObject *self, PyObject *args){
    PyObject *in_obj;
    if (!PyArg_ParseTuple(args, "O", &in_obj)){
        return NULL;
    }
    bool result = dap_events_socket_is_ready_to_read(((PyDapEventsSocketObject*)in_obj)->t_events_socket);
    if (result)
        return Py_BuildValue("O", Py_True);
    else
        return Py_BuildValue("O", Py_False);
}
PyObject *dap_events_socket_is_ready_to_write_py(PyObject *self, PyObject *args){
    PyObject *in_obj;
    if (!PyArg_ParseTuple(args, "O", &in_obj)){
        return NULL;
    }
    bool result = dap_events_socket_is_ready_to_write(((PyDapEventsSocketObject*)in_obj)->t_events_socket);
    if (result)
        return Py_BuildValue("O", Py_True);
    else
        return Py_BuildValue("O", Py_False);
}
PyObject *dap_events_socket_set_readable_py(PyObject *self, PyObject *args){
    PyObject *in_obj;
    PyObject *l_is_ready;
    if (!PyArg_ParseTuple(args, "O", &in_obj, &l_is_ready)){
        return NULL;
    }
    bool is_ready = true;
    if (l_is_ready == Py_False)
        is_ready = false;
    dap_events_socket_set_readable(((PyDapEventsSocketObject*)in_obj)->t_events_socket, is_ready);
    return  PyLong_FromLong(0);
}
PyObject *dap_events_socket_set_writable_py(PyObject *self, PyObject *args){
    PyObject *in_obj;
    PyObject *l_is_ready;
    if (!PyArg_ParseTuple(args, "O", &in_obj, &l_is_ready)){
        return NULL;
    }
    bool is_ready = true;
    if (l_is_ready == Py_False)
        is_ready = false;
    dap_events_socket_set_writable(((PyDapEventsSocketObject*)in_obj)->t_events_socket, is_ready);
    return  PyLong_FromLong(0);
}

PyObject *dap_events_socket_write_py(PyObject *self, PyObject *args){
    PyObject *in_obj;
    PyBytesObject *in_bytes;
    size_t in_size;
    if (!PyArg_ParseTuple(args, "O|S|n", &in_obj, &in_bytes, &in_size)){
        return NULL;
    }
    char *in_v_bytes = PyBytes_AsString((PyObject*)in_bytes);
    size_t res_size = dap_events_socket_write(((PyDapEventsSocketObject*)in_obj)->t_events_socket,
                                              in_v_bytes, in_size);
    return Py_BuildValue("n", res_size);
}
PyObject *dap_events_socket_write_f_py(PyObject *self, PyObject *args){
    PyObject *in_object;
    const char *format;
    PyTupleObject *pto;
    if (!PyArg_ParseTuple(args, "O|s|O", &in_object, &format, &pto)){
        return  NULL;
    }
    va_list ap;
    if (!PyArg_VaParse(args, format, ap)){
        return NULL;
    }
    size_t res_size = dap_events_socket_write_f(((PyDapEventsSocketObject*)in_object)->t_events_socket, format, ap);
    return Py_BuildValue("n", res_size);
}
PyObject *dap_events_socket_read_py(PyObject *self, PyObject *args){
    PyObject *in_object;
    size_t in_size;
    if (!PyArg_ParseTuple(args, "O|S|n", &in_object, &in_size)){
        return NULL;
    }
    void *res = DAP_NEW_SIZE(void*, in_size);
    size_t res_size = dap_events_socket_read(((PyDapEventsSocketObject*)in_object)->t_events_socket, res, in_size);
    PyBytesObject *res_obj = (PyBytesObject*)PyBytes_FromString(res);
    return  Py_BuildValue("nS", res_size, res_obj);
}

PyObject *dap_events_socket_delete_py(PyObject *self, PyObject *args){
    PyObject *in_object;
    PyObject *in_bool;
    if (!PyArg_ParseTuple(args, "O|O", &in_object, &in_bool)){
        return NULL;
    }
    bool boolean = true;
    if (in_bool == Py_False)
        boolean = false;
    dap_events_socket_delete(((PyDapEventsSocketObject*)in_object)->t_events_socket, boolean);
    return  PyLong_FromLong(0);
}

PyObject *dap_events_socket_shrink_buf_in_py(PyObject *self, PyObject *args){
    PyObject *in_object;
    size_t in_size;
    if(!PyArg_ParseTuple(args, "O|n", &in_object, &in_size)){
        return NULL;
    }
    dap_events_socket_shrink_buf_in(((PyDapEventsSocketObject*)in_object)->t_events_socket, in_size);
    return  PyLong_FromLong(0);
}

