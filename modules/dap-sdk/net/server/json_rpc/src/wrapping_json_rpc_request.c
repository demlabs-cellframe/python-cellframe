#include "wrapping_json_rpc_request.h"

struct _w_json_rpc_handler *handlers = NULL;

void _w_dap_json_rpc_request_handler(dap_json_rpc_params_t *a_params, dap_json_rpc_response_t *a_response, const char *a_method){
//    PyObject * obj_argv
    int count_params = a_params->lenght;
    PyObject *args = PyTuple_New(count_params);
    PyObject *obj_response = _PyObject_New(&DapJSONRPCResponse_DapJSONRPCResponseType);
    ((PyDapJSONRPCResponseObject*)obj_response)->response = a_response;
    PyTuple_SetItem(args, 0, obj_response);
    for (int i=1; i < count_params; i++){
        bool tmp_bool;
        double tmp_double;
        int tmp_int;
        char *tmp_string;
        PyObject *obj_ptr = NULL;
        switch (a_params->params[i]->type) {
            case TYPE_PARAM_BOOLEAN:
                tmp_bool = (bool)a_params->params[i]->value_param;
                obj_prt = tmp_bool ? PyTuple_SetItem(args, i, Py_True) : PyTuple_SetItem(args, i, Py_False);
                break;
            case TYPE_PARAM_DOUBLE:
                tmp_double = (double)a_params->params[i]->value_param;
                obj_prt = PyLong_FromDouble(tmp_double);
                break;
            case TYPE_PARAM_INTEGER:
                tmp_int = (int)a_params->params[i]->value_param;
                obj_ptr = PyLong_FromLong(tmp_int);
                break;
            case TYPE_PARAM_STRING:
                tmp_string = (char*)a_params->params[i]->value_param;
                obj_ptr = PyUnicode_FromString(tmp_string);
                break;
            
        }
        if (obj_ptr != NULL){
            PyTuple_SetItem(args, i, obj_ptr);
        }
        struct _w_json_rpc_handler *func = NULL;
        HASH_FIND_STR(handlers, a_method, func);
        if (func != NULL){
            //Called python func
            PyObject *obj_result = PyObject_CallObject(func->call_func, args);
            if (!obj_result){
                log_it(L_ERROR, "Can't called method: %s", a_method);
                a_response->type_result = TYPE_RESPONSE_NULL;
                a_response->error = dap_json_rpc_error_search_by_code(1);
                return
            }
        } else {
            log_it(L_WARNING, "%s method can't be called, it is not in the python function call table.");
            a_response->type_result = TYPE_RESPONSE_NULL;
            a_response->error = dap_json_rpc_error_search_by_code(1);
        }
    }
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