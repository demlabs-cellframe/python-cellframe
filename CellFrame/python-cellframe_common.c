#include <Python.h>
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION > 11)
#include <pytypedefs.h>
#endif
#include <patchlevel.h>
#include <frameobject.h>

#include "python-cellframe_common.h"

void _PyErr_logIt(const dap_log_level_t a_level, const char *a_tag, const char *a_msg){
    _log_it("\0", 0, a_tag, a_level, "%s", a_msg);
}

char* _PyErr_get_stacktrace(PyObject *a_obj){
    if (!a_obj){
        return "No stack trace";
    }
    PyTracebackObject *l_traceback = (PyTracebackObject*)a_obj;
    char  *s = "\tStack trace:\n";
    size_t cnt = 0;
    while (l_traceback != NULL)  {
        const char *l_name, *l_file;
#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION < 10
        l_name = l_file = "unknown";
#else
        PyCodeObject *l_code = PyFrame_GetCode(l_traceback->tb_frame);
        l_name = PyUnicode_AsUTF8(l_code->co_name);
        l_file = PyUnicode_AsUTF8(l_code->co_filename);
#endif
        int l_lineo = l_traceback->tb_lineno;
        s = dap_strdup_printf("%s\t\t(%zu) File \"%s\", line %d, in %s\n", s, cnt, l_file, l_lineo, l_name);
        l_traceback = l_traceback->tb_next;
        cnt++;
    }
    return s;
}

void python_error_in_log_it(const char *a_tag){
    PyObject *type, *value, *trackback;

    PyErr_Fetch(&type, &value, &trackback);
    PyErr_NormalizeException(&type, &value, &trackback);

    PyObject* str_exc_value = PyObject_Repr(value);
    PyObject* exect_value_str = PyUnicode_AsEncodedString(str_exc_value, "utf-8", "strict");

    const char *l_str_value = PyBytes_AS_STRING(exect_value_str);

    char *l_str = dap_strdup_printf(
            "An exception occurred while executing a Python script.\n"
            "\t%s\n%s", l_str_value ? l_str_value : "(null)",
            trackback ? _PyErr_get_stacktrace(trackback) : "(null)"
    );

    _PyErr_logIt(L_ERROR, a_tag, l_str);
    DAP_DELETE(l_str);

    PyErr_Restore(type, value, trackback);
}
