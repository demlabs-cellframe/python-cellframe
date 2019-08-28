#include "python-cellframe.h"

PyMODINIT_FUNC PyInit_libCellFrame(void){

    if (PyType_Ready(&DapObject_DapObjectType) < 0 )
               return NULL;

    PyObject *module = PyModule_Create(&CellFramePythonModule);

    CellFrame_error = PyErr_NewException("libCellFrame.error", NULL, NULL);
    PyModule_AddObject(module, "error", CellFrame_error);

    //PyModule_AddObject(module, "Dap", (PyObject*)&DapObject_DapObjectType);
    return module;
}

static PyObject *python_cellframe_init(PyObject *self, PyObject *args){
    const char *app_name;
    const char *file_name_log;
    const char *config_dir;
    const char *log_level;
    const char *JSON_str;
    if (!PyArg_ParseTuple(args, "s", &JSON_str)){
        return NULL;
    }

//    PyObject* JSONModuleString = PyUnicode_FromString("json");
    PyObject *JSON_Module = PyImport_ImportModule("json");
    if (JSON_Module == NULL) {
        printf("ERROR importing module");
        return NULL;
    }
    PyObject* JSONLoadsFunction = PyObject_GetAttrString(JSON_Module, "loads");
    if (JSONLoadsFunction == NULL)
        return NULL;
    PyObject* argsInLoadsJSON = PyTuple_Pack(1,PyUnicode_FromString(JSON_str));
    PyObject* result = PyObject_CallObject(JSONLoadsFunction, argsInLoadsJSON);
    if (result == NULL)
        return NULL;
    PyObject* getModules = PyDict_GetItemString(result, "modules");
    if (getModules == NULL)
        return NULL;
    PyObject* getDap = PyDict_GetItemString(result, "DAP");
    if (getDap == NULL)
        return NULL;
//    /*Parse DAP*/
    PyObject* config_dir_PyObject = PyDict_GetItemString(getDap, "config_dir");
    PyObject* application_name_PyObject = PyDict_GetItemString(getDap, "application_name");
    PyObject* file_name_log_PyObject = PyDict_GetItemString(getDap, "file_name_log");
    PyObject* logLevel_PyObject = PyDict_GetItemString(getDap, "log_level");
    if (config_dir_PyObject == NULL || application_name_PyObject == NULL ||
            logLevel_PyObject == NULL || file_name_log_PyObject == NULL)
        return NULL;
    app_name = PyUnicode_AsUTF8(application_name_PyObject);
    file_name_log = PyUnicode_AsUTF8(file_name_log_PyObject);
    config_dir = PyUnicode_AsUTF8(config_dir_PyObject);
    log_level = PyUnicode_AsUTF8(logLevel_PyObject);

    if (dap_common_init(app_name, file_name_log) != 0){
        PyErr_SetString(CellFrame_error, "Can't init common functions module");
        return NULL;
    }
    dap_config_init(config_dir);
    if ((g_config = dap_config_open(app_name) ) == NULL){
        PyErr_SetString(CellFrame_error, "Can't init general configurations");
        return NULL;
    }
    return PyLong_FromLong(0);
}

static PyObject *python_cellframe_deinit(PyObject *self, PyObject *args){
    const char *in;
    return PyLong_FromLong(0);
}

int main(int argc, char **argv) {
    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }

    /* Add a built-in module, before Py_Initialize */
    PyImport_AppendInittab("libCellFrame", PyInit_libCellFrame);

    /* Pass argv[0] to the Python interpreter */
    Py_SetProgramName(program);

    /* Initialize the Python interpreter.  Required. */
    Py_Initialize();

    /* Optionally import the module; alternatively,
       import can be deferred until the embedded script
       imports it. */
    PyImport_ImportModule("libCellFrame");

    PyMem_RawFree(program);
    return 0;
}
