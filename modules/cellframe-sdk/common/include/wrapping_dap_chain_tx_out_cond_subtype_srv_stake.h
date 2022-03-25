/*
 * Authors:
 * Alexey V. Stratulat <alexey.stratulat@demlabs.net>
 * DeM Labs Inc.   https://demlabs.net
 * CellFrame       https://cellframe.net
 * Sources         https://gitlab.demlabs.net/cellframe
 * Copyright  (c) 2017-2021
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
#include "wrapping_dap_chain_tx_out_cond.h"
#include "wrapping_dap_chain_common.h"

#ifdef __cplusplus
extern "C"{
#endif

int DapChainTxOutCondSubTypeSrvStake_init(PyDapChainTxOutCondObject* self, PyObject* args, PyObject *kwds);

PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_stake_get_uid(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_stake_get_addr(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_stake_get_value(PyObject *self, void *closure);

extern PyTypeObject DapChainTxOutCondSubTypeSrvStakeObjectType;

#ifdef __cplusplus
};
#endif
