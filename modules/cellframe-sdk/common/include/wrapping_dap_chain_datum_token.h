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

#ifndef _WRAPPING_DAP_CHAIN_DATUM_TOKEN_
#define _WRAPPING_DAP_CHAIN_DATUM_TOKEN_

#include "Python.h"
#include "dap_chain_datum_token.h"
#include "wrapping_dap_chain_common.h"
#include "dap_tsd.h"

#ifdef __cplusplus
extern "C" {
#endif

/* DAP chain datum token */

typedef struct PyDapChainDatumToken{
    PyObject_HEAD
    dap_chain_datum_token_t *token;
    size_t token_size;
}PyDapChainDatumTokenObject;

PyObject *wrapping_dap_chain_datum_token_get_ticker(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_datum_token_get_type_str(PyObject *self, void *closure);
//PyObject *wrapping_dap_chain_datum_token_get_size(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_datum_token_get_data(PyObject *self, void *closure);

extern PyTypeObject DapChainDatumTokenObjectType;

/* ------------------------------------------- */

/* DAP chain datum token emission */

typedef struct PyDapChainDatumTokenEmission{
    PyObject_HEAD
    dap_chain_datum_token_emission_t *token_emission;
    size_t token_size;
}PyDapChainDatumTokenEmissionObject;

PyObject *wrapping_dap_chain_datum_token_emission_get_version(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_datum_token_emission_get_type_str(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_datum_token_emission_get_ticker(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_datum_token_emission_get_addr(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_datum_token_emission_get_value(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_datum_token_emission_get_nonce(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_datum_token_emission_get_data(PyObject *self, void *closure);

extern PyTypeObject DapChainDatumTokenEmissionObjectType;

/* ------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif //_WRAPPING_DAP_CHAIN_DATUM_TOKEN_
