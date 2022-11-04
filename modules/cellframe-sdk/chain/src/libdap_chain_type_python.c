#include "libdap-python.h"
#include "libdap_chain_type_python.h"


static PyMethodDef DapChainTypeMethods[] = {
        {"CHAIN_TYPE_FIRST", (PyCFunction)CHAIN_TYPE_FIRST_PY, METH_NOARGS|METH_STATIC, ""},
        {"CHAIN_TYPE_TOKEN", (PyCFunction)CHAIN_TYPE_TOKEN_PY, METH_NOARGS|METH_STATIC, ""},
        {"CHAIN_TYPE_EMISSION", (PyCFunction)CHAIN_TYPE_EMISSION_PY, METH_NOARGS|METH_STATIC, ""},
        {"CHAIN_TYPE_TX", (PyCFunction)CHAIN_TYPE_TX_PY, METH_NOARGS|METH_STATIC, ""},
        {"CHAIN_TYPE_LAST", (PyCFunction)CHAIN_TYPE_LAST_PY, METH_NOARGS|METH_STATIC, ""},
        {}
};

PyTypeObject DapChainTypeObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.ChainType", sizeof(PyChainTypeObject),
        "Chain type objects",
        .tp_methods = DapChainTypeMethods);

PyObject *CHAIN_TYPE_FIRST_PY(__attribute__((unused)) PyObject *self, __attribute__((unused)) PyObject *args)
{
    PyObject *dap_chain_obj = _PyObject_New(&DapChainTypeObjectType);
    ((PyChainTypeObject*)dap_chain_obj)->chain_type = CHAIN_TYPE_FIRST;
    return Py_BuildValue("O", &dap_chain_obj);
}
PyObject *CHAIN_TYPE_TOKEN_PY(__attribute__((unused)) PyObject *self, __attribute__((unused)) PyObject *args)
{
    PyObject *dap_chain_obj = _PyObject_New(&DapChainTypeObjectType);
    ((PyChainTypeObject*)dap_chain_obj)->chain_type = CHAIN_TYPE_TOKEN;
    return Py_BuildValue("O", &dap_chain_obj);
}
PyObject *CHAIN_TYPE_EMISSION_PY(__attribute__((unused)) PyObject *self, __attribute__((unused)) PyObject *args)
{
    PyObject *dap_chain_obj = _PyObject_New(&DapChainTypeObjectType);
    ((PyChainTypeObject*)dap_chain_obj)->chain_type = CHAIN_TYPE_EMISSION;
    return Py_BuildValue("O", &dap_chain_obj);
}
PyObject *CHAIN_TYPE_TX_PY(__attribute__((unused)) PyObject *self, __attribute__((unused)) PyObject *args)
{
    PyObject *dap_chain_obj = _PyObject_New(&DapChainTypeObjectType);
    ((PyChainTypeObject*)dap_chain_obj)->chain_type = CHAIN_TYPE_TX;
    return Py_BuildValue("O", &dap_chain_obj);
}
PyObject* CHAIN_TYPE_LAST_PY(__attribute__((unused)) PyObject *self, __attribute__((unused)) PyObject *args)
{
    PyObject *dap_chain_obj = _PyObject_New(&DapChainTypeObjectType);
    ((PyChainTypeObject*)dap_chain_obj)->chain_type = CHAIN_TYPE_LAST;
    return Py_BuildValue("O", &dap_chain_obj);
}
