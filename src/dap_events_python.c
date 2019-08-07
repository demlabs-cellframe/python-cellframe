#include "dap_events_python.h"

PyObject *dap_events_init_py(PyObject *self, PyObject *args){
    uint32_t a_threads_count;
    size_t conn_t;
    if (!PyArg_ParseTuple(args, "I|n", &a_threads_count, &conn_t)){
        return NULL;
    }
    int32_t result = dap_events_init(a_threads_count, conn_t);
    return PyLong_FromLong(result);
}
PyObject *dap_events_deinit_py(void){
    dap_events_deinit();
    return PyLong_FromLong(0);
}

PyObject *PyDapEventsObject_new(PyTypeObject *type_object, PyObject *args, PyObject *kwds){
    PyDapEventsObject *new_EO = (PyDapEventsObject*)PyType_GenericNew(type_object, args, kwds);
    new_EO->t_events = dap_events_new();
    return (PyObject *)new_EO;
}

void PyDapEventsObject_dealloc(PyDapEventsObject *eventsObject){
    dap_events_delete(eventsObject->t_events);
    Py_TYPE(eventsObject)->tp_free((PyObject*)eventsObject);
}

//void dap_events_socket_remove_and_delete( dap_events_socket_t* a_es );
PyObject *dap_events_socket_remove_and_delete_py(PyDapEventsObject *self, PyObject *args){
    PyObject *in_obj;
    PyObject *in_bool;
    if (!PyArg_ParseTuple(args, "O|O", &in_obj, &in_bool)){
        return NULL;
    }
    bool preserve_inheritor = true;
    if (in_bool == Py_False)
        preserve_inheritor = false;
    dap_events_socket_remove_and_delete(((PyDapEventsSocketObject*)in_obj)->t_events_socket, preserve_inheritor);
    return PyLong_FromLong(0);
}

PyObject *dap_events_kill_socket_py(PyDapEventsObject *self, PyObject *args){
    PyObject *in_obj;
    if (!PyArg_ParseTuple(args, "O", &in_obj)){
        return NULL;
    }
    dap_events_kill_socket(((PyDapEventsSocketObject*)in_obj)->t_events_socket);
    return PyLong_FromLong(0);
}

PyObject *dap_events_start_py(PyDapEventsObject *self){
    int32_t result = dap_events_start(self->t_events);
    return PyLong_FromLong(result);
}
PyObject *dap_events_wait_py(PyDapEventsObject *self){
    int32_t result = dap_events_wait(self->t_events);
    return PyLong_FromLong(result);
}
