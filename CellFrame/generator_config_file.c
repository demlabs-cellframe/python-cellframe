#include "generator_config_file.h"

#define LOG_TAG "generator_config_file"

static void writeGroupInFile(FILE *file, const char *group){
    fputs("[", file);
    fputs(group, file);
    fputs("]\n", file);
}

static void writeKeyAndValueInFile(FILE *file, const char *key, const char *value){
    fputs(key, file);
    fputs("=", file);
    fputs(value, file);
    fputs("\n", file);
}


char* getCharFromPyObject(PyObject *obj){
    char *res = "NULL\0";
    if (PyUnicode_Check(obj)){
        res = strdup(PyUnicode_AsUTF8(obj));
    }
    if (PyLong_Check(obj)){
        int i = _PyLong_AsInt(obj);
        res = dap_itoa(i);
    }
    if (PyBool_Check(obj)){
        res = (obj == Py_True) ? "true\0" : "false\0";
    }
    if (PyBytes_Check(obj)){
        size_t size_bytes = (size_t)PyBytes_Size(obj);
        res = calloc(size_bytes+1, sizeof(char));
        memcpy(res, PyBytes_AsString(obj), size_bytes);
        res[size_bytes+1] = '\0';
    }
    if (PyList_Check(obj)){
        dap_string_t *str = dap_string_new("[");
        Py_ssize_t size_list = PyList_Size(obj);
        for (Py_ssize_t i = 0; i < size_list;i++){
            char *d = getCharFromPyObject(PyList_GetItem(obj, i));
            str = dap_string_append(str, d);
            if (i != size_list - 1){
                str = dap_string_append(str, ",");
            }
        }
        str = dap_string_append(str, "]");
        res  = strdup(str->str);
        dap_string_free(str, true);
    }
    return res;
}

void writeContectSectorToFile(FILE *file, PyObject *sector){
    PyGILState_STATE state;
    state = PyGILState_Ensure();
    PyObject *list_keys = PyDict_Keys(sector);
    PyGILState_Release(state);
    Py_ssize_t count_keys = PyList_Size(list_keys);
    char *key;
    PyObject *obj_value;
    char *value;
    for (Py_ssize_t i = 0; i < count_keys; i++){
        key = getCharFromPyObject(PyList_GetItem(list_keys, i));
        obj_value = PyDict_GetItemString(sector, key);
        value = getCharFromPyObject(obj_value);
        writeKeyAndValueInFile(file, key, value);
    }
}

void writeSectorsToFile(FILE *file, PyObject *sectors, int count, ...){
    log_it(L_DEBUG, "writeSectorToFile function has started...");
    if (!PyDict_Check(sectors)){
        log_it(L_ERROR, "An input object doesn't have a sector");
        return;
    }
    PyObject *list_sectors = PyDict_Keys(sectors);
    log_it(L_DEBUG, "WSTF1");
    Py_ssize_t count_sectors = PyList_Size(list_sectors);
    char *name_sector = NULL;
    bool _this_obj_not_processes = false;
    PyObject *sector;
    for (Py_ssize_t i = 0; i < count_sectors; i++){
        name_sector = getCharFromPyObject(PyList_GetItem(list_sectors, i));
        va_list args;
        va_start(args, count);
        for (int l=0; l < count; l++){
            if (strcmp(name_sector, va_arg(args, char*)) == 0){
                _this_obj_not_processes = true;
            }
        }
        if (!_this_obj_not_processes){
            sector = PyDict_GetItemString(sectors, name_sector);
            writeGroupInFile(file, name_sector);
            writeContectSectorToFile(file, sector);
        }
        DAP_FREE(name_sector);
    }
}

void writeChainFiles(char *base_path, PyObject *chains_conf){
    PyObject *list_names = PyDict_GetItemString(chains_conf, "name_cfg_files");
    PyObject *dict_conf = PyDict_GetItemString(chains_conf, "conf_files");
    for (Py_ssize_t i=0; i < PyList_Size(list_names); i++){
        char *name = dap_strdup(PyUnicode_AsUTF8(PyList_GetItem(list_names, i)));
        PyObject *cfg = PyDict_GetItemString(dict_conf, name);
        FILE *file_chain;
        char *path_file = dap_strjoin(NULL, base_path, name, ".cfg", NULL);
        if ((file_chain = fopen(path_file, "w")) == NULL){
            log_it(L_WARNING,"Can't create a \"%s\" file", path_file);
            break;
        }
        writeSectorsToFile(file_chain, cfg, 0);
        fclose(file_chain);
    }
}

void createConfNetworks(char *base_path, PyObject *nets){
    log_it(L_DEBUG, "Creating network settings ...");
    PyObject *name_nets = PyDict_Keys(nets);
    Py_ssize_t count_nets = PyList_Size(name_nets);
    char *name_net_dir;
    char *name_net_cfg;
    char *name_net_cfg_tmp;
    char *value = NULL;
    for (Py_ssize_t i =0; i < count_nets; i++){
        value = getCharFromPyObject(PyList_GetItem(name_nets, i));
        log_it(L_DEBUG, "net: %s", value);
        name_net_cfg_tmp = dap_strjoin(NULL, base_path, "/", value, NULL);
        name_net_dir = dap_strjoin(NULL, name_net_cfg_tmp, "/", NULL);
        name_net_cfg = dap_strjoin(NULL, name_net_cfg_tmp, ".cfg", NULL);
        DAP_FREE(name_net_cfg_tmp);
        if (!dap_valid_ascii_symbols(name_net_cfg)){
            break;
        }
        dap_mkdir_with_parents(name_net_dir);
        FILE *cfg_file_net;
        if ((cfg_file_net = fopen(name_net_cfg, "w")) == NULL){
            log_it(L_WARNING,"Can't create a \"%s\" file", name_net_cfg);
            break;
        }
        DAP_FREE(name_net_cfg);
        PyObject *conf = PyDict_GetItemString(nets, value);
        writeSectorsToFile(cfg_file_net, conf, 2, "name_cfg_files", "conf_files");
        fclose(cfg_file_net);
        writeChainFiles(name_net_dir, conf);
        DAP_FREE(name_net_dir);
        DAP_FREE(value);
    }
}

int gen_config_files(const char *cfgDir, const char *app_name, PyObject *cfg_JSON){
    if (!dap_valid_ascii_symbols(cfgDir)){
        return -1;
    }
    char *network_dir = dap_strjoin(NULL, cfgDir, "/network", NULL);
    dap_mkdir_with_parents(network_dir);
    char *main_config_files = dap_strjoin(NULL, cfgDir, "//", app_name, ".cfg", NULL);
    FILE *main_file;
    if ((main_file = fopen(main_config_files, "w")) == NULL){
        log_it(L_WARNING, "Can't open a \"%s\" file", main_config_files);
        return -3;
    }
    writeSectorsToFile(main_file, cfg_JSON, 1, "networks");
    fclose(main_file);
    createConfNetworks(network_dir, PyDict_GetItemString(cfg_JSON, "networks"));
    DAP_FREE(main_config_files);
    DAP_FREE(network_dir);
    return 0;
}
