#pragma once

#include "dap_common.h"
#include "http_status_code.h"
#include "Python.h"

#undef LOG_TAG
#define LOG_TAG "wrapping_http_status_code"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyHTTPStatusCode{
    PyObject_HEAD
    http_status_code_t http_status;
}PyHttpStatusCodeObject;

static http_status_code_t _get_status_code_from_init(int a_code){
    switch (a_code) {
//    case
    /*####### 1xx - Informational #######*/
    case 100: return Http_Status_OK;
    case 101: return Http_Status_SwitchingProtocols;
    case 102: return Http_Status_Processing;
    case 103: return Http_Status_EarlyHints;

        /*####### 2xx - Successful #######*/
    case 200: return Http_Status_OK;
    case 201: return Http_Status_Created;
    case 202: return Http_Status_Accepted;
    case 203: return Http_Status_NonAuthoritativeInformation;
    case 204: return Http_Status_NoContent;
    case 205: return Http_Status_ResetContent;
    case 206: return Http_Status_PartialContent;
    case 207: return Http_Status_MultiStatus;
    case 208: return Http_Status_AlreadyReported;
    case 226: return Http_Status_IMUsed;

        /*####### 3xx - Redirection #######*/
    case 300: return Http_Status_MultipleChoices;
    case 301: return Http_Status_MovedPermanently;
    case 302: return Http_Status_Found;
    case 303: return Http_Status_SeeOther;
    case 304: return Http_Status_NotModified;
    case 305: return Http_Status_UseProxy;
    case 307: return Http_Status_TemporaryRedirect;
    case 308: return Http_Status_PermanentRedirect;

        /*####### 4xx - Client Error #######*/
    case 400: return Http_Status_BadRequest;
    case 401: return Http_Status_Unauthorized;
    case 402: return Http_Status_PaymentRequired;
    case 403: return Http_Status_Forbidden;
    case 404: return Http_Status_NotFound;
    case 405: return Http_Status_MethodNotAllowed;
    case 406: return Http_Status_NotAcceptable;
    case 407: return Http_Status_ProxyAuthenticationRequired;
    case 408: return Http_Status_RequestTimeout;
    case 409: return Http_Status_Conflict;
    case 410: return Http_Status_Gone;
    case 411: return Http_Status_LengthRequired;
    case 412: return Http_Status_PreconditionFailed;
    case 413: return Http_Status_PayloadTooLarge;
    case 414: return Http_Status_URITooLong;
    case 415: return Http_Status_UnsupportedMediaType;
    case 416: return Http_Status_RangeNotSatisfiable;
    case 417: return Http_Status_ExpectationFailed;
    case 418: return Http_Status_ImATeapot;
    case 422: return Http_Status_UnprocessableEntity;
    case 423: return Http_Status_Locked;
    case 424: return Http_Status_FailedDependency;
    case 426: return Http_Status_UpgradeRequired;
    case 428: return Http_Status_PreconditionRequired;
    case 429: return Http_Status_TooManyRequests;
    case 431: return Http_Status_RequestHeaderFieldsTooLarge;
    case 451: return Http_Status_UnavailableForLegalReasons;

        /*####### 5xx - Server Error #######*/
    case 500: return Http_Status_InternalServerError;
    case 501: return Http_Status_NotImplemented;
    case 502: return Http_Status_BadGateway;
    case 503: return Http_Status_ServiceUnavailable;
    case 504: return Http_Status_GatewayTimeout;
    case 505: return Http_Status_HTTPVersionNotSupported;
    case 506: return Http_Status_VariantAlsoNegotiates;
    case 507: return Http_Status_InsufficientStorage;
    case 508: return Http_Status_LoopDetected;
    case 510: return Http_Status_NotExtended;
    default:
        log_it(L_ERROR, "Unknown status code %d. Returned default status: Internal Server Error", a_code);
        return Http_Status_InternalServerError;
    }
}

static PyObject *wrapping_http_status_code_set_py(PyObject *self, PyObject *args){
    PyObject *l_obj_status_code;
    if (!PyArg_ParseTuple(args, "O", &l_obj_status_code)){
        return NULL;
    }
    if (PyLong_Check(l_obj_status_code)){
        int code = PyLong_AsLong(l_obj_status_code);
        ((PyHttpStatusCodeObject*)self)->http_status = _get_status_code_from_init(code);
    }
    return Py_None;
}

static PyObject *wrapping_http_code_set_ok(PyObject *self, PyObject *args){
    (void)args;
    ((PyHttpStatusCodeObject*)self)->http_status = Http_Status_OK;
    return Py_None;
}

static PyObject *wrapping_http_code_set_bad_request(PyObject *self, PyObject *args){
    (void)args;
    ((PyHttpStatusCodeObject*)self)->http_status = Http_Status_BadRequest;
    return Py_None;
}
static PyMethodDef PyDapHttpCodeMethods[] = {
    {"set", wrapping_http_status_code_set_py, METH_VARARGS, "Set a status code for a HTTP response"},
    {"OK", wrapping_http_code_set_ok, METH_NOARGS, "Set an \"OK\" status code"},
    {"BadRequest", wrapping_http_code_set_bad_request, METH_NOARGS, "Set a \"Bad request\" status code"},
    {NULL, NULL, 0, NULL}
};



static PyTypeObject HTTPCode_HTTPCodeType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.HTTPCode",             /* tp_name */
    sizeof(PyHttpStatusCodeObject),                   /* tp_basicsize */
    0,                                                 /* tp_itemsize */
    0,                                                 /* tp_dealloc */
    0,                                                 /* tp_print */
    0,                                                 /* tp_getattr */
    0,                                                 /* tp_setattr */
    0,                                                 /* tp_reserved */
    0,                                                 /* tp_repr */
    0,                                                 /* tp_as_number */
    0,                                                 /* tp_as_sequence */
    0,                                                 /* tp_as_mapping */
    0,                                                 /* tp_hash  */
    0,                                                 /* tp_call */
    0,                                                 /* tp_str */
    0,                                                 /* tp_getattro */
    0,                                                 /* tp_setattro */
    0,                                                 /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                           /* tp_flags */
    "HTTP code object",                         /* tp_doc */
    0,		                                       /* tp_traverse */
    0,                        		               /* tp_clear */
    0,		                                       /* tp_richcompare */
    0,                        		               /* tp_weaklistoffset */
    0,		                                       /* tp_iter */
    0,                        		               /* tp_iternext */
    PyDapHttpCodeMethods,                   /* tp_methods */
    0,                                                 /* tp_members */
    0,                                                 /* tp_getset */
    0,                                                 /* tp_base */
    0,                                                 /* tp_dict */
    0,                                                 /* tp_descr_get */
    0,                                                 /* tp_descr_set */
    0,                                                 /* tp_dictoffset */
    0,                                                 /* tp_init */
    0,                                                 /* tp_alloc */
    PyType_GenericNew,                                 /* tp_new */
};

#undef LOG_TAG

#ifdef __cplusplus
}
#endif
