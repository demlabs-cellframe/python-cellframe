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
    log_it(L_DEBUG, "Write chain config to file: sectors list size %d", count_sectors);
    for (Py_ssize_t i = 0; i < count_sectors; i++){
        name_sector = getCharFromPyObject(PyList_GetItem(list_sectors, i));
        va_list args;
        va_start(args, count);
        log_it(L_DEBUG, "  name_sector=\"%s\" ( count=%d )", name_sector, count);
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
        log_it(L_DEBUG, "  sector=\"%s\"", sector);
        writeGroupInFile(file, name_sector);
        writeContectSectorToFile(file, sector);
    }
}

void createConfNetworks(dap_string_t *base_path, PyObject *nets){
    PyObject *name_nets = PyDict_Keys(nets);
    Py_ssize_t count_nets = PyList_Size(name_nets);
    char *l_net_name;
    PyObject *obj_net;
    dap_string_t *l_path_net_str;
    dap_string_t *l_net_file_path;
    FILE *file_net;
    PyObject *list_files;
    PyObject *obj_conf_files;
    Py_ssize_t count_list_file;
    dap_string_t *l_chain_file_path;
    char *name_cfg_chain_file;
    FILE *l_chain_file;
    for (Py_ssize_t i =0; i < count_nets; i++){
        l_net_name = getCharFromPyObject(PyList_GetItem(name_nets, i));
        l_path_net_str = dap_string_new(base_path->str);
        l_path_net_str = dap_string_append(l_path_net_str, "/");
        l_path_net_str = dap_string_append(l_path_net_str, l_net_name);
        l_net_file_path = dap_string_new(l_path_net_str->str);
        l_net_file_path = dap_string_append(l_net_file_path, ".cfg");
        l_path_net_str = dap_string_append(l_path_net_str, "/");
        if (!dap_valid_ascii_symbols(l_path_net_str->str)){
            return;
        }
        dap_mkdir_with_parents(l_path_net_str->str);
        if ((file_net = fopen(l_net_file_path->str, "w")) == NULL){
            log_it(L_WARNING,"Can't create file \"%s\"", l_net_file_path->str);
            return;
        }
        obj_net = PyDict_GetItemString(nets, l_net_name);
        writeSectorsToFile(file_net, obj_net, 2, "name_cfg_files", "conf_files");
        fclose(file_net);
        log_it(L_INFO,"Generated network \"%s\" on path %s", l_net_name, l_net_file_path->str);

        list_files = PyDict_GetItemString(obj_net, "name_cfg_files");
        count_list_file = PyList_Size(list_files);
        obj_conf_files = PyDict_GetItemString(obj_net, "conf_files");
        for (Py_ssize_t index = 0; index < count_list_file; index++){
            name_cfg_chain_file = getCharFromPyObject(PyList_GetItem(list_files, index));
            l_chain_file_path = dap_string_new(l_path_net_str->str);
            l_chain_file_path = dap_string_append(l_chain_file_path, name_cfg_chain_file);
            l_chain_file_path = dap_string_append(l_chain_file_path, ".cfg");
            if ((l_chain_file = fopen(l_chain_file_path->str, "w")) == NULL){
                log_it(L_WARNING,"Can't create file \"%s\" ", l_chain_file_path->str);
                return;
            }
            log_it(L_INFO, "Created config for chain \"%s\" on path %s",  name_cfg_chain_file, l_chain_file_path->str );
            writeSectorsToFile( l_chain_file, PyDict_GetItemString(obj_conf_files, name_cfg_chain_file), 0 );
            fclose(l_chain_file);
            if ( l_chain_file_path )
                log_it(L_INFO,"Produced output for %s", l_chain_file_path->str);
            else
                log_it(L_ERROR,"No output for %s", l_chain_file_path->str);
            dap_string_free(l_chain_file_path, true);
        }
        dap_string_free(l_net_file_path, true);
        dap_string_free(l_path_net_str, true);
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
