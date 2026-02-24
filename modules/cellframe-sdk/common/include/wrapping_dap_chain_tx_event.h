/*
 * Authors:
 * Roman Khlopkov <roman.khlopkov@demlabs.net>
 * DeM Labs Inc.   https://demlabs.net
 * CellFrame       https://cellframe.net
 * Sources         https://gitlab.demlabs.net/cellframe
 * Copyright  (c) 2025
 * All rights reserved.

 This file is part of CellFrame (PythonCellFrame) the open source project

    DAP (Distributed Applications Platform) is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    DAP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with any DAP based project.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _WRAPPING_DAP_CHAIN_TX_EVENT_
#define _WRAPPING_DAP_CHAIN_TX_EVENT_

#include <Python.h>
#include "dap_chain_datum_tx_event.h"
#include "wrapping_dap_hash.h"
#include "dap_chain_datum_tx_items.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct PyDapChainTxEvent{
    PyObject_HEAD
    dap_chain_tx_item_event_t *tx_event;
}PyDapChainTxEventObject;

int PyDapChainTxEvent_init(PyObject *self, PyObject *args, PyObject *kwds);

PyObject *wrapping_dap_chain_tx_event_get_type(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_event_get_version(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_event_get_group_name(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_event_get_group_size(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_event_get_event_type(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_event_get_timestamp(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_event_get_srv_uid(PyObject *self, void *closure);

// Static methods for event types
PyObject *TX_EVENT_TYPE_STAKE_EXT_STARTED_PY(PyObject *self, PyObject *args);
PyObject *TX_EVENT_TYPE_STAKE_EXT_BID_PLACED_PY(PyObject *self, PyObject *args);
PyObject *TX_EVENT_TYPE_STAKE_EXT_ENDED_PY(PyObject *self, PyObject *args);
PyObject *TX_EVENT_TYPE_STAKE_EXT_CANCELLED_PY(PyObject *self, PyObject *args);

extern PyTypeObject DapChainTxEventObjectType;

#ifdef __cplusplus
}
#endif

#endif //_WRAPPING_DAP_CHAIN_TX_EVENT_ 
