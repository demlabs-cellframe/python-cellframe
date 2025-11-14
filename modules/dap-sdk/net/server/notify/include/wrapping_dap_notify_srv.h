#pragma once

#include "Python.h"
#include "dap_notify_srv.h"

typedef struct PyDapNotifySrvObject{
    PyObject_HEAD
} PyDapNotifySrvObject;

extern PyTypeObject DapNotifySrvObjectType;

PyObject *wrapping_dap_notify_srv_send(PyObject *self, PyObject *args);
PyObject *wrapping_dap_notify_srv_sendf(PyObject *self, PyObject *args);


