#include "dap_chain_plugins_manifest.h"

void dap_chain_plugins_list_char_delete_all(dap_chain_plugins_list_char_t *list){
    dap_chain_plugins_list_char_t *element;
    dap_chain_plugins_list_char_t *tmp;
    LL_FOREACH_SAFE(list, element, tmp){
        DAP_FREE(element->value);
        LL_DELETE(list, element);
    }
}

dap_chain_plugins_list_manifest_t *dap_chain_plugins_manifest_new(const char *name, const char *version, const dap_chain_plugins_list_char_t *dep, const char *author,
                                                    const char *description){
    dap_chain_plugins_list_manifest_t *man = (dap_chain_plugins_list_manifest_t*)DAP_NEW(dap_chain_plugins_list_manifest_t);
    man->name = dap_strdup(name);
    man->author = dap_strdup(author);
    man->version = dap_strdup(version);
    man->description = dap_strdup(description);
    man->dependencys = NULL;
    //copy informantion from dep to man->dependencys
    int len_dep;
    dap_chain_plugins_list_char_t *char_t;
    LL_COUNT((dap_chain_plugins_list_char_t*)dep, char_t, len_dep);
    LL_FOREACH((dap_chain_plugins_list_char_t*)dep, char_t){
        LL_APPEND(man->dependencys, char_t);
    }
    man->dependencys = (dap_chain_plugins_list_char_t*)dep;
    return man;
}

static dap_chain_plugins_list_char_t* JSON_array_to_dap_list_char(json_object *j_obj){
    dap_chain_plugins_list_char_t *list = NULL;
    dap_chain_plugins_list_char_t *element = NULL;
    for (int i = 0; i < json_object_array_length(j_obj); i++){
        element = (dap_chain_plugins_list_char_t*)DAP_NEW(dap_chain_plugins_list_char_t);
        element->value = dap_strdup(json_object_get_string(json_object_array_get_idx(j_obj, i)));
        LL_APPEND(list, element);
    }
    return list;
}

void dap_chain_plugins_manifest_list_create(){
    manifests = NULL;
}

int dap_chain_plugins_manifest_name_cmp(dap_chain_plugins_list_manifest_t *man, const char *name){
    return dap_strcmp(man->name, name);
}

dap_chain_plugins_list_manifest_t* dap_chain_plugins_add_manifest_from_file(const char *file_path){
    //READ File in char
    log_it(L_INFO, "Parse json file");
    FILE *file = fopen(file_path, "rt");
    if (file == NULL){
        log_it(L_ERROR, "Error open manifest files along the way %s", file_path);
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
    dap_chain_plugins_list_char_t *dep = JSON_array_to_dap_list_char(j_dependencys);
    DAP_FREE(json);
    dap_chain_plugins_list_char_t *tmp = NULL;
    dap_chain_plugins_list_manifest_t *manifest = dap_chain_plugins_manifest_new(name, version, dep, author, description);
    return manifest;
}

void dap_chain_plugins_manifest_list_add_manifest(dap_chain_plugins_list_manifest_t *man){
    LL_APPEND(manifests, man);
}

dap_chain_plugins_list_manifest_t* dap_chain_plugins_manifests_get_list(){
    return manifests;
}

dap_chain_plugins_list_manifest_t *dap_chain_plugins_manifest_list_get_name(const char *name){
    dap_chain_plugins_list_manifest_t *element;
    LL_SEARCH(manifests, element, name, dap_chain_plugins_manifest_name_cmp);
    return element;
}

char* dap_chain_plugins_manifests_get_list_dependencyes(dap_chain_plugins_list_manifest_t *element){
    if (element->dependencys == NULL) {
        return NULL;
    } else {
        char *result = "";
        dap_chain_plugins_list_char_t *el;
        int max_count_list = 0;
        LL_COUNT(element->dependencys, el, max_count_list);
        int count_list = 1;
        LL_FOREACH(element->dependencys, el){
            if (count_list != max_count_list)
                result = dap_strjoin(NULL, result, el->value, ", ", NULL);
            else
                result = dap_strjoin(NULL, result, el->value, NULL);
        }
        return result;
    }
}

bool dap_chain_plugins_manifest_list_add_from_file(const char *file_path){
    dap_chain_plugins_list_manifest_t *manifest = dap_chain_plugins_add_manifest_from_file(file_path);
    if (manifest == NULL)
        return false;
    dap_chain_plugins_manifest_list_add_manifest(manifest);
    return true;
}

bool dap_chain_plugins_manifest_list_delete_name(const char *name){
    dap_chain_plugins_list_manifest_t *element;
    LL_SEARCH(manifests, element, name, dap_chain_plugins_manifest_name_cmp);
    if (element == NULL)
        return false;
    DAP_FREE(element->name);
    DAP_FREE(element->version);
    DAP_FREE(element->author);
    DAP_FREE(element->description);
    dap_chain_plugins_list_char_delete_all(element->dependencys);
    LL_DELETE(manifests, element);
    return true;
}
void dap_chain_plugins_manifest_list_delete_all(void){
    dap_chain_plugins_list_manifest_t *element;
    dap_chain_plugins_list_manifest_t *tmp;
    LL_FOREACH_SAFE(manifests, element, tmp){
        DAP_FREE(element->name);
        DAP_FREE(element->version);
        DAP_FREE(element->author);
        DAP_FREE(element->description);
        dap_chain_plugins_list_char_delete_all(element->dependencys);
        LL_DELETE(manifests, element);
    }
//    LL_FOREACH_SAFE()
}
