#include "wrapping_dap_client_stage.h"

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

PyObject *dap_client_stage_str_getter(PyDapClientStageObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("s", dap_client_stage_str(self->stage));
}

PyObject *dap_client_stage_int_getter(PyDapClientStageObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("I", ((PyDapClientStageObject*)self)->stage);
}
