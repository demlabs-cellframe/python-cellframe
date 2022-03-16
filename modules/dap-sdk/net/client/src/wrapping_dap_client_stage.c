#include "wrapping_dap_client_stage.h"

PyMethodDef DapClientStageMethods[] = {
        {"STAGE_BEGIN", (PyCFunction)STAGE_BEGIN_PY, METH_NOARGS | METH_STATIC, ""},
        {"STAGE_ENC_INIT", (PyCFunction)STAGE_ENC_INIT_PY, METH_NOARGS | METH_STATIC, ""},
        {"STAGE_STREAM_CTL", (PyCFunction)STAGE_STREAM_CTL_PY, METH_NOARGS | METH_STATIC, ""},
        {"STAGE_STREAM_SESSION", (PyCFunction)STAGE_STREAM_SESSION_PY, METH_NOARGS | METH_STATIC, ""},
        {"STAGE_STREAM_CONNECTED", (PyCFunction)STAGE_STREAM_CONNECTED_PY, METH_NOARGS | METH_STATIC, ""},
        {"STAGE_STREAM_STREAMING", (PyCFunction)STAGE_STREAM_STREAMING_PY, METH_NOARGS | METH_STATIC, ""},
        {"STAGE_STREAM_ABORT",(PyCFunction) STAGE_STREAM_ABORT_PY, METH_NOARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject dapClientStageObject_dapClientStageType = {
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
        0,		                                                      /* tp_iter */
        0,		                                                      /* tp_iternext */
        DapClientStageMethods,                                              /* tp_methods */
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

PyObject *STAGE_BEGIN_PY(){
    PyDapClientStageObject *obj_stage = (PyDapClientStageObject*)_PyObject_New(&dapClientStageObject_dapClientStageType);
    obj_stage->stage = STAGE_BEGIN;
    return (PyObject *)obj_stage;
}
PyObject *STAGE_ENC_INIT_PY(){
    PyDapClientStageObject *obj_stage = (PyDapClientStageObject*)_PyObject_New(&dapClientStageObject_dapClientStageType);
    obj_stage->stage = STAGE_ENC_INIT;
    return (PyObject *)obj_stage;
}
PyObject *STAGE_STREAM_CTL_PY(){
    PyDapClientStageObject *obj_stage = (PyDapClientStageObject*)_PyObject_New(&dapClientStageObject_dapClientStageType);
    obj_stage->stage = STAGE_STREAM_CTL;
    return (PyObject *)obj_stage;
}
PyObject *STAGE_STREAM_SESSION_PY(){
    PyDapClientStageObject *obj_stage = (PyDapClientStageObject*)_PyObject_New(&dapClientStageObject_dapClientStageType);
    obj_stage->stage = STAGE_STREAM_SESSION;
    return (PyObject *)obj_stage;
}
PyObject *STAGE_STREAM_CONNECTED_PY(){
    PyDapClientStageObject *obj_stage = (PyDapClientStageObject*)_PyObject_New(&dapClientStageObject_dapClientStageType);
    obj_stage->stage = STAGE_STREAM_CONNECTED;
    return (PyObject *)obj_stage;
}
PyObject *STAGE_STREAM_STREAMING_PY(){
    PyDapClientStageObject *obj_stage = (PyDapClientStageObject*)_PyObject_New(&dapClientStageObject_dapClientStageType);
    obj_stage->stage = STAGE_STREAM_STREAMING;
    return (PyObject *)obj_stage;
}
PyObject *STAGE_STREAM_ABORT_PY(){
    PyDapClientStageObject *obj_stage = (PyDapClientStageObject*)_PyObject_New(&dapClientStageObject_dapClientStageType);
    obj_stage->stage = STAGE_STREAM_ABORT;
    return (PyObject *)obj_stage;
}