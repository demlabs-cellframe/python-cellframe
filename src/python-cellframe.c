#include "python-cellframe.h"

PyMODINIT_FUNC PyInit_libCellFrame(void){
    PyObject *module = PyModule_Create(&CellFramePythonModule);
    return module;
}

PyObject *python_cellframe_init(PyObject *self, PyObject *args){
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
