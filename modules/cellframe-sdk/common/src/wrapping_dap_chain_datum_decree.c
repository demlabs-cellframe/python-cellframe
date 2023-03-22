#include "wrapping_dap_chain_datum_decree.h"
#include "libdap-python.h"
#include "datetime.h"

#define PVT(a)((PyDapChainDatumDecreeObject*)a)

PyObject* wrapping_dap_chain_datum_decree_get_ts_created(PyObject *self, void* closure) {
    (void)closure;
    PyDateTime_IMPORT;
    PyObject *obj_ts_float = PyLong_FromLong(PVT(self)->decree->header.ts_created);
    PyObject *obj_ts = Py_BuildValue("(O)", obj_ts_float);
    PyObject *obj_dt = PyDateTime_FromTimestamp(obj_ts);
    return obj_dt;
}
PyObject* wrapping_dap_chain_datum_decree_get_type(PyObject *self, void* closure);
PyObject* wrapping_dap_chain_datum_decree_get_sub_type(PyObject *self, void* closure);
//PyObject* wrapping_dap_chain_datum_decree_get_data(PyObject *self, void* closure);
//PyObject* wrapping_dap_chain_datum_decree_get_signs(PyObject *self, void* closure);

PyGetSetDef  DapChainDatumDecreeGetSet[] = {
        {"type", (getter)wrapping_dap_chain_datum_decree_get_ts_created, NULL, NULL, NULL},
        {}
};

PyMethodDef DapChainDatumDecreeMethods[] = {
        {}
};

PyTypeObject DapChainDatumDecreeObjectType = DAP_PY_TYPE_OBJECT("CellFrame.Chain.DatumDecree", sizeof(PyDapChainDatumDecreeObject),
                                                                "Chain datum type decree object", .tp_getset = DapChainDatumDecreeGetSet,
                                                                .tp_methods = DapChainDatumDecreeMethods);
