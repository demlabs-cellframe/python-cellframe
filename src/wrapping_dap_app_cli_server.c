#include "wrapping_dap_app_cli_server.h"

#define LOG_TAG "wrapping_dap_app_cli_server"

int dap_chain_node_cli_init_py(dap_config_t *g_config){
    log_it(L_DEBUG, "Init app cli server");
    dap_chain_node_cli_init(g_config);
}
void dap_chain_node_cli_delete_py(void){
    dap_chain_node_cli_delete();
}

static int wrapping_cmdfunc(int argc, char **argv, char **str_reply){

//    PyGILState_STATE status;
//    dap_chain_node_cli_set_reply_text(str_reply, "working callback python");
//    status  = PyGILState_Ensure();
//    PyGILState_Release(status);
    log_it(L_DEBUG, "Wrapping activy");
    PyObject *arglist;
    PyObject *result;
    log_it(L_DEBUG, "STEP 1");
    PyObject *obj_argv = stringToPyList(argv);
//    log_it(L_ERROR, "STEP 2");
//    PyObject *obj_str_reply = stringToPyList(str_reply);
    log_it(L_DEBUG, "STEP 2");
//    arglist = Py_BuildValue("i|O|O", argc, obj_argv, obj_str_reply);
    arglist = Py_BuildValue("O", obj_argv);
    log_it(L_DEBUG, "STEP 4");
    log_it(L_DEBUG, "Adress %zu object python", binded_object_cmdfunc);
//    result = PyEval_CallObject(binded_object_cmdfunc, arglist);
    result = PyObject_CallObject(binded_object_cmdfunc, arglist);
    if (result)
        log_it(L_DEBUG, "result = %i", PyLong_AsSize_t(result));
    else
        log_it(L_DEBUG, "Function can't called");
//    log_it(L_ERROR, "STEP 5");
//    Py_DECREF(arglist);
//    Py_DECREF(obj_argv);
//    Py_DECREF(obj_str_reply);
//    int r = -1;
//    if (PyLong_Check(result)){
//        r = (int)PyLong_AsLong(result);
//    }
//    Py_DECREF(result);
//    return r;
//    PyGILState_Release(status);
    return 0;
}

PyObject *DapChainNodeCliObject_new(PyTypeObject *type_object, PyObject *args, PyObject *kwds){
    PyDapAppCliServerObject *obj = (PyDapAppCliServerObject*)PyType_GenericNew(type_object, args, kwds);
    obj->func = wrapping_cmdfunc;
    return (PyObject *)obj;
}

PyObject *dap_chain_node_cli_cmd_item_create_py(PyObject *a_self, PyObject *a_args){
    (void) a_self;
    const char *name, *doc, *doc_ex;
    PyObject *obj_cmdfunc;
    //if (!PyArg_ParseTuple(a_args, "s|O:set_callback|s|s", &name, &obj_cmdfunc, &doc, &doc_ex)){
    if (!PyArg_ParseTuple(a_args, "s|O|s|s", &name, &obj_cmdfunc, &doc, &doc_ex)){
            return NULL;
    }else {
        if (!PyCallable_Check(obj_cmdfunc)){
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
    }
    Py_XINCREF(obj_cmdfunc);
    Py_XDECREF(binded_object_cmdfunc);
    binded_object_cmdfunc = obj_cmdfunc;
    log_it(L_DEBUG, "Adress obj_cmdfunc %zu", obj_cmdfunc);
//    dap_chain_node_cli_cmd_item_create(name, ((PyDapAppCliServerObject*)obj_cmdfunc)->func, doc, doc_ex);
    dap_chain_node_cli_cmd_item_create(name, wrapping_cmdfunc, doc, doc_ex);
    return PyLong_FromLong(0);
}

PyObject *dap_chain_node_cli_set_reply_text_py(PyObject *self, PyObject *args){
    (void) self;
    log_it(L_ERROR, "reply_text");
    PyObject *obj_str_reply_list;
    const char *str_list;
    if (!PyArg_ParseTuple(args, "O|O", &obj_str_reply_list))
        return NULL;
    char **str_reply_list = PyListToString(obj_str_reply_list);
    dap_chain_node_cli_set_reply_text(str_reply_list, str_list);
    return PyLong_FromLong(0);
}

PyObject *dap_chain_node_addr_get_by_alias_py(PyObject *self, PyObject *args){
    (void) self;
    PyObject *chain_net;
    const char *alias;
    if (!PyArg_ParseTuple(args, "O|s", &chain_net, &alias))
        return NULL;
    PyObject *obj_node_addr = _PyObject_New(&DapChainNodeAddrObject_DapChainNodeAddrObjectType);
    ((PyDapChainNodeAddrObject*)obj_node_addr)->node_addr = dap_chain_node_addr_get_by_alias(((PyDapChainNetObject*)chain_net)->chain_net, alias);
    return Py_BuildValue("O", obj_node_addr);
}

char **PyListToString(PyObject *list){
    Py_ssize_t size = PyList_Size(list);
    char **res = calloc(sizeof(char**), (size_t)size);
    for (Py_ssize_t i=0; i < size;i++ ){
        char *data = PyBytes_AsString(PyList_GetItem(list, i));
        res[(size_t)i] = data;
    }
    return res;
}
PyObject *stringToPyList(char **list){
    log_it(L_DEBUG, "String to  PyList");
    size_t size = sizeof(list) / sizeof(list[0]);
    log_it(L_DEBUG, "len: %zu", size);
    PyObject *obj = PyList_New((Py_ssize_t)size);
    for (size_t i=0; i < size; i++){
        log_it(L_DEBUG, "Send data \"%s\"", list[i]);
        PyObject *data = PyBytes_FromString(list[i]);
        PyList_Append(obj, data);
    }
    return obj;
}
