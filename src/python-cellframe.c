#include "python-cellframe.h"


static PyObject *python_cellframe_init(PyObject *self, PyObject *args){
    const char *app_name;
    const char *file_name_log;
    const char *config_dir;
    const char *log_level;
    const char *JSON_str;
    init_crypto = false;
    if (!PyArg_ParseTuple(args, "s", &JSON_str)){
        return NULL;
    }

    PyObject *JSON_Module = PyImport_ImportModule("json");
    if (JSON_Module == NULL) {
        PyErr_SetString(CellFrame_error, "ERROR importing module");
        return NULL;
    }
    PyObject* JSONLoadsFunction = PyObject_GetAttrString(JSON_Module, "loads");
    if (JSONLoadsFunction == NULL)
        return NULL;
    PyObject* argsInLoadsJSON = PyTuple_Pack(1,PyUnicode_FromString(JSON_str));
    PyObject* result = PyObject_CallObject(JSONLoadsFunction, argsInLoadsJSON);
    if (result == NULL)
        return NULL;
    PyObject* getModules = PyDict_GetItemString(result, "modules");
    if (getModules == NULL)
        return NULL;
    PyObject* getDap = PyDict_GetItemString(result, "DAP");
    if (getDap == NULL)
        return NULL;
    /*Parse DAP*/
    PyObject* config_dir_PyObject = PyDict_GetItemString(getDap, "config_dir");
    PyObject* application_name_PyObject = PyDict_GetItemString(getDap, "application_name");
    PyObject* file_name_log_PyObject = PyDict_GetItemString(getDap, "file_name_log");
    PyObject* logLevel_PyObject = PyDict_GetItemString(getDap, "log_level");
    if (config_dir_PyObject == NULL || application_name_PyObject == NULL ||
            logLevel_PyObject == NULL || file_name_log_PyObject == NULL)
        return NULL;
    app_name = PyUnicode_AsUTF8(application_name_PyObject);
    file_name_log = PyUnicode_AsUTF8(file_name_log_PyObject);
    config_dir = PyUnicode_AsUTF8(config_dir_PyObject);
    log_level = PyUnicode_AsUTF8(logLevel_PyObject);

    if (dap_common_init(app_name, file_name_log) != 0){
        PyErr_SetString(CellFrame_error, "Can't init common functions module");
        return NULL;
    }

    dap_config_init(config_dir);
    if ((g_config = dap_config_open(app_name) ) == NULL){
        PyErr_SetString(CellFrame_error, "Can't init general configurations");
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
        if (strcmp(c_value, "Crypto") == 0){            //Init crypto
            log_it(L_INFO, "Initializing the %s module", c_value);
            init_crypto = true;
            if (dap_crypto_init() != 0){
                PyErr_SetString(CellFrame_error, "An error occurred while initializing the libdap-crypto-python module.");
                return NULL;
            }
        }
        if (strcmp(c_value, "ServerCore") == 0){
            PyObject* getServerCoreData = PyDict_GetItemString(result, "ServerCore");
            if (getServerCoreData == NULL){
                PyErr_SetString(CellFrame_error, "Initialization failed. ServerCore object not found in JSON."
                                " No settings are specified for initializing libdap-server-core-python.");
                return NULL;
            }
            PyObject* Nl_thread_cnt = PyDict_GetItemString(getServerCoreData, "thread_cnt");
            PyObject* Nl_conn = PyDict_GetItemString(getServerCoreData, "conn");
            if ( (Nl_thread_cnt == NULL || Nl_conn == NULL) || !PyNumber_Check(Nl_thread_cnt) ||
                 !PyNumber_Check(Nl_conn)){
                PyErr_SetString(CellFrame_error, "Failed to initialize ServerCore. "
                                                 "Fields thread_cnt and conn are not numerical or absent.");
                return NULL;
            }
            PyObject *ll_thread_cnt= PyNumber_Long(Nl_thread_cnt);
            PyObject *ll_conn = PyNumber_Long(Nl_conn);
            uint32_t ul_thread_cnt = (uint32_t)PyLong_AsUnsignedLong(ll_thread_cnt);
            size_t ul_conn = PyLong_AsSize_t(ll_conn);
            if(dap_server_core_init(ul_thread_cnt, ul_conn) != 0 ){
                PyErr_SetString(CellFrame_error, "Failed to initialize ServerCore.");
                return NULL;
            }
        }
        if (strcmp(c_value, "Chain") == 0){
            if(init_chain_py() != 0){
                PyErr_SetString(CellFrame_error, "Failed to initialize Chain. "
                                                 "Fields thread_cnt and conn are not numerical or absent.");
                return NULL;
            }
            if (dap_chain_cs_init_py() != 0){
                PyErr_SetString(CellFrame_error, "Failed to initialize Chain CS. "
                                                 "Fields thread_cnt and conn are not numerical or absent.");
                return NULL;
            }
        }
    }
    return PyLong_FromLong(0);
}

PyMODINIT_FUNC PyInit_CellFrame(void){

    if (PyType_Ready(&DapObject_DapObjectType) < 0 || PyType_Ready(&dapCrypto_dapCryptoType) < 0 ||
            PyType_Ready(&ServerCore_ServerCoreType) < 0 || PyType_Ready(&dapEvents_dapEventsType) < 0 ||
            PyType_Ready(&dapEventsSocket_dapEventsSocketType) < 0 ||
            PyType_Ready(&CryptoKeyTypeObjecy_CryptoKeyTypeObjecyType) < 0 ||
            PyType_Ready(&CryptoDataTypeObjecy_CryptoDataTypeObjecyType) < 0 ||
            // === Chain ===
            PyType_Ready(&dapChainObject_dapChainType) < 0 ||
            PyType_Ready(&dapChainTypeObject_dapChainTypeType) < 0 ||
            PyType_Ready(&dapChainAtomPtr_dapChainAtomPtrType) < 0 ||
            PyType_Ready(&DapChainCell_DapChainCellObjectType) < 0 ||
            PyType_Ready(&ChainCommonObject_ChainCommonType) < 0 ||

            PyType_Ready(&DapChainIdObject_DapChainIdObjectType) < 0 ||
            PyType_Ready(&DapChainCellIdObject_DapChainCellIdObjectType) < 0 ||
            PyType_Ready(&DapChainNodeAddrObject_DapChainNodeAddrObjectType) < 0 ||
            PyType_Ready(&DapChainHashSlowObject_DapChainHashSlowObjectType) < 0 ||
            PyType_Ready(&DapChainHashFastObject_DapChainHashFastObjectType) < 0 ||
            PyType_Ready(&DapChainHashSlowKindObject_DapChainHashSlowKindObjectType) < 0 ||
            PyType_Ready(&DapChainAddrObject_DapChainAddrObjectType) < 0 ||

            PyType_Ready(&DapChainCsObject_DapChainCsObjectType) < 0 ||
            PyType_Ready(&DapChainDatumTypeIdObject_DapChainDatumTypeIdObjectType) < 0 ||
            PyType_Ready(&DapChainDatumObject_DapChainDatumObjectType) < 0 ||
            PyType_Ready(&DapChainDatumIterObject_DapChainDatumIterObjectType) < 0 ||
            PyType_Ready(&DapChainDatumToken_DapChainDatumTokenObjectType) < 0 ||
            PyType_Ready(&DapChainDatumTokenEmission_DapChainDatumTokenEmissionObjectType) < 0 ||
            PyType_Ready(&DapChainTxItemObject_DapChainTxItemTypeObjectType) < 0 ||
            PyType_Ready(&DapChainTxType_DapChainTxCondTypeObject) < 0 ||
            PyType_Ready(&DapChainDatumTx_DapChainDatumTxObjectType) < 0 ||
            PyType_Ready(&DapChainTxOutCond_DapChainTxOutCondObjectType) < 0 ||
            PyType_Ready(&DapChainLedger_DapChainLedgerType) < 0 ||
            // =============
            // === Chain net ===
            PyType_Ready(&DapChainNetObject_DapChainNetObjectType) < 0 ||
            PyType_Ready(&DapChainNodeCliObject_DapChainNodeCliObjectType) < 0 ||
            PyType_Ready(&DapChainNodeClientObject_DapChainNodeClientObjectType) < 0 ||
            PyType_Ready(&DapChainNodeInfoObject_DapChainNodeInfoObjectType) < 0 ||
            PyType_Ready(&DapChainNetNodeObject_DapChainNetNodeObjectType) < 0 ||
            PyType_Ready(&DapChainNetStateObject_DapChainNetStateObjectType) < 0
            // =============
            )
               return NULL;

    PyObject *module = PyModule_Create(&CellFramePythonModule);

    CellFrame_error = PyErr_NewException("libCellFrame.error", NULL, NULL);
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

    PyModule_AddObject(module, "Crypto", (PyObject*)&dapCrypto_dapCryptoType);

    PyModule_AddObject(module, "ServerCore", (PyObject*)&ServerCore_ServerCoreType);
    PyModule_AddObject(module, "Events", (PyObject*)&dapEvents_dapEventsType);
    PyModule_AddObject(module, "EventsSocket", (PyObject*)&dapEventsSocket_dapEventsSocketType);

    PyModule_AddObject(module, "CryptoKeyType", (PyObject*)&CryptoKeyTypeObjecy_CryptoKeyTypeObjecyType);
    PyModule_AddObject(module, "CryptoDataType", (PyObject*)&CryptoDataTypeObjecy_CryptoDataTypeObjecyType);

    // === Chain ===
    PyModule_AddObject(module, "Chain", (PyObject*)&dapChainObject_dapChainType);
    PyModule_AddObject(module, "ChainType", (PyObject*)&dapChainTypeObject_dapChainTypeType);
    PyModule_AddObject(module, "ChainAtomIter", (PyObject*)&dapChainAtomPtr_dapChainAtomPtrType);
    PyModule_AddObject(module, "ChainCell", (PyObject*)&DapChainCell_DapChainCellObjectType);
    PyModule_AddObject(module, "ChainCommon", (PyObject*)&ChainCommonObject_ChainCommonType);

    PyModule_AddObject(module, "ChainID", (PyObject*)&DapChainIdObject_DapChainIdObjectType);
    PyModule_AddObject(module, "ChainCellID", (PyObject*)&DapChainCellIdObject_DapChainCellIdObjectType);
    PyModule_AddObject(module, "ChainNodeAddr", (PyObject*)&DapChainNodeAddrObject_DapChainNodeAddrObjectType);
//    PyModule_AddObject(module, "ChainNetID", (PyObject*)&DapChainNetIdObject_DapChainNetIdObjectType);
    PyModule_AddObject(module, "ChainHashSlow", (PyObject*)&DapChainHashSlowObject_DapChainHashSlowObjectType);
    PyModule_AddObject(module, "ChainHashFast", (PyObject*)&DapChainHashFastObject_DapChainHashFastObjectType);
    PyModule_AddObject(module, "ChainHashSlowKind", (PyObject*)&DapChainHashSlowKindObject_DapChainHashSlowKindObjectType);
    PyModule_AddObject(module, "ChainAddr", (PyObject*)&DapChainAddrObject_DapChainAddrObjectType);

    PyModule_AddObject(module, "ChainCS", (PyObject*)&DapChainCsObject_DapChainCsObjectType);
    PyModule_AddObject(module, "ChainDatumTypeID", (PyObject*)&DapChainDatumTypeIdObject_DapChainDatumTypeIdObjectType);
    PyModule_AddObject(module, "ChainDatum", (PyObject*)&DapChainDatumObject_DapChainDatumObjectType);
    PyModule_AddObject(module, "ChainDatumIter", (PyObject*)&DapChainDatumIterObject_DapChainDatumIterObjectType);
    PyModule_AddObject(module, "ChainDatumToken", (PyObject*)&DapChainDatumToken_DapChainDatumTokenObjectType);
    PyModule_AddObject(module, "ChainDatumTokenEmisson", (PyObject*)&DapChainDatumTokenEmission_DapChainDatumTokenEmissionObjectType);
    PyModule_AddObject(module, "ChainTxItemType", (PyObject*)&DapChainTxItemObject_DapChainTxItemTypeObjectType);
    PyModule_AddObject(module, "ChainTxCondType", (PyObject*)&DapChainTxType_DapChainTxCondTypeObject);
    PyModule_AddObject(module, "ChainDatumTx", (PyObject*)&DapChainDatumTx_DapChainDatumTxObjectType);
    PyModule_AddObject(module, "ChainTxOutCond", (PyObject*)&DapChainTxOutCond_DapChainTxOutCondObjectType);
    PyModule_AddObject(module, "ChainLedger", (PyObject*)&DapChainLedger_DapChainLedgerType);
    // =============
    // === Chain net ===
    PyModule_AddObject(module, "ChainNet", (PyObject*)&DapChainNetObject_DapChainNetObjectType);
    PyModule_AddObject(module, "ChainNodeCLI", (PyObject*)&DapChainNodeCliObject_DapChainNodeCliObjectType);
    PyModule_AddObject(module, "ChainNodeClient", (PyObject*)&DapChainNodeClientObject_DapChainNodeClientObjectType);
    PyModule_AddObject(module, "ChainNodeInfo", (PyObject*)&DapChainNodeInfoObject_DapChainNodeInfoObjectType);
    PyModule_AddObject(module, "ChainNetNode", (PyObject*)&DapChainNetNodeObject_DapChainNetNodeObjectType);
    PyModule_AddObject(module, "ChainNetState", (PyObject*)&DapChainNetStateObject_DapChainNetStateObjectType);
    // =============


    return module;
}

static PyObject *python_cellframe_deinit(PyObject *self, PyObject *args){
    dap_config_close(g_config);
    dap_config_deinit();
    if (init_crypto)
        dap_crypto_deinit();
    if (init_chain){
        deinit_chain_py();
        dap_chain_cs_deinit_py();
    }
    return PyLong_FromLong(0);
}

int main(int argc, char **argv) {
    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }

    /* Add a built-in module, before Py_Initialize */
    PyImport_AppendInittab("CellFrame", PyInit_CellFrame);

    /* Pass argv[0] to the Python interpreter */
    Py_SetProgramName(program);

    /* Initialize the Python interpreter.  Required. */
    Py_Initialize();

    /* Optionally import the module; alternatively,
       import can be deferred until the embedded script
       imports it. */
    PyImport_ImportModule("CellFrame");

    PyMem_RawFree(program);
    return 0;
}
