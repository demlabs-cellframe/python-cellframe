#include "dap_common.h"
#include "libdap-app-cli-python.h"

#define LOG_TAG "libdap-app-cli-crypto"

/**
 * @brief dap_app_cli_main_py
 * @param self
 * @param args
 * @return
 */
PyObject* dap_app_cli_main_py(PyObject *self, PyObject *args)
{
    (void) self;
    char *l_app_name = NULL;
    char *l_socket_path = NULL;
    int l_argc = 0;
    char ** l_argv = NULL;
    PyObject* l_args_py = NULL;
    if (!PyArg_ParseTuple(args, "ssO", &l_app_name,&l_socket_path, &l_args_py))
            return NULL;

    Py_ssize_t l_args_py_size = PyList_Size(l_args_py);
    if ( l_args_py_size ){
        l_argc = l_args_py_size;
        l_argv = DAP_NEW_Z_SIZE(char*, sizeof(char**)*l_argc  );
        for (  int i = 0; i< l_argc; i++){
            l_argv[i] = PyBytes_AsString(PyList_GetItem(l_args_py, i));
        }
        return PyLong_FromLong((long) dap_app_cli_main(l_app_name,l_socket_path,l_argc,l_argv));
    }else
        return PyLong_FromLong(-1024);

}
