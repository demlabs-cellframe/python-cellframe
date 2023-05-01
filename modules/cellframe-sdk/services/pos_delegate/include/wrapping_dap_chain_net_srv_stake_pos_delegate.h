#include "Python.h"

typedef struct PyDapChainNetSrvStakePosDelegate{
    PyObject_HEAD
}PyDapChainNetSrvStakePosDelegateObject;

PyObject *wrapping_dap_chain_net_srv_stake_check_validator(PyObject *self, PyObject *argv);

extern PyTypeObject PyDapChainNetSrvStakePosDelegateObjectType;
