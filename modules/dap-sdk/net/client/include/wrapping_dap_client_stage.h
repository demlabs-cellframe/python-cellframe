#ifndef _WRAPPING_DAP_CLIENT_STAGE_
#define _WRAPPING_DAP_CLIENT_STAGE_
#include <Python.h>
#include "dap_client.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct PyDapClientStage{
    PyObject_HEAD
    dap_client_stage_t stage;
}PyDapClientStageObject;

PyObject *STAGE_BEGIN_PY();
PyObject *STAGE_ENC_INIT_PY();
PyObject *STAGE_STREAM_CTL_PY();
PyObject *STAGE_STREAM_SESSION_PY();
PyObject *STAGE_STREAM_CONNECTED_PY();
PyObject *STAGE_STREAM_STREAMING_PY();
PyObject *STAGE_STREAM_ABORT_PY();

static PyMethodDef DapClientStageMethods[] = {
        {"STAGE_BEGIN", (PyCFunction)STAGE_BEGIN_PY, METH_NOARGS | METH_STATIC, ""},
        {"STAGE_ENC_INIT", (PyCFunction)STAGE_ENC_INIT_PY, METH_NOARGS | METH_STATIC, ""},
        {"STAGE_STREAM_CTL", (PyCFunction)STAGE_STREAM_CTL_PY, METH_NOARGS | METH_STATIC, ""},
        {"STAGE_STREAM_SESSION", (PyCFunction)STAGE_STREAM_SESSION_PY, METH_NOARGS | METH_STATIC, ""},
        {"STAGE_STREAM_CONNECTED", (PyCFunction)STAGE_STREAM_CONNECTED_PY, METH_NOARGS | METH_STATIC, ""},
        {"STAGE_STREAM_STREAMING", (PyCFunction)STAGE_STREAM_STREAMING_PY, METH_NOARGS | METH_STATIC, ""},
        {"STAGE_STREAM_ABORT",(PyCFunction) STAGE_STREAM_ABORT_PY, METH_NOARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyObject *dap_client_stage_str_getter(PyDapClientStageObject *self, void *closure);
PyObject *dap_client_stage_int_getter(PyDapClientStageObject *self, void *closure);

static PyGetSetDef DapClientStageGetSets[] = {
    {"Name", (getter)dap_client_stage_str_getter, NULL, NULL, NULL},
    {"Value", (getter)dap_client_stage_int_getter, NULL, NULL, NULL},
    {NULL}
};

static PyTypeObject dapClientStageObject_dapClientStageType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ClientStage",                                      /* tp_name */
        sizeof(PyDapClientStageObject),                               /* tp_basicsize */
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
        "Client stage objects",                                              /* tp_doc */
        0,		                                                      /* tp_traverse */
        0,		                                                      /* tp_clear */
        0,		                                                      /* tp_richcompare */
        0,		                                                      /* tp_weaklistoffset */
        0,                                                      /* tp_iter */
        0,		                                                      /* tp_iternext */
        DapClientStageMethods,                                              /* tp_methods */
        0,                                                            /* tp_members */
        DapClientStageGetSets,                                        /* tp_getset */
        0,                                                            /* tp_base */
        0,                                                            /* tp_dict */
        0,                                                            /* tp_descr_get */
        0,                                                            /* tp_descr_set */
        0,                                                            /* tp_dictoffset */
        0,                                                            /* tp_init */
        0,                                                            /* tp_alloc */
        PyType_GenericNew,                                            /* tp_new */

};

static bool PyDapClientStageObject_Check(PyObject *obj){
    return PyObject_TypeCheck(obj, &dapClientStageObject_dapClientStageType);
}




#ifdef __cplusplus
}
#endif

#endif // _WRAPPING_DAP_CLIENT_STAGE_
