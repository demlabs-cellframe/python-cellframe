#ifndef CELLFRAME_SERVICES_H
#define CELLFRAME_SERVICES_H

#include <Python.h>

#ifdef __cplusplus
extern "C" {
#endif

// Get method definitions for services module
PyMethodDef* cellframe_services_get_methods(void);

#ifdef __cplusplus
}
#endif

#endif /* CELLFRAME_SERVICES_H */
