#include "wrapping_dap_chain_tx_out_cond_subtype_srv_stake_lock.h"
#include "wrapping_dap_chain_tx_out_cond.h"
#include "dap_chain_net_srv_stake_lock.h"
#include "python-cellframe_common.h"
#include "wrapping_dap_hash.h"

#define PVT(a) (((PyDapChainTxOutCondObject*)a)->out_cond->subtype.srv_stake_lock)

int DapChainTxOutCondSubtypeSrvStakeLock_new(PyDapChainTxOutCondObject *self, PyObject *args, PyObject *kwds);

PyGetSetDef DapChainTxOutCondSubtypeSrvStakeLockGetsSetsDef[]={
        {"timeUnlock", (getter)wrapping_dap_chain_net_srv_stake_lock_get_time_unlock, NULL, "", NULL},
        {"flags", (getter)wrapping_dap_chain_net_srv_stake_lock_get_flags, NULL, "", NULL},
        {"reinvestPercent", (getter)wrapping_dap_chain_net_srv_stake_lock_get_reinvest_percent, NULL, "", NULL},
        {"hashTokenDelegate", (getter)wrapping_dap_chain_net_srv_stake_lock_get_token_delegated, NULL, "", NULL},
        {"usedBy", (getter)wrapping_dap_chain_tx_out_cound_used_by, NULL, "", NULL},
        {}
};

PyTypeObject DapChainTxOutCondSubTypeSrvStakeLockObjectType = {
        .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "CellFrame.ChainTxOutCondSubTypeSrvStakeLock",
        .tp_basicsize = sizeof(PyDapChainTxOutCondObject),
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_TYPE_SUBCLASS,
        "Chain tx cond subtype srv stake lock object",
        .tp_getset = DapChainTxOutCondSubtypeSrvStakeLockGetsSetsDef,
        .tp_base = &DapChainTxOutCondObjectType,
        .tp_init = (initproc)DapChainTxOutCondSubtypeSrvStakeLock_new,
        .tp_new = PyType_GenericNew
};

PyObject *wrapping_dap_chain_net_srv_stake_lock_get_time_unlock(PyObject *self, void *closure) {
    UNUSED(closure);
    PyDateTime_IMPORT;
    PyObject *obj_ts_long =  Py_BuildValue("(k)", PVT(self).time_unlock);
    PyObject *obj_ts = PyDateTime_FromTimestamp(obj_ts_long);
    return obj_ts;
}
PyObject *wrapping_dap_chain_net_srv_stake_lock_get_flags(PyObject *self, void *closure) {
    UNUSED(closure);
    return Py_BuildValue("I", PVT(self).flags);
}
PyObject *wrapping_dap_chain_net_srv_stake_lock_get_reinvest_percent(PyObject *self, void *closure) {
    UNUSED(closure);
    uint256_t l_int_percent;
    DIV_256(PVT(self).reinvest_percent, GET_256_FROM_64(1000000000000000000ULL), &l_int_percent);
    return Py_BuildValue("b", dap_chain_uint256_to(l_int_percent));
}
PyObject *wrapping_dap_chain_net_srv_stake_lock_get_token_delegated(PyObject *self, void *closure) {
    UNUSED(closure);
    Py_RETURN_NONE;
}

int DapChainTxOutCondSubtypeSrvStakeLock_new(PyDapChainTxOutCondObject *self, PyObject *args, PyObject *kwds) {
    const char *kwlist[] = {
        "srvUID",
        "value",
        "timeStaking",
        "reinvestParcent",
        NULL
    };
    PyObject *obj_srv_uid;
    PyObject *obj_value;
    PyObject *obj_time_staking;
    PyObject *obj_reinvest_percent;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OOOO", (const char**)kwlist, &obj_srv_uid, &obj_value, &obj_time_staking, &obj_reinvest_percent)) {
        return -1;
    }
    if (!PyDapChainNetSrvUid_Check((PyDapChainNetSrvUIDObject*)obj_srv_uid)) {
        PyErr_SetString(PyExc_Exception,
                        "The first argument srvUID is set incorrectly it should be an object of type CellFrame.Services.ServiceUID.");
        return -1;
    }
    if (!DapMathObject_Check(obj_value)) {
        PyErr_SetString(PyExc_Exception,
                        "The second argument value is set incorrectly, it should be an object of type DAP.Math.");
        return -1;
    }
    if (!PyDateTime_Check(obj_time_staking)) {
        PyErr_SetString(PyExc_Exception,
                        "The third argument of timeStaking is set incorrectly, it should be an object of DateTime type.");
        return -1;
    }
    if (!DapMathObject_Check(obj_reinvest_percent)) {
        PyErr_SetString(PyExc_Exception,
                        "The fourth argument reinvestPercent is set incorrectly, it should be an object of type DAP.Math.");
        return -1;
    }
    uint64_t l_time_staking = PyDateTime_to_timestamp_uint64(obj_time_staking);
    self->out_cond = dap_chain_datum_tx_item_out_cond_create_srv_stake_lock(((PyDapChainNetSrvUIDObject *) obj_srv_uid)->net_srv_uid,
                                                           ((DapMathObject *) obj_value)->value, l_time_staking,
                                                           ((DapMathObject *) obj_reinvest_percent)->value);
    return 0;
}
