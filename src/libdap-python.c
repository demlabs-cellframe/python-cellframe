#include "libdap-python.h"

static PyObject *dap_init(PyObject *self, PyObject *args){
    const char *data;
    char *system_configs_dir;
    char *dap_app_name;
    char *dap_app_name_logs;
    if (!PyArg_ParseTuple(args, "s", &data))
        return NULL;
    int lenSystemConfigDir=0;
    int lenDapAppName=0;
    int countSeparators=0;
    int lenMassives = 0;
    while (*(data+lenMassives) != '\0'){
        if (*(data+lenMassives)=='\n'){
            countSeparators += 1;
        }else {
            if (countSeparators == 0)
                lenSystemConfigDir++;
            if (countSeparators == 1)
                lenDapAppName++;
        }
        lenMassives++;
    }
    system_configs_dir = calloc(lenSystemConfigDir, sizeof(char));
    dap_app_name = calloc(lenDapAppName, sizeof(char));
    dap_app_name_logs = calloc((lenDapAppName+9), sizeof(char));
    memcpy(system_configs_dir,data,lenSystemConfigDir);
    memcpy(dap_app_name, data+lenSystemConfigDir+1, lenDapAppName);
    memcpy(dap_app_name_logs, dap_app_name, lenDapAppName);
    const char* log = "_logs.txt";
    memcpy(dap_app_name_logs+lenDapAppName, log,9);
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

static PyObject *dap_deinit(PyObject *self){
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
    const char *data;
    if (!PyArg_ParseTuple(args, "s", &data))
        return NULL;
    char* dap_log_leve_char;
    char* string_output;
    int len_log_level_char=0;
    int len_string_output=0;
    int countSeparators=0;
    int lenMassives = 0;
    while (*(data+lenMassives) != '\0'){
        if (*(data+lenMassives)=='\n'){
            countSeparators += 1;
        }else {
            if (countSeparators == 0)
                len_log_level_char++;
            if (countSeparators == 1)
                len_string_output++;
        }
        lenMassives++;
    }
    if (len_log_level_char == 0 || len_string_output == 0)
        return PyLong_FromLong(-1);
    dap_log_leve_char = calloc(len_log_level_char, sizeof(char));
    string_output = calloc(len_string_output, sizeof(char));
    memcpy(dap_log_leve_char, data, len_log_level_char);
    memcpy(string_output, data+len_log_level_char+1, len_string_output);
    dap_log_level_t log_level = convert_const_char_to_dap_log_level(dap_log_leve_char);
    if (log_level == -1)
        return PyLong_FromLong(-1);
    log_it(log_level, string_output);

    return PyLong_FromLong(0);
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
