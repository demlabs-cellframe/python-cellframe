#include "cf_chain_internal.h"
#include "dap_chain_net_utils.h"

#define LOG_TAG "python_cellframe_chain"

// =========================================
// CHAIN NET UTILS
// =========================================

PyObject* dap_chain_net_get_default_chain_by_chain_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    unsigned int l_chain_type = 0;
    
    if (!PyArg_ParseTuple(a_args, "OI", &l_net_capsule, &l_chain_type)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_net_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = dap_chain_net_get_default_chain_by_chain_type(l_net, (dap_chain_type_t)l_chain_type);
    if (!l_chain) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(l_chain, "dap_chain_t", NULL);
}

// =========================================
// CHAIN POLICY OPERATIONS
// =========================================
