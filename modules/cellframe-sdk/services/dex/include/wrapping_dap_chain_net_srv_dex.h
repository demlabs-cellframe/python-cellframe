/*
 * Authors:
 * DeM Labs Inc.   https://demlabs.net
 * CellFrame       https://cellframe.net
 * Copyright  (c) 2017-2025
 * License: GNU General Public License
 */

#pragma once

#include <Python.h>
#include "dap_chain_net_srv_dex.h"
#include "wrapping_dap_chain_net_srv_dex_order.h"

typedef struct PyDapChainNetSrvDex {
    PyObject_HEAD
} PyDapChainNetSrvDexObject;

// Static methods
PyObject *wrapping_dap_chain_net_srv_dex_get_orders(PyObject *self, PyObject *args, PyObject *kwargs);
PyObject *wrapping_dap_chain_net_srv_dex_get_pairs(PyObject *self, PyObject *args);

extern PyTypeObject DapChainNetSrvDexObjectType;
