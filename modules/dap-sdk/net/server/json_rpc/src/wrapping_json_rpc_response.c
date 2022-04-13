#include "wrapping_json_rpc_response.h"

#define LOG_TAG "wrapping_json_rpc_response"

PyMethodDef PyDapJSONRPCResponseMethods[] = {
        {NULL, NULL, 0, NULL}
};

PyGetSetDef PyDapJSONRPCResponseGetsSets[] = {
        {"ID", (getter)wrapping_json_rpc_response_get_id, NULL, "ID request", NULL},
        {"Error", (getter)wrapping_json_rpc_response_get_error, (setter)wrapping_json_rpc_response_set_error, "", NULL },
        {"Result", (getter)wrapping_json_rpc_response_get_result, (setter)wrapping_json_rpc_response_set_result, "", NULL},
        {NULL, NULL, NULL, NULL, NULL}
};

PyTypeObject DapJsonRpcResponseobjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "DAP.Net.JSONRPCResponse",             /* tp_name */
        sizeof(PyDapJSONRPCResponseObject),                   /* tp_basicsize */
        0,                                                 /* tp_itemsize */
        0,                                                 /* tp_dealloc */
        0,                                                 /* tp_print */
        0,                                                 /* tp_getattr */
        0,                                                 /* tp_setattr */
        0,                                                 /* tp_reserved */
        0,                                                 /* tp_repr */
        0,                                                 /* tp_as_number */
        0,                                                 /* tp_as_sequence */
        0,                                                 /* tp_as_mapping */
        0,                                                 /* tp_hash  */
        0,                                                 /* tp_call */
        0,                                                 /* tp_str */
        0,                                                 /* tp_getattro */
        0,                                                 /* tp_setattro */
        0,                                                 /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                           /* tp_flags */
        "Dap JSON RPC response object",                         /* tp_doc */
        0,                                               /* tp_traverse */
        0,                                               /* tp_clear */
        0,                                               /* tp_richcompare */
        0,                                               /* tp_weaklistoffset */
        0,                                               /* tp_iter */
        0,                                               /* tp_iternext */
        PyDapJSONRPCResponseMethods,                   /* tp_methods */
        0,                                                 /* tp_members */
        PyDapJSONRPCResponseGetsSets,                          /* tp_getset */
        0,                                                 /* tp_base */
        0,                                                 /* tp_dict */
        0,                                                 /* tp_descr_get */
        0,                                                 /* tp_descr_set */
        0,                                                 /* tp_dictoffset */
        0,                                                 /* tp_init */
        0,                                                 /* tp_alloc */
        PyType_GenericNew,                                 /* tp_new */
};

int wrapping_json_rpc_response_set_result(PyObject *self, PyObject *value, void *closure){
    UNUSED(closure);
    dap_json_rpc_response_t *l_resp = ((PyDapJSONRPCResponseObject*)self)->response;
    if (value == NULL){
        if (l_resp->type_result == TYPE_RESPONSE_STRING){
            DAP_FREE(l_resp->result_string);
        }
        l_resp->type_result = TYPE_RESPONSE_NULL;
        return 0;
    }
    if(PyUnicode_Check(value)){
        const char *tmp_ptr = PyUnicode_AsUTF8(value);
        size_t l_size_str = dap_strlen(tmp_ptr);
        l_resp->result_string = DAP_NEW_SIZE(char, l_size_str);
        memcpy(l_resp->result_string, tmp_ptr, l_size_str);
        l_resp->type_result = TYPE_RESPONSE_STRING;
    }
    if (PyBool_Check(value)){
        if (value == Py_True){
            l_resp->result_boolean = true;
        } else {
            l_resp->result_boolean = false;
        }
        l_resp->type_result = TYPE_RESPONSE_BOOLEAN;
    }
    if (PyLong_Check(value)){
        int tmp = _PyLong_AsInt(value);
        l_resp->result_int = tmp;
        l_resp->type_result = TYPE_RESPONSE_INTEGER;
    }
    if (PyFloat_Check(value)){
        l_resp->result_double = PyFloat_AsDouble(value);
        l_resp->type_result = TYPE_RESPONSE_DOUBLE;
    }
    return 0;
}
PyObject *wrapping_json_rpc_response_get_result(PyObject *self, void *closure){
    UNUSED(closure);
    dap_json_rpc_response_t *l_resp = ((PyDapJSONRPCResponseObject*)self)->response;
    switch (l_resp->type_result) {
    case TYPE_RESPONSE_BOOLEAN:
        if (l_resp->result_boolean)
            Py_RETURN_TRUE;
        else
            Py_RETURN_FALSE;
    case TYPE_RESPONSE_INTEGER:
        return PyLong_FromLong(l_resp->result_int);
    case TYPE_RESPONSE_DOUBLE:
        return PyLong_FromDouble(l_resp->result_double);
    case TYPE_RESPONSE_STRING:
        return Py_BuildValue("s", l_resp->result_string);
    case TYPE_RESPONSE_NULL:
    default:
        break;
    }
    Py_RETURN_NONE;
}
PyObject *wrapping_json_rpc_response_get_error(PyObject *self, void *closure){
    UNUSED(closure);
    dap_json_rpc_response_t* l_resp = ((PyDapJSONRPCResponseObject*)self)->response;
    if (l_resp->error)
        return Py_BuildValue("is", l_resp->error->code_error, l_resp->error->msg);
    else
        return PyTuple_New(2);
}
int wrapping_json_rpc_response_set_error(PyObject *self, PyObject *args, void *closure){
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
PyObject *wrapping_json_rpc_response_get_id(PyObject *self, void *closure){
    UNUSED(closure);
    dap_json_rpc_response_t* l_resp = ((PyDapJSONRPCResponseObject*)self)->response;
    return  PyLong_FromLong(l_resp->id);
}
