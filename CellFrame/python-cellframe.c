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
            events = _PyObject_New(&dapEvents_dapEventsType);
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
            if (dap_chain_net_srv_init(g_config) != 0){
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

    if (    PyType_Ready( &DapCoreObjectType ) < 0 ||
            PyType_Ready( &g_crypto_type_py ) < 0 ||
            PyType_Ready( &g_crypto_cert_type_py ) < 0 ||
            PyType_Ready( &ServerCore_ServerCoreType ) < 0 ||
            PyType_Ready( &dapEvents_dapEventsType ) < 0 ||
            PyType_Ready( &dapEventsSocket_dapEventsSocketType ) < 0 ||
            PyType_Ready( &CryptoKeyTypeObject_CryptoKeyTypeObjectType ) < 0 ||
            PyType_Ready( &CryptoDataTypeObject_CryptoDataTypeObjectType ) < 0 ||
            PyType_Ready(&DapSignObject_DapSignObjectType) < 0 ||
            PyType_Ready(&DapSignTypeObject_DapSignTypeObjectType) < 0 ||
            // === Chain ===
            PyType_Ready( &dapChainObject_dapChainType ) < 0 ||
            PyType_Ready( &dapChainTypeObject_dapChainTypeType ) < 0 ||
            PyType_Ready( &DapChainAtomIter_DapChainAtomIterType ) < 0 ||
            PyType_Ready(&DapChainAtomPtr_DapChainAtomPtrType) < 0 ||
            PyType_Ready( &DapChainCell_DapChainCellObjectType ) < 0 ||
//            PyType_Ready(&ChainCommonObject_ChainCommonType) < 0 ||

            PyType_Ready( &DapChainCellIDObject_DapChainCellIDType) < 0 ||
            PyType_Ready( &DapChainCellIDObject_DapChainCellIDType) < 0 ||
            PyType_Ready( &DapChainNodeAddrObject_DapChainNodeAddrObjectType) < 0 ||
            PyType_Ready( &DapChainHashSlowObject_DapChainHashSlowObjectType) < 0 ||
            PyType_Ready( &DapHashFastObject_DapHashFastObjectType) < 0 ||
            PyType_Ready( &DapChainHashSlowObject_DapChainHashSlowObjectType) < 0 ||
            PyType_Ready( &DapChainAddrObject_DapChainAddrObjectType) < 0 ||

            PyType_Ready( &DapChainCsObject_DapChainCsObjectType) < 0 ||
            PyType_Ready( &DapChainDatumTypeIdObject_DapChainDatumTypeIdObjectType) < 0 ||
            PyType_Ready( &DapChainDatumObject_DapChainDatumObjectType) < 0 ||
            PyType_Ready(&DapChainDatumIterObject_DapChainDatumIterObjectType) < 0 ||
            PyType_Ready(&DapChainDatumToken_DapChainDatumTokenObjectType) < 0 ||
            PyType_Ready(&DapChainTxTokenExt_DapChainTxTokenExtType) < 0 ||
            PyType_Ready(&DapChainDatumTokenEmission_DapChainDatumTokenEmissionObjectType) < 0 ||
            PyType_Ready(&DapChainTxItemObject_DapChainTxItemTypeObjectType) < 0 ||
            PyType_Ready(&DapChainTxCondType_DapChainTxCondTypeObject) < 0 ||
            PyType_Ready(&DapChainDatumTx_DapChainDatumTxObjectType) < 0 ||
            PyType_Ready(&DapChainTxOutCond_DapChainTxOutCondType) < 0 ||
            PyType_Ready(&DapChainTxOutCondSubTypeSrvPay_DapChainTxOutCondSubTypeSrvPayObject) < 0 ||
            PyType_Ready(&DapChainTxOutCondSubTypeSrvStake_DapChainTxOutCondSubTypeSrvStakeObject) < 0 ||
            PyType_Ready(&DapChainTxOutCondSubTypeSrvXchange_DapChainTxOutCondSubTypeSrvXchangeObject) < 0 ||
            PyType_Ready(&DapChainLedger_DapChainLedgerType) < 0 ||
            PyType_Ready(&DapChainTxInObject_DapChainTxInTypeObjectType) < 0 ||
            PyType_Ready(&DapChainTxInCondObject_DapChainTxInCondTypeObjectType) < 0 ||
            PyType_Ready(&DapChainTxOutObject_DapChainTxOutTypeObjectType) < 0 ||
            PyType_Ready(&DapChainTxPkeyObject_DapChainTxPkeyTypeObjectType) < 0 ||
            PyType_Ready(&DapChainTxReceiptObject_DapChainTxReceiptTypeObjectType) < 0 ||
            PyType_Ready(&DapChainTxOutExtObject_DapChainTxOutExtTypeObjectType) < 0 ||
            // =============
            // === Chain net ===
            PyType_Ready(&DapChainNetObject_DapChainNetObjectType) < 0 ||
            PyType_Ready(&DapChainNodeCliObject_DapChainNodeCliObjectType) < 0 ||
            PyType_Ready(&DapChainNodeClientObject_DapChainNodeClientObjectType) < 0 ||
            PyType_Ready(&DapChainNodeInfoObject_DapChainNodeInfoObjectType) < 0 ||
            PyType_Ready(&DapChainNetNodeObject_DapChainNetNodeObjectType) < 0 ||
            PyType_Ready(&DapChainNetStateObject_DapChainNetStateObjectType) < 0 ||
            // =============
            // === Chain net srv ===
            PyType_Ready(&DapChainNetSrvObject_DapChainNetSrvObjectType) < 0 ||
            PyType_Ready(&DapChainNetSrvClientObject_DapChainNetSrvClientObjectType) < 0 ||
            PyType_Ready(&DapChainNetSrvDatumObject_DapChainNetSrvDatumObjectType) < 0 ||
            PyType_Ready(&DapChainNetSrvOrderDirectionObject_DapChainNetSrvOrderDirectionObjectType) < 0 ||
            PyType_Ready(&DapChainNetSrvOrderObject_DapChainNetSrvOrderObjectType) < 0 ||
            PyType_Ready(&DapChainNetSrvUIDObject_DapChainNetSrvUIDObjectType) < 0 ||
            PyType_Ready(&DapChainNetSrvPriceObject_DapChainNetSrvPriceObjectType) < 0 ||
            PyType_Ready(&DapChainNetSrvPriceUnitUIDObject_DapChainNetSrvPriceUnitUIDObjectType) < 0 ||
            // =============
            // === Chain cs dag poa
            PyType_Ready(&DapChainCsDagPoaObject_DapChainCsDagPoaObjectType) < 0 ||
            // =============

            PyType_Ready(&DapChainGDBObject_DapChainGDBType) < 0 ||

            PyType_Ready(&DapHTTP_DapHTTPType) < 0 ||
            PyType_Ready(&DapEncServer_DapEncServerType) < 0 ||
            PyType_Ready(&DapStream_DapStreamType) < 0 ||
            PyType_Ready(&DapStreamCtl_DapStreamCtlType) < 0 ||
            PyType_Ready(&DapMempool_DapMempoolType) < 0 ||
            // ====
            PyType_Ready(&DapAppCli_dapAppCliType ) < 0 ||
            PyType_Ready(&DapChainWallet_dapChainWalletType) < 0 ||
            PyType_Ready(&HTTPCode_HTTPCodeType) < 0 ||
            PyType_Ready(&DapJSONRPCRequest_DapJSONRPCRequestType) < 0 ||
            PyType_Ready(&DapJSONRPCResponse_DapJSONRPCResponseType) < 0 ||
            #ifdef DAP_SUPPORT_PYTHON_PLUGINS
                PyType_Ready(&DapHTTPSimple_DapHTTPSimpleType) < 0 ||
                PyType_Ready(&dapAppContext_dapAppContextType) < 0
            #else
                PyType_Ready(&DapHTTPSimple_DapHTTPSimpleType) < 0
            #endif
            ){
        log_it(L_CRITICAL,"Not all python type objects are initialized");
        return NULL;
    }

    PyObject *module = PyModule_Create(&CellFramePythonModule);
    PyModule_AddStringConstant(module, "__author__", "Alexey Stratulat <alexey.stratulat@demlabs.net>");
    PyModule_AddStringConstant(module, "__version__", DAP_VERSION);

    CellFrame_error = PyErr_NewException("CellFrame.error", NULL, NULL);

    PyModule_AddObject(module, "error", CellFrame_error);

    PyModule_AddObject(module, "DEBUG", PyLong_FromLong(L_DEBUG));
    PyModule_AddObject(module, "INFO", PyLong_FromLong(L_INFO));
    PyModule_AddObject(module, "NOTICE", PyLong_FromLong(L_NOTICE));
    PyModule_AddObject(module, "MESSAGE", PyLong_FromLong(L_MSG));
    PyModule_AddObject(module, "DAP", PyLong_FromLong(L_DAP));
    PyModule_AddObject(module, "WARNING", PyLong_FromLong(L_WARNING));
    PyModule_AddObject(module, "ATT", PyLong_FromLong(L_ATT));
    PyModule_AddObject(module, "ERROR", PyLong_FromLong(L_ERROR));
    PyModule_AddObject(module, "CRITICAL", PyLong_FromLong(L_CRITICAL));

    PyModule_AddObject(module, "Crypto", (PyObject*)&g_crypto_type_py);
    PyModule_AddObject(module, "Cert", (PyObject*)&g_crypto_cert_type_py);
    PyModule_AddObject(module, "SignType", (PyObject*)&DapSignTypeObject_DapSignTypeObjectType);
    PyModule_AddObject(module, "Sign", (PyObject*)&DapSignObject_DapSignObjectType);

    PyModule_AddObject(module, "ServerCore", (PyObject*)&ServerCore_ServerCoreType);
    PyModule_AddObject(module, "Events", (PyObject*)&dapEvents_dapEventsType);
    PyModule_AddObject(module, "EventsSocket", (PyObject*)&dapEventsSocket_dapEventsSocketType);

    PyModule_AddObject(module, "CryptoKeyType", (PyObject*)&CryptoKeyTypeObject_CryptoKeyTypeObjectType);
    PyModule_AddObject(module, "CryptoDataType", (PyObject*)&CryptoDataTypeObject_CryptoDataTypeObjectType);

    // === Chain ===
    PyModule_AddObject(module, "Chain", (PyObject*)&dapChainObject_dapChainType);
    PyModule_AddObject(module, "ChainType", (PyObject*)&dapChainTypeObject_dapChainTypeType);
    PyModule_AddObject(module, "AtomIter", (PyObject*)&DapChainAtomIter_DapChainAtomIterType);
    PyModule_AddObject(module, "AtomPtr", (PyObject*)&DapChainAtomPtr_DapChainAtomPtrType);
    PyModule_AddObject(module, "Cell", (PyObject*)&DapChainCell_DapChainCellObjectType);

    PyModule_AddObject(module, "ChainID", (PyObject*)&DapChainIDObject_DapChainIDType);
    PyModule_AddObject(module, "CellID", (PyObject*)&DapChainCellIDObject_DapChainCellIDType);
    PyModule_AddObject(module, "NodeAddr", (PyObject*)&DapChainNodeAddrObject_DapChainNodeAddrObjectType);
////    PyModule_AddObject(module, "ChainNetID", (PyObject*)&DapChainNetIdObject_DapChainNetIdObjectType);
    PyModule_AddObject(module, "HashSlow", (PyObject*)&DapChainHashSlowObject_DapChainHashSlowObjectType);
    PyModule_AddObject(module, "HashFast", (PyObject*)&DapHashFastObject_DapHashFastObjectType);
//    PyModule_AddObject(module, "ChainHAshSlowKind", (PyObject*)&DapChainSlowKindObject_DapChainSlowKindType);
    PyModule_AddObject(module, "ChainAddr", (PyObject*)&DapChainAddrObject_DapChainAddrObjectType);
//
    PyModule_AddObject(module, "CS", (PyObject*)&DapChainCsObject_DapChainCsObjectType);
    PyModule_AddObject(module, "DatumTypeId", (PyObject*)&DapChainDatumTypeIdObject_DapChainDatumTypeIdObjectType);
    PyModule_AddObject(module, "Datum", (PyObject*)&DapChainDatumObject_DapChainDatumObjectType);
    PyModule_AddObject(module, "DatumIter", (PyObject*)&DapChainDatumIterObject_DapChainDatumIterObjectType);
    PyModule_AddObject(module, "ChainDatumToken", (PyObject*)&DapChainDatumToken_DapChainDatumTokenObjectType);
    PyModule_AddObject(module, "ChainDatumTokenExt", (PyObject*)&DapChainTxTokenExt_DapChainTxTokenExtType);
    PyModule_AddObject(module, "ChainDatumTokenEmisson", (PyObject*)&DapChainDatumTokenEmission_DapChainDatumTokenEmissionObjectType);
    PyModule_AddObject(module, "ChainTxItemType", (PyObject*)&DapChainTxItemObject_DapChainTxItemTypeObjectType);
    PyModule_AddObject(module, "ChainTxCondType", (PyObject*)&DapChainTxCondType_DapChainTxCondTypeObject);
    PyModule_AddObject(module, "ChainDatumTx", (PyObject*)&DapChainDatumTx_DapChainDatumTxObjectType);
    PyModule_AddObject(module, "ChainTxOutCond", (PyObject*)&DapChainTxOutCond_DapChainTxOutCondType);
    PyModule_AddObject(module, "ChainTxOutCondSubtypeSrvPay", (PyObject*)&DapChainTxOutCondSubTypeSrvPay_DapChainTxOutCondSubTypeSrvPayObject);
    PyModule_AddObject(module, "ChainTxOutCondSubtypeSrvStake", (PyObject*)&DapChainTxOutCondSubTypeSrvStake_DapChainTxOutCondSubTypeSrvStakeObject);
    PyModule_AddObject(module, "ChainTxOutCondSubtypeSrvXchange", (PyObject*)&DapChainTxOutCondSubTypeSrvXchange_DapChainTxOutCondSubTypeSrvXchangeObject);
    PyModule_AddObject(module, "ChainLedger", (PyObject*)&DapChainLedger_DapChainLedgerType);
    PyModule_AddObject(module, "ChainTxIn", (PyObject*)&DapChainTxInObject_DapChainTxInTypeObjectType);
    PyModule_AddObject(module, "ChainTxInCond", (PyObject*)&DapChainTxInCondObject_DapChainTxInCondTypeObjectType);
    PyModule_AddObject(module, "ChainTxOut", (PyObject*)&DapChainTxOutObject_DapChainTxOutTypeObjectType);
    PyModule_AddObject(module, "ChainTxPkey", (PyObject*)&DapChainTxPkeyObject_DapChainTxPkeyTypeObjectType);
    PyModule_AddObject(module, "ChainTxReceipt", (PyObject*)&DapChainTxReceiptObject_DapChainTxReceiptTypeObjectType);
    PyModule_AddObject(module, "ChainTxOutExt", (PyObject*)&DapChainTxOutExtObject_DapChainTxOutExtTypeObjectType);
    // =============
    // === Chain net ===
    PyModule_AddObject(module, "ChainNet", (PyObject*)&DapChainNetObject_DapChainNetObjectType);
    PyModule_AddObject(module, "ChainNodeClient", (PyObject*)&DapChainNodeClientObject_DapChainNodeClientObjectType);
    PyModule_AddObject(module, "ChainNodeInfo", (PyObject*)&DapChainNodeInfoObject_DapChainNodeInfoObjectType);
    PyModule_AddObject(module, "ChainNetNode", (PyObject*)&DapChainNetNodeObject_DapChainNetNodeObjectType);
    PyModule_AddObject(module, "ChainNetState", (PyObject*)&DapChainNetStateObject_DapChainNetStateObjectType);
    // =============
    // === Chain net srv ===
    PyModule_AddObject(module, "ChainNetSrv", (PyObject*)&DapChainNetSrvObject_DapChainNetSrvObjectType);
    PyModule_AddObject(module, "ChainNetSrvClient", (PyObject*)&DapChainNetSrvClientObject_DapChainNetSrvClientObjectType);
    PyModule_AddObject(module, "ChainNetSrvDatum", (PyObject*)&DapChainNetSrvDatumObject_DapChainNetSrvDatumObjectType);
    PyModule_AddObject(module , "ChainNetSrvPrice", (PyObject*)&DapChainNetSrvPriceObject_DapChainNetSrvPriceObjectType);
    PyModule_AddObject(module, "ChainNetSrvOrder", (PyObject*)&DapChainNetSrvOrderObject_DapChainNetSrvOrderObjectType);
    PyModule_AddObject(module, "ChainNetSrvOrderDirection", (PyObject*)&DapChainNetSrvOrderDirectionObject_DapChainNetSrvOrderDirectionObjectType);
    PyModule_AddObject(module, "ChainNetSrvUID", (PyObject*)&DapChainNetSrvUIDObject_DapChainNetSrvUIDObjectType);
    PyModule_AddObject(module, "ChainNetSrvPriceUnitUID", (PyObject*)&DapChainNetSrvPriceUnitUIDObject_DapChainNetSrvPriceUnitUIDObjectType);
    // =============
    // === Chain cs dag poa
    PyModule_AddObject(module, "ChainCsDagPoa", (PyObject*)&DapChainCsDagPoaObject_DapChainCsDagPoaObjectType);
    // =============

    PyModule_AddObject(module, "ChainGDB", (PyObject*)&DapChainGDBObject_DapChainGDBType);
    PyModule_AddObject(module, "ChainWallet", (PyObject*)&DapChainWallet_dapChainWalletType);

    PyModule_AddObject(module, "Http", (PyObject*)&DapHTTP_DapHTTPType);
    PyModule_AddObject(module, "HttpSimple", (PyObject*)&DapHTTPSimple_DapHTTPSimpleType);
    PyModule_AddObject(module, "HttpCode", (PyObject*)&HTTPCode_HTTPCodeType);
    PyModule_AddObject(module, "EncHttp", (PyObject*)&DapEncServer_DapEncServerType);
    PyModule_AddObject(module, "Stream", (PyObject*)&DapStream_DapStreamType);
    PyModule_AddObject(module, "StreamCtl", (PyObject*)&DapStreamCtl_DapStreamCtlType);
    PyModule_AddObject(module, "Mempool", (PyObject*)&DapMempool_DapMempoolType);

    // ==============
    PyModule_AddObject(module, "AppCli", (PyObject*)&DapAppCli_dapAppCliType);
    PyModule_AddObject(module, "AppCliServer", (PyObject*)&DapChainNodeCliObject_DapChainNodeCliObjectType);
    #ifdef DAP_SUPPORT_PYTHON_PLUGINS
        PyModule_AddObject(module, "AppContext", (PyObject*)&dapAppContext_dapAppContextType);
    #endif
    PyModule_AddObject(module, "JSONRPCRequest", (PyObject*)&DapJSONRPCRequest_DapJSONRPCRequestType);
    PyModule_AddObject(module, "JSONRPCResponse", (PyObject*)&DapJSONRPCResponse_DapJSONRPCResponseType);

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
