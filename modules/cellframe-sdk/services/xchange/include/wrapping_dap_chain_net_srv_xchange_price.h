#include <Python.h>
#include "dap_chain_net_srv_xchange.h"

typedef struct PyDapChainNetSrvXchangePrice {
    PyObject_HEAD
    dap_chain_net_srv_xchange_price_t *price;
}PyDapChainNetSrvXchangePriceObject;

extern PyTypeObject PyDapChainNetSrvXchangePriceObjectType;
