#include "math_python.h"

PyGetSetDef DapMathGetsSets[] = {
        {"coins", (getter)wrapping_dap_math_get_coins, NULL, NULL, NULL},
        {NULL}
};

PyMethodDef DapMathMethods[] = {
        {"balanceToCoins", wrapping_dap_chain_balance_to_coins,
         METH_VARARGS | METH_STATIC, "The function calculates the number of coins from the number of datoshi."},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapMathObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "DAP.Core.Math",
        .tp_basicsize = sizeof(DapMathObject),
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_doc = "Dap math methods",
        .tp_getset = DapMathGetsSets,
        .tp_methods = DapMathMethods,
        .tp_new = PyType_GenericNew,
};

PyObject *wrapping_dap_math_get_coins(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("s", dap_chain_balance_to_coins(((DapMathObject*)self)->value));
}

PyObject *wrapping_dap_chain_balance_to_coins(PyObject *self, PyObject *args){
    (void)self;
    uint64_t l_balance;
    if(!PyArg_ParseTuple(args, "k", &l_balance)){
        return NULL;
    }
//    dap_chain_uint256_to()
    uint256_t l_balance_256 = dap_chain_uint256_from(l_balance);
    return Py_BuildValue("s", dap_chain_balance_to_coins(l_balance_256));
}
