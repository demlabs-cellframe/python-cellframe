/*
 * Authors:
 * Alexey Stratulat <alexey.stratulat@demlabs.net>
 * DeM Labs Inc.   https://demlabs.net
 * CellFrame       https://cellframe.net
 * Sources         https://gitlab.demlabs.net/cellframe
 * Copyright  (c) 2017-2020
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
#include "Python.h"
#include "dap_enc_key.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyCryptoKeyType{
    PyObject_HEAD
}PyCryptoKeyTypeObject;

PyObject *get_ENC_KEY_TYPE_IAES();
PyObject *get_ENC_KEY_TYPE_OAES();
PyObject *get_ENC_KEY_TYPE_RLWE_NEWHOPE();
PyObject *get_ENC_KEY_TYPE_SIDH_CLN16();
PyObject *get_ENC_KEY_TYPE_DEFEO();
PyObject *get_ENC_KEY_TYPE_MSRLN();
PyObject *get_ENC_KEY_TYPE_MSRLN16();
PyObject *get_ENC_KEY_TYPE_RLWE_BCNS15();
PyObject *get_ENC_KEY_TYPE_LWE_FRODO();
PyObject *get_ENC_KEY_TYPE_SIDH_IQC_REF();
PyObject *get_ENC_KEY_TYPE_CODE_MCBITS();
PyObject *get_ENC_KEY_TYPE_NTRU();
PyObject *get_ENC_KEY_TYPE_MLWE_KYBER();
PyObject *get_ENC_KEY_TYPE_SIG_PICNIC();
PyObject *get_ENC_KEY_TYPE_SIG_BLISS();
PyObject *get_ENC_KEY_TYPE_SIG_TESLA();
PyObject *get_ENC_KEY_TYPE_SIG_DILITHIUM();
PyObject *get_ENC_KEY_TYPE_NULL();

extern PyTypeObject DapCryptoKeyTypeObjectType;

#ifdef __cplusplus
}
#endif
