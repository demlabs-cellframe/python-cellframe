from API_CellFrame import logItNotice, logItInfo, ChainNet, ChainNetSrv, ChainNetSrvUID

def requested():
    logItInfo("func requested")

def response_success():
    logItInfo("func response success")

def response_error():
    logItInfo("func response error")

def next_success():
    logItInfo("func next success")

def init():
    logItNotice("Init demoServer")
    ch_uid = ChainNetSrvUID.fromStr("154")
    srv = ChainNetSrv(
        ch_uid, 
        requested, 
        response_success,
        response_error,
        next_success
    )
    return 0
