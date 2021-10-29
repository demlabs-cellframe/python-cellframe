#include "wrapping_json_rpc_request.h"

#define LOG_TAG "wrapping_json_rpc_request"

struct _w_json_rpc_handler *handlers = NULL;

void _w_dap_json_rpc_request_handler(dap_json_rpc_params_t *a_params, dap_json_rpc_response_t *a_response, const char *a_method){
    int count_params = a_params->lenght;
    PyDapJSONRPCResponseObject *obj_response = PyObject_NEW(PyDapJSONRPCResponseObject, &DapJSONRPCResponse_DapJSONRPCResponseType);
    obj_response = PyObject_Init((PyObject*)obj_response, &DapJSONRPCResponse_DapJSONRPCResponseType);
    obj_response->response = a_response;
    PyObject *obj_params = PyList_New(count_params);
    for (int i=0; i < count_params; i++){
        bool tmp_bool;
        double tmp_double;
        int tmp_int;
        char *tmp_string;
        PyObject *obj_ptr = NULL;
        switch (a_params->params[i]->type) {
            case TYPE_PARAM_BOOLEAN:
                tmp_bool = *(bool*)a_params->params[i]->value_param;
                obj_ptr = tmp_bool ? Py_True : Py_False;
                break;
            case TYPE_PARAM_DOUBLE:
                tmp_double = *(double*)(a_params->params[i]->value_param);
                obj_ptr = PyLong_FromDouble(tmp_double);
                break;
            case TYPE_PARAM_INTEGER:
                tmp_int = *(int*)a_params->params[i]->value_param;
                obj_ptr = PyLong_FromLong(tmp_int);
                break;
            case TYPE_PARAM_STRING:
                tmp_string = (char*)a_params->params[i]->value_param;
                obj_ptr = PyUnicode_FromString(tmp_string);
                break;
            
        }
        if (obj_ptr != NULL){
            PyList_SetItem(obj_params, i, obj_ptr);
        }
        PyObject *args = Py_BuildValue("OO", obj_params, (PyObject*)obj_response);
        Py_XINCREF(args);
        struct _w_json_rpc_handler *func = NULL;
        HASH_FIND_STR(handlers, a_method, func);
        if (func != NULL){
            PyGILState_STATE GILState = PyGILState_Ensure();
            PyObject_Dir((PyObject*)obj_response);
            //Called python func
            PyObject *obj_result = PyObject_CallObject(func->call_func, args);
            PyErr_Print();
            PyGILState_Release(GILState);
            if (!obj_result){
                log_it(L_ERROR, "Can't called method: %s", a_method);
                a_response->type_result = TYPE_RESPONSE_NULL;
                a_response->error = dap_json_rpc_error_JSON_create();
                a_response->error->code_error = 0xF1;
                a_response->error->msg = "Can't called method";
                //a_response->error = dap_json_rpc_error_search_by_code(1);
                return;
            }
        } else {
            log_it(L_WARNING, "%s method can't be called, it is not in the python function call table.");
            a_response->type_result = TYPE_RESPONSE_NULL;
            a_response->error = dap_json_rpc_error_search_by_code(1);
        }
        Py_XDECREF(args);
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
    struct _w_json_rpc_handler *handler = DAP_NEW(struct _w_json_rpc_handler);
    handler->method = dap_strdup(method);
    Py_INCREF(obj_func);
    handler->call_func = obj_func;
    HASH_ADD_STR(handlers, method, handler);
    return Py_None;
}
