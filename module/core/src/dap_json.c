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

#include "dap_json.h"
#include "dap_strfuncs.h"
#include <string.h>

#define LOG_TAG "dap_json"

// Object creation and destruction
dap_json_t* dap_json_object_new(void)
{
    return json_object_new_object();
}

dap_json_t* dap_json_parse_string(const char* a_json_string)
{
    if (!a_json_string) {
        log_it(L_ERROR, "JSON string is NULL");
        return NULL;
    }
    
    enum json_tokener_error jerr;
    struct json_object *l_json = json_tokener_parse_verbose(a_json_string, &jerr);
    
    if (jerr != json_tokener_success) {
        log_it(L_ERROR, "Failed to parse JSON: %s", json_tokener_error_desc(jerr));
        return NULL;
    }
    
    return l_json;
}

void dap_json_object_free(dap_json_t* a_json)
{
    if (a_json) {
        json_object_put(a_json);
    }
}

// Value object creation (for simple types)
dap_json_t* dap_json_object_new_int(int a_value)
{
    return json_object_new_int(a_value);
}

dap_json_t* dap_json_object_new_string(const char* a_value)
{
    if (!a_value) {
        log_it(L_ERROR, "String value is NULL");
        return NULL;
    }
    
    return json_object_new_string(a_value);
}

dap_json_t* dap_json_object_new_double(double a_value)
{
    return json_object_new_double(a_value);
}

dap_json_t* dap_json_object_new_bool(bool a_value)
{
    return json_object_new_boolean(a_value);
}

// Array creation and manipulation
dap_json_array_t* dap_json_array_new(void)
{
    return json_object_new_array();
}

void dap_json_array_free(dap_json_array_t* a_array)
{
    if (a_array) {
        json_object_put(a_array);
    }
}

int dap_json_array_add(dap_json_array_t* a_array, dap_json_t* a_item)
{
    if (!a_array || !a_item) {
        log_it(L_ERROR, "Array or item is NULL");
        return -1;
    }
    
    return json_object_array_add(a_array, a_item);
}

size_t dap_json_array_length(dap_json_array_t* a_array)
{
    if (!a_array) {
        return 0;
    }
    
    return json_object_array_length(a_array);
}

dap_json_t* dap_json_array_get_idx(dap_json_array_t* a_array, size_t a_idx)
{
    if (!a_array) {
        return NULL;
    }
    
    return json_object_array_get_idx(a_array, a_idx);
}

// Object field manipulation
int dap_json_object_add_string(dap_json_t* a_json, const char* a_key, const char* a_value)
{
    if (!a_json || !a_key || !a_value) {
        log_it(L_ERROR, "JSON object, key or value is NULL");
        return -1;
    }
    
    struct json_object *l_string = json_object_new_string(a_value);
    if (!l_string) {
        log_it(L_ERROR, "Failed to create JSON string object");
        return -1;
    }
    
    return json_object_object_add(a_json, a_key, l_string);
}

int dap_json_object_add_int(dap_json_t* a_json, const char* a_key, int a_value)
{
    if (!a_json || !a_key) {
        log_it(L_ERROR, "JSON object or key is NULL");
        return -1;
    }
    
    struct json_object *l_int = json_object_new_int(a_value);
    if (!l_int) {
        log_it(L_ERROR, "Failed to create JSON int object");
        return -1;
    }
    
    return json_object_object_add(a_json, a_key, l_int);
}

int dap_json_object_add_double(dap_json_t* a_json, const char* a_key, double a_value)
{
    if (!a_json || !a_key) {
        log_it(L_ERROR, "JSON object or key is NULL");
        return -1;
    }
    
    struct json_object *l_double = json_object_new_double(a_value);
    if (!l_double) {
        log_it(L_ERROR, "Failed to create JSON double object");
        return -1;
    }
    
    return json_object_object_add(a_json, a_key, l_double);
}

int dap_json_object_add_bool(dap_json_t* a_json, const char* a_key, bool a_value)
{
    if (!a_json || !a_key) {
        log_it(L_ERROR, "JSON object or key is NULL");
        return -1;
    }
    
    struct json_object *l_bool = json_object_new_boolean(a_value);
    if (!l_bool) {
        log_it(L_ERROR, "Failed to create JSON boolean object");
        return -1;
    }
    
    return json_object_object_add(a_json, a_key, l_bool);
}

int dap_json_object_add_object(dap_json_t* a_json, const char* a_key, dap_json_t* a_value)
{
    if (!a_json || !a_key || !a_value) {
        log_it(L_ERROR, "JSON object, key or value is NULL");
        return -1;
    }
    
    // Increase reference count since json-c will manage the object
    json_object_get(a_value);
    
    return json_object_object_add(a_json, a_key, a_value);
}

int dap_json_object_add_array(dap_json_t* a_json, const char* a_key, dap_json_array_t* a_array)
{
    if (!a_json || !a_key || !a_array) {
        log_it(L_ERROR, "JSON object, key or array is NULL");
        return -1;
    }
    
    // Increase reference count since json-c will manage the array
    json_object_get(a_array);
    
    return json_object_object_add(a_json, a_key, a_array);
}

// Object field access
const char* dap_json_object_get_string(dap_json_t* a_json, const char* a_key)
{
    if (!a_json || !a_key) {
        return NULL;
    }
    
    struct json_object *l_obj = NULL;
    if (!json_object_object_get_ex(a_json, a_key, &l_obj)) {
        return NULL;
    }
    
    return json_object_get_string(l_obj);
}

int dap_json_object_get_int(dap_json_t* a_json, const char* a_key)
{
    if (!a_json || !a_key) {
        return 0;
    }
    
    struct json_object *l_obj = NULL;
    if (!json_object_object_get_ex(a_json, a_key, &l_obj)) {
        return 0;
    }
    
    return json_object_get_int(l_obj);
}

double dap_json_object_get_double(dap_json_t* a_json, const char* a_key)
{
    if (!a_json || !a_key) {
        return 0.0;
    }
    
    struct json_object *l_obj = NULL;
    if (!json_object_object_get_ex(a_json, a_key, &l_obj)) {
        return 0.0;
    }
    
    return json_object_get_double(l_obj);
}

bool dap_json_object_get_bool(dap_json_t* a_json, const char* a_key)
{
    if (!a_json || !a_key) {
        return false;
    }
    
    struct json_object *l_obj = NULL;
    if (!json_object_object_get_ex(a_json, a_key, &l_obj)) {
        return false;
    }
    
    return json_object_get_boolean(l_obj);
}

dap_json_t* dap_json_object_get_object(dap_json_t* a_json, const char* a_key)
{
    if (!a_json || !a_key) {
        return NULL;
    }
    
    struct json_object *l_obj = NULL;
    if (!json_object_object_get_ex(a_json, a_key, &l_obj)) {
        return NULL;
    }
    
    return l_obj;
}

dap_json_array_t* dap_json_object_get_array(dap_json_t* a_json, const char* a_key)
{
    if (!a_json || !a_key) {
        return NULL;
    }
    
    struct json_object *l_obj = NULL;
    if (!json_object_object_get_ex(a_json, a_key, &l_obj)) {
        return NULL;
    }
    
    return l_obj;
}

// String conversion
const char* dap_json_to_string(dap_json_t* a_json)
{
    if (!a_json) {
        return NULL;
    }
    
    return json_object_to_json_string(a_json);
}

char* dap_json_to_string_pretty(dap_json_t* a_json)
{
    if (!a_json) {
        return NULL;
    }
    
    const char *l_json_str = json_object_to_json_string_ext(a_json, JSON_C_TO_STRING_PRETTY);
    if (!l_json_str) {
        return NULL;
    }
    
    return strdup(l_json_str);
}

// Type checking
bool dap_json_is_null(dap_json_t* a_json)
{
    if (!a_json) {
        return true;
    }
    
    return json_object_is_type(a_json, json_type_null);
}

bool dap_json_is_string(dap_json_t* a_json)
{
    if (!a_json) {
        return false;
    }
    
    return json_object_is_type(a_json, json_type_string);
}

bool dap_json_is_int(dap_json_t* a_json)
{
    if (!a_json) {
        return false;
    }
    
    return json_object_is_type(a_json, json_type_int);
}

bool dap_json_is_double(dap_json_t* a_json)
{
    if (!a_json) {
        return false;
    }
    
    return json_object_is_type(a_json, json_type_double);
}

bool dap_json_is_bool(dap_json_t* a_json)
{
    if (!a_json) {
        return false;
    }
    
    return json_object_is_type(a_json, json_type_boolean);
}

bool dap_json_is_object(dap_json_t* a_json)
{
    if (!a_json) {
        return false;
    }
    
    return json_object_is_type(a_json, json_type_object);
}

bool dap_json_is_array(dap_json_t* a_json)
{
    if (!a_json) {
        return false;
    }
    
    return json_object_is_type(a_json, json_type_array);
}

// Advanced object manipulation
dap_json_t* dap_json_from_file(const char* a_file_path)
{
    if (!a_file_path) {
        log_it(L_ERROR, "File path is NULL");
        return NULL;
    }
    
    return json_object_from_file(a_file_path);
}

bool dap_json_object_get_ex(dap_json_t* a_json, const char* a_key, dap_json_t** a_value)
{
    if (!a_json || !a_key || !a_value) {
        return false;
    }
    
    return json_object_object_get_ex(a_json, a_key, a_value);
}

int dap_json_object_del(dap_json_t* a_json, const char* a_key)
{
    if (!a_json || !a_key) {
        log_it(L_ERROR, "JSON object or key is NULL");
        return -1;
    }
    
    json_object_object_del(a_json, a_key);
    return 0;
}

// Extended value getters with default
const char* dap_json_object_get_string_default(dap_json_t* a_json, const char* a_key, const char* a_default)
{
    const char* l_value = dap_json_object_get_string(a_json, a_key);
    return l_value ? l_value : a_default;
}

int dap_json_object_get_int_default(dap_json_t* a_json, const char* a_key, int a_default)
{
    if (!a_json || !a_key) {
        return a_default;
    }
    
    struct json_object *l_obj = NULL;
    if (!json_object_object_get_ex(a_json, a_key, &l_obj)) {
        return a_default;
    }
    
    return json_object_get_int(l_obj);
}

int64_t dap_json_object_get_int64(dap_json_t* a_json, const char* a_key)
{
    if (!a_json || !a_key) {
        return 0;
    }
    
    struct json_object *l_obj = NULL;
    if (!json_object_object_get_ex(a_json, a_key, &l_obj)) {
        return 0;
    }
    
    return json_object_get_int64(l_obj);
}

int64_t dap_json_object_get_int64_default(dap_json_t* a_json, const char* a_key, int64_t a_default)
{
    if (!a_json || !a_key) {
        return a_default;
    }
    
    struct json_object *l_obj = NULL;
    if (!json_object_object_get_ex(a_json, a_key, &l_obj)) {
        return a_default;
    }
    
    return json_object_get_int64(l_obj);
}

dap_json_type_t dap_json_get_type(dap_json_t* a_json)
{
    if (!a_json) {
        return DAP_JSON_TYPE_NULL;
    }
    
    enum json_type l_type = json_object_get_type(a_json);
    
    switch (l_type) {
        case json_type_null:
            return DAP_JSON_TYPE_NULL;
        case json_type_boolean:
            return DAP_JSON_TYPE_BOOLEAN;
        case json_type_double:
            return DAP_JSON_TYPE_DOUBLE;
        case json_type_int:
            return DAP_JSON_TYPE_INT;
        case json_type_object:
            return DAP_JSON_TYPE_OBJECT;
        case json_type_array:
            return DAP_JSON_TYPE_ARRAY;
        case json_type_string:
            return DAP_JSON_TYPE_STRING;
        default:
            return DAP_JSON_TYPE_NULL;
    }
}

// Tokener functions for parsing with error handling
dap_json_t* dap_json_tokener_parse_verbose(const char* a_str, dap_json_tokener_error_t* a_error)
{
    if (!a_str) {
        if (a_error) {
            *a_error = DAP_JSON_TOKENER_ERROR_PARSE_NULL;
        }
        return NULL;
    }
    
    enum json_tokener_error l_jerr;
    struct json_object *l_json = json_tokener_parse_verbose(a_str, &l_jerr);
    
    if (a_error) {
        // Map json-c error codes to our enum
        switch (l_jerr) {
            case json_tokener_success:
                *a_error = DAP_JSON_TOKENER_SUCCESS;
                break;
            case json_tokener_error_depth:
                *a_error = DAP_JSON_TOKENER_ERROR_DEPTH;
                break;
            case json_tokener_error_parse_eof:
                *a_error = DAP_JSON_TOKENER_ERROR_PARSE_EOF;
                break;
            case json_tokener_error_parse_unexpected:
                *a_error = DAP_JSON_TOKENER_ERROR_PARSE_UNEXPECTED;
                break;
            case json_tokener_error_parse_null:
                *a_error = DAP_JSON_TOKENER_ERROR_PARSE_NULL;
                break;
            case json_tokener_error_parse_boolean:
                *a_error = DAP_JSON_TOKENER_ERROR_PARSE_BOOLEAN;
                break;
            case json_tokener_error_parse_number:
                *a_error = DAP_JSON_TOKENER_ERROR_PARSE_NUMBER;
                break;
            case json_tokener_error_parse_array:
                *a_error = DAP_JSON_TOKENER_ERROR_PARSE_ARRAY;
                break;
            case json_tokener_error_parse_object_key_name:
                *a_error = DAP_JSON_TOKENER_ERROR_PARSE_OBJECT_KEY_NAME;
                break;
            case json_tokener_error_parse_object_key_sep:
                *a_error = DAP_JSON_TOKENER_ERROR_PARSE_OBJECT_KEY_SEP;
                break;
            case json_tokener_error_parse_object_value_sep:
                *a_error = DAP_JSON_TOKENER_ERROR_PARSE_OBJECT_VALUE_SEP;
                break;
            case json_tokener_error_parse_string:
                *a_error = DAP_JSON_TOKENER_ERROR_PARSE_STRING;
                break;
            case json_tokener_error_parse_comment:
                *a_error = DAP_JSON_TOKENER_ERROR_PARSE_COMMENT;
                break;
            default:
                *a_error = DAP_JSON_TOKENER_ERROR_SIZE;
                break;
        }
    }
    
    return l_json;
}

const char* dap_json_tokener_error_desc(dap_json_tokener_error_t a_jerr)
{
    // Map our error codes back to json-c for description
    enum json_tokener_error l_jerr;
    
    switch (a_jerr) {
        case DAP_JSON_TOKENER_SUCCESS:
            l_jerr = json_tokener_success;
            break;
        case DAP_JSON_TOKENER_ERROR_DEPTH:
            l_jerr = json_tokener_error_depth;
            break;
        case DAP_JSON_TOKENER_ERROR_PARSE_EOF:
            l_jerr = json_tokener_error_parse_eof;
            break;
        case DAP_JSON_TOKENER_ERROR_PARSE_UNEXPECTED:
            l_jerr = json_tokener_error_parse_unexpected;
            break;
        case DAP_JSON_TOKENER_ERROR_PARSE_NULL:
            l_jerr = json_tokener_error_parse_null;
            break;
        case DAP_JSON_TOKENER_ERROR_PARSE_BOOLEAN:
            l_jerr = json_tokener_error_parse_boolean;
            break;
        case DAP_JSON_TOKENER_ERROR_PARSE_NUMBER:
            l_jerr = json_tokener_error_parse_number;
            break;
        case DAP_JSON_TOKENER_ERROR_PARSE_ARRAY:
            l_jerr = json_tokener_error_parse_array;
            break;
        case DAP_JSON_TOKENER_ERROR_PARSE_OBJECT_KEY_NAME:
            l_jerr = json_tokener_error_parse_object_key_name;
            break;
        case DAP_JSON_TOKENER_ERROR_PARSE_OBJECT_KEY_SEP:
            l_jerr = json_tokener_error_parse_object_key_sep;
            break;
        case DAP_JSON_TOKENER_ERROR_PARSE_OBJECT_VALUE_SEP:
            l_jerr = json_tokener_error_parse_object_value_sep;
            break;
        case DAP_JSON_TOKENER_ERROR_PARSE_STRING:
            l_jerr = json_tokener_error_parse_string;
            break;
        case DAP_JSON_TOKENER_ERROR_PARSE_COMMENT:
            l_jerr = json_tokener_error_parse_comment;
            break;
        default:
            l_jerr = json_tokener_error_size;
            break;
    }
    
    return json_tokener_error_desc(l_jerr);
}

// Reference counting (important for json-c compatibility)
dap_json_t* dap_json_object_get_ref(dap_json_t* a_json)
{
    if (!a_json) {
        return NULL;
    }
    
    return json_object_get(a_json);
}
