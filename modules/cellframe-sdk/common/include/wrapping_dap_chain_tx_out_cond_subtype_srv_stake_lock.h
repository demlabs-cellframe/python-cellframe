/*
* Authors:
* Alexey V. Stratulat <alexey.stratulat@demlabs.net>
* DeM Labs Inc.   https://demlabs.net
* CellFrame       https://cellframe.net
* Sources         https://gitlab.demlabs.net/cellframe
* Copyright  (c) 2017-2022
* All rights reserved.

This file is part of DAP (Deus Applications Prototypes) the open source project

DAP (Deus Applicaions Prototypes) is free software: you can redistribute it and/or modify
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

#pragma once

#include <Python.h>
#include "dap_chain_net_srv_stake_lock.h"

typedef struct PyDapChainTxOutCondSubTypeSrvStakeLock{
    PyObject_HEAD
    dap_chain_tx_out_cond_t *out_cond;
}PyDapChainTxOutCondSubTypeSrvStakeLock_Object;

extern PyTypeObject DapChainTxOutCondSubTypeSrvStakeLockObjectType;

PyObject *wrapping_dap_chain_net_srv_stake_lock_get_time_unlock(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_stake_lock_get_flags(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_stake_lock_get_reinvest_percent(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_stake_lock_get_token_delegated(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_stake_lock_get_pkey_delegated(PyObject *self, void *closure);