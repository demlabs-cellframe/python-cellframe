#include "wrapping_dap_chain_net_srv_xchange.h"
#include "wrapping_dap_chain_net_srv_xchange_price.h"
#include "dap_chain_net_srv_xchange.h"
#include "wrapped_dap_chain_wallet_python.h"
#include "libdap_chain_net_python.h"
#include "libdap-python.h"

PyMethodDef DapChainNetSrvXchangeMethods[] = {
        {
                "getOrders",
                wrapping_dap_chain_net_srv_xchange_get_orders,
                METH_VARARGS | METH_STATIC,
                        "The function receives a list of prices for exchange; if there are no prices, then an empty list is returned."},
        {0}
};

PyObject *wrapping_dap_chain_net_srv_xchange_get_orders(PyObject *self, PyObject *argv){
    (void)self;
    PyObject *obj_net;
    if (!PyArg_ParseTuple(argv, "O", &obj_net)) {
        return NULL;
    }
    dap_list_t *l_list_prices = dap_chain_net_srv_xchange_get_prices(((PyDapChainNetObject*)obj_net)->chain_net);
    size_t l_list_prices_count = dap_list_length(l_list_prices);
    dap_list_t *tmp = l_list_prices;
    PyObject *obj_list_price = PyList_New((Py_ssize_t)l_list_prices_count);
    for (size_t i = 0; i < l_list_prices_count; i++){
        dap_chain_net_srv_xchange_price_t *l_price = (dap_chain_net_srv_xchange_price_t*)tmp->data;
        PyDapChainNetSrvXchangeOrderObject *l_obj_price = PyObject_New(PyDapChainNetSrvXchangeOrderObject,
                                                                       &PyDapChainNetSrvXchangeOrderObjectType);
        l_obj_price->price = DAP_NEW(dap_chain_net_srv_xchange_price_t);
        memcpy(l_obj_price->price, l_price, sizeof(dap_chain_net_srv_xchange_price_t));
        PyList_Append(obj_list_price, (PyObject*)l_obj_price);
        Py_XDECREF(l_obj_price);
        tmp = tmp->next;
    }
    dap_list_free(l_list_prices);
    return obj_list_price;
}



PyTypeObject DapChainNetSrvXchangeObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.Service.Xchange",
        sizeof(PyDapChainNetSrvXchangeObject), "Object for work service xchange",
        .tp_methods = DapChainNetSrvXchangeMethods);
