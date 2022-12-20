#include "dap_events_python.h"

PyMethodDef PyDapEventsObject_methods[] = {
        {"start", (PyCFunction)dap_events_start_py, METH_NOARGS, ""},
        {"wait", (PyCFunction)dap_events_wait_py, METH_NOARGS, ""},
        {"killSocket", (PyCFunction)dap_events_socket_kill_socket_py, METH_VARARGS, ""},
        {"removeAndDeleteSocket", (PyCFunction)dap_events_socket_remove_and_delete_py, METH_VARARGS, ""},
        {NULL, NULL, 0, NULL}  /* Sentinel */
};

PyTypeObject DapEventsObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "DAP.Net.Events",           /* tp_name */
        sizeof(PyDapEventsObject),		        /* tp_basicsize */
        0,			                        /* tp_itemsize */
        (destructor)PyDapEventsObject_dealloc,	/* tp_dealloc */
        0,                         			/* tp_print */
        0,                         			/* tp_getattr */
        0,                         			/* tp_setattr */
        0,                         			/* tp_reserved */
        0,                         			/* tp_repr */
        0,                         			/* tp_as_number */
        0,                         			/* tp_as_sequence */
        0,                         			/* tp_as_mapping */
        0,                         			/* tp_hash  */
        0,                         			/* tp_call */
        0,                         			/* tp_str */
        0,                         			/* tp_getattro */
        0,                         			/* tp_setattro */
        0,                         			/* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,   			/* tp_flags */
        "DapEvents objects",           		/* tp_doc */
        0,		               			/* tp_traverse */
        0,		               			/* tp_clear */
        0,		               			/* tp_richcompare */
        0,		               			/* tp_weaklistoffset */
        0,		               			/* tp_iter */
        0,		               			/* tp_iternext */
        PyDapEventsObject_methods,			/* tp_methods */
        0,						/* tp_members */
        0,						/* tp_getset */
        0,                         			/* tp_base */
        0,                         			/* tp_dict */
        0,                         			/* tp_descr_get */
        0,                         			/* tp_descr_set */
        0,                         			/* tp_dictoffset */
        0,						/* tp_init */
        0,                         			/* tp_alloc */
        PyDapEventsObject_new,	                /* tp_new */
};

PyObject *PyDapEventsObject_new(PyTypeObject *type_object, PyObject *args, PyObject *kwds){
    PyDapEventsObject *new_EO = (PyDapEventsObject*)PyType_GenericNew(type_object, args, kwds);
    new_EO->t_events = dap_events_new();
    return (PyObject *)new_EO;
}

void PyDapEventsObject_dealloc(PyDapEventsObject *eventsObject){
    dap_events_delete(eventsObject->t_events);
    Py_TYPE(eventsObject)->tp_free((PyObject*)eventsObject);
}

PyObject *dap_events_socket_remove_and_delete_py(PyDapEventsObject *self, PyObject *args){
    PyObject *in_obj;
    PyObject *in_bool;
    if (!PyArg_ParseTuple(args, "O|O", &in_obj, &in_bool)){
        return NULL;
    }
    bool preserve_inheritor = true;
    if (in_bool == Py_False)
        preserve_inheritor = false;
    dap_events_socket_remove_and_delete_mt(((PyDapEventsSocketObject*)in_obj)->t_events_socket->worker,((PyDapEventsSocketObject*)in_obj)->t_events_socket->uuid);
    return PyLong_FromLong(0);
}

PyObject *dap_events_socket_kill_socket_py(PyDapEventsObject *self, PyObject *args){
    PyObject *in_obj;
    if (!PyArg_ParseTuple(args, "O", &in_obj)){
        return NULL;
    }
    dap_events_socket_remove_and_delete_mt(((PyDapEventsSocketObject*)in_obj)->t_events_socket->worker,((PyDapEventsSocketObject*)in_obj)->t_events_socket->uuid);
    return PyLong_FromLong(0);
}

PyObject *dap_events_start_py(PyDapEventsObject *self, PyObject *args){
    int32_t result = dap_events_start(self->t_events);
    return PyLong_FromLong(result);
}
PyObject *dap_events_wait_py(PyDapEventsObject *self, PyObject *args){
    int32_t result = dap_events_wait(self->t_events);
    return PyLong_FromLong(result);
}
