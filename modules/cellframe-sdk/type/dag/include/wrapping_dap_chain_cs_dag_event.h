#pragma once

#include <Python.h>
#include <datetime.h>
#include "dap_chain_cs_dag_event.h"
#include "wrapping_dap_chain_common.h"
#include "wrapping_dap_hash.h"
#include "wrapping_dap_chain_datum.h"

typedef struct PyDapChainCsDagEvent{
    PyObject_HEAD
    dap_chain_cs_dag_event_t *event;
    size_t event_size;
}PyDapChainCsDagEventObject;

PyObject *wrapping_dap_chain_cs_dag_event_get_version(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_cs_dag_event_get_round_id(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_cs_dag_event_get_ts_created(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_cs_dag_event_get_chain_id(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_cs_dag_event_get_cell_id(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_cs_dag_event_get_hash_count(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_cs_dag_event_get_signs_count(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_cs_dag_event_get_links(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_cs_dag_event_get_datum(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_cs_dag_event_get_signs(PyObject *self, void *closure);

static PyMethodDef PyDapChainCsDagEventMethodsDef[] = {
        {NULL, NULL, 0, NULL}
};

static PyGetSetDef PyDapChainCsDagEventGetsSetsDef[] = {
        {"version", (getter)wrapping_dap_chain_cs_dag_event_get_version, NULL, NULL, NULL},
        {"roundId", (getter)wrapping_dap_chain_cs_dag_event_get_round_id, NULL, NULL, NULL},
        {"created", (getter)wrapping_dap_chain_cs_dag_event_get_ts_created, NULL, NULL, NULL},
        {"chainId", (getter)wrapping_dap_chain_cs_dag_event_get_chain_id, NULL, NULL, NULL},
        {"cellId", (getter)wrapping_dap_chain_cs_dag_event_get_cell_id, NULL, NULL, NULL},
        {"hashCount", (getter)wrapping_dap_chain_cs_dag_event_get_hash_count, NULL, NULL, NULL},
        {"signsCount", (getter)wrapping_dap_chain_cs_dag_event_get_signs_count, NULL, NULL, NULL},
        {"links", (getter)wrapping_dap_chain_cs_dag_event_get_links, NULL, NULL, NULL},
        {"datum", (getter)wrapping_dap_chain_cs_dag_event_get_datum, NULL, NULL, NULL},
        {"signs", (getter)wrapping_dap_chain_cs_dag_event_get_signs, NULL, NULL, NULL},
        {NULL}
};

static PyTypeObject DapChainCsDagEvent_DapChainCsDagEventType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainCsDagEvent",                                            /* tp_name */
        sizeof(PyDapChainCsDagEventObject),                                     /* tp_basicsize */
        0,                                                            /* tp_itemsize */
        0,                         /* tp_dealloc */
        0,                                                            /* tp_print */
        0,                                                            /* tp_getattr */
        0,                                                            /* tp_setattr */
        0,                                                            /* tp_reserved */
        0,                                                            /* tp_repr */
        0,                                                            /* tp_as_number */
        0,                                                            /* tp_as_sequence */
        0,                                                            /* tp_as_mapping */
        0,                                                            /* tp_hash  */
        0,                                                            /* tp_call */
        0,                                                            /* tp_str */
        0,                                                            /* tp_getattro */
        0,                                                            /* tp_setattro */
        0,                                                            /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                                      /* tp_flags */
        "Chain cs dag event objects",                                              /* tp_doc */
        0,		                                                      /* tp_traverse */
        0,		                                                      /* tp_clear */
        0,		                                                      /* tp_richcompare */
        0,		                                                      /* tp_weaklistoffset */
        0,		                                                      /* tp_iter */
        0,		                                                      /* tp_iternext */
        PyDapChainCsDagEventMethodsDef,                             /* tp_methods */
        0,                                                            /* tp_members */
        PyDapChainCsDagEventGetsSetsDef,                             /* tp_getset */
        0,                                                            /* tp_base */
        0,                                                            /* tp_dict */
        0,                                                            /* tp_descr_get */
        0,                                                            /* tp_descr_set */
        0,                                                            /* tp_dictoffset */
        0,                                                            /* tp_init */
        0,                                                            /* tp_alloc */
        PyType_GenericNew,                                            /* tp_new */
};
