#pragma once

#include "Python.h"
#include "libdap-python.h"
//#include "frameobject.h"
#include "dap_common.h"
#include "dap_strfuncs.h"

void python_error_in_log_it(const char *a_tag);
void _PyErr_logIt(const dap_log_level_t a_level, const char *a_tag, const char *a_msg);
PyObject *python_get_config_item(dap_config_t* a_config, const char *a_section, const char *a_key, PyObject *obj_default);

#define Py_BuildNone Py_BuildValue("")