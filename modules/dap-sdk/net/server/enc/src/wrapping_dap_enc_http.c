#include "wrapping_dap_enc_http.h"

PyObject *enc_http_reply_py(PyObject *self, PyObject *args){
    PyObject *l_obj_bytes;
    if (!PyArg_ParseTuple(args, "O", &l_obj_bytes)){
        return NULL;
    }
    size_t l_bytes_size = (size_t)PyBytes_Size(l_obj_bytes);
    void *l_bytes = PyBytes_AsString(l_obj_bytes);
    size_t l_len = enc_http_reply(((PyDapEncServerObject*)self)->enc_http_delegate, l_bytes, l_bytes_size);
    return PyLong_FromLong(l_len);
}

PyObject *enc_http_request_decode_py(PyObject *self, PyObject *args){
    (void)self;
    PyObject *obj_sh;
    if (!PyArg_ParseTuple(args, "O", &obj_sh)){
        return NULL;
    }
    PyDapEncServerObject *obj_enc_delegate = PyObject_NEW(PyDapEncServerObject, &DapEncServer_DapEncServerType);
    PyObject_Dir((PyObject*)obj_enc_delegate);
    obj_enc_delegate->enc_http_delegate = enc_http_request_decode(((PyDapHttpSimpleObject*)obj_sh)->sh);
    return Py_BuildValue("O", obj_enc_delegate);
}

PyObject *enc_http_is_null_py(PyObject *self, PyObject *args){
    (void)args;
    if (((PyDapEncServerObject*)self)->enc_http_delegate)
        return Py_BuildValue("O", Py_False);
    else
        return Py_BuildValue("O", Py_True);
}

PyObject *enc_http_reply_encode_py(PyObject *self, PyObject *args){
    PyObject *l_obj_sh;
    if (!PyArg_ParseTuple(args, "O", &l_obj_sh)){
        return NULL;
    }
    enc_http_reply_encode(((PyDapHttpSimpleObject*)l_obj_sh)->sh, ((PyDapEncServerObject*)self)->enc_http_delegate);
    return Py_BuildValue("(O)", Py_None);
}

void enc_http_delegate_delete_py(PyObject *self){
    PyTypeObject *tp = Py_TYPE(self);
    if (((PyDapEncServerObject*)self)->enc_http_delegate){
        enc_http_delegate_delete(((PyDapEncServerObject*)self)->enc_http_delegate);
    }
    tp->tp_free(self);
    Py_DECREF(self);
}

PyObject *enc_http_add_proc_py(PyObject *self, PyObject *args){
    (void)self;
    PyObject *obj_server;
    const char *HTTP_URL;
    if (!PyArg_ParseTuple(args, "O|s", &obj_server, &HTTP_URL))
        return NULL;
    enc_http_add_proc(DAP_HTTP(((PyDapServerObject*)obj_server)->t_server), HTTP_URL);
    return PyLong_FromLong(0);
}

PyObject *enc_http_get_action_py(PyDapEncServerObject *self, void *clouser){
    (void)clouser;
    return Py_BuildValue("s", self->enc_http_delegate->action);
}

PyObject *enc_http_get_request_py(PyDapEncServerObject *self, void *clouser){
    (void)clouser;
    if (self->enc_http_delegate->request){
        return PyBytes_FromString(self->enc_http_delegate->request);
    }else{
        return Py_None;
    }
}

PyObject *enc_http_get_url_path_py(PyDapEncServerObject *self, void *clouser){
    (void)clouser;
    return  Py_BuildValue("s", self->enc_http_delegate->url_path);
}
PyObject *enc_http_get_in_query_py(PyDapEncServerObject *self, void *clouser){
    (void)clouser;
    return  Py_BuildValue("s", self->enc_http_delegate->in_query);
}
