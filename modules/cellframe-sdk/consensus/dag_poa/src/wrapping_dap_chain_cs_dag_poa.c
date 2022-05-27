#include "wrapping_dap_chain_cs_dag_poa.h"

#define LOG_TAG "wrapping_dao_chain_cs_dag_poa"

PyMethodDef DapChainCsDagPoaMethods[] = {
        {"setPresign", (PyCFunction)wrapping_dap_chain_cs_dag_poa_presign_callback_set, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainCsDagPoaObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainCsDagPoa",            /* tp_name */
        sizeof(PyDapChainCsDagPoaObject),     /* tp_basicsize */
        0,                               /* tp_itemsize */
        0,                               /* tp_dealloc */
        0,                               /* tp_print */
        0,                               /* tp_getattr */
        0,                               /* tp_setattr */
        0,                               /* tp_reserved */
        0,                               /* tp_repr */
        0,                               /* tp_as_number */
        0,                               /* tp_as_sequence */
        0,                               /* tp_as_mapping */
        0,                               /* tp_hash  */
        0,                               /* tp_call */
        0,               /* tp_str */
        0,                               /* tp_getattro */
        0,                               /* tp_setattro */
        0,                               /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,         /* tp_flags */
        "Chain net object",              /* tp_doc */
        0,		                         /* tp_traverse */
        0,		                         /* tp_clear */
        0,		                         /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        0,		                         /* tp_iter */
        0,		                         /* tp_iternext */
        DapChainCsDagPoaMethods,              /* tp_methods */
        0,                               /* tp_members */
        0,                               /* tp_getset */
        0,                               /* tp_base */
        0,                               /* tp_dict */
        0,                               /* tp_descr_get */
        0,                               /* tp_descr_set */
        0,                               /* tp_dictoffset */
        0,                               /* tp_init */
        0,                               /* tp_alloc */
        PyType_GenericNew,               /* tp_new */
};

typedef struct _wrapping_dap_chain_cs_dag_poa_callback{
    PyObject *func;
    PyObject *arg;
}_wrapping_dap_chain_cs_dag_poa_callback_t;

int _wrapping_callback_handler(dap_chain_t *a_chain, dap_chain_cs_dag_event_t *a_event, size_t a_event_size, void* a_arg){
    if(!a_arg){
        log_it(L_ERROR, "The Python function cannot be run because the argument passed is not a function. ");
        return  -1;
    }
    _wrapping_dap_chain_cs_dag_poa_callback_t *l_callback = (_wrapping_dap_chain_cs_dag_poa_callback_t*)a_arg;

    PyGILState_STATE state = PyGILState_Ensure();
    PyDapChainObject *l_obj_chain = PyObject_New(PyDapChainObject, &DapChainObjectType);
    l_obj_chain->chain_t = a_chain;
    PyDapChainCsDagEventObject *l_obj_event = PyObject_New(PyDapChainCsDagEventObject, &DapChainCsDagEventType);
    l_obj_event->event = a_event;
    l_obj_event->event_size = a_event_size;
    PyObject *argv = Py_BuildValue("OOO", l_obj_chain, l_obj_event, l_callback->arg);
    PyObject *res = PyEval_CallObject(l_callback->func, argv);
    Py_XDECREF(argv);
    PyGILState_Release(state);
    if (res){
        if (PyLong_Check(res)){
            long l_res = PyLong_AsLong(res);
            return l_res;
        } else {
            log_it(L_ERROR, "Python function was executed but returned not a number.");
            return -3;
        }
    }else{
        log_it(L_ERROR, "An error occurred while executing a Python function. ");
        return -2;
    }
}

PyObject* wrapping_dap_chain_cs_dag_poa_presign_callback_set(PyObject *self, PyObject *args){
    (void)self;
    PyDapChainObject *obj_chain;
    PyObject *obj_func;
    PyObject *obj_arg;
    if (!PyArg_ParseTuple(args, "OOO", &obj_chain, &obj_func, &obj_arg)){
        PyErr_SetString(PyExc_AttributeError, "Argument must be callable");
        return NULL;
    }
    if (!PyDapChain_Check(obj_chain)){
        PyErr_SetString(PyExc_AttributeError, "Invalid first parameter passed to function. The first "
                                              "argument must be an instance of an object of type Chain. ");
        return NULL;
    }
    if (!PyCallable_Check(obj_func)){
        PyErr_SetString(PyExc_AttributeError, "The second argument is not correct. This function must "
                                              "accept a function that will be called from the callback. ");
        return NULL;
    }
    _wrapping_dap_chain_cs_dag_poa_callback_t *l_callback = DAP_NEW(_wrapping_dap_chain_cs_dag_poa_callback_t);
    l_callback->func = obj_func;
    l_callback->arg = obj_arg;
    Py_INCREF(obj_func);
    Py_INCREF(obj_arg);
    dap_chain_cs_dag_poa_presign_callback_set(obj_chain->chain_t, _wrapping_callback_handler, l_callback);
    Py_RETURN_NONE;
}
