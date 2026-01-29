#include "cf_common_utils.h"
#include "dap_chain_atom.h"

#define LOG_TAG "python_cellframe_common"

PyObject *dap_chain_atom_verify_res_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    int l_res = 0;
    if (!PyArg_ParseTuple(a_args, "i", &l_res)) {
        return NULL;
    }
    const char *l_str = dap_chain_atom_verify_res_to_str((dap_chain_atom_verify_res_t)l_res);
    return PyUnicode_FromString(l_str ? l_str : "unknown");
}

int cf_common_register_chain_atom(PyObject *module) {
    static PyMethodDef methods[] = {
        {"dap_chain_atom_verify_res_to_str", dap_chain_atom_verify_res_to_str_py, METH_VARARGS,
         "Convert atom verify result enum to string"},
        {NULL, NULL, 0, NULL}
    };

    return cf_common_register_methods(module, methods);
}
