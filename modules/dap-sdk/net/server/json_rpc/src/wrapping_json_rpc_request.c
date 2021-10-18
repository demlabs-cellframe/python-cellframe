#include "wrapping_json_rpc_request.h"

struct _w_json_rpc_handler *handlers = NULL;

void _w_dap_json_rpc_request_handler(dap_json_rpc_params_t *a_params, dap_json_rpc_response_t *a_response){
//    PyObject * obj_argv
    int count_params = a_params->lenght;
    PyObject *args = PyTuple_New(count_params);
}

PyObject* dap_json_rpc_request_reg_handler_py(PyObject *self, PyObject *args){
    PyObject *obj_func = NULL;
    char *method = NULL;
    if (!PyArg_ParseTuple(args, "sO", &method, &obj_func)) {
        PyErr_SetString(PyExc_ValueError, "The function was called with invalid arguments , this function must take two"
                                          "arguments, a string and a callback function.");
        return NULL;
    }
    if(!PyCallable_Check(obj_func)){
        PyErr_SetString(PyExc_ValueError, "The second argument is not a callback function.");
        return NULL;
    }
    if (dap_json_rpc_registration_request_handler(method, _w_dap_json_rpc_request_handler) != 0){
        PyErr_SetString(PyExc_IndexError, "A handler with this name has already been registered.");
        return NULL;
    }
    struct _w_json_rpc_handler *handler = DAP_NEW(_w_json_rpc_handler);
    handler->method = dap_strdup(method);
    handler->call_func = obj_func;
    HASH_ADD_STR(handlers, method, handler);
    return Py_None;
}