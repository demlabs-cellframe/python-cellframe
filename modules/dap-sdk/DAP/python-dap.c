#include "python-dap.h"

#define LOG_TAG "python-DAP"

static bool s_init_crypto = false;
static bool s_init_events = false;
static bool s_init_server = false;
static bool s_init_server_core = false;
static bool s_init_http = false;
static bool s_init_http_simple = false;
static bool s_init_http_folder = false;
static bool s_init_http_enc = false;
static bool s_init_stream = false;
static bool s_init_stream_ctl = false;
static bool s_init_net_client = false;

//static bool s_init_chain = false;
//static bool s_init_app_cli = false;
//static bool s_init_mempool = false;

////static bool s_init_http_client_simple = false;
//static bool s_init_wallet = false;
//static bool s_init_cs_dag = false;
//static bool s_init_cs_dag_poa = false;
//static bool s_init_cs_dag_pos = false;
//static bool s_init_chain_net_srv = false;
static bool s_init_ks = false;

static bool submodules_deint;

PyObject* DAP_error = NULL;


#ifdef _WIN32

BOOL WINAPI consoleHandler(DWORD dwType){
    if (dwType == CTRL_C_EVENT){
        log_it(L_NOTICE, "Handler Ctrl+C");
        dap_server_loop_stop();
        deinit_modules();
    }
    return TRUE;
}
#else

void sigfunc(int sig){
    if (sig == SIGINT){
        log_it(L_NOTICE, "Handler Ctrl+C");
        dap_events_stop_all();
        deinit_modules();
    }
}
#endif

PyObject *python_DAP_init(PyObject *self, PyObject *args){
    const char *app_name;
    const char *file_name_log;
    const char *config_dir;
    const char *log_level;
    const char *JSON_str;
    g_config = NULL;

    s_init_ks = true;
    submodules_deint = false;

    PyObject *events = NULL;

    #ifdef _WIN32
        setConsoleCtrlHandler((PHANDLER_ROUTINE)consoleHandler, TRUE);
    #else
        signal(SIGINT, sigfunc);
    #endif

    if (!PyArg_ParseTuple(args, "s", &JSON_str)){
        PyErr_SetString(DAP_error, "ERROR in function call signature: can't get one String argument");
        return NULL;
    }

    PyObject *JSON_Module = PyImport_ImportModule("json");
    if (JSON_Module == NULL) {
        PyErr_SetString(DAP_error, "ERROR importing module");
        return NULL;
    }

    PyObject* JSONLoadsFunction = PyObject_GetAttrString(JSON_Module, "loads");
    if (JSONLoadsFunction == NULL) {
        PyErr_SetString(DAP_error, "Can't find \"loads\" section");
        return NULL;
    }

    PyObject* argsInLoadsJSON = PyTuple_Pack(1,PyUnicode_FromString(JSON_str));
    PyObject* result = PyObject_CallObject(JSONLoadsFunction, argsInLoadsJSON);
    if (result == NULL){
        PyErr_SetString(DAP_error, "ERROR in JSONLoadsFunction");
        return NULL;
    }
    PyObject* getModules = PyDict_GetItemString(result, "modules");
    if (getModules == NULL){
        PyErr_SetString(DAP_error, "Can't find \"modules\" section");
        return NULL;
    }

    // DAP or Core
    PyObject* getDap = PyDict_GetItemString(result, "DAP");

    if (getDap == NULL)
        getDap = PyDict_GetItemString(result, "Core");

    if( getDap == NULL ){
        PyErr_SetString(DAP_error, "Can't find \"Core\" or \"DAP\" section");
        return NULL;
    }

    /*Parse DAP*/
    PyObject* config_dir_PyObject = PyDict_GetItemString(getDap, "config_dir");
    PyObject* application_name_PyObject = PyDict_GetItemString(getDap, "application_name");
    PyObject* file_name_log_PyObject = PyDict_GetItemString(getDap, "file_name_log");
    PyObject* logLevel_PyObject = PyDict_GetItemString(getDap, "log_level");
    if (config_dir_PyObject == NULL || application_name_PyObject == NULL ||
            logLevel_PyObject == NULL || file_name_log_PyObject == NULL){
        PyErr_SetString(DAP_error, "config_dir or application_name or file_name_log or log_level");
        return NULL;
    }
    app_name = PyUnicode_AsUTF8(application_name_PyObject);
    file_name_log = PyUnicode_AsUTF8(file_name_log_PyObject);
    config_dir = PyUnicode_AsUTF8(config_dir_PyObject);
    log_level = PyUnicode_AsUTF8(logLevel_PyObject);

    if (dap_common_init(app_name, file_name_log,NULL ) != 0){
        PyErr_SetString(DAP_error, "Can't init common functions module");
        return NULL;
    }
    dap_set_appname(app_name);
    if ( dap_strcmp( log_level, "L_DEBUG" )==0 || dap_strcmp( log_level, "DEBUG" )==0  ){
        dap_log_level_set(L_DEBUG);
    }else if ( dap_strcmp( log_level, "L_INFO" )==0 || dap_strcmp( log_level, "INFO" )==0  ){
        dap_log_level_set(L_INFO);
    }else if ( dap_strcmp( log_level, "L_NOTICE" )==0 || dap_strcmp( log_level, "NOTICE" )==0  ){
        dap_log_level_set(L_NOTICE);
    }else if ( dap_strcmp( log_level, "L_MSG" )==0 || dap_strcmp( log_level, "MSG" )==0  ){
        dap_log_level_set(L_MSG);
    }else if ( dap_strcmp( log_level, "L_DAP" )==0 || dap_strcmp( log_level, "DAP" )==0  ){
        dap_log_level_set(L_DAP);
    }else if ( dap_strcmp( log_level, "L_WARNING" )==0 || dap_strcmp( log_level, "WARNING" )==0  ){
        dap_log_level_set(L_WARNING);
    }else if ( dap_strcmp( log_level, "L_ATT" )==0 || dap_strcmp( log_level, "ATTENTION" )==0  ){
        dap_log_level_set(L_ATT);
    }else if ( dap_strcmp( log_level, "L_ERROR" )==0 || dap_strcmp( log_level, "ERROR" )==0  ){
        dap_log_level_set(L_ERROR);
    }else if ( dap_strcmp( log_level, "L_CRITICAL" )==0 || dap_strcmp( log_level, "CRITICAL" )==0  ){
        dap_log_level_set(L_CRITICAL);
    }
    //generation config files
    PyObject *configure = PyDict_GetItemString(result, "Configuration");
    int res_gen_config_file = gen_config_files(config_dir, app_name, configure);
    switch (res_gen_config_file) {
    case -1:
        PyErr_SetString(DAP_error, "Can't generate configuration files. Can name directory path contains not ASCII simbols.");
        return NULL;
    case -3:
        PyErr_SetString(DAP_error, "Can't generate configuration files. Can't open file stream.");
        return NULL;
    }
    //Init config
    dap_config_init(config_dir);
    if ((g_config = dap_config_open(app_name) ) == NULL){
        PyErr_SetString(DAP_error, "Can't init general configurations");
        return NULL;
    }
    //Init modules
    log_it(L_INFO, "Initializing modules ...");
    if (!PyList_Check(getModules)){
        PyErr_SetString(DAP_error, "Can't find an array of module names");
        return NULL;
    }
    Py_ssize_t size_list = PyList_Size(getModules);
    for (int i=0; i < size_list;i++){
        PyObject *value = PyList_GetItem(getModules, i);
        const char *c_value = PyUnicode_AsUTF8(value);
        log_it(L_INFO, "Initializing the %s module ", c_value);
        int rc = 0;
        if (strcmp(c_value, "Crypto") == 0){            //Init crypto
//            log_it(L_INFO, "Initializing the %s module", c_value);
            s_init_crypto = true;
            rc = dap_crypto_init();
            if (rc != 0){
                char *msg = dap_strdup_printf("An error occurred while initializing the libdap-crypto-python module. Code: %i", rc);
                PyErr_SetString(DAP_error, msg);
                return NULL;
            }
            rc = dap_cert_init();
            if (rc != 0){
                char *msg = dap_strdup_printf("An error occurred while initializing the libdap-crypto-cert-python module. Code: %i", rc);
                PyErr_SetString(DAP_error, msg);
                return NULL;
            }
        } else if (strcmp(c_value, "Events") == 0){
            rc = dap_events_init(0,0);
            if (rc != 0){
                char *msg = dap_strdup_printf("An error occurred while initializing the dap_events module. Code: %i", rc);
                PyErr_SetString(DAP_error, msg);
                return NULL;
            }
            events = _PyObject_New(&dapEvents_dapEventsType);
            ((PyDapEventsObject*)events)->t_events = dap_events_new();
            dap_events_start(((PyDapEventsObject*)events)->t_events);
            s_init_events = true;
        } else if (strcmp(c_value, "Server") == 0){
            rc = dap_server_init();
            if(rc != 0 ){
                char *msg = dap_strdup_printf("Failed to initialize Server. Code: %i", rc);
                PyErr_SetString(DAP_error, msg);
                return NULL;
            }
            s_init_server = true;
        }else if (strcmp(c_value, "ServerCore") == 0){
            PyObject* getServerCoreData = PyDict_GetItemString(result, "ServerCore");
            if (getServerCoreData == NULL){
                PyErr_SetString(DAP_error, "Initialization failed. ServerCore object not found in JSON."
                                " No settings are specified for initializing libdap-server-core-python.");
                return NULL;
            }
            PyObject* Nl_thread_cnt = PyDict_GetItemString(getServerCoreData, "thread_cnt");
            PyObject* Nl_conn = PyDict_GetItemString(getServerCoreData, "conn");
            if ( (Nl_thread_cnt == NULL || Nl_conn == NULL) || !PyNumber_Check(Nl_thread_cnt) ||
                 !PyNumber_Check(Nl_conn)){
                PyErr_SetString(DAP_error, "Failed to initialize ServerCore. "
                                                 "Fields thread_cnt and conn are not numerical or absent.");
                return NULL;
            }
            PyObject *ll_thread_cnt= PyNumber_Long(Nl_thread_cnt);
            PyObject *ll_conn = PyNumber_Long(Nl_conn);
            uint32_t ul_thread_cnt = (uint32_t)PyLong_AsUnsignedLong(ll_thread_cnt);
            size_t ul_conn = PyLong_AsSize_t(ll_conn);
            rc = dap_server_core_init(ul_thread_cnt, ul_conn);
            if(rc != 0 ){
                char *msg = dap_strdup_printf("Failed to initialize ServerCore. Code: %i", rc);
                PyErr_SetString(DAP_error, msg);
                return NULL;
            }
            s_init_server_core = true;
        }/* else if (strcmp(c_value, "Chain") == 0){
            if(init_chain_py() != 0){
                PyErr_SetString(DAP_error, "Failed to initialize Chain. "
                                                 "Fields thread_cnt and conn are not numerical or absent.");
                return NULL;
            }
            if (dap_chain_cs_init_py() != 0){
                PyErr_SetString(DAP_error, "Failed to initialize Chain CS. "
                                                 "Fields thread_cnt and conn are not numerical or absent.");
                return NULL;
            }
        }*/ else if (strcmp(c_value, "Http") == 0){
            rc = dap_http_init();
            if(rc != 0){
                char* msg = dap_strdup_printf("Failed to initialize Http module. Code: %i", rc);
                PyErr_SetString(DAP_error, msg);
                return NULL;
            }
            s_init_http = true;
        } else if (strcmp(c_value, "HttpSimple") == 0){
            rc = dap_http_simple_module_init();
            if (rc != 0){
                char* msg = dap_strdup_printf("Failed to initialize HttpSimple module. Code: %i", rc);
                PyErr_SetString(DAP_error, msg);
                return NULL;
            }
            s_init_http_simple = true;
        } else if (strcmp(c_value, "EncHttp") == 0){
            rc = enc_http_init();
            if(rc != 0){
                char *msg = dap_strdup_printf("Failed to initialize EncHttp module. Code: %i", rc);
                PyErr_SetString(DAP_error, msg);
                return NULL;
            }
            s_init_http_enc = true;
        } else if (strcmp(c_value, "HttpFolder") == 0){
            rc = dap_http_folder_init();
            if (rc !=0){
                char *msg = dap_strdup_printf("Failed to initialize http folder module. Code: %i", rc);
                PyErr_SetString(DAP_error, msg);
                return NULL;
            }
            s_init_http_folder = true;
        } else if (strcmp(c_value, "Stream") == 0){
            PyObject* getStreamData = PyDict_GetItemString(result, "Stream");
            if (getStreamData == NULL){
                PyErr_SetString(DAP_error, "Initialization failed. Stream object not found in JSON."
                                " No settings are specified for initializing libdap-stream-python.");
                return NULL;
            }
            PyObject *debugDumpStreamHeadersObj = PyDict_GetItemString(getStreamData, "DebugDumpStreamHeaders");
            if (debugDumpStreamHeadersObj == NULL || !PyBool_Check(debugDumpStreamHeadersObj)){
                PyErr_SetString(DAP_error, "Failed to initialize Stream. "
                                                 "Fields DebugDumpStreamHeaders are not boolean type.");
                return NULL;
            }
            rc = dap_stream_init(g_config);
            if(rc != 0){
                char* msg = dap_strdup_printf("Failed to initialize Stream module. Code %i", rc);
                PyErr_SetString(DAP_error, msg);
                return NULL;
            }
            s_init_stream = true;
        } else if (strcmp(c_value, "StreamCtl") == 0){
            rc = dap_stream_ctl_init();
            if (rc != 0){
                char *msg = dap_strdup_printf("Failed to initialize StreamCtl module. Code: %i", rc);
                PyErr_SetString(DAP_error, msg);
                return NULL;
            }
            s_init_stream_ctl = true;
        }
        /*else if (strcmp(c_value, "Mempool") == 0){
            if (dap_datum_mempool_init() != 0){
                PyErr_SetString(DAP_error, "Failed to initialize Mempool module. ");
                return NULL;
            }
            s_init_mempool = true;

//        } else if (strcmp(c_value, "HttpClientSimple") == 0){
//            if (dap_http_client_simple_init() != 0){
//                PyErr_SetString(DAP_error, "Failed to initialize HttpClientSimple module. ");
//                return NULL;
//            }
//            s_init_http_client_simple = true;
        } else if (strcmp(c_value, "Wallet") == 0){
            if (dap_chain_wallet_init_py() != 0){
                PyErr_SetString(DAP_error, "Failed to initialize Wallet module. ");
                return NULL;
            }
            s_init_wallet = true;
        } else if (strcmp(c_value, "ChainCSDag") == 0){
            if (dap_chain_cs_dag_init() != 0)
            {
                PyErr_SetString(DAP_error, "Failed to initialize ChainCSDag module. ");
                return NULL;
            }
        } else if (strcmp(c_value, "ChainCSDagPoa") == 0){
            if (dap_chain_cs_dag_poa_init() != 0){
                PyErr_SetString(DAP_error, "Failed to initialize ChainCSDagPoa module. ");
                return NULL;
            }
        } else if (strcmp(c_value, "ChainCSDagPos") == 0){
            if (dap_chain_cs_dag_pos_init() != 0){
                PyErr_SetString(DAP_error, "Failed to initialize ChainCSDagPos module. ");
                return NULL;
            } */
        /*} else if (strcmp(c_value, "StreamChChain") == 0){
            if (dap_stream_ch_chain_init() != 0 ){
                PyErr_SetString(DAP_error, "Failed to initialize StreamChChain module. ");
                return NULL;
            }
        } else if (strcmp(c_value, "StreamChChainNet") == 0){
            if (dap_stream_ch_chain_net_init() != 0 ){
                PyErr_SetString(DAP_error, "Failed to initialize StreamChChainNet module. ");
                return NULL;
            }
        }else if (strcmp(c_value, "StreamChChainNetSrv") == 0){
            if (dap_stream_ch_chain_net_init() != 0 ){
                PyErr_SetString(DAP_error, "Failed to initialize StreamChChainNetSrv module. ");
                return NULL;
            }
        }else if (strcmp(c_value, "EncKS") == 0){
//            if (dap_enc_ks_init())
//            if (dap_enc_ks_
        } else if (strcmp(c_value, "GlobalDB") == 0){
            if (dap_chain_global_db_init(g_config) != 0){
                PyErr_SetString(DAP_error, "Failed to initialize GlobalDB module. ");
                return NULL;
            }
        }*/else if (strcmp(c_value, "Client") == 0){
            rc = dap_client_init();
            if (rc != 0){
                char* msg = dap_strdup_printf("Failed to initialize Client module. Code: %i", rc);
                PyErr_SetString(DAP_error, msg);
                return NULL;
            }
            s_init_net_client = true;
        }/*else if (strcmp(c_value, "GDB") == 0){
            if (dap_chain_gdb_init_py() != 0){
                PyErr_SetString(DAP_error, "Failed to initialize GDB module. ");
                return NULL;
            }
        }else if (strcmp(c_value, "Net") == 0){
            if (dap_chain_net_init_py() != 0 ){
                PyErr_SetString(DAP_error, "Failed to initialize chain net module. ");
                return NULL;
            }
        }/*else if (strcmp(c_value, "AppCliServer") == 0){
            if (dap_chain_node_cli_init(g_config) != 0 ){
                PyErr_SetString(DAP_error, "Failed to initialize AppCliServer " );
                return NULL;
            }
        }*/else{
            log_it(L_WARNING,"Unknown module \"%s\"", c_value);
        }
    }
    if (events == NULL)
        return PyLong_FromLong(0);
    else
        return Py_BuildValue("iO", PyLong_FromLong(0), events);
}

PyMODINIT_FUNC PyInit_libDAP(void){
    if (
            //CRYPTO
            PyType_Ready(&g_crypto_base58_type_py) < 0 ||
            PyType_Ready(&g_crypto_base64_type_py) < 0 ||
            PyType_Ready(&DapSignObject_DapSignObjectType) < 0 ||
            PyType_Ready(&DapSignTypeObject_DapSignTypeObjectType) < 0 ||
            PyType_Ready(&g_crypto_cert_type_metadata_type_py) < 0 ||
            PyType_Ready(&g_crypto_cert_metadata_type_py) < 0 ||
            PyType_Ready(&g_crypto_cert_type_py) < 0 ||
            PyType_Ready(&DapPkeyTypeObject_DapPkeyTypeObjectType) < 0 ||
            PyType_Ready(&DapPkeyObject_DapPkeyObjectType) < 0 ||
            PyType_Ready(&DapHashTypeObject_DapChainHashTypeObjectType) < 0 ||
            PyType_Ready(&DapHashFastObject_DapHashFastObjectType) < 0 ||
            PyType_Ready(&CryptoKeyObjecy_CryptoKeyObjecyType) < 0 ||
            PyType_Ready(&g_crypto_type_py) < 0 ||
            PyType_Ready(&CryptoKeyTypeObjecy_CryptoKeyTypeObjecyType) < 0 ||
//            PyType_Ready(&PyCryptoKeyObject_PyCryptoKeyType) < 0 ||
            PyType_Ready(&CryptoDataTypeObjecy_CryptoDataTypeObjecyType) < 0 ||
            PyType_Ready(&g_crypto_cert_type_py) < 0 ||
            PyType_Ready(&g_crypto_cert_type_metadata_type_py) < 0 ||
            PyType_Ready(&DapHashTypeObject_DapChainHashTypeObjectType) < 0  ||
            PyType_Ready(&DapHashFastObject_DapHashFastObjectType) < 0 ||
            PyType_Ready(&DapPkeyTypeObject_DapPkeyTypeObjectType) < 0 ||
            PyType_Ready(&DapPkeyObject_DapPkeyObjectType) < 0 ||
            PyType_Ready(&DapSignTypeObject_DapSignTypeObjectType) < 0 ||
            PyType_Ready(&DapSignObject_DapSignObjectType) < 0 ||
            // net/core
            PyType_Ready(&dapEvents_dapEventsType) < 0 ||
            PyType_Ready(&dapEventsSocket_dapEventsSocketType) < 0 ||
            PyType_Ready(&dapServer_dapServerType) < 0 ||
            PyType_Ready(&ServerCore_ServerCoreType) < 0 ||
            // net/client
            PyType_Ready(&dapClientObject_dapClientType) < 0 ||
            PyType_Ready(&dapClientErrorObject_dapClientErrorType) < 0 ||
            PyType_Ready(&dapClientStageStatusObject_dapClientStageStatusType) < 0 ||
            PyType_Ready(&dapClientStageObject_dapClientStageType) < 0 ||
            // net/server/enc
            PyType_Ready(&DapEncServer_DapEncServerType) < 0 ||
            // net/server/http
            PyType_Ready(&DapHTTPSimple_DapHTTPSimpleType) < 0 ||
            PyType_Ready(&HTTPCode_HTTPCodeType) < 0 ||
            PyType_Ready(&DapHTTP_DapHTTPType) < 0 ||
            PyType_Ready(&DapHttpFolder_DapHttpFolderType) < 0 ||
            //app_context
            #ifdef DAP_SUPPORT_PYTHON_PLUGINS
            PyType_Ready(&dapAppContext_dapAppContextType) < 0 ||
            #endif
            // net/stream
            PyType_Ready(&dapStreamChObject_dapStreamChType) < 0 ||
            PyType_Ready(&dapStreamWorkerObject_dapStreamWorkerType) < 0 ||
            PyType_Ready(&DapStream_DapStreamType) < 0 ||
            PyType_Ready(&DapStreamCtl_DapStreamCtlType) < 0
            ){
        log_it(L_CRITICAL,"Not all py modules are ready for init");
        return NULL;
    }
    PyObject *module = PyModule_Create(&DAPPythonModule);
    PyModule_AddStringConstant(module, "__author__", "Alexey Stratulat <alexey.stratulat@demlabs.net>");
    PyModule_AddStringConstant(module, "__version__", DAP_VERSION);
    DAP_error = PyErr_NewException("DAP.error", NULL, NULL);

    PyModule_AddObject(module, "error", DAP_error);
    //CRYPTO
    PyModule_AddObject(module, "Base64", (PyObject*)&g_crypto_base64_type_py);
    PyModule_AddObject(module, "Base58", (PyObject*)&g_crypto_base58_type_py);
    PyModule_AddObject(module, "Sign", (PyObject*)&DapSignObject_DapSignObjectType);
    PyModule_AddObject(module, "SignType", (PyObject*)&DapSignTypeObject_DapSignTypeObjectType);
    PyModule_AddObject(module, "CertMetadataType", (PyObject*)&g_crypto_cert_type_metadata_type_py);
    PyModule_AddObject(module, "CertMetadata", (PyObject*)&g_crypto_cert_metadata_type_py);
    PyModule_AddObject(module, "Cert", (PyObject*)&g_crypto_cert_type_py);
    PyModule_AddObject(module, "PkeyType", (PyObject*)&DapPkeyTypeObject_DapPkeyTypeObjectType);
    PyModule_AddObject(module, "Pkey", (PyObject*)&DapPkeyObject_DapPkeyObjectType);
    PyModule_AddObject(module, "HashType", (PyObject*)&DapHashTypeObject_DapChainHashTypeObjectType);
    PyModule_AddObject(module, "HashFast", (PyObject*)&DapHashFastObject_DapHashFastObjectType);
    PyModule_AddObject(module, "EncKey", (PyObject*)&CryptoKeyObjecy_CryptoKeyObjecyType);
    PyModule_AddObject(module, "Crypto", (PyObject *)&g_crypto_type_py);
    PyModule_AddObject(module, "CryptoDataType", (PyObject *)&CryptoDataTypeObjecy_CryptoDataTypeObjecyType);
//    PyModule_AddObject(module, "CryptoKey", (PyObject *)&PyCryptoKeyObject_PyCryptoKeyType);
    PyModule_AddObject(module, "KeyType", (PyObject *)&CryptoKeyTypeObjecy_CryptoKeyTypeObjecyType);
    PyModule_AddObject(module, "Cert", (PyObject*)&g_crypto_cert_type_py);
    PyModule_AddObject(module, "CertMetadataType", (PyObject*)&g_crypto_cert_type_metadata_type_py);
    PyModule_AddObject(module, "HashType", (PyObject*)&DapHashTypeObject_DapChainHashTypeObjectType);
    PyModule_AddObject(module, "HashFast", (PyObject*)&DapHashFastObject_DapHashFastObjectType);
    PyModule_AddObject(module, "PkeyType", (PyObject*)&DapPkeyTypeObject_DapPkeyTypeObjectType);
    PyModule_AddObject(module, "Pkey", (PyObject*)&DapPkeyObject_DapPkeyObjectType);
    PyModule_AddObject(module, "SignType", (PyObject*)&DapSignTypeObject_DapSignTypeObjectType);
    PyModule_AddObject(module, "Sign", (PyObject*)&DapSignObject_DapSignObjectType);
    // net/core
    PyModule_AddObject(module, "DapEvents", (PyObject*)&dapEvents_dapEventsType);
    PyModule_AddObject(module, "DapEventsSocket", (PyObject*)&dapEventsSocket_dapEventsSocketType);
    PyModule_AddObject(module, "DapServer", (PyObject*)&dapServer_dapServerType);
    PyModule_AddObject(module, "ServerCore", (PyObject*)&ServerCore_ServerCoreType);
    // net/client
    PyModule_AddObject(module, "Client", (PyObject*)&dapClientObject_dapClientType);
    PyModule_AddObject(module, "ClientError", (PyObject*)&dapClientErrorObject_dapClientErrorType);
    PyModule_AddObject(module, "ClientStageStatus", (PyObject*)&dapClientStageStatusObject_dapClientStageStatusType);
    PyModule_AddObject(module, "ClientStage", (PyObject*)&dapClientStageObject_dapClientStageType);
    // net/server/enc
    PyModule_AddObject(module, "EncHttp", (PyObject*)&DapEncServer_DapEncServerType);
    // net/server/http
    PyModule_AddObject(module, "HttpSimple", (PyObject*)&DapHTTPSimple_DapHTTPSimpleType);
    PyModule_AddObject(module, "HTTPCode", (PyObject*)&HTTPCode_HTTPCodeType);
    PyModule_AddObject(module, "Http", (PyObject*)&DapHTTP_DapHTTPType);
    PyModule_AddObject(module, "HttpFolder", (PyObject*)&DapHttpFolder_DapHttpFolderType);
    // net/stream
    PyModule_AddObject(module, "StreamCH", (PyObject*)&dapStreamChObject_dapStreamChType);
    PyModule_AddObject(module, "StreamCtl", (PyObject*)&DapStreamCtl_DapStreamCtlType);
    PyModule_AddObject(module, "StreamWorker", (PyObject*)&dapStreamWorkerObject_dapStreamWorkerType);
    PyModule_AddObject(module, "Stream", (PyObject*)&DapStream_DapStreamType);
    //app_context
    #ifdef DAP_SUPPORT_PYTHON_PLUGINS
    PyModule_AddObject(module, "AppContext", (PyObject*)&dapAppContext_dapAppContextType);
    #endif
    return module;
}

void deinit_modules(void){
    if (!submodules_deint){
        log_it(L_NOTICE, "Start deint submodules");
        if (s_init_crypto){
            dap_crypto_deinit();
            dap_cert_deinit();
        }
        if (s_init_stream){
            dap_stream_deinit();
        }
        if (s_init_stream_ctl){
            dap_stream_ctl_deinit();
        }
        if (s_init_http_folder){
            dap_http_folder_deinit();
        }
        if (s_init_http){
            dap_http_deinit();
        }
        if (s_init_server_core){
            dap_server_core_deinit();
        }
        if (s_init_ks){
            dap_enc_ks_deinit();
        }
        dap_config_close(g_config);
        dap_config_deinit();
        dap_common_deinit();
        submodules_deint = true;
    }
//    if (s_init_wallet){
//        dap_chain_wallet_deinit_py();
//    }
    dap_config_close(g_config);
    dap_config_deinit();
    dap_common_deinit();
}

PyObject *python_DAP_deinit(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    deinit_modules();
    return PyLong_FromLong(0);
}

//#ifndef DAP_SUPPORT_PYTHON_PLUGINS
//int main(int argc, char **argv) {
//    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
//    if (program == NULL) {
//        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
//        exit(1);
//    }

//    /* Add a built-in module, before Py_Initialize */
//    PyImport_AppendInittab("DAP", PyInit_libDAP);

//    /* Pass argv[0] to the Python interpreter */
//    Py_SetProgramName(program);

//    /* Initialize the Python interpreter.  Required. */
//    Py_Initialize();

//    /* Optionally import the module; alternatively,
//       import can be deferred until the embedded script
//       imports it. */
//    PyImport_ImportModule(".");

//    PyMem_RawFree(program);
//    return 0;
//}
//#endif
