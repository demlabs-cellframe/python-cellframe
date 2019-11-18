#include "libdap_chain_type_python.h"

PyObject* CHAIN_TYPE_FIRST_PY(){
    PyObject *dap_chain_obj = _PyObject_New(&dapChainTypeObject_dapChainTypeType);
    ((PyChainTypeObject*)dap_chain_obj)->chain_type = CHAIN_TYPE_FIRST;
    return Py_BuildValue("O", &dap_chain_obj);
}
PyObject* CHAIN_TYPE_TOKEN_PY(){
    PyObject *dap_chain_obj = _PyObject_New(&dapChainTypeObject_dapChainTypeType);
    ((PyChainTypeObject*)dap_chain_obj)->chain_type = CHAIN_TYPE_TOKEN;
    return Py_BuildValue("O", &dap_chain_obj);
}
PyObject* CHAIN_TYPE_EMISSION_PY(){
    PyObject *dap_chain_obj = _PyObject_New(&dapChainTypeObject_dapChainTypeType);
    ((PyChainTypeObject*)dap_chain_obj)->chain_type = CHAIN_TYPE_EMISSION;
    return Py_BuildValue("O", &dap_chain_obj);
}
PyObject* CHAIN_TYPE_TX_PY(){
    PyObject *dap_chain_obj = _PyObject_New(&dapChainTypeObject_dapChainTypeType);
    ((PyChainTypeObject*)dap_chain_obj)->chain_type = CHAIN_TYPE_TX;
    return Py_BuildValue("O", &dap_chain_obj);
}
PyObject* CHAIN_TYPE_LAST_PY(){
    PyObject *dap_chain_obj = _PyObject_New(&dapChainTypeObject_dapChainTypeType);
    ((PyChainTypeObject*)dap_chain_obj)->chain_type = CHAIN_TYPE_LAST;
    return Py_BuildValue("O", &dap_chain_obj);
}
