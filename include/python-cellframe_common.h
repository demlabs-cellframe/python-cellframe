#pragma once

#include "Python.h"
#include "libdap-python.h"
//#include "frameobject.h"
#include "dap_common.h"
#include "dap_strfuncs.h"

void python_error_in_log_it(const char *a_tag);
void _PyErr_logIt(const dap_log_level_t a_level, const char *a_tag, const char *a_msg);
