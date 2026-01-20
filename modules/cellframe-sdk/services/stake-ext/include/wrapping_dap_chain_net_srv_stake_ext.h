#include "Python.h"
#include "dap_chain_net.h"
#include "dap_chain_net_srv_stake_ext.h"

typedef struct PyDapChainNetSrvStakeExtObject{
    PyObject_HEAD
    dap_chain_net_t *net;
}PyDapChainNetSrvStakeExtObject;

extern PyTypeObject PyDapChainNetSrvStakeExtObjectType; 
