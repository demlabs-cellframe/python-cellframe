from API_CellFrame import logItInfo, logItNotice, logItWarning, ChainNet, ChainNetSrv, ChainNetSrvUID

def requested(srv, usage_id, client_remote, data):
    logItInfo("[server] func requested")
    return 0

def response_success(srv, usage_id, client_remote, data):
    logItNotice("[server] func response success")
    return 0

def response_error(srv, usage_id, client_remote, data):
    logItWarning("[server] func response error")

def next_success(srv, usage_id, client_remote, data):
    logItNotice("[server] func next success")
    return 0

def custom_data(srv, usage_id, client_remote, data):
    logItNotice("[server] Input data: " + data.decode("utf-8"))
    return data


def init():
    logItNotice("Init demoServer")
    ch_uid = ChainNetSrvUID(123)
    srv_object = ChainNetSrv(
        ch_uid,
        "py_service",
        requested,
        response_success,
        response_error,
        next_success,
        custom_data
    )
    return 0
