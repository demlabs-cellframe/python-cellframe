#include "dap_common.h"
#include "libdap-app-cli-python.h"

#define LOG_TAG "libdap-app-cli-crypto"

PyMethodDef AppCliMethods[] = {
        {"main", dap_app_cli_main_py, METH_VARARGS | METH_STATIC, "Main CLI function"},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapAppCliObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.AppCli",             /* tp_name */
        sizeof(PyAppCliObject),         /* tp_basicsize */
        0,                         /* tp_itemsize */
        0,                         /* tp_dealloc */
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
        "AppCli object",           /* tp_doc */
        0,		               /* tp_traverse */
        0,		               /* tp_clear */
        0,		               /* tp_richcompare */
        0,		               /* tp_weaklistoffset */
        0,		               /* tp_iter */
        0,		               /* tp_iternext */
        AppCliMethods,             /* tp_methods */
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
        0,                         /* tp_free */
        0,                         /* tp_is_gc*/
        0,                          /* tp_bases*/
        0,                           /* tp_mro */
        0,                           /* tp_cache */
        0,                           /* tp_subclasses */
        0,                           /* tp_weaklist */
        0,                           /* tp_del */
        0,                          /* tp_version_tag*/
        0,                         /* tp_finalize*/
};

/**
 * @brief dap_app_cli_main_py
 * @param self
 * @param args
 * @return
 */
PyObject* dap_app_cli_main_py(PyObject *self, PyObject *args)
{
    (void) self;
    char *l_app_name    = NULL;
    char *l_socket_path = NULL;
    int l_argc          = 0;
    char **l_argv       = NULL;
    PyObject *l_argv_py = NULL;

    PyObject *l_from_list_obj   = NULL;
    PyObject *l_value_obj       = NULL;
    if (!PyArg_ParseTuple(args, "ssO", &l_app_name, &l_socket_path, & l_argv_py))
        return NULL;
    Py_ssize_t l_argv_size_py = PyList_Size(l_argv_py);
    l_argc = (int)l_argv_size_py;
    if (l_argv_size_py > 1){
        l_argv = PyMem_Calloc((size_t)l_argv_size_py, sizeof(char**));
        for (Py_ssize_t i=0; i < l_argv_size_py; i++){
            l_from_list_obj = PyList_GetItem(l_argv_py, i);
            l_value_obj = PyList_GetItem(l_argv_py, i);
            l_argv[i] = dap_strdup(PyUnicode_AsUTF8(l_value_obj));
        }
        int res = dap_app_cli_main(l_app_name, l_socket_path, l_argc, l_argv);
        for (Py_ssize_t i=0; i < l_argv_size_py; i++){
            DAP_FREE(l_argv[i]);
        }
        PyMem_Free(l_argv);
        return PyLong_FromLong(res);
    }
    else
        return PyLong_FromLong(-1024);
}
