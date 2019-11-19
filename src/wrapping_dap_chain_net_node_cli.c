#include "wrapping_dap_chain_net_node_cli.h"

int dap_chain_node_cli_init_py(dap_config_t *g_config){
    dap_chain_node_cli_init(g_config);
}
void dap_chain_node_cli_delete_py(void){
    dap_chain_node_cli_delete();
}

static int wrapping_cmdfunc(int argc, char **argv, char **str_reply){
    PyObject *arglist;
    PyObject *result;
    PyObject *obj_argv = stringToPyList(argv);
    PyObject *obj_str_reply = stringToPyList(str_reply);
    arglist = Py_BuildValue("i|O|O", argc, obj_argv, obj_str_reply);
    result = PyObject_CallObject(binded_object_cmdfunc, arglist);
    Py_DECREF(arglist);
    Py_DECREF(obj_argv);
    Py_DECREF(obj_str_reply);
    int r = -1;
    if (PyLong_Check(result)){
        r = (int)PyLong_AsLong(result);
    }
    Py_DECREF(result);
    return r;
}

PyObject *DapChainNodeCliObject_new(PyTypeObject *type_object, PyObject *args, PyObject *kwds){
    PyDapChainNodeCliObject *obj = (PyDapChainNodeCliObject*)PyType_GenericNew(type_object, args, kwds);
    obj->func = wrapping_cmdfunc;
    return (PyObject *)obj;
}

PyObject *dap_chain_node_cli_cmd_item_create_py(PyObject *self, PyObject *args){
    const char *name, *doc, *doc_ex;
    PyObject *obj_cmdfunc;
    if (!PyArg_ParseTuple(args, "s|O:set_callback|s|s", &name, &obj_cmdfunc, &doc, &doc_ex)){
        return NULL;
    } else {
        if (!PyCallable_Check(obj_cmdfunc)){
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
    }
    Py_XINCREF(obj_cmdfunc);
    Py_XDECREF(binded_object_cmdfunc);
    binded_object_cmdfunc = obj_cmdfunc;
    dap_chain_node_cli_cmd_item_create(name, ((PyDapChainNodeCliObject*)obj_cmdfunc)->func, doc, doc_ex);
    return PyLong_FromLong(0);
}

PyObject *dap_chain_node_cli_set_reply_text_py(PyObject *self, PyObject *args){
    PyObject *obj_str_reply_list;
    const char *str_list;
    if (!PyArg_ParseTuple(args, "O|O", &obj_str_reply_list))
        return NULL;
    char **str_reply_list = PyListToString(obj_str_reply_list);
    dap_chain_node_cli_set_reply_text(str_reply_list, str_list);
    return PyLong_FromLong(0);
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
    size_t size = sizeof(list) / sizeof(list[0]);
    PyObject *obj = PyList_New((Py_ssize_t)size);
    for (size_t i=0; i < size; i++){
        PyObject *data = PyBytes_FromString(list[i]);
        PyList_Append(obj, data);
    }
    return obj;
}
