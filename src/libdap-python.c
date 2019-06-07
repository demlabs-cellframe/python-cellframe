#include "libdap-python.h"

static PyObject *dap_init(PyObject *self, PyObject *args)
{
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
    while (*(data+lenMassives) != '\0' ||*(data+lenMassives) != NULL)
    {
        if (*(data+lenMassives)=='\n')
        {
            countSeparators += 1;
        }
        else {
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
    dap_config_open(dap_app_name);
    dap_common_init(dap_app_name_logs);
    return PyLong_FromLong(0);
}

static PyObject *dap_deinit(PyObject *self)
{
    dap_config_deinit();
    dap_common_deinit();
    return PyLong_FromLong(0);
}

PyMODINIT_FUNC PyInit_dap(void)
{
    return PyModule_Create(&dapmodule);
}

int main(int argc, char **argv) {
    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }

    /* Add a built-in module, before Py_Initialize */
    PyImport_AppendInittab("dap", PyInit_dap);

    /* Pass argv[0] to the Python interpreter */
    Py_SetProgramName(program);

    /* Initialize the Python interpreter.  Required. */
    Py_Initialize();

    /* Optionally import the module; alternatively,
       import can be deferred until the embedded script
       imports it. */
    PyImport_ImportModule("dap");

    PyMem_RawFree(program);
    return 0;
}
