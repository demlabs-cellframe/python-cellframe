#include "Python.h"
#include "dap_chain_net.h"
#include "dap_chain_net_srv_auctions.h"

typedef struct PyDapChainNetSrvAuctionsObject{
    PyObject_HEAD
    dap_chain_net_t *net;
}PyDapChainNetSrvAuctionsObject;

// Transaction creation functions
PyObject *wrapping_dap_chain_net_srv_auctions_bid_tx_create(PyObject *self, PyObject *argv);
PyObject *wrapping_dap_chain_net_srv_auctions_bid_withdraw_tx_create(PyObject *self, PyObject *argv);

// Information retrieval functions
PyObject *wrapping_dap_chain_net_srv_auctions_get_info(PyObject *self, PyObject *argv);
PyObject *wrapping_dap_chain_net_srv_auctions_get_list(PyObject *self, PyObject *argv);
PyObject *wrapping_dap_chain_net_srv_auctions_get_stats(PyObject *self, PyObject *argv);
PyObject *wrapping_dap_chain_net_srv_auctions_get_events(PyObject *self, PyObject *argv);

extern PyTypeObject PyDapChainNetSrvAuctionsObjectType; 