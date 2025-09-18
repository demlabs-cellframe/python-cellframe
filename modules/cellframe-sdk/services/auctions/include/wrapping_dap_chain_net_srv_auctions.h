#include "Python.h"
#include "dap_chain_net.h"
#include "dap_chain_net_srv_auctions.h"

typedef struct PyDapChainNetSrvAuctionsObject{
    PyObject_HEAD
    dap_chain_net_t *net;
}PyDapChainNetSrvAuctionsObject;

extern PyTypeObject PyDapChainNetSrvAuctionsObjectType; 