/*
 * Authors:
 * Dmitry Gerasimov <ceo@cellframe.net>
 * DeM Labs Inc.   https://demlabs.net
 * Copyright  (c) 2025
 * All rights reserved.

 This file is part of Python-Cellframe

    Python-Cellframe is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Python-Cellframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with any Python-Cellframe based project.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <Python.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize DapJSON Python bindings
 * @return PyObject* - DapJSON type object or NULL on error
 */
PyObject *cf_dap_json_init(void);

#ifdef __cplusplus
}
#endif
