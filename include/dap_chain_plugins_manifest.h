#ifndef _DAP_CHAIN_PLUGINS_MANIFEST_
#define _DAP_CHAIN_PLUGINS_MANIFEST_
#include "dap_common.h"
#include "dap_list.h"
#include "dap_strfuncs.h"
#include "stdio.h"
#include "json-c/json_object.h"
#include "json-c/json_tokener.h"

#ifdef __cplusplus
extern "C"{
#endif

#undef LOG_TAG
#define LOG_TAG "dap_chain_plugins_manifest"

enum data_in_manifest{
    NAME,
    VERSION,
    DEPENDENCYS,
    AUTHOR,
    DESCRIPTION
};

typedef struct manifest{
    char *name;
    char *version;
    dap_list_t *dependencys;
    char *author;
    char *description;
}manifest_t;

manifest_t *dap_chain_plugins_manifest_new(const char *name, const char *version, const dap_list_t *dep, const char *author,
                         const char *description);
void dap_chain_plugins_manifest_free(manifest_t *manifest);
manifest_t* dap_chain_plugins_add_manifest_from_file(const char *file_path);

static dap_list_t *manifests;

void dap_chain_plugins_manifest_list_create();
manifest_t *dap_chain_plugins_manifest_get_list(size_t index);
void dap_chain_plugins_manifest_list_free();
void dap_chain_plugins_manifest_list_add_manifest(manifest_t *manifest);
bool dap_chain_plugins_manifest_list_add_from_file(const char *file_path);


#ifdef __cplusplus
}
#endif
#endif
