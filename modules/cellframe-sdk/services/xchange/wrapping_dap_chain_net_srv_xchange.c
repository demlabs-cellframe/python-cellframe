#include "wrapping_dap_chain_net_srv_xchange.h"
#include "wrapping_dap_chain_net_srv_xchange_price.h"
#include "dap_chain_net_srv_xchange.h"
#include "libdap_chain_net_python.h"
#include "libdap-python.h"

PyMethodDef DapChainNetSrvXchangeMethods[] = {
        {
            "getPrices",
            wrapping_dap_chain_net_srv_xchange_get_prices,
            METH_VARARGS | METH_STATIC,
            "The function receives a list of prices for exchange; if there are no prices, then an empty list is returned."},
        {NULL, NULL, 0, NULL}
};

PyObject *wrapping_dap_chain_net_srv_xchange_get_prices(PyObject *self, PyObject *argv){
    (void)self;
    PyObject *obj_net;
    if (!PyArg_ParseTuple(argv, "O", &obj_net)) {
        return NULL;
    }
    dap_list_t *l_list_prices = dap_chain_net_srv_xchange_get_prices(((PyDapChainNetObject*)obj_net)->chain_net);
    dap_list_t *tmp = l_list_prices;
    PyObject *obj_list_price = PyList_New(0);
    while (tmp) {
        dap_chain_net_srv_xchange_price_t *l_price = (dap_chain_net_srv_xchange_price_t*)tmp->data;
        PyObject *l_obj_price = wrapping_dap_chain_net_srv_xchange_price_create_object(l_price);
        PyList_Append(obj_list_price, l_obj_price);
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
