#pragma once

#include <Python.h>
#include "dap_common.h"
#include "dap_strfuncs.h"
#include "dap_string.h"
#include"dap_file_utils.h"

static PyGILState_STATE GIL_state;

char* getCharFromPyObject(PyObject *obj);

/**
 * @brief writeSectorToFile
 * @param file
 * @param sector
 * @param exception
 */
void writeSectorToFile(FILE *file, PyObject *sector);

/**
 * @brief writeSectorsToFile
 * @param file
 * @param sectors
 * @param count
 */
void writeSectorsToFile(FILE *file, PyObject *sectors, int count, ...);

/**
 * @brief gen_config_files
 * @param cfgDir
 * @param app_name
 * @param cfg_JSON
 * @return
 */
int gen_config_files(const char *cfgDir, const char *app_name, PyObject *cfg_JSON);

