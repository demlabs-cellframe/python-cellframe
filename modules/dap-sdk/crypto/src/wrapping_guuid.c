#include "wrapping_guuid.h"
#include "libdap-python.h"

int PyCryptoGUUID_init(PyCryptoGUUIDObject *self, PyObject *argv, PyObject *kwds){
    PyObject *in = NULL;
    char **kwords[] = {NULL};
    if (!PyArg_ParseTupleAndKeywords(argv, kwds, "|O", kwords, &in))
        return -1;
    if (!in) {
        self->guuid = dap_guuid_new();
        return 0;
    }
    if (!PyUnicode_Check(in)) {
        PyErr_SetString(PyExc_Exception, "The argument is set incorrectly. The constructor must be called either without arguments or with a string containing the GUUID in hex format.");
        return -1;
    }
    const char *l_hex = PyUnicode_AsUTF8(in);
    self->guuid = dap_guuid_from_hex_str(l_hex);
    return 0;
}

PyObject *PyCryptoGUUID_toStr(PyCryptoGUUIDObject *self){
    const char *l_str = dap_guuid_to_hex_str(self->guuid);
    return Py_BuildValue("s", l_str);
}

// PyObject *PyCryptoGUUID_compare(PyCryptoGUUIDObject *self, PyObject *other, int op) {
//     if (!PyObject_TypeCheck(other, &PyCryptoGUUIDObjectType)){
//         return Py_NotImplemented;
//     }
//     switch (op)
//     {
//     case Py_EQ:
        
//         /* code */
//         break;
//     case Py_NE:
//         break;
//     default:
//         return Py_NotImplemented;
//     }
// }

PyTypeObject PyCryptoGUUIDObjectType = DAP_PY_TYPE_OBJECT("DAP.Crypto.GUUID", sizeof(PyCryptoGUUIDObject),
                                                          "GUUID for using dap-sdk and cellframe-sdk",
                                                          .tp_str = (reprfunc)PyCryptoGUUID_toStr,
                                                          .tp_init = (initproc)PyCryptoGUUID_init);