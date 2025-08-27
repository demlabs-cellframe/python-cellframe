/*
 * Authors:
 * Dmitry Gerasimov <ceo@cellframe.net>
 * DeM Labs Inc.   https://demlabs.net
 * DAP SDK  https://gitlab.demlabs.net/dap/dap-sdk
 * Copyright  (c) 2025
 * All rights reserved.

 This file is part of DAP SDK the open source project

    DAP SDK is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    DAP SDK is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with any DAP SDK based project.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "dap_common.h"
#include "json.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DAP JSON object type (wrapper around json-c)
 */
typedef struct json_object dap_json_t;

/**
 * @brief DAP JSON array type (wrapper around json-c)
 */
typedef struct json_object dap_json_array_t;

// Object creation and destruction
dap_json_t* dap_json_object_new(void);
dap_json_t* dap_json_parse_string(const char* a_json_string);
void dap_json_object_free(dap_json_t* a_json);

// Value object creation (for simple types)
dap_json_t* dap_json_object_new_int(int a_value);
dap_json_t* dap_json_object_new_string(const char* a_value);
dap_json_t* dap_json_object_new_double(double a_value);
dap_json_t* dap_json_object_new_bool(bool a_value);

// Array creation and manipulation
dap_json_array_t* dap_json_array_new(void);
void dap_json_array_free(dap_json_array_t* a_array);
int dap_json_array_add(dap_json_array_t* a_array, dap_json_t* a_item);
size_t dap_json_array_length(dap_json_array_t* a_array);
dap_json_t* dap_json_array_get_idx(dap_json_array_t* a_array, size_t a_idx);

// Object field manipulation
int dap_json_object_add_string(dap_json_t* a_json, const char* a_key, const char* a_value);
int dap_json_object_add_int(dap_json_t* a_json, const char* a_key, int a_value);
int dap_json_object_add_double(dap_json_t* a_json, const char* a_key, double a_value);
int dap_json_object_add_bool(dap_json_t* a_json, const char* a_key, bool a_value);
int dap_json_object_add_object(dap_json_t* a_json, const char* a_key, dap_json_t* a_value);
int dap_json_object_add_array(dap_json_t* a_json, const char* a_key, dap_json_array_t* a_array);

// Object field access
const char* dap_json_object_get_string(dap_json_t* a_json, const char* a_key);
int dap_json_object_get_int(dap_json_t* a_json, const char* a_key);
double dap_json_object_get_double(dap_json_t* a_json, const char* a_key);
bool dap_json_object_get_bool(dap_json_t* a_json, const char* a_key);
dap_json_t* dap_json_object_get_object(dap_json_t* a_json, const char* a_key);
dap_json_array_t* dap_json_object_get_array(dap_json_t* a_json, const char* a_key);

// String conversion
const char* dap_json_to_string(dap_json_t* a_json);
char* dap_json_to_string_pretty(dap_json_t* a_json);

// Advanced object manipulation
dap_json_t* dap_json_from_file(const char* a_file_path);
bool dap_json_object_get_ex(dap_json_t* a_json, const char* a_key, dap_json_t** a_value);
int dap_json_object_del(dap_json_t* a_json, const char* a_key);

// Extended value getters with default
const char* dap_json_object_get_string_default(dap_json_t* a_json, const char* a_key, const char* a_default);
int dap_json_object_get_int_default(dap_json_t* a_json, const char* a_key, int a_default);
int64_t dap_json_object_get_int64(dap_json_t* a_json, const char* a_key);
int64_t dap_json_object_get_int64_default(dap_json_t* a_json, const char* a_key, int64_t a_default);

// JSON type enumeration
typedef enum {
    DAP_JSON_TYPE_NULL,
    DAP_JSON_TYPE_BOOLEAN,
    DAP_JSON_TYPE_DOUBLE,
    DAP_JSON_TYPE_INT,
    DAP_JSON_TYPE_OBJECT,
    DAP_JSON_TYPE_ARRAY,
    DAP_JSON_TYPE_STRING
} dap_json_type_t;

// Type checking
bool dap_json_is_null(dap_json_t* a_json);
bool dap_json_is_string(dap_json_t* a_json);
bool dap_json_is_int(dap_json_t* a_json);
bool dap_json_is_double(dap_json_t* a_json);
bool dap_json_is_bool(dap_json_t* a_json);
bool dap_json_is_object(dap_json_t* a_json);
bool dap_json_is_array(dap_json_t* a_json);
dap_json_type_t dap_json_get_type(dap_json_t* a_json);

// Tokener functions for parsing with error handling
typedef enum {
    DAP_JSON_TOKENER_SUCCESS,
    DAP_JSON_TOKENER_ERROR_DEPTH,
    DAP_JSON_TOKENER_ERROR_PARSE_EOF,
    DAP_JSON_TOKENER_ERROR_PARSE_UNEXPECTED,
    DAP_JSON_TOKENER_ERROR_PARSE_NULL,
    DAP_JSON_TOKENER_ERROR_PARSE_BOOLEAN,
    DAP_JSON_TOKENER_ERROR_PARSE_NUMBER,
    DAP_JSON_TOKENER_ERROR_PARSE_ARRAY,
    DAP_JSON_TOKENER_ERROR_PARSE_OBJECT_KEY_NAME,
    DAP_JSON_TOKENER_ERROR_PARSE_OBJECT_KEY_SEP,
    DAP_JSON_TOKENER_ERROR_PARSE_OBJECT_VALUE_SEP,
    DAP_JSON_TOKENER_ERROR_PARSE_STRING,
    DAP_JSON_TOKENER_ERROR_PARSE_COMMENT,
    DAP_JSON_TOKENER_ERROR_SIZE
} dap_json_tokener_error_t;

dap_json_t* dap_json_tokener_parse_verbose(const char* a_str, dap_json_tokener_error_t* a_error);
const char* dap_json_tokener_error_desc(dap_json_tokener_error_t a_jerr);

// Reference counting (important for json-c compatibility)
dap_json_t* dap_json_object_get_ref(dap_json_t* a_json);

#ifdef __cplusplus
}
#endif