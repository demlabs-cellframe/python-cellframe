#include "generator_config_file.h"

#define LOG_TAG "generator_config_file"

static void writeGroupInFile(FILE *file, char *group){
    fputs("[", file);
    fputs(group, file);
    fputs("]\n", file);
}

static void writeKeyAndValueInFile(FILE *file, char *key, char *value){
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
    PyObject *list_keys = PyDict_Keys(sector);
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
    PyObject *list_sectors = PyDict_Keys(sectors);
    Py_ssize_t count_sectors = PyList_Size(list_sectors);
    char *name_sector;
    PyObject *sector;
    bool _this_obj_not_processes;
    for (Py_ssize_t i = 0; i < count_sectors; i++){
        name_sector = getCharFromPyObject(PyList_GetItem(list_sectors, i));
        va_list args;
        va_start(args, count);
        for (int l=0; l < count;l++){
            _this_obj_not_processes = true;
            if (strcmp(name_sector, va_arg(args, char*)) == 0){
                _this_obj_not_processes = false;
                va_end(args);
                break;
            }
        }
        if (!_this_obj_not_processes){
            break;
        }
        va_end(args);
        sector = PyDict_GetItemString(sectors, name_sector);
        writeGroupInFile(file, name_sector);
        writeContectSectorToFile(file, sector);
    }
}

void createConfNetworks(dap_string_t *base_path, PyObject *nets){
    PyObject *name_nets = PyDict_Keys(nets);
    Py_ssize_t count_nets = PyList_Size(name_nets);
    char *name_net;
    PyObject *obj_net;
    dap_string_t *path_net;
    dap_string_t *base_path_net_file;
    FILE *file_net;
    PyObject *list_files;
    PyObject *obj_conf_files;
    Py_ssize_t count_list_file;
    dap_string_t *path_cfg_net_file;
    char *name_cfg_net_file;
    FILE *cfg_net_file;
    for (Py_ssize_t i =0; i < count_nets; i++){
        name_net = getCharFromPyObject(PyList_GetItem(name_nets, i));
        path_net = dap_string_new(base_path->str);
        path_net = dap_string_append(path_net, "/");
        path_net = dap_string_append(path_net, name_net);
        base_path_net_file = dap_string_new(path_net->str);
        base_path_net_file = dap_string_append(base_path_net_file, ".cfg");
        path_net = dap_string_append(path_net, "/");
        if (!dap_valid_ascii_symbols(path_net->str)){
            return;
        }
        dap_mkdir_with_parents(path_net->str);
        if ((file_net = fopen(base_path_net_file->str, "w")) == NULL){
            return;
        }
        obj_net = PyDict_GetItemString(nets, name_net);
        writeSectorsToFile(file_net, obj_net, 2, "name_cfg_files", "conf_files");
        fclose(file_net);
        list_files = PyDict_GetItemString(obj_net, "name_cfg_files");
        count_list_file = PyList_Size(list_files);
        obj_conf_files = PyDict_GetItemString(obj_net, "conf_files");
        for (Py_ssize_t index = 0; index < count_list_file; index++){
            name_cfg_net_file = getCharFromPyObject(PyList_GetItem(list_files, index));
            path_cfg_net_file = dap_string_new(path_net->str);
            path_cfg_net_file = dap_string_append(path_cfg_net_file, name_cfg_net_file);
            path_cfg_net_file = dap_string_append(path_cfg_net_file, ".cfg");
            if ((cfg_net_file = fopen(path_cfg_net_file->str, "w")) == NULL){
                return;
            }
            writeSectorsToFile(cfg_net_file, PyDict_GetItemString(obj_conf_files, name_cfg_net_file), 0);
            fclose(cfg_net_file);
            dap_string_free(path_cfg_net_file, true);
        }
        dap_string_free(base_path_net_file, true);
        dap_string_free(path_net, true);
    }
}

int gen_config_files(const char *cfgDir, const char *app_name, PyObject *cfg_JSON){
    dap_string_t *string_cfg_dir = dap_string_new(cfgDir);
    dap_string_t *networks_dir = dap_string_new(string_cfg_dir->str);
    networks_dir = dap_string_append(networks_dir, "/network");
    if (!dap_valid_ascii_symbols(cfgDir)){
        return -1;
    }
    dap_mkdir_with_parents(networks_dir->str);
    dap_string_t *main_cfg_file = dap_string_new(string_cfg_dir->str);
    main_cfg_file = dap_string_append(main_cfg_file, "/");
    main_cfg_file = dap_string_append(main_cfg_file, app_name);
    main_cfg_file = dap_string_append(main_cfg_file, ".cfg");
    FILE *main_file;
    if ((main_file = fopen(main_cfg_file->str, "w")) == NULL){
        return -3;
    }
    writeSectorsToFile(main_file, cfg_JSON, 1, "networks");
    fclose(main_file);
    createConfNetworks(networks_dir, PyDict_GetItemString(cfg_JSON, "networks"));
    dap_string_free(main_cfg_file, true);
    dap_string_free(networks_dir, true);
    dap_string_free(string_cfg_dir, true);
    return 0;
}
