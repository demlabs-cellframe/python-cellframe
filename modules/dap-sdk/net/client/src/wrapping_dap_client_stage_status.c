#include "wrapping_dap_client_stage_status.h"

PyObject *STAGE_STATUS_NONE_PY(){
    PyDapClientStageStatusObject *obj_stage_status = (PyDapClientStageStatusObject*)_PyObject_New(
            &dapClientStageStatusObject_dapClientStageStatusType);
    PyObject_Dir((PyObject*)obj_stage_status);
    obj_stage_status->stage_status = STAGE_STATUS_NONE;
    return (PyObject *)obj_stage_status;
}
// Enc init stage
PyObject *STAGE_STATUS_IN_PROGRESS_PY(){
    PyDapClientStageStatusObject *obj_stage_status = (PyDapClientStageStatusObject*)_PyObject_New(
            &dapClientStageStatusObject_dapClientStageStatusType);
    PyObject_Dir((PyObject*)obj_stage_status);
    obj_stage_status->stage_status = STAGE_STATUS_IN_PROGRESS;
    return (PyObject *)obj_stage_status;
}
PyObject *STAGE_STATUS_ABORTING_PY(){
    PyDapClientStageStatusObject *obj_stage_status = (PyDapClientStageStatusObject*)_PyObject_New(
            &dapClientStageStatusObject_dapClientStageStatusType);
    obj_stage_status->stage_status = STAGE_STATUS_ABORTING;
    return (PyObject *)obj_stage_status;
}
PyObject *STAGE_STATUS_ERROR_PY(){
    PyDapClientStageStatusObject *obj_stage_status = (PyDapClientStageStatusObject*)_PyObject_New(
            &dapClientStageStatusObject_dapClientStageStatusType);
    PyObject_Dir((PyObject*)obj_stage_status);
    obj_stage_status->stage_status = STAGE_STATUS_ERROR;
    return (PyObject *)obj_stage_status;
}
PyObject *STAGE_STATUS_DONE_PY(){
    PyDapClientStageStatusObject *obj_stage_status = (PyDapClientStageStatusObject*)_PyObject_New(
            &dapClientStageStatusObject_dapClientStageStatusType);
    obj_stage_status->stage_status = STAGE_STATUS_DONE;
    return (PyObject *)obj_stage_status;
}

PyObject *dap_client_stage_status_str_getter(PyDapClientStageStatusObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("s", dap_client_stage_status_str(self->stage_status));
}
PyObject *dap_client_stage_status_int_getter(PyDapClientStageStatusObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("I", self->stage_status);
}
