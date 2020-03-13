#ifndef _DAP_CHAIN_PLUGINS_MANIFEST_
#define _DAP_CHAIN_PLUGINS_MANIFEST_
#include "dap_common.h"
#include "dap_strfuncs.h"
#include "stdio.h"
#include "json-c/json_object.h"
#include "json-c/json_tokener.h"
#include "utlist.h"

#ifdef __cplusplus
extern "C"{
#endif

#undef LOG_TAG
#define LOG_TAG "dap_chain_plugins_manifest"

typedef struct dap_chain_plugins_list_char{
    char *value;
    struct dap_chain_plugins_list_char *next;
}dap_chain_plugins_list_char_t;

typedef struct dap_list_manifest{
    char *name;
    char *version;
    char *author;
    char *description;
    dap_chain_plugins_list_char_t *dependencys;
    struct dap_list_manifest *next;
}dap_chain_plugins_list_manifest_t;

dap_chain_plugins_list_manifest_t* manifests;

dap_chain_plugins_list_manifest_t *dap_chain_plugins_manifest_new(const char *name, const char *version, const dap_chain_plugins_list_char_t *dep, const char *author,
                         const char *description);

void dap_chain_plugins_manifest_list_create();

dap_chain_plugins_list_manifest_t* dap_chain_plugins_manifests_get_list();

dap_chain_plugins_list_manifest_t* dap_chain_plugins_add_manifest_from_file(const char *file_path);

bool dap_chain_plugins_manifest_list_add_from_file(const char *path_file);

#ifdef __cplusplus
}
#endif
#endif
