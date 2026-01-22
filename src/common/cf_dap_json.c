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

#include "cf_dap_json.h"
#include "dap_json.h"
#include "dap_common.h"
#include "dap_math_ops.h"
#include <Python.h>
#include <structmember.h>

#define LOG_TAG "cf_dap_json"

// ============================================================================
// Python DapJSON Type Definition
// ============================================================================

typedef struct {
    PyObject_HEAD
    dap_json_t *json;
    bool owned;  // True if we own the json object and should free it
} PyDapJSONObject;

static PyTypeObject PyDapJSONObjectType;

// ============================================================================
// Helper Functions
// ============================================================================

/**
 * @brief Create a new Python DapJSON object wrapping a C dap_json_t
 * @param a_json The C dap_json_t pointer (can be borrowed or owned)
 * @param a_owned If true, the Python object will own and free the json on dealloc
 * @return New Python DapJSON object or NULL on error
 */
static PyObject* PyDapJSON_from_json(dap_json_t *a_json, bool a_owned) {
    if (!a_json) {
        Py_RETURN_NONE;
    }
    
    PyDapJSONObject *self = (PyDapJSONObject *)PyDapJSONObjectType.tp_alloc(&PyDapJSONObjectType, 0);
    if (!self) {
        if (a_owned) {
            dap_json_object_free(a_json);
        }
        return NULL;
    }
    
    self->json = a_json;
    self->owned = a_owned;
    return (PyObject *)self;
}

// ============================================================================
// Python DapJSON Object Methods
// ============================================================================

/**
 * @brief Constructor for DapJSON object
 */
static PyObject *PyDapJSON_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    PyDapJSONObject *self = (PyDapJSONObject *)type->tp_alloc(type, 0);
    if (self) {
        self->json = dap_json_object_new();
        self->owned = true;
        
        if (!self->json) {
            Py_DECREF(self);
            PyErr_SetString(PyExc_MemoryError, "Failed to create dap_json object");
            return NULL;
        }
    }
    return (PyObject *)self;
}

/**
 * @brief Destructor for DapJSON object
 */
static void PyDapJSON_dealloc(PyDapJSONObject *self) {
    if (self->json && self->owned) {
        dap_json_object_free(self->json);
    }
    Py_TYPE(self)->tp_free((PyObject *)self);
}

/**
 * @brief Parse JSON string
 * @param json_str: str - JSON string to parse
 * @return DapJSON object or None on error
 */
static PyObject *PyDapJSON_parse(PyObject *self, PyObject *args) {
    const char *json_str;
    if (!PyArg_ParseTuple(args, "s", &json_str)) {
        return NULL;
    }
    
    dap_json_t *json = dap_json_parse_string(json_str);
    if (!json) {
        PyErr_SetString(PyExc_ValueError, "Failed to parse JSON string");
        return NULL;
    }
    
    return PyDapJSON_from_json(json, true);
}

/**
 * @brief Convert JSON to string
 * @return str - JSON as string
 */
static PyObject *PyDapJSON_to_string(PyDapJSONObject *self, PyObject *args) {
    char *json_str = dap_json_to_string(self->json);
    if (!json_str) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to convert JSON to string");
        return NULL;
    }
    
    PyObject *result = Py_BuildValue("s", json_str);
    DAP_DELETE(json_str);
    return result;
}

/**
 * @brief Convert JSON to pretty string
 * @return str - JSON as pretty-formatted string
 */
static PyObject *PyDapJSON_to_string_pretty(PyDapJSONObject *self, PyObject *args) {
    char *json_str = dap_json_to_string_pretty(self->json);
    if (!json_str) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to convert JSON to pretty string");
        return NULL;
    }
    
    PyObject *result = Py_BuildValue("s", json_str);
    DAP_DELETE(json_str);
    return result;
}

/**
 * @brief Add string field to JSON object
 * @param key: str - Field key
 * @param value: str - Field value
 */
static PyObject *PyDapJSON_add_string(PyDapJSONObject *self, PyObject *args) {
    const char *key, *value;
    if (!PyArg_ParseTuple(args, "ss", &key, &value)) {
        return NULL;
    }
    
    if (dap_json_object_add_string(self->json, key, value) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to add string field");
        return NULL;
    }
    
    Py_RETURN_NONE;
}

/**
 * @brief Add integer field to JSON object
 * @param key: str - Field key
 * @param value: int - Field value
 */
static PyObject *PyDapJSON_add_int(PyDapJSONObject *self, PyObject *args) {
    const char *key;
    int value;
    if (!PyArg_ParseTuple(args, "si", &key, &value)) {
        return NULL;
    }
    
    if (dap_json_object_add_int(self->json, key, value) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to add int field");
        return NULL;
    }
    
    Py_RETURN_NONE;
}

/**
 * @brief Add int64 field to JSON object
 * @param key: str - Field key
 * @param value: int - Field value (Python long)
 */
static PyObject *PyDapJSON_add_int64(PyDapJSONObject *self, PyObject *args) {
    const char *key;
    long long value;
    if (!PyArg_ParseTuple(args, "sL", &key, &value)) {
        return NULL;
    }
    
    if (dap_json_object_add_int64(self->json, key, (int64_t)value) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to add int64 field");
        return NULL;
    }
    
    Py_RETURN_NONE;
}

/**
 * @brief Add uint64 field to JSON object
 * @param key: str - Field key
 * @param value: int - Field value (Python long, unsigned)
 */
static PyObject *PyDapJSON_add_uint64(PyDapJSONObject *self, PyObject *args) {
    const char *key;
    unsigned long long value;
    if (!PyArg_ParseTuple(args, "sK", &key, &value)) {
        return NULL;
    }
    
    if (dap_json_object_add_uint64(self->json, key, (uint64_t)value) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to add uint64 field");
        return NULL;
    }
    
    Py_RETURN_NONE;
}

/**
 * @brief Add double field to JSON object
 * @param key: str - Field key
 * @param value: float - Field value
 */
static PyObject *PyDapJSON_add_double(PyDapJSONObject *self, PyObject *args) {
    const char *key;
    double value;
    if (!PyArg_ParseTuple(args, "sd", &key, &value)) {
        return NULL;
    }
    
    if (dap_json_object_add_double(self->json, key, value) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to add double field");
        return NULL;
    }
    
    Py_RETURN_NONE;
}

/**
 * @brief Add bool field to JSON object
 * @param key: str - Field key
 * @param value: bool - Field value
 */
static PyObject *PyDapJSON_add_bool(PyDapJSONObject *self, PyObject *args) {
    const char *key;
    PyObject *py_value;
    if (!PyArg_ParseTuple(args, "sO", &key, &py_value)) {
        return NULL;
    }
    
    bool value = PyObject_IsTrue(py_value);
    if (dap_json_object_add_bool(self->json, key, value) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to add bool field");
        return NULL;
    }
    
    Py_RETURN_NONE;
}

/**
 * @brief Add null field to JSON object
 * @param key: str - Field key
 */
static PyObject *PyDapJSON_add_null(PyDapJSONObject *self, PyObject *args) {
    const char *key;
    if (!PyArg_ParseTuple(args, "s", &key)) {
        return NULL;
    }
    
    if (dap_json_object_add_null(self->json, key) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to add null field");
        return NULL;
    }
    
    Py_RETURN_NONE;
}

/**
 * @brief Add object field to JSON object
 * @param key: str - Field key
 * @param value: DapJSON - Field value (another JSON object)
 */
static PyObject *PyDapJSON_add_object(PyDapJSONObject *self, PyObject *args) {
    const char *key;
    PyDapJSONObject *value_obj;
    if (!PyArg_ParseTuple(args, "sO!", &key, &PyDapJSONObjectType, &value_obj)) {
        return NULL;
    }
    
    // Get a reference to the value object to keep it alive
    dap_json_t *value_ref = dap_json_object_ref(value_obj->json);
    if (dap_json_object_add_object(self->json, key, value_ref) != 0) {
        dap_json_object_free(value_ref);
        PyErr_SetString(PyExc_RuntimeError, "Failed to add object field");
        return NULL;
    }
    
    Py_RETURN_NONE;
}

/**
 * @brief Get string field from JSON object
 * @param key: str - Field key
 * @return str or None if not found
 */
static PyObject *PyDapJSON_get_string(PyDapJSONObject *self, PyObject *args) {
    const char *key;
    if (!PyArg_ParseTuple(args, "s", &key)) {
        return NULL;
    }
    
    const char *value = dap_json_object_get_string(self->json, key);
    if (!value) {
        Py_RETURN_NONE;
    }
    
    return Py_BuildValue("s", value);
}

/**
 * @brief Get int field from JSON object
 * @param key: str - Field key
 * @return int or 0 if not found
 */
static PyObject *PyDapJSON_get_int(PyDapJSONObject *self, PyObject *args) {
    const char *key;
    if (!PyArg_ParseTuple(args, "s", &key)) {
        return NULL;
    }
    
    int value = dap_json_object_get_int(self->json, key);
    return Py_BuildValue("i", value);
}

/**
 * @brief Get int64 field from JSON object
 * @param key: str - Field key
 * @return int (Python long) or 0 if not found
 */
static PyObject *PyDapJSON_get_int64(PyDapJSONObject *self, PyObject *args) {
    const char *key;
    if (!PyArg_ParseTuple(args, "s", &key)) {
        return NULL;
    }
    
    int64_t value = dap_json_object_get_int64(self->json, key);
    return Py_BuildValue("L", (long long)value);
}

/**
 * @brief Get uint64 field from JSON object
 * @param key: str - Field key
 * @return int (Python long, unsigned) or 0 if not found
 */
static PyObject *PyDapJSON_get_uint64(PyDapJSONObject *self, PyObject *args) {
    const char *key;
    if (!PyArg_ParseTuple(args, "s", &key)) {
        return NULL;
    }
    
    uint64_t value = dap_json_object_get_uint64(self->json, key);
    return Py_BuildValue("K", (unsigned long long)value);
}

/**
 * @brief Get double field from JSON object
 * @param key: str - Field key
 * @return float or 0.0 if not found
 */
static PyObject *PyDapJSON_get_double(PyDapJSONObject *self, PyObject *args) {
    const char *key;
    if (!PyArg_ParseTuple(args, "s", &key)) {
        return NULL;
    }
    
    double value = dap_json_object_get_double(self->json, key);
    return Py_BuildValue("d", value);
}

/**
 * @brief Get bool field from JSON object
 * @param key: str - Field key
 * @return bool or False if not found
 */
static PyObject *PyDapJSON_get_bool(PyDapJSONObject *self, PyObject *args) {
    const char *key;
    if (!PyArg_ParseTuple(args, "s", &key)) {
        return NULL;
    }
    
    bool value = dap_json_object_get_bool(self->json, key);
    return PyBool_FromLong(value);
}

/**
 * @brief Get object field from JSON object
 * @param key: str - Field key
 * @return DapJSON or None if not found
 */
static PyObject *PyDapJSON_get_object(PyDapJSONObject *self, PyObject *args) {
    const char *key;
    if (!PyArg_ParseTuple(args, "s", &key)) {
        return NULL;
    }
    
    dap_json_t *value = dap_json_object_get_object(self->json, key);
    if (!value) {
        Py_RETURN_NONE;
    }
    
    // Borrowed reference - don't free
    return PyDapJSON_from_json(value, false);
}

/**
 * @brief Check if key exists in JSON object
 * @param key: str - Field key
 * @return bool - True if key exists
 */
static PyObject *PyDapJSON_has_key(PyDapJSONObject *self, PyObject *args) {
    const char *key;
    if (!PyArg_ParseTuple(args, "s", &key)) {
        return NULL;
    }
    
    bool has_key = dap_json_object_has_key(self->json, key);
    return PyBool_FromLong(has_key);
}

/**
 * @brief Delete key from JSON object
 * @param key: str - Field key
 */
static PyObject *PyDapJSON_del_key(PyDapJSONObject *self, PyObject *args) {
    const char *key;
    if (!PyArg_ParseTuple(args, "s", &key)) {
        return NULL;
    }
    
    if (dap_json_object_del(self->json, key) != 0) {
        PyErr_SetString(PyExc_KeyError, "Key not found");
        return NULL;
    }
    
    Py_RETURN_NONE;
}

/**
 * @brief Get JSON type
 * @return str - Type name: "null", "bool", "int", "double", "string", "object", "array"
 */
static PyObject *PyDapJSON_get_type(PyDapJSONObject *self, PyObject *args) {
    dap_json_type_t type = dap_json_get_type(self->json);
    
    const char *type_str;
    switch (type) {
        case DAP_JSON_TYPE_NULL:    type_str = "null"; break;
        case DAP_JSON_TYPE_BOOLEAN: type_str = "bool"; break;
        case DAP_JSON_TYPE_INT:     type_str = "int"; break;
        case DAP_JSON_TYPE_DOUBLE:  type_str = "double"; break;
        case DAP_JSON_TYPE_STRING:  type_str = "string"; break;
        case DAP_JSON_TYPE_OBJECT:  type_str = "object"; break;
        case DAP_JSON_TYPE_ARRAY:   type_str = "array"; break;
        default:                    type_str = "unknown"; break;
    }
    
    return Py_BuildValue("s", type_str);
}

/**
 * @brief Create new array
 * @return DapJSON - New empty array
 */
static PyObject *PyDapJSON_array_new(PyObject *cls, PyObject *args) {
    dap_json_t *array = dap_json_array_new();
    if (!array) {
        PyErr_SetString(PyExc_MemoryError, "Failed to create JSON array");
        return NULL;
    }
    
    return PyDapJSON_from_json(array, true);
}

/**
 * @brief Add item to array
 * @param item: DapJSON - Item to add
 */
static PyObject *PyDapJSON_array_add(PyDapJSONObject *self, PyObject *args) {
    PyDapJSONObject *item_obj;
    if (!PyArg_ParseTuple(args, "O!", &PyDapJSONObjectType, &item_obj)) {
        return NULL;
    }
    
    // Get a reference to keep the item alive
    dap_json_t *item_ref = dap_json_object_ref(item_obj->json);
    if (dap_json_array_add(self->json, item_ref) != 0) {
        dap_json_object_free(item_ref);
        PyErr_SetString(PyExc_RuntimeError, "Failed to add item to array");
        return NULL;
    }
    
    Py_RETURN_NONE;
}

/**
 * @brief Get array length
 * @return int - Array length
 */
static PyObject *PyDapJSON_array_length(PyDapJSONObject *self, PyObject *args) {
    size_t length = dap_json_array_length(self->json);
    return Py_BuildValue("n", length);
}

/**
 * @brief Get array item by index
 * @param index: int - Item index
 * @return DapJSON or None if index out of bounds
 */
static PyObject *PyDapJSON_array_get(PyDapJSONObject *self, PyObject *args) {
    Py_ssize_t index;
    if (!PyArg_ParseTuple(args, "n", &index)) {
        return NULL;
    }
    
    if (index < 0) {
        PyErr_SetString(PyExc_IndexError, "Array index cannot be negative");
        return NULL;
    }
    
    dap_json_t *item = dap_json_array_get_idx(self->json, (size_t)index);
    if (!item) {
        PyErr_SetString(PyExc_IndexError, "Array index out of range");
        return NULL;
    }
    
    // Borrowed reference - don't free
    return PyDapJSON_from_json(item, false);
}

// ============================================================================
// Python DapJSON Method Definitions
// ============================================================================

static PyMethodDef PyDapJSON_methods[] = {
    {"parse", (PyCFunction)PyDapJSON_parse, METH_VARARGS | METH_STATIC, "Parse JSON string"},
    {"to_string", (PyCFunction)PyDapJSON_to_string, METH_NOARGS, "Convert to JSON string"},
    {"to_string_pretty", (PyCFunction)PyDapJSON_to_string_pretty, METH_NOARGS, "Convert to pretty JSON string"},
    
    // Object methods
    {"add_string", (PyCFunction)PyDapJSON_add_string, METH_VARARGS, "Add string field"},
    {"add_int", (PyCFunction)PyDapJSON_add_int, METH_VARARGS, "Add int field"},
    {"add_int64", (PyCFunction)PyDapJSON_add_int64, METH_VARARGS, "Add int64 field"},
    {"add_uint64", (PyCFunction)PyDapJSON_add_uint64, METH_VARARGS, "Add uint64 field"},
    {"add_double", (PyCFunction)PyDapJSON_add_double, METH_VARARGS, "Add double field"},
    {"add_bool", (PyCFunction)PyDapJSON_add_bool, METH_VARARGS, "Add bool field"},
    {"add_null", (PyCFunction)PyDapJSON_add_null, METH_VARARGS, "Add null field"},
    {"add_object", (PyCFunction)PyDapJSON_add_object, METH_VARARGS, "Add object field"},
    
    {"get_string", (PyCFunction)PyDapJSON_get_string, METH_VARARGS, "Get string field"},
    {"get_int", (PyCFunction)PyDapJSON_get_int, METH_VARARGS, "Get int field"},
    {"get_int64", (PyCFunction)PyDapJSON_get_int64, METH_VARARGS, "Get int64 field"},
    {"get_uint64", (PyCFunction)PyDapJSON_get_uint64, METH_VARARGS, "Get uint64 field"},
    {"get_double", (PyCFunction)PyDapJSON_get_double, METH_VARARGS, "Get double field"},
    {"get_bool", (PyCFunction)PyDapJSON_get_bool, METH_VARARGS, "Get bool field"},
    {"get_object", (PyCFunction)PyDapJSON_get_object, METH_VARARGS, "Get object field"},
    
    {"has_key", (PyCFunction)PyDapJSON_has_key, METH_VARARGS, "Check if key exists"},
    {"del_key", (PyCFunction)PyDapJSON_del_key, METH_VARARGS, "Delete key"},
    {"get_type", (PyCFunction)PyDapJSON_get_type, METH_NOARGS, "Get JSON type"},
    
    // Array methods
    {"array_new", (PyCFunction)PyDapJSON_array_new, METH_NOARGS | METH_STATIC, "Create new array"},
    {"array_add", (PyCFunction)PyDapJSON_array_add, METH_VARARGS, "Add item to array"},
    {"array_length", (PyCFunction)PyDapJSON_array_length, METH_NOARGS, "Get array length"},
    {"array_get", (PyCFunction)PyDapJSON_array_get, METH_VARARGS, "Get array item by index"},
    
    {NULL, NULL, 0, NULL}
};

// ============================================================================
// Python DapJSON Type Definition
// ============================================================================

static PyTypeObject PyDapJSONObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "CellFrame.DapJSON",
    .tp_doc = "DAP JSON object wrapper",
    .tp_basicsize = sizeof(PyDapJSONObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = PyDapJSON_new,
    .tp_dealloc = (destructor)PyDapJSON_dealloc,
    .tp_methods = PyDapJSON_methods,
};

// ============================================================================
// Module Initialization
// ============================================================================

PyObject *cf_dap_json_init(void) {
    if (PyType_Ready(&PyDapJSONObjectType) < 0) {
        return NULL;
    }
    
    Py_INCREF(&PyDapJSONObjectType);
    return (PyObject *)&PyDapJSONObjectType;
}
