#include "wrapping_dap_chain_tx_out_cond_subtype_srv_stake_lock.h"
#include "wrapping_dap_chain_tx_out_cond.h"
#include "dap_chain_net_srv_stake_lock.h"
#include "wrapping_dap_hash.h"

#define PVT(a) ((dap_chain_net_srv_stake_lock_cond_params_t*)((PyDapChainTxOutCondObject*)a)->out_cond->params)

PyGetSetDef DapChainTxOutCondSubtypeSrvStakeLockGetsSetsDef[]={
        {"timeUnlock", (getter)wrapping_dap_chain_net_srv_stake_lock_get_time_unlock, NULL, NULL, NULL},
        {"flags", (getter)wrapping_dap_chain_net_srv_stake_lock_get_flags, NULL, NULL, NULL},
        {"reinvestPercent", (getter)wrapping_dap_chain_net_srv_stake_lock_get_reinvest_percent, NULL, NULL, NULL},
        {"hashTokenDelegate", (getter)wrapping_dap_chain_net_srv_stake_lock_get_token_delegated, NULL, NULL, NULL},
        {"hashPkeyDelegate", (getter)wrapping_dap_chain_net_srv_stake_lock_get_pkey_delegated, NULL, NULL, NULL},
        {NULL}
};

PyTypeObject DapChainTxOutCondSubTypeSrvStakeLockObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainTxOutCondSubTypeSrvStakeLock",       /* tp_name */
        sizeof(PyDapChainTxOutCondSubTypeSrvStakeLock_Object),/* tp_basicsize */
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
        0,                               /* tp_str */
        0,                               /* tp_getattro */
        0,                               /* tp_setattro */
        0,                               /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASE_EXC_SUBCLASS,         /* tp_flags */
        "Chain tx cond subtype srv stake lock object",             /* tp_doc */
        0,                                 /* tp_traverse */
        0,                                 /* tp_clear */
        0,                                 /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        0,                                 /* tp_iter */
        0,                                 /* tp_iternext */
        0,       /* tp_methods */
        0,                               /* tp_members */
        DapChainTxOutCondSubtypeSrvStakeLockGetsSetsDef,                               /* tp_getset */
        &DapChainTxOutCondObjectType,                               /* tp_base */
        0,                               /* tp_dict */
        0,                               /* tp_descr_get */
        0,                               /* tp_descr_set */
        0,                               /* tp_dictoffset */
        0,                               /* tp_init */
        0,                               /* tp_alloc */
        PyType_GenericNew,               /* tp_new */
};

PyObject *wrapping_dap_chain_net_srv_stake_lock_get_time_unlock(PyObject *self, void *closure) {
    UNUSED(closure);
    dap_chain_net_srv_stake_lock_cond_params_t *cond = PVT(self);
    PyDateTime_IMPORT;
    PyObject *obj_ts_long =  Py_BuildValue("(k)", cond->time_unlock);
    PyObject *obj_ts = PyDateTime_FromTimestamp(obj_ts_long);
    return obj_ts;
}
PyObject *wrapping_dap_chain_net_srv_stake_lock_get_flags(PyObject *self, void *closure) {
    UNUSED(closure);
    dap_chain_net_srv_stake_lock_cond_params_t *cond = PVT(self);
    return Py_BuildValue("I", cond->flags);
}
PyObject *wrapping_dap_chain_net_srv_stake_lock_get_reinvest_percent(PyObject *self, void *closure) {
    UNUSED(closure);
    dap_chain_net_srv_stake_lock_cond_params_t *cond = PVT(self);
    return Py_BuildValue("b", cond->reinvest_percent);
}
PyObject *wrapping_dap_chain_net_srv_stake_lock_get_token_delegated(PyObject *self, void *closure) {
    UNUSED(closure);
    dap_chain_net_srv_stake_lock_cond_params_t *cond = PVT(self);
    PyDapHashFastObject *obj_hf = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    obj_hf->hash_fast = &cond->token_delegated;
    return (PyObject*)obj_hf;
}
PyObject *wrapping_dap_chain_net_srv_stake_lock_get_pkey_delegated(PyObject *self, void *closure) {
    UNUSED(closure);
    dap_chain_net_srv_stake_lock_cond_params_t *cond = PVT(self);
    PyDapHashFastObject *obj_hf = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    obj_hf->hash_fast = &cond->pkey_delegated;
    return (PyObject*)obj_hf;
}
