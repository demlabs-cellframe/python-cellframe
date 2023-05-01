#include "wrapping_dap_chain_net_srv_stake_pos_delegate.h"
#include "libdap_chain_net_python.h"
#include "wrapping_dap_hash.h"
#include "dap_chain_net_srv_stake_pos_delegate.h"

PyObject *wrapping_dap_chain_net_srv_stake_check_validator(PyObject *self, PyObject *argv){
    (void)self;
    PyObject *obj_chain_net;
    PyObject *obj_tx_hash;
    uint16_t time_connect;
    uint16_t time_response;
    if (!PyArg_ParseTuple(argv, "OOHH", &obj_chain_net, &obj_tx_hash, &time_connect, &time_response)) {
        return NULL;
    }
    if (!PyDapChainNet_Check((PyDapChainNetObject*)obj_chain_net)) {
        PyErr_SetString(PyExc_AttributeError, "");
        return NULL;
    }
    if (!PyDapHashFast_Check((PyDapHashFastObject*)obj_tx_hash)) {
        PyErr_SetString(PyExc_AttributeError, "");
        return NULL;
    }
    dap_stream_ch_chain_rnd_t l_out = {0};
    bool res = dap_chain_net_srv_stake_check_validator(((PyDapChainNetObject*)obj_chain_net)->chain_net,
                                            ((PyDapHashFastObject*)obj_tx_hash)->hash_fast,
                                            &l_out, time_connect, time_response);
    if (res)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

static PyMethodDef PyDapChainNetSrvStakePosDelegateMethods[] = {
        {
            "checkValidator",
            wrapping_dap_chain_net_srv_stake_check_validator,
            METH_VARARGS | METH_STATIC,
            ""
            },
        {}
};

PyTypeObject PyDapChainNetSrvStakePosDelegateObjectType = DAP_PY_TYPE_OBJECT(
            "CellFrame.Services.StakePosDelegate",
            sizeof(PyDapChainNetSrvStakePosDelegateObject),
            "CellFrame.Services.StakePosDelegate object",
            .tp_methods = PyDapChainNetSrvStakePosDelegateMethods
        );
