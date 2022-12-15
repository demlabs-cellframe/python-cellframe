#pragma once

#include "Python.h"
#include "dap_events.h"
#include "dap_events_socket_python.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyDapEvents{
    PyObject_HEAD
    dap_events_t *t_events;
}PyDapEventsObject;

void PyDapEventsObject_dealloc(PyDapEventsObject *eventsObject);

PyObject *PyDapEventsObject_new(PyTypeObject *type_object, PyObject *args, PyObject *kwds);
PyObject *dap_events_socket_remove_and_delete_py(PyDapEventsObject *self, PyObject *args);

PyObject *dap_events_socket_kill_socket_py(PyDapEventsObject *self, PyObject *args);

PyObject *dap_events_start_py(PyDapEventsObject *self, PyObject *args);
PyObject *dap_events_wait_py(PyDapEventsObject *self, PyObject *args);

extern PyTypeObject DapEventsObjectType;

#ifdef __cplusplus
}
#endif
