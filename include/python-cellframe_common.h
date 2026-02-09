#pragma once

#include "Python.h"
#include "libdap-python.h"
//#include "frameobject.h"
#include "dap_common.h"
#include "dap_strfuncs.h"

void python_error_in_log_it(const char *a_tag);
#define _PyErr_logIt(a_level, a_tag, _fmt, ...) (void)\
    ((a_level) >= g_dap_log_level && (_log_it((a_level), _LOG_LVL(a_level) "[%s] " _fmt, a_tag, ##__VA_ARGS__), 1))
PyObject *python_get_config_item(dap_config_t* a_config, const char *a_section, const char *a_key, PyObject *obj_default);

#define Py_BuildNone Py_BuildValue("")