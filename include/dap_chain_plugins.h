/*
* Authors:
* Alexey V. Stratulat <alexey.stratulat@demlabs.net>
* DeM Labs Inc.   https://demlabs.net
* DeM Labs Open source community https://gitlab.demlabs.net/cellframe/libdap-plugins-python
* Copyright  (c) 2017-2020
* All rights reserved.

This file is part of DAP (Deus Applications Prototypes) the open source project

   DAP (Deus Applicaions Prototypes) is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   DAP is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with any DAP based project.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _DAP_CHAIN_PLUGINS_
#define _DAP_CHAIN_PLUGINS_
#include <Python.h>
#include "dap_config.h"
#include "dap_common.h"
#include "dap_file_utils.h"
#include "dap_chain_plugins_manifest.h"
#include "dap_chain_plugins_list.h"
#include "python-cellframe.h"
#include "dap_chain_plugins_command.h"

#ifdef __cplusplus
extern "C"{
#endif

static PyObject *s_sys_path;

const char *s_plugins_root_path;

int dap_chain_plugins_init();
void dap_chain_plugins_deinit();
void dap_chain_plugins_loading();
void dap_chain_plugins_load_plugin(const char *a_dir_path, const char *a_name);
int dap_chain_plugins_reload_plugin(const char * a_name_plugin);

#ifdef __cplusplus
}
#endif

#endif //_DAP_CHAIN_PLUGINS_
