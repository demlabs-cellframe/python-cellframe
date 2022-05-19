#include "python-cellframe_common.h"
#include <frameobject.h>

void _PyErr_logIt(const dap_log_level_t a_level, const char *a_tag, const char *a_msg){
    _log_it(a_tag, a_level, (char*)a_msg);
}

char* _PyErr_get_stacktrace(PyObject *a_obj){
    assert(PyTraceBack_Check(a_obj));
    PyTracebackObject *l_traceback = (PyTracebackObject*)a_obj;
    char  *s = "\tStack trace:\n";
    size_t cnt = 0;
    while (l_traceback != NULL)  {
        PyCodeObject *l_code = l_traceback->tb_frame->f_code;
        const char *l_name = PyUnicode_AsUTF8(l_code->co_name);
        const char *l_file = PyUnicode_AsUTF8(l_code->co_filename);
        int l_lineo = ((PyTracebackObject *) a_obj)->tb_lineno;
        s = dap_strdup_printf("%s\t\t(%zu) File \"%s\", line %d, in %s\n", s, cnt, l_file, l_lineo, l_name);
        l_traceback = l_traceback->tb_next;
        cnt++;
    }
    return s;
}

void python_error_in_log_it(const char *a_tag){
    
    PyObject *type, *value, *trackback;
    
    // When using PyErr_Restore() there is no need to use Py_XDECREF for these 3 pointers
    //PyErr_Restore(pyExcType, pyExcValue, pyExcTraceback);

    PyErr_Fetch(&type, &value, &trackback);
    PyErr_NormalizeException(&type, &value, &trackback);

    PyObject* str_exc_value = PyObject_Repr(value);
    PyObject* pyExcValueStr = PyUnicode_AsEncodedString(str_exc_value, "utf-8", "Error ~");
        
    const char *l_str_value = PyBytes_AS_STRING(pyExcValueStr);;
    
    _PyErr_logIt(L_ERROR, a_tag, dap_strdup_printf(
            "An exception occurred while executing a Python script.\n"
            "\t%s\n%s", l_str_value, _PyErr_get_stacktrace(trackback)
            ));

    PyErr_Restore(type, value, trackback);
}
