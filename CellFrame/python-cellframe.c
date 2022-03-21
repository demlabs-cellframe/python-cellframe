#include "python-cellframe.h"

#define LOG_TAG "python-cellframe"

static bool s_init_crypto = false;
static bool s_init_chain = false;
static bool s_init_app_cli = false;
static bool s_init_stream = false;
static bool s_init_stream_ctl = false;
static bool s_init_http_folder = false;
static bool s_init_http = false;
static bool s_init_http_enc = false;
static bool s_init_server_core = false;
static bool s_init_mempool = false;

//static bool s_init_http_client_simple = false;
static bool s_init_wallet = false;
static bool s_init_cs_dag = false;
static bool s_init_cs_dag_poa = false;
static bool s_init_cs_dag_pos = false;
static bool s_init_chain_net_srv = false;
static bool s_init_ks = false;

static bool submodules_deint;

PyObject* CellFrame_error = NULL;


#ifdef _WIN32

BOOL WINAPI consoleHandler(DWORD dwType){
    if (dwType == CTRL_C_EVENT){
        log_it(L_NOTICE, "Execution terminated. Ctrl+C is pressed");
        dap_server_loop_stop();
        deinit_modules();
    }
    return TRUE;
}
#else

void sigfunc(int sig){
    if (sig == SIGINT){
        log_it(L_NOTICE, "Execution terminated. Ctrl+C is pressed");
        dap_events_stop_all();
        deinit_modules();
    }
}
#endif

PyObject *python_cellframe_init(PyObject *self, PyObject *args){
    const char *app_name;
    const char *file_name_log;
    const char *config_dir;
    const char *log_level;
    const char *JSON_str;

    s_init_ks = true;
    submodules_deint = false;

    PyObject *events = NULL;

    #ifdef _WIN32
        setConsoleCtrlHandler((PHANDLER_ROUTINE)consoleHandler, TRUE);
    #else
        signal(SIGINT, sigfunc);
    #endif

    if (!PyArg_ParseTuple(args, "s", &JSON_str)){
        PyErr_SetString(CellFrame_error, "Argument must be a string");
        return NULL;
    }

    PyObject *JSON_Module = PyImport_ImportModule("json");
    if (JSON_Module == NULL) {
        PyErr_SetString(CellFrame_error, "json module was not imported");
        return NULL;
    }

    PyObject* JSONLoadsFunction = PyObject_GetAttrString(JSON_Module, "loads");
    if (JSONLoadsFunction == NULL) {
        PyErr_SetString(CellFrame_error, "json.loads function was not imported");
        return NULL;
    }

    PyObject* argsInLoadsJSON = PyTuple_Pack(1,PyUnicode_FromString(JSON_str));
    PyObject* result = PyObject_CallObject(JSONLoadsFunction, argsInLoadsJSON);
    if (result == NULL){
        PyErr_SetString(CellFrame_error, "Erron in \"json.loads\" function. Argument must be a JSON format string");
        return NULL;
    }
    PyObject* getModules = PyDict_GetItemString(result, "modules");
    if (getModules == NULL){
        PyErr_SetString(CellFrame_error, "Can't find \"modules\" object in JSON string");
        return NULL;
    }

    // DAP or Core
    PyObject* getDap = PyDict_GetItemString(result, "DAP");

    if (getDap == NULL)
        getDap = PyDict_GetItemString(result, "Core");

    if( getDap == NULL ){
        PyErr_SetString(CellFrame_error, "Can't find \"Core\" or \"DAP\" object in JSON string");
        return NULL;
    }

    /*Parse DAP*/
    PyObject* config_dir_PyObject = PyDict_GetItemString(getDap, "config_dir");
    if (config_dir_PyObject == NULL) {
        PyErr_SetString(CellFrame_error, "Can't find \"config_dir\" key in \"DAP\" object");
        return NULL;
    }
    PyObject* application_name_PyObject = PyDict_GetItemString(getDap, "application_name");
    if (application_name_PyObject == NULL) {
        PyErr_SetString(CellFrame_error, "Can't find \"application_name\" key in \"DAP\" object");
        return NULL;
    }
    PyObject* file_name_log_PyObject = PyDict_GetItemString(getDap, "file_name_log");
    if (file_name_log_PyObject == NULL) {
        PyErr_SetString(CellFrame_error, "Can't find \"file_name_log\" key in \"DAP\" object");
        return NULL;
    }
    PyObject* logLevel_PyObject = PyDict_GetItemString(getDap, "log_level");
    if (logLevel_PyObject == NULL) {
        PyErr_SetString(CellFrame_error, "Can't find \"log_level\" key in \"DAP\" object");
        return NULL;
    }
    app_name = PyUnicode_AsUTF8(application_name_PyObject);
    file_name_log = PyUnicode_AsUTF8(file_name_log_PyObject);
    config_dir = PyUnicode_AsUTF8(config_dir_PyObject);
    log_level = PyUnicode_AsUTF8(logLevel_PyObject);

    if (dap_common_init(app_name, file_name_log, NULL ) != 0){
        PyErr_SetString(CellFrame_error, "Can't initialize CellFrame SDK library");
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
        PyErr_SetString(CellFrame_error, "Can't generate configuration files. Directory path must contain only ASCII simbols");
        return NULL;
    case -3:
        PyErr_SetString(CellFrame_error, "Can't generate configuration files. Can't open a file stream");
        return NULL;
    }
    //Init config
    dap_config_init(config_dir);
    if ((g_config = dap_config_open(app_name) ) == NULL){
        PyErr_SetString(CellFrame_error, "Can't inititialize general configurations");
        return NULL;
    }
    //Init modules
    log_it(L_INFO, "Initializing modules ...");
    if (!PyList_Check(getModules)){
        PyErr_SetString(CellFrame_error, "Can't find an array of module names");
        return NULL;
    }
    Py_ssize_t size_list = PyList_Size(getModules);
    for (int i=0; i < size_list;i++){
        PyObject *value = PyList_GetItem(getModules, i);
        const char *c_value = PyUnicode_AsUTF8(value);
        log_it(L_INFO, "Initializing \"%s\" module ...", c_value);
        if (strcmp(c_value, "Crypto") == 0){            //Init crypto
//            log_it(L_INFO, "Initializing the %s module", c_value);
            s_init_crypto = true;
            if (dap_crypto_init() != 0){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"Crypto\" module");
                return NULL;
            }
            dap_cert_init();
        } else if (strcmp(c_value, "Events") == 0){
            dap_events_init(0,0);
            events = _PyObject_New(&DapEventsObjectType);
            ((PyDapEventsObject*)events)->t_events = dap_events_new();
            dap_events_start(((PyDapEventsObject*)events)->t_events);
        } else if (strcmp(c_value, "Server") == 0){
            if(dap_server_init() != 0 ){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"Server\" module");
                return NULL;
            }
        }else if (strcmp(c_value, "ServerCore") == 0){
            PyObject* getServerCoreData = PyDict_GetItemString(result, "ServerCore");
            if (getServerCoreData == NULL){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"ServerCore\" module."
                                "Can't find \"ServerCore\" object in JSON string");
                return NULL;
            }
            PyObject* Nl_thread_cnt = PyDict_GetItemString(getServerCoreData, "thread_cnt");
            PyObject* Nl_conn = PyDict_GetItemString(getServerCoreData, "conn");
            if ( (Nl_thread_cnt == NULL || Nl_conn == NULL) || !PyNumber_Check(Nl_thread_cnt) ||
                 !PyNumber_Check(Nl_conn)){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"ServerCore\" module."
                                                 "Values of \"thread_cnt\" and \"conn\" keys aren't numerical or are absent");
                return NULL;
            }
            PyObject *ll_thread_cnt= PyNumber_Long(Nl_thread_cnt);
            PyObject *ll_conn = PyNumber_Long(Nl_conn);
            uint32_t ul_thread_cnt = (uint32_t)PyLong_AsUnsignedLong(ll_thread_cnt);
            size_t ul_conn = PyLong_AsSize_t(ll_conn);
            if(dap_server_core_init(ul_thread_cnt, ul_conn) != 0 ){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"ServerCore\" module");
                return NULL;
            }
            s_init_server_core = true;
        } else if (strcmp(c_value, "Chain") == 0){
            if(init_chain_py() != 0){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"Chain\" module");
                return NULL;
            }
            if (dap_chain_cs_init_py() != 0){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"ChainCS\" module");
                return NULL;
            }
        } else if (strcmp(c_value, "Http") == 0){
            if(dap_http_init() != 0){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"Http\" module");
                return NULL;
            }
            s_init_http = true;
        } else if (strcmp(c_value, "HttpSimple") == 0){
            if (dap_http_simple_module_init() != 0){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"HttpSimple\" module");
                return NULL;
            }
        } else if (strcmp(c_value, "EncHttp") == 0){
            if(enc_http_init() != 0){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"EncHttp\" module");
                return NULL;
            }
            s_init_http_enc = true;
        } else if (strcmp(c_value, "HttpFolder") == 0){
            if (dap_http_folder_init() !=0){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"HttpFolder\" module");
                return NULL;
            }
            s_init_http_folder = true;
        } else if (strcmp(c_value, "Stream") == 0){
            PyObject* getStreamData = PyDict_GetItemString(result, "Stream");
            if (getStreamData == NULL){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"Stream\" module."
                                "Can't find \"ServerCore\" object in JSON string");
                return NULL;
            }
            PyObject *debugDumpStreamHeadersObj = PyDict_GetItemString(getStreamData, "DebugDumpStreamHeaders");
            if (debugDumpStreamHeadersObj == NULL || !PyBool_Check(debugDumpStreamHeadersObj)){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"Stream\" module."
                                                 "Value of \"DebugDumpStreamHeaders\" key isn't boolean");
                return NULL;
            }
            if(dap_stream_init(g_config) != 0){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"Stream\" module");
                return NULL;
            }
            s_init_stream = true;
        } else if (strcmp(c_value, "StreamCtl") == 0){
            if (dap_stream_ctl_init_py(32) != 0){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"StreamCtl\" module");
                return NULL;
            }
            s_init_stream_ctl = true;
        } else if (strcmp(c_value, "Mempool") == 0){
            if (dap_datum_mempool_init() != 0){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"Mempool\" module");
                return NULL;
            }
            s_init_mempool = true;
        /*
        } else if (strcmp(c_value, "HttpClientSimple") == 0){
            if (dap_http_client_simple_init() != 0){
                PyErr_SetString(CellFrame_error, "Failed to initialize HttpClientSimple module. ");
                return NULL;
            }
            s_init_http_client_simple = true;*/
        } else if (strcmp(c_value, "Wallet") == 0){
            if (dap_chain_wallet_init_py() != 0){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"Wallet\" module");
                return NULL;
            }
            s_init_wallet = true;
/*        } else if (strcmp(c_value, "ChainCSDag") == 0){
            if (dap_chain_cs_dag_init() != 0)
            {
                PyErr_SetString(CellFrame_error, "Failed to initialize ChainCSDag module. ");
                return NULL;
            }
        } else if (strcmp(c_value, "ChainCSDagPoa") == 0){
            if (dap_chain_cs_dag_poa_init() != 0){
                PyErr_SetString(CellFrame_error, "Failed to initialize ChainCSDagPoa module. ");
                return NULL;
            }
        } else if (strcmp(c_value, "ChainCSDagPos") == 0){
            if (dap_chain_cs_dag_pos_init() != 0){
                PyErr_SetString(CellFrame_error, "Failed to initialize ChainCSDagPos module. ");
                return NULL;
            } */
        } else if (strcmp(c_value, "ChainNetSrv") == 0){
            if (dap_chain_net_srv_init() != 0){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"ChainNetSrv\" module");
                return NULL;
            }
        }else if (strcmp(c_value, "StreamChChain") == 0){
            if (dap_stream_ch_chain_init() != 0 ){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"StreamChChain\" module");
                return NULL;
            }
        } else if (strcmp(c_value, "StreamChChainNet") == 0){
            if (dap_stream_ch_chain_net_init() != 0 ){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"StreamChChainNet\" module");
                return NULL;
            }
        }else if (strcmp(c_value, "StreamChChainNetSrv") == 0){
            if (dap_stream_ch_chain_net_init() != 0 ){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"StreamChChainNetSrv\" module");
                return NULL;
            }
        }else if (strcmp(c_value, "EncKS") == 0){
//            if (dap_enc_ks_init())
//            if (dap_enc_ks_
        } else if (strcmp(c_value, "GlobalDB") == 0){
            if (dap_chain_global_db_init(g_config) != 0){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"GlobalDB\" module");
                return NULL;
            }
        }else if (strcmp(c_value, "Client") == 0){
            if (dap_client_init() != 0){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"Client\" module");
                return NULL;
            }
        }else if (strcmp(c_value, "GDB") == 0){
            if (dap_chain_gdb_init_py() != 0){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"GDB\" module");
                return NULL;
            }
        }else if (strcmp(c_value, "Net") == 0){
            if (dap_chain_net_init_py() != 0 ){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"Net\" module");
                return NULL;
            }
        }else if (strcmp(c_value, "AppCliServer") == 0){
            if (dap_chain_node_cli_init(g_config) != 0 ){
                PyErr_SetString(CellFrame_error, "Failed to initialize \"AppCliServer\" module" );
                return NULL;
            }
        }else{
            log_it(L_WARNING,"Unknown module: \"%s\"", c_value);
        }
        log_it(L_INFO, "Initialization of module is completed");
    }
    if (events == NULL)
        return PyLong_FromLong(0);
    else
        return Py_BuildValue("iO", PyLong_FromLong(0), events);
}

PyMODINIT_FUNC PyInit_libCellFrame(void){
            /* === DAP === */
            // === Core ===
    if (    PyType_Ready( &DapCoreObjectType ) < 0 ||
            PyType_Ready( &DapLogitObjectType ) < 0 ||
#ifdef DAP_SUPPORT_PYTHON_PLUGINS
            PyType_Ready(&DapAppContextObjectType) < 0 ||
#endif
            // === Network ==
            PyType_Ready( &DapServerCoreObjectType ) < 0 ||
            PyType_Ready( &DapEventsObjectType ) < 0 ||
            PyType_Ready( &DapEventsSocketObjectType ) < 0 ||
            PyType_Ready( &DapHttpCodeObjectType) < 0 ||
            PyType_Ready( &DapHttpObjectType) < 0 ||
            PyType_Ready( &DapEncServerObjectType) < 0 ||
            PyType_Ready( &DapStreamObjectType) < 0 ||
            PyType_Ready( &DapStreamCtlObjectType) < 0 ||
            // === Crypto ===
            PyType_Ready( &DapCryptoObjectType ) < 0 ||
            PyType_Ready( &DapCryptoCertObjectType ) < 0 ||
            PyType_Ready( &DapCryptoKeyTypeObjectType ) < 0 ||
            PyType_Ready( &DapCryptoDataTypeObjectType ) < 0 ||
            PyType_Ready( &DapCryproSignTypeObjectType ) < 0 ||
            PyType_Ready( &DapCryptoSignObjectType ) < 0 ||
            /* === Cellframe === */
            // ==== Common ===
            PyType_Ready(&DapAppCliObjectType ) < 0 ||
            PyType_Ready(&DapChainWalletObjectType) < 0 ||
            PyType_Ready(&DapJsonRpcRequestObjectType) < 0 ||
            PyType_Ready(&DapJsonRpcResponseobjectType) < 0 ||
            // === Chain ===
            PyType_Ready( &DapChainObjectType ) < 0 ||
            PyType_Ready( &DapChainTypeObjectType ) < 0 ||
            PyType_Ready( &DapChainAtomIterObjectType ) < 0 ||
            PyType_Ready( &DapChainAtomPtrObjectType ) < 0 ||
            PyType_Ready( &DapChainCellObjectType ) < 0 ||
            PyType_Ready( &DapChainGdbObjectType) < 0 ||
            PyType_Ready( &DapChainMempoolObjectType) < 0 ||
            PyType_Ready( &DapChainCellIdObjectType) < 0 ||
            PyType_Ready( &DapChainNodeAddrObjectType) < 0 ||
            PyType_Ready( &DapChainHashSlowObjectType) < 0 ||
            PyType_Ready( &DapChainHashFastObjectType) < 0 ||
            PyType_Ready( &DapChainAddrObjectType) < 0 ||
            PyType_Ready( &DapChainCsObjectType) < 0 ||
            PyType_Ready( &DapChainLedgerObjectType) < 0 ||
            // === Chain datum
            /// Common
            PyType_Ready( &DapChainDatumTypeIdObjectType) < 0 ||
            PyType_Ready( &DapChainDatumObjectType) < 0 ||
            PyType_Ready( &DapChainDatumIterObjectType) < 0 ||
            /// Token
            PyType_Ready( &DapChainDatumTokenObjectType) < 0 ||
            PyType_Ready( &DapChainDatumTokenEmissionObjectType) < 0 ||
            /// Custom
            PyType_Ready( &DapChainDatumCustomObjectType) < 0 ||
            /// Transaction
            PyType_Ready( &DapChainDatumTxObjectType) < 0 ||
            PyType_Ready( &DapChainTxItemTypeObjectType) < 0 ||
            PyType_Ready( &DapChainTxTokenExtType) < 0 ||
            PyType_Ready( &DapChainTxCondTypeObjectType) < 0 ||
            PyType_Ready( &DapChainTxOutCondObjectType) < 0 ||
            PyType_Ready( &DapChainTxOutCondSubTypeSrvPayObjectType) < 0 ||
            PyType_Ready( &DapChainTxOutCondSubTypeSrvStakeObjectType) < 0 ||
            PyType_Ready( &DapChainTxOutCondSubTypeSrvXchangeObjectType) < 0 ||
            PyType_Ready( &DapChainTxInObjectType) < 0 ||
            PyType_Ready( &DapChainTxInCondObjectType) < 0 ||
            PyType_Ready( &DapChainTxOutObjectType) < 0 ||
            PyType_Ready( &DapChainTxPkeyObjectType) < 0 ||
            PyType_Ready( &DapChainTxReceiptObjectType) < 0 ||
            PyType_Ready( &DapChainTxOutExtObjectType) < 0 ||
            // === Chain net ===
            /// Node
            PyType_Ready( &DapChainNodeObjectType) < 0 ||
            PyType_Ready( &DapChainNodeCliObjectType) < 0 ||
            PyType_Ready( &DapChainNodeClientObjectType) < 0 ||
            PyType_Ready( &DapChainNodeInfoObjectType) < 0 ||
            /// Net
            PyType_Ready( &DapChainNetObjectType) < 0 ||
            PyType_Ready( &DapChainNetStateObjectType) < 0 ||
            /// Srv
            PyType_Ready( &DapChainNetSrvObjectType) < 0 ||
            PyType_Ready( &DapChainNetSrvClientObjectType) < 0 ||
            PyType_Ready( &DapChainNetSrvOrderDirectionObjectType) < 0 ||
            PyType_Ready( &DapChainNetSrvOrderObjectType) < 0 ||
            PyType_Ready( &DapChainNetSrvUidObjectType) < 0 ||
            PyType_Ready( &DapChainNetSrvPriceObjectType) < 0 ||
            PyType_Ready( &DapChainNetSrvPriceUnitUidObjectType) < 0 ||
            // === Chain consensuses
            PyType_Ready( &DapChainCsDagPoaObjectType) < 0
            ){
        log_it(L_CRITICAL,"Not all python type objects are initialized");
        return NULL;
    }

    PyObject *module = PyModule_Create(&CellFramePythonModule);
    PyModule_AddStringConstant(module, "__author__", "Alexey Stratulat <alexey.stratulat@demlabs.net>");
    PyModule_AddStringConstant(module, "__version__", DAP_VERSION);

    CellFrame_error = PyErr_NewException("CellFrame.error", NULL, NULL);

    PyModule_AddObject(module, "error", CellFrame_error);

    PyModule_AddObject(module, "logIt", (PyObject *)&DapLogitObjectType);

    PyModule_AddObject(module, "Crypto", (PyObject*)&DapCryptoObjectType);
    PyModule_AddObject(module, "Cert", (PyObject*)&DapCryptoCertObjectType);
    PyModule_AddObject(module, "SignType", (PyObject*)&DapCryproSignTypeObjectType);
    PyModule_AddObject(module, "Sign", (PyObject*)&DapCryptoSignObjectType);

    PyModule_AddObject(module, "ServerCore", (PyObject*)&DapServerCoreObjectType);
    PyModule_AddObject(module, "Events", (PyObject*)&DapEventsObjectType);
    PyModule_AddObject(module, "EventsSocket", (PyObject*)&DapEventsSocketObjectType);

    PyModule_AddObject(module, "CryptoKeyType", (PyObject*)&DapCryptoKeyTypeObjectType);
    PyModule_AddObject(module, "CryptoDataType", (PyObject*)&DapCryptoDataTypeObjectType);

    // === Chain ===
    PyModule_AddObject(module, "Chain", (PyObject*)&DapChainObjectType);
    PyModule_AddObject(module, "ChainType", (PyObject*)&DapChainTypeObjectType);
    PyModule_AddObject(module, "ChainAtomIter", (PyObject*)&DapChainAtomIterObjectType);
    PyModule_AddObject(module, "ChainAtomPtr", (PyObject*)&DapChainAtomPtrObjectType);
    PyModule_AddObject(module, "ChainCell", (PyObject*)&DapChainCellObjectType);
//    PyModule_AddObject(module, "ChainCommon", (PyObject*)&ChainCommonObject_ChainCommonType);

    PyModule_AddObject(module, "ChainID", (PyObject*)&DapChainIdObjectType);
    PyModule_AddObject(module, "ChainCellID", (PyObject*)&DapChainCellIdObjectType);
    PyModule_AddObject(module, "ChainNodeAddr", (PyObject*)&DapChainNodeAddrObjectType);
    PyModule_AddObject(module, "ChainNetID", (PyObject*)&DapChainNetIdObjectType);
    PyModule_AddObject(module, "ChainHashSlow", (PyObject*)&DapChainHashSlowObjectType);
    PyModule_AddObject(module, "ChainHashFast", (PyObject*)&DapChainHashFastObjectType);
//    PyModule_AddObject(module, "ChainHAshSlowKind", (PyObject*)&DapChainSlowKindObject_DapChainSlowKindType);
    PyModule_AddObject(module, "ChainAddr", (PyObject*)&DapChainAddrObjectType);
//
    PyModule_AddObject(module, "ChainCS", (PyObject*)&DapChainCsObjectType);
    PyModule_AddObject(module, "ChainDatumTypeID", (PyObject*)&DapChainDatumTypeIdObjectType);
    PyModule_AddObject(module, "ChainDatum", (PyObject*)&DapChainDatumObjectType);
    PyModule_AddObject(module, "ChainDatumIter", (PyObject*)&DapChainDatumIterObjectType);
    PyModule_AddObject(module, "ChainDatumToken", (PyObject*)&DapChainDatumTokenObjectType);
    PyModule_AddObject(module, "ChainDatumTokenExt", (PyObject*)&DapChainTxTokenExtType);
    PyModule_AddObject(module, "ChainDatumTokenEmisson", (PyObject*)&DapChainDatumTokenEmissionObjectType);
    PyModule_AddObject(module, "ChainTxItemType", (PyObject*)&DapChainTxItemTypeObjectType);
    PyModule_AddObject(module, "ChainTxCondType", (PyObject*)&DapChainTxCondTypeObjectType);
    PyModule_AddObject(module, "ChainDatumTx", (PyObject*)&DapChainDatumTxObjectType);
    PyModule_AddObject(module, "ChainTxOutCond", (PyObject*)&DapChainTxOutCondObjectType);
    PyModule_AddObject(module, "ChainTxOutCondSubtypeSrvPay", (PyObject*)&DapChainTxOutCondSubTypeSrvPayObjectType);
    PyModule_AddObject(module, "ChainTxOutCondSubtypeSrvStake", (PyObject*)&DapChainTxOutCondSubTypeSrvStakeObjectType);
    PyModule_AddObject(module, "ChainTxOutCondSubtypeSrvXchange", (PyObject*)&DapChainTxOutCondSubTypeSrvXchangeObjectType);
    PyModule_AddObject(module, "ChainLedger", (PyObject*)&DapChainLedgerObjectType);
    PyModule_AddObject(module, "ChainTxIn", (PyObject*)&DapChainTxInObjectType);
    PyModule_AddObject(module, "ChainTxInCond", (PyObject*)&DapChainTxInCondObjectType);
    PyModule_AddObject(module, "ChainTxOut", (PyObject*)&DapChainTxOutObjectType);
    PyModule_AddObject(module, "ChainTxPkey", (PyObject*)&DapChainTxPkeyObjectType);
    PyModule_AddObject(module, "ChainTxReceipt", (PyObject*)&DapChainTxReceiptObjectType);
    PyModule_AddObject(module, "ChainTxOutExt", (PyObject*)&DapChainTxOutExtObjectType);
    // =============
    // === Chain net ===
    PyModule_AddObject(module, "ChainNet", (PyObject*)&DapChainNetObjectType);
    PyModule_AddObject(module, "ChainNodeClient", (PyObject*)&DapChainNodeClientObjectType);
    PyModule_AddObject(module, "ChainNodeInfo", (PyObject*)&DapChainNodeInfoObjectType);
    PyModule_AddObject(module, "ChainNetNode", (PyObject*)&DapChainNodeObjectType);
    PyModule_AddObject(module, "ChainNetState", (PyObject*)&DapChainNetStateObjectType);
    // =============
    // === Chain net srv ===
    PyModule_AddObject(module, "ChainNetSrv", (PyObject*)&DapChainNetSrvObjectType);
    PyModule_AddObject(module, "ChainNetSrvClient", (PyObject*)&DapChainNetSrvClientObjectType);
    PyModule_AddObject(module, "ChainNetSrvDatum", (PyObject*)&DapChainDatumCustomObjectType);
    PyModule_AddObject(module , "ChainNetSrvPrice", (PyObject*)&DapChainNetSrvPriceObjectType);
    PyModule_AddObject(module, "ChainNetSrvOrder", (PyObject*)&DapChainNetSrvOrderObjectType);
    PyModule_AddObject(module, "ChainNetSrvOrderDirection", (PyObject*)&DapChainNetSrvOrderDirectionObjectType);
    PyModule_AddObject(module, "ChainNetSrvUID", (PyObject*)&DapChainNetSrvUidObjectType);
    PyModule_AddObject(module, "ChainNetSrvPriceUnitUID", (PyObject*)&DapChainNetSrvPriceUnitUidObjectType);
    // =============
    // === Chain cs dag poa
    PyModule_AddObject(module, "ChainCsDagPoa", (PyObject*)&DapChainCsDagPoaObjectType);
    // =============

    PyModule_AddObject(module, "ChainGDB", (PyObject*)&DapChainGdbObjectType);
    PyModule_AddObject(module, "ChainWallet", (PyObject*)&DapChainWalletObjectType);

    PyModule_AddObject(module, "Http", (PyObject*)&DapHttpObjectType);
    PyModule_AddObject(module, "HttpSimple", (PyObject*)&DapHTTPSimple_DapHTTPSimpleType);
    PyModule_AddObject(module, "HttpCode", (PyObject*)&DapHttpCodeObjectType);
    PyModule_AddObject(module, "EncHttp", (PyObject*)&DapEncServerObjectType);
    PyModule_AddObject(module, "Stream", (PyObject*)&DapStreamObjectType);
    PyModule_AddObject(module, "StreamCtl", (PyObject*)&DapStreamCtlObjectType);
    PyModule_AddObject(module, "Mempool", (PyObject*)&DapChainMempoolObjectType);

    // ==============
    PyModule_AddObject(module, "AppCli", (PyObject*)&DapAppCliObjectType);
    PyModule_AddObject(module, "AppCliServer", (PyObject*)&DapChainNodeCliObjectType);
    #ifdef DAP_SUPPORT_PYTHON_PLUGINS
        PyModule_AddObject(module, "AppContext", (PyObject*)&DapAppContextObjectType);
    #endif
    PyModule_AddObject(module, "JSONRPCRequest", (PyObject*)&DapJsonRpcRequestObjectType);
    PyModule_AddObject(module, "JSONRPCResponse", (PyObject*)&DapJsonRpcResponseobjectType);

    return module;
}

void deinit_modules(void){
    if (!submodules_deint){
        log_it(L_NOTICE, "Deintializing modules ...");
        if (s_init_crypto){
            dap_crypto_deinit();
            dap_cert_deinit();
        }
        if (s_init_chain){
            deinit_chain_py();
            dap_chain_cs_deinit_py();
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
        log_it(L_INFO, "Deinitialization of modules is completed");
    }
    if (s_init_wallet){
        dap_chain_wallet_deinit_py();
    }
    dap_config_close(g_config);
    dap_config_deinit();
    dap_common_deinit();
}

PyObject *python_cellframe_deinit(PyObject *self, PyObject *args){
    deinit_modules();
    return PyLong_FromLong(0);
}

#ifndef DAP_SUPPORT_PYTHON_PLUGINS
int main(int argc, char **argv) {
    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cann't decode argv[0]\n");
        exit(1);
    }

    /* Add a built-in module, before Py_Initialize */
    PyImport_AppendInittab("CellFrame", PyInit_libCellFrame);

    /* Pass argv[0] to the Python interpreter */
    Py_SetProgramName(program);

    /* Initialize the Python interpreter.  Required. */
    Py_Initialize();

    /* Optionally import the module; alternatively,
       import can be deferred until the embedded script
       imports it. */
    PyImport_ImportModule(".");

    PyMem_RawFree(program);
    return 0;
}
#endif
