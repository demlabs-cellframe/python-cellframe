#ifndef _DAP_EVENTS_PYTHON_H
#define _DAP_EVENTS_PYTHON_H


#include "Python.h"
#include "dap_events.h"
//#include "dap_events_socket_python.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyDapEvents{
    PyObject_HEAD
    dap_events_t *t_events;
}PyDapEventsObject;

static PyTypeObject dapEvents_dapEventsType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libDapServerCore.DapEvents",             /* tp_name */
    sizeof(PyDapEventsObject), /* tp_basicsize */
    0,                         /* tp_itemsize */
    0,                         /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_compare */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "DapEvents objects",           /* tp_doc */
};

PyObject *dap_events_init_py(PyObject *self, PyObject *args); //uint32_t a_threads_count, size_t conn_t ); // Init server module
PyObject *dap_events_deinit_py(); // Deinit server module

PyObject *dap_events_new_py();
PyObject *dap_events_delete_py(PyObject *self, PyObject *args); //dap_events_t * sh );
//void dap_events_socket_remove_and_delete( dap_events_socket_t* a_es );
PyObject *dap_events_socket_remove_and_delete_py(PyObject *self, PyObject *args); //dap_events_socket_t* a_es, bool preserve_inheritor );

PyObject *dap_events_kill_socket_py(PyObject *self, PyObject *args); //dap_events_socket_t *a_es );

PyObject *dap_events_start_py(PyObject *self, PyObject *args); //dap_events_t *sh );
PyObject *dap_events_wait_py(PyObject *self, PyObject *args); //dap_events_t *sh );

#ifdef __cplusplus
}
#endif

#endif
