#include "wrapping_guuid.h"
#include "libdap-python.h"

int PyCryptoGUUID_init(PyCryptoGUUIDObject *self, PyObject *argv, PyObject *kwds){
    PyObject *in_str_or_net_id = NULL;
    uint64_t service_id;
    char **kwords[] = {NULL};
    if (!PyArg_ParseTupleAndKeywords(argv, kwds, "|OK", kwords, &in_str_or_net_id, &service_id))
        return -1;
    if (!in_str_or_net_id) {
        self->guuid = dap_guuid_new();
        return 0;
    }
    if (PyUnicode_Check(in_str_or_net_id)) {
        const char *l_hex = PyUnicode_AsUTF8(in_str_or_net_id);
        self->guuid = dap_guuid_from_hex_str(l_hex);
        return 0;
    } else if (PyLong_Check(in_str_or_net_id)) {
        uint64_t l_net_id = PyLong_AsUnsignedLongLong(in_str_or_net_id);
        self->guuid = dap_guuid_compose(l_net_id, service_id);;
        return 0;
    }
    PyErr_SetString(PyExc_Exception, "The argument is set incorrectly. The constructor must be called either without arguments or with a string containing the GUUID"
                                     "in hex formatThe argument is set incorrectly. The constructor must be called either without arguments or with a string "
                                     "containing the GUUID in hex format or must accept two numbers with a network ID and a service ID.");
    return -1;
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