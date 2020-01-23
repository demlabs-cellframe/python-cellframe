#include "dap_chain_plugins_manifest.h"

dap_list_t *JSON_array_to_dap_list(json_object *obj){
    int len = json_object_array_length(obj);
    dap_list_t *list = dap_list_alloc();
    json_object *j_data;
    char *data;
    for (int i = 0; i < len; i++){
        j_data = json_object_array_get_idx(obj, i);
        data = dap_strdup(json_object_get_string(j_data));
        dap_list_append(list, data);
    }
    return list;
}

manifest_t *dap_chain_plugins_manifest_new(const char *name, const char *version, const dap_list_t *dep, const char *author,
                         const char *description){
    manifest_t *manifest = DAP_NEW(manifest_t);
    manifest->name = dap_strdup(name);
    manifest->version = dap_strdup(version);
    manifest->dependencys = dap_list_copy(dep);
    manifest->author = dap_strdup(author);
    manifest->description = dap_strdup(description);
    return manifest;
}
void dap_chain_plugins_manifest_free(manifest_t *manifest){
    free(manifest->name);
    free(manifest->version);
    free(manifest->author);
    free(manifest->description);
    dap_list_free(manifest->dependencys);
    free(manifest);
}
manifest_t* dap_chain_plugins_add_manifest_from_file(const char *file_path){
    //READ File in char
    log_it(L_INFO, "Parse json file");
    FILE *file = fopen(file_path, "rt");
    if (file == NULL){
        log_it(L_INFO, "Parse json file");
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    size_t size_file = (size_t)ftell(file);
    char *json = DAP_NEW_SIZE(char, size_file);
    rewind(file);
    fread(json, sizeof(char), size_file, file);
    fclose(file);
    //Parse JSON
    json_object *j_obj = json_tokener_parse(json);
    json_object *j_name = json_object_object_get(j_obj, "name");
    json_object *j_version = json_object_object_get(j_obj, "version");
    json_object *j_dependencys = json_object_object_get(j_obj, "dependencys");
    json_object *j_author = json_object_object_get(j_obj, "author");
    json_object *j_description = json_object_object_get(j_obj, "description");
    const char *name, *version, *author, *description;
    name = json_object_get_string(j_name);
    version = json_object_get_string(j_version);
    author = json_object_get_string(j_author);
    description = json_object_get_string(j_description);
    dap_list_t *dep = JSON_array_to_dap_list(j_dependencys);
    DAP_FREE(json);
    manifest_t *manifest = dap_chain_plugins_manifest_new(name, version, dep, author, description);
    return manifest;
}

//--------------------------------------------------------
void dap_chain_plugins_manifest_list_create(){
    manifests = dap_list_alloc();
}
manifest_t *dap_chain_plugins_manifest_get_list(size_t index){
    return (manifest_t*)dap_list_nth_data(manifests, (unsigned int)index);
}

void dap_chain_plugins_manifest_list_free(){
    unsigned int len = dap_list_length(manifests);
    for (unsigned int i = 0; i < len;i++){
        manifest_t* manifest = dap_list_nth_data(manifests, i);
        dap_chain_plugins_manifest_free(manifest);
    }
    dap_list_free(manifests);
}

void dap_chain_plugins_manifest_list_add_manifest(manifest_t *manifest){
    manifests = dap_list_append(manifests, manifest);
}
bool dap_chain_plugins_manifest_list_add_from_file(const char *file_path){
    manifest_t *manifest = dap_chain_plugins_add_manifest_from_file(file_path);
    if (manifest == NULL)
        return false;
    dap_chain_plugins_manifest_list_add_manifest(manifest);
    return true;
}
