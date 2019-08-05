#ifndef _DAP_EVENTS_SOCKET_PYTHON_H_
#define _DAP_EVENTS_SOCKET_PYTHON_H_

#include "Python.h"
#include "dap_events_socket.h"
#include "dap_events_python.h"
#include "dap_common.h"

typedef struct PyDapEventsSocket{
    PyObject_HEAD
    dap_events_socket_t *t_events_socket;
}PyDapEventsSocketObject;

static PyTypeObject dapEventsSocket_dapEventsSocketType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libDapServerCore.DapEventsSocket",             /* tp_name */
    sizeof(PyDapEventsSocketObject), /* tp_basicsize */
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
    "DapEventsSocket objects",           /* tp_doc */
};


PyObject *dap_events_socket_init_py(); //  Init clients module
PyObject *dap_events_socket_deinit_py(); // Deinit clients module

PyObject *dap_events_socket_create_after_py(PyObject *self, PyObject *args);//dap_events_socket_t * a_es);

//PyObject *dap_events_socket_wrap_no_add_py(PyObject *self, PyObject *args);//struct dap_events * a_events,
                                           // int s, dap_events_socket_callbacks_t * a_callbacks); // Create new client and add it to the list


PyObject *dap_events_socket_find_py(PyObject *self, PyObject *args);//int sock, struct dap_events * sh); // Find client by socket

PyObject *dap_events_socket_is_ready_to_read_py(PyObject *self, PyObject *args);//dap_events_socket_t * sc);
PyObject *dap_events_socket_is_ready_to_write_py(PyObject *self, PyObject *args);//dap_events_socket_t * sc);
PyObject *dap_events_socket_set_readable_py(PyObject *self, PyObject *args);//dap_events_socket_t * sc,bool is_ready);
PyObject *dap_events_socket_set_writable_py(PyObject *self, PyObject *args);//dap_events_socket_t * sc,bool is_ready);

PyObject *dap_events_socket_write_py(PyObject *self, PyObject *args);//dap_events_socket_t *sc, const void * data, size_t data_size);
PyObject *dap_events_socket_write_f_py(PyObject *self, PyObject *args);//dap_events_socket_t *sc, const char * format,...);
PyObject *dap_events_socket_read_py(PyObject *self, PyObject *args);//dap_events_socket_t *sc, void * data, size_t data_size);

PyObject *dap_events_socket_delete_py(PyObject *self, PyObject *args);//dap_events_socket_t *sc,bool preserve_inheritor); // Removes the client from the list

PyObject *dap_events_socket_shrink_buf_in_py(PyObject *self, PyObject *args);//dap_events_socket_t * cl, size_t shrink_size);

#ifdef __cplusplus
}
#endif

#endif
