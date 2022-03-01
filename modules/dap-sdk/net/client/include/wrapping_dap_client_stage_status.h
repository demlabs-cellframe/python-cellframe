#ifndef _WRAPPING_DAP_CLIENT_STAGE_STATUS_
#define _WRAPPING_DAP_CLIENT_STAGE_STATUS_

#include <Python.h>
#include "dap_client.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct PyDapClientStageStatus{
    PyObject_HEAD
    dap_client_stage_status_t stage_status;
}PyDapClientStageStatusObject;

PyObject *STAGE_STATUS_NONE_PY();
// Enc init stage
PyObject *STAGE_STATUS_IN_PROGRESS_PY();
PyObject *STAGE_STATUS_ABORTING_PY();
PyObject *STAGE_STATUS_ERROR_PY();
PyObject *STAGE_STATUS_DONE_PY();


PyMethodDef DapClientStageStatusMethods[] = {
        {"STAGE_STATUS_NONE", STAGE_STATUS_NONE_PY, METH_NOARGS, ""},
        // Enc init stage
        {"STAGE_STATUS_IN_PROGRESS", STAGE_STATUS_IN_PROGRESS_PY, METH_NOARGS | METH_STATIC, ""},
        {"STAGE_STATUS_ABORTING", STAGE_STATUS_ABORTING_PY, METH_NOARGS | METH_STATIC, ""},
        {"STAGE_STATUS_ERROR", STAGE_STATUS_ERROR_PY, METH_NOARGS | METH_STATIC, ""},
        {"STAGE_STATUS_DONE", STAGE_STATUS_DONE_PY, METH_NOARGS | METH_STATIC, ""},

        {NULL, NULL, 0, NULL}
};

static PyTypeObject dapClientStageStatusObject_dapClientStageStatusType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ClientStageStatus",                                      /* tp_name */
        sizeof(PyDapClientStageStatusObject),                               /* tp_basicsize */
        0,                                                            /* tp_itemsize */
        0,                                                            /* tp_dealloc */
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
        "Client stage status object",                                              /* tp_doc */
        0,		                                                      /* tp_traverse */
        0,		                                                      /* tp_clear */
        0,		                                                      /* tp_richcompare */
        0,		                                                      /* tp_weaklistoffset */
        0,		                                                      /* tp_iter */
        0,		                                                      /* tp_iternext */
        DapClientStageStatusMethods,                                              /* tp_methods */
        0,                                                            /* tp_members */
        0,                                                            /* tp_getset */
        0,                                                            /* tp_base */
        0,                                                            /* tp_dict */
        0,                                                            /* tp_descr_get */
        0,                                                            /* tp_descr_set */
        0,                                                            /* tp_dictoffset */
        0,                                                            /* tp_init */
        0,                                                            /* tp_alloc */
        PyType_GenericNew,                                            /* tp_new */

};


#ifdef  __cplusplus
}
#endif

#endif //_WRAPPING_DAP_CLIENT_STAGE_STATUS_