#include "wrapping_cert.h"

int dap_cert_init_py(void){
    return dap_cert_init();
}
void dap_cert_deinit_py(void){
    dap_cert_deinit();
}
PyObject* dap_cert_add_folder_py(PyObject *self, PyObject *args){
    (void)self;
    const char *a_folder_path;
    if(!PyArg_ParseTuple(args, "s", &a_folder_path))
        return NULL;
    dap_cert_add_folder(a_folder_path);
    return PyLong_FromLong(0);
}
