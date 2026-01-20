/*
 * Authors:
 * Dmitriy Gerasimov <naeper@demlabs.net>
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
#include <stddef.h>
#include <stdbool.h>

// Forward declarations
typedef struct dap_chain_datum_decree dap_chain_datum_decree_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Python Decree Object Type
 * Wraps dap_chain_datum_decree_t for Python access
 */
typedef struct {
    PyObject_HEAD
    dap_chain_datum_decree_t *decree;
    size_t decree_size;
    bool owned;  // True if we own the decree and should free it
} PyDapDecreeObject;

/**
 * @brief Create a Python DapDecree object from C decree
 * @param a_decree C decree pointer (borrowed or owned)
 * @param a_size Size of decree in bytes
 * @param a_owned If true, Python object owns and will free the decree
 * @return New Python DapDecree object or NULL on error
 */
PyObject* PyDapDecree_from_decree(dap_chain_datum_decree_t *a_decree, size_t a_size, bool a_owned);

/**
 * @brief Initialize Governance Python bindings module
 * Includes: Decree type + Governance Manager
 * @return 0 on success, negative on error
 */
int cellframe_governance_init(PyObject *module);

/**
 * @brief Initialize Governance Manager type
 * @return 0 on success, negative on error
 */
int cellframe_governance_manager_init(PyObject *module);

/**
 * @brief Cleanup Governance Manager (Python handlers)
 */
void cellframe_governance_manager_cleanup(void);

#ifdef __cplusplus
}
#endif
