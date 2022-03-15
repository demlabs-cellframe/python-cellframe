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

#ifndef _WRAPPING_DAP_SIGN_
#define _WRAPPING_DAP_SIGN_

#include <Python.h>
#include "dap_sign.h"
#include "wrapping_dap_pkey.h"

typedef struct PyDapSignType{
    PyObject_HEAD
    dap_sign_type_t *sign_type;
}PyDapSignTypeObject;

PyObject *PyDapSignType_to_str(PyObject *self);

extern PyTypeObject DapSignTypeObject_DapSignTypeObjectType;

typedef struct PyDapSign{
    PyObject_HEAD
    dap_sign_t *sign;
}PyDapSignObject;

PyObject *wrapping_dap_sign_get_type(PyObject *self, void *closure);
PyObject *wrapping_dap_sign_get_pkey(PyObject *self, void *closure);
PyObject *wrapping_dap_sign_get_pkey_hash(PyObject *self, void *closure);
PyObject *wrapping_dap_sign_get_size(PyObject *self, void *closure);

extern PyTypeObject DapSignObject_DapSignObjectType;

static bool PyDapSignObject_Check(PyObject *self){
    return PyObject_TypeCheck(self, &DapSignObject_DapSignObjectType);
}

#endif // _WRAPPING_DAP_SIGN_
