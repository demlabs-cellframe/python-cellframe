#include "libdap-python.h"

#define LOG_TAG "libdap-python"

PyMethodDef DapCoreMethods[] = {
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapCoreObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "DAP.Core",             /* tp_name */
    sizeof(PyDapObject),             /* tp_basicsize */
    0,                         /* tp_itemsize */
    0,			       /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,   /* tp_flags */
    "Dap objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    DapCoreMethods,                /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,         /* tp_new */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

static PyMethodDef DapCommonMethods[] = {
        {"execWithRes", (PyCFunction)dap_exec_with_ret, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};
PyTypeObject DapCommonObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "Dap.common",
        .tp_basicsize = sizeof(PyDapObject),
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_doc = "Dap common methods",
        .tp_methods = DapCommonMethods,
        .tp_new = PyType_GenericNew,
};

PyObject* dap_exec_with_ret(PyObject* self, PyObject *args){
    (void)self;
    char *str;
    if (!PyArg_ParseTuple(args, "s", &str)){
        return NULL;
    }
    char *l_ret = exec_with_ret_multistring(str);
    PyObject *l_res_obj = Py_BuildValue("s", l_ret);
    DAP_DELETE(l_ret);
    return l_res_obj;
}

static PyMethodDef DapLogitMethods[] = {
    {"debug", (PyCFunction)dap_log_it_debug, METH_VARARGS | METH_STATIC, "Log a message with the DEBUG level."},
    {"info", (PyCFunction)dap_log_it_info, METH_VARARGS | METH_STATIC, "Log a message with the INFO level."},
    {"notice", (PyCFunction)dap_log_it_notice, METH_VARARGS | METH_STATIC, "Log a message with the NOTICE level."},
    {"message", (PyCFunction)dap_log_it_message, METH_VARARGS | METH_STATIC, "Log a message with the MESSAGE level."},
    {"dap", (PyCFunction)dap_log_it_dap, METH_VARARGS | METH_STATIC, "Log a message with the DAP level."},
    {"warning", (PyCFunction)dap_log_it_warning, METH_VARARGS | METH_STATIC, "Log a message with the WARNING level."},
    {"att", (PyCFunction)dap_log_it_att, METH_VARARGS | METH_STATIC, "Log a message with the ATT level."},
    {"error", (PyCFunction)dap_log_it_error, METH_VARARGS | METH_STATIC, "Log a message with the ERROR level."},
    {"critical", (PyCFunction)dap_log_it_critical, METH_VARARGS | METH_STATIC, "Log a message with the CRITICAL level."},
    {NULL, NULL, 0, NULL}
};

PyTypeObject DapLogitObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "Dap.logIt",
    .tp_basicsize = sizeof(PyDapObject),
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = "Dap LogIt methods",
    .tp_methods = DapLogitMethods,
    .tp_new = PyType_GenericNew,
};

PyObject *dap_set_log_level(PyObject *self, PyObject *args){
    short int new_log_level;
    if (!PyArg_ParseTuple(args, "h", &new_log_level))
        return NULL;
    if (new_log_level < 0 || new_log_level > 10 ) {
        return PyLong_FromLong(-1);
    } else {
        dap_log_level_set(new_log_level);
        return PyLong_FromLong(0);
    }
}

PyObject* dap_log_it(PyObject* self, PyObject* args){
    short int log_level;
    const char* string_output;
    const char* string_name_plugin = NULL;
    if (!PyArg_ParseTuple(args, "hs|s", &log_level, &string_output, &string_name_plugin))
        return NULL;
    if (log_level < 0 || log_level > 10 ) {
        return PyLong_FromLong(-1);
    } else {
        if (string_name_plugin){
            char *str = dap_strdup_printf("[plugin: %s] %s", string_name_plugin, string_output);
            log_it(log_level, str);
            DAP_FREE(str);
        } else {
            log_it(log_level, string_output);
        }
        return PyLong_FromLong(0);
    }
}

PyObject* dap_log_it_debug(PyObject* self, PyObject* args){
    const char* string_output;
    const char* string_name_plugin = NULL;
    if (!PyArg_ParseTuple(args, "s|s", &string_output, &string_name_plugin)){
        return NULL;
    }
    if (string_name_plugin) {
        char *str = dap_strdup_printf("[plugin: %s] %s", string_name_plugin, string_output);
        log_it(L_DEBUG, str);
        DAP_FREE(str);
    } else {
        log_it(L_DEBUG, string_output);
    }
    return PyLong_FromLong(0);
}
PyObject* dap_log_it_info(PyObject* self, PyObject* args){
    const char* string_output;
    const char* string_name_plugin = NULL;
    if (!PyArg_ParseTuple(args, "s|s", &string_output, &string_name_plugin)){
        return NULL;
    }
    if (string_name_plugin) {
        char *str = dap_strdup_printf("[plugin: %s] %s", string_name_plugin, string_output);
        log_it(L_INFO, str);
        DAP_FREE(str);
    } else {
        log_it(L_INFO, string_output);
    }
    return PyLong_FromLong(0);
}
PyObject* dap_log_it_notice(PyObject* self, PyObject* args){
    const char* string_output;
    const char* string_name_plugin = NULL;
    if (!PyArg_ParseTuple(args, "s|s", &string_output, &string_name_plugin)){
        return NULL;
    }
    if (string_name_plugin){
        char *str = dap_strdup_printf("[plugin: %s] %s", string_name_plugin, string_output);
        log_it(L_NOTICE, str);
        DAP_FREE(str);
    }else{
        log_it(L_NOTICE, string_output);
    }
    return PyLong_FromLong(0);
}
PyObject* dap_log_it_message(PyObject* self, PyObject* args){
    const char* string_output;
    const char* string_name_plugin = NULL;
    if (!PyArg_ParseTuple(args, "s|s", &string_output, &string_name_plugin)){
        return NULL;
    }
    if (string_name_plugin) {
        char *str = dap_strdup_printf("[plugin: %s] %s", string_name_plugin, string_output);
        log_it(L_MSG, str);
        DAP_FREE(str);
    } else {
        log_it(L_MSG, string_output);
    }
    return PyLong_FromLong(0);
}
PyObject* dap_log_it_dap(PyObject* self, PyObject* args){
    const char* string_output;
    const char* string_name_plugin = NULL;
    if (!PyArg_ParseTuple(args, "s|s", &string_output, &string_name_plugin)){
        return NULL;
    }
    if (string_name_plugin) {
        char *str = dap_strdup_printf("[plugin: %s] %s", string_name_plugin, string_output);
        log_it(L_DAP, str);
        DAP_FREE(str);
    } else {
        log_it(L_DAP, string_output);
    }
    return PyLong_FromLong(0);
}
PyObject* dap_log_it_warning(PyObject* self, PyObject* args){
    const char* string_output;
    const char* string_name_plugin = NULL;
    if (!PyArg_ParseTuple(args, "s|s", &string_output, &string_name_plugin)){
        return NULL;
    }
    if (string_name_plugin) {
        char *str = dap_strdup_printf("[plugin: %s] %s", string_name_plugin, string_output);
        log_it(L_WARNING, str);
        DAP_FREE(str);
    } else {
        log_it(L_WARNING, string_output);
    }
    return PyLong_FromLong(0);
}
PyObject* dap_log_it_att(PyObject* self, PyObject* args){
    const char* string_output;
    const char* string_name_plugin = NULL;
    if (!PyArg_ParseTuple(args, "s|s", &string_output, &string_name_plugin)){
        return NULL;
    }
    if (string_name_plugin) {
        char *str = dap_strdup_printf("[plugin: %s] %s", string_name_plugin, string_output);
        log_it(L_ATT, str);
        DAP_FREE(str);
    } else {
        log_it(L_ATT, string_output);
    }
    return PyLong_FromLong(0);
}
PyObject* dap_log_it_error(PyObject* self, PyObject* args){
    const char* string_output;
    const char* string_name_plugin = NULL;
    if (!PyArg_ParseTuple(args, "s|s", &string_output, &string_name_plugin)){
        return NULL;
    }
    if (string_name_plugin) {
        char *str = dap_strdup_printf("[plugin: %s] %s", string_name_plugin, string_output);
        log_it(L_ERROR, str);
        DAP_FREE(str);
    } else {
        log_it(L_ERROR, string_output);
    }
    return PyLong_FromLong(0);
}
PyObject* dap_log_it_critical(PyObject* self, PyObject* args){
    const char* string_output;
    const char* string_name_plugin = NULL;
    if (!PyArg_ParseTuple(args, "s|s", &string_output, &string_name_plugin)){
        return NULL;
    }
    if (string_name_plugin) {
        char *str = dap_strdup_printf("[plugin: %s] %s", string_name_plugin, string_output);
        log_it(L_CRITICAL, str);
        DAP_FREE(str);
    } else {
        log_it(L_CRITICAL, string_output);
    }
    return PyLong_FromLong(0);
}

PyObject* py_m_dap_config_get_item(PyObject *self, PyObject *args){
    const char *section_path;
    const char *item_name;
    if (!PyArg_ParseTuple(args, "s|s", &section_path, &item_name))
        return NULL;
    const char *res = dap_config_get_item_str(g_config, section_path, item_name);
    if (res == NULL){
        PyErr_SetString(PyExc_ValueError, "Value can't be obtained. Either no such section or a key is missing in section");
        return NULL;
    }
    return Py_BuildValue("s", res);
}

PyObject* py_m_dap_config_get_item_default(PyObject *self, PyObject *args){
    const char *section_path;
    const char *item_name;
    const char *def;
    if (!PyArg_ParseTuple(args, "s|s|s", &section_path, &item_name, &def))
        return NULL;
    const char *res = dap_config_get_item_str_default(g_config, section_path, item_name, def);
    return Py_BuildValue("s", res);
}

PyObject *dapListToPyList(dap_list_t *list ){
    unsigned int len = dap_list_length(list);
    PyObject *obj = PyList_New((Py_ssize_t)len);
    for (unsigned int l = 0; l <len; l++){
        void *data = dap_list_nth_data(list, l);
        PyObject *obj_data = PyBytes_FromString((const char*)data);
        PyList_Append(obj, obj_data);
    }
    return obj;
}

dap_list_t *pyListToDapList(PyObject *list ){
    Py_ssize_t len = PyList_Size(list);
    dap_list_t *dap_list = dap_list_alloc();
    for (Py_ssize_t i = 0; i < len; i++){
        PyObject *obj = PyList_GetItem(list, i);
        void *data = PyBytes_AsString(obj);
        dap_list = dap_list_append(dap_list, data);
    }
    return dap_list;
}
