#include "wrapping_json_rpc_response.h"

int set_result(PyObject *self, PyObject *args, void *closure){
    UNUSED(closure);
}
PyObject *get_result(PyObject *self, void *closure){
    UNUSED(closure);
    dap_json_rpc_response_t *l_resp = ((PyDapJSONRPCResponseObject*)self)->response;
    switch (l_resp->type_result) {
    case TYPE_RESPONSE_BOOLEAN:
        return l_resp->result_boolean ? Py_BuildValue("O", Py_True) : Py_BuildValue("O", Py_False);
        break;
    case TYPE_RESPONSE_INTEGER:
        return PyLong_FromLong(l_resp->result_int);
        break;
    case TYPE_RESPONSE_DOUBLE:
        return PyLong_FromDouble(l_resp->result_double);
        break;
    case TYPE_RESPONSE_STRING:
        return Py_BuildValue("s", l_resp->result_string);
        break;
    case TYPE_RESPONSE_NULL:
        return Py_BuildValue("O", Py_None);
        break;
    }
}
PyObject *get_error(PyObject *self, void *closure){
    UNUSED(closure);
    dap_json_rpc_response_t* l_resp = ((PyDapJSONRPCResponseObject*)self)->response;
    return Py_BuildValue("is", l_resp->error->code_error, l_resp->error->msg);
}
int set_error(PyObject *self, PyObject *args, void *closure){
    UNUSED(closure);
    if (args == NULL){
        if (((PyDapJSONRPCResponseObject*)self)->response->error == NULL){
            return -1;
        }
        DAP_FREE(((PyDapJSONRPCResponseObject*)self)->response->error->msg);
        DAP_FREE(((PyDapJSONRPCResponseObject*)self)->response->error);
        return 0;
    }
    int code;
    char *message;
    if(!PyArg_ParseTuple(args, "is", &code, &message)){
        return -1;
    }
    ((PyDapJSONRPCResponseObject*)self)->response->error = DAP_NEW(dap_json_rpc_error_t);
    ((PyDapJSONRPCResponseObject*)self)->response->error->code_error = code;
    size_t lenght_message = dap_strlen(message);
    ((PyDapJSONRPCResponseObject*)self)->response->error->msg = DAP_NEW_SIZE(char, lenght_message);
    memcpy(((PyDapJSONRPCResponseObject*)self)->response->error->msg, message, lenght_message);
    return 0;
}
PyObject *get_id(PyObject *self, void *closure){
    UNUSED(closure);
    dap_json_rpc_response_t* l_resp = ((PyDapJSONRPCResponseObject*)self)->response;
    return  PyLong_FromLong(l_resp->id);
}
