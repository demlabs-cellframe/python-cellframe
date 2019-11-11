#include "libdap-python.h"

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
    if (!PyArg_ParseTuple(args, "h|s", &log_level, &string_output))
        return NULL;
    if (log_level < 0 || log_level > 10 ) {
        return PyLong_FromLong(-1);
    } else {
        log_it(log_level, string_output);
        return PyLong_FromLong(0);
    }
}

PyObject* dap_log_it_debug(PyObject* self, PyObject* args){
    const char* string_output;
    if (!PyArg_ParseTuple(args, "s", &string_output)){
        return NULL;
    }
    log_it(L_DEBUG, string_output);
    return PyLong_FromLong(0);
}
PyObject* dap_log_it_info(PyObject* self, PyObject* args){
    const char* string_output;
    if (!PyArg_ParseTuple(args, "s", &string_output)){
        return NULL;
    }
    log_it(L_INFO, string_output);
    return PyLong_FromLong(0);
}
PyObject* dap_log_it_notice(PyObject* self, PyObject* args){
    const char* string_output;
    if (!PyArg_ParseTuple(args, "s", &string_output)){
        return NULL;
    }
    log_it(L_NOTICE, string_output);
    return PyLong_FromLong(0);
}
PyObject* dap_log_it_message(PyObject* self, PyObject* args){
    const char* string_output;
    if (!PyArg_ParseTuple(args, "s", &string_output)){
        return NULL;
    }
    log_it(L_MSG, string_output);
    return PyLong_FromLong(0);
}
PyObject* dap_log_it_dap(PyObject* self, PyObject* args){
    const char* string_output;
    if (!PyArg_ParseTuple(args, "s", &string_output)){
        return NULL;
    }
    log_it(L_DAP, string_output);
    return PyLong_FromLong(0);
}
PyObject* dap_log_it_warning(PyObject* self, PyObject* args){
    const char* string_output;
    if (!PyArg_ParseTuple(args, "s", &string_output)){
        return NULL;
    }
    log_it(L_WARNING, string_output);
    return PyLong_FromLong(0);
}
PyObject* dap_log_it_att(PyObject* self, PyObject* args){
    const char* string_output;
    if (!PyArg_ParseTuple(args, "s", &string_output)){
        return NULL;
    }
    log_it(L_ATT, string_output);
    return PyLong_FromLong(0);
}
PyObject* dap_log_it_error(PyObject* self, PyObject* args){
    const char* string_output;
    if (!PyArg_ParseTuple(args, "s", &string_output)){
        return NULL;
    }
    log_it(L_ERROR, string_output);
    return PyLong_FromLong(0);
}
PyObject* dap_log_it_critical(PyObject* self, PyObject* args){
    const char* string_output;
    if (!PyArg_ParseTuple(args, "s", &string_output)){
        return NULL;
    }
    log_it(L_CRITICAL, string_output);
    return PyLong_FromLong(0);
}

PyObject* py_m_dap_config_get_item(PyObject *self, PyObject *args){
    const char *section_path;
    const char *item_name;
    if (!PyArg_ParseTuple(args, "s|s", &section_path, &item_name))
        return NULL;
    const char *res = dap_config_get_item_str(g_config, section_path, item_name);
    if (res == NULL){
        PyErr_SetString(PyExc_ValueError, "The value could not be obtained. Or there is no section. Or missing key in the section.");
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
