#include "libdap-python.h"

static PyObject *dap_init(PyObject *self, PyObject *args){
    //const char *data;
    const char *system_configs_dir;
    const char *dap_app_name;
    char *dap_app_name_logs;
    if (!PyArg_ParseTuple(args, "s|s", &system_configs_dir, &dap_app_name))
        return NULL;
    int len_dap_app_name = strlen(dap_app_name);
    dap_app_name_logs = calloc((len_dap_app_name+9), sizeof(char));
    memcpy(dap_app_name_logs, dap_app_name, len_dap_app_name);
    const char* log = "_logs.txt";
    memcpy(dap_app_name_logs+len_dap_app_name, log,9);
    dap_config_init(system_configs_dir);
    if ((g_config = dap_config_open(dap_app_name) ) == NULL){
        log_it(L_CRITICAL, "Can't init general configurations");
        return PyLong_FromLong(-1);
    }
    if (dap_common_init(dap_app_name_logs)!=0){
        log_it(L_CRITICAL, "Can't init common functions module");
        return PyLong_FromLong(-2);
    }
    dap_log_level_set( dap_config_get_item_bool_default(g_config,"general","debug_mode", false)? L_DEBUG: L_NOTICE );
    return PyLong_FromLong(0);
}

static PyObject *dap_deinit(){
    dap_config_deinit();
    dap_common_deinit();
    return PyLong_FromLong(0);
}

static PyObject *dap_set_log_level(PyObject *self, PyObject *args){
    const char *data;
    if (!PyArg_ParseTuple(args, "s", &data))
        return NULL;
    dap_log_level_t new_log_level = convert_const_char_to_dap_log_level(data);
    if (new_log_level == -1) {
        return PyLong_FromLong(-1);
    } else {
        dap_log_level_set(new_log_level);
        return PyLong_FromLong(0);
    }
}

static dap_log_level_t convert_const_char_to_dap_log_level(const char* string){
    if (strcmp(string,"DEBUG") == 0){
        return L_DEBUG;
    }
    if (strcmp(string, "INFO") == 0){
        return L_INFO;
    }
    if (strcmp(string, "NOTICE") == 0){
        return L_NOTICE;
    }
    if (strcmp(string, "WARNING") == 0){
        return L_WARNING;
    }
    if (strcmp(string, "ERROR") == 0){
        return L_ERROR;
    }
    if (strcmp(string, "CRITICAL") == 0){
        return L_CRITICAL;
    }
    return -1;
}

static PyObject* dap_log_it(PyObject* self, PyObject* args){
    const char* dap_log_leve_char;
    const char* string_output;
    if (!PyArg_ParseTuple(args, "s|s", &dap_log_leve_char, &string_output))
        return NULL;
    dap_log_level_t log_level = convert_const_char_to_dap_log_level(dap_log_leve_char);
    if (log_level == -1)
        return PyLong_FromLong(-1);
    log_it(log_level, string_output);

    return PyLong_FromLong(0);
}

static PyObject* py_m_dap_config_get_item(PyObject *self, PyObject *args){
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

static PyObject* py_m_dap_config_get_item_default(PyObject *self, PyObject *args){
    const char *section_path;
    const char *item_name;
    const char *def;
    if (!PyArg_ParseTuple(args, "s|s|s", &section_path, &item_name, &def))
        return NULL;
    const char *res = dap_config_get_item_str_default(g_config, section_path, item_name, def);
    return Py_BuildValue("s", res);
}

PyMODINIT_FUNC PyInit_libdap_python_module(void){
    return PyModule_Create(&dapmodule);
}

int main(int argc, char **argv) {
    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }

    /* Add a built-in module, before Py_Initialize */
    PyImport_AppendInittab("libdap_python_module", PyInit_libdap_python_module);

    /* Pass argv[0] to the Python interpreter */
    Py_SetProgramName(program);

    /* Initialize the Python interpreter.  Required. */
    Py_Initialize();

    /* Optionally import the module; alternatively,
       import can be deferred until the embedded script
       imports it. */
    PyImport_ImportModule("libdap_python_module");

    PyMem_RawFree(program);
    return 0;
}
