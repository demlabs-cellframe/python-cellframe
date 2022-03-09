from API_CellFrame import logItNotice, logItInfo, ChainNet, ChainNetSrv, ChainNetSrvUID

server_object = None

def requested(srv, usage_id, client_remote, data):
    logItInfo("[server] func requested")

def response_success(srv, usage_id, client_remote, data):
    logItInfo("[server] func response success")

def response_error(srv, usage_id, client_remote, data):
    logItInfo("[server] func response error")

def next_success(srv, usage_id, client_remote, data):
    logItInfo("[server] func next success")

def in_data_with_return_data(srv, usage_id, client_remote, data):
    logItNotice("[server] Input data: " + data.decode("utf-8"))
    return data


def init():
    logItNotice("Init demoServer")
    ch_uid = ChainNetSrvUID(123)
    srv_object = ChainNetSrv(
        ch_uid,
        requested,
        response_success,
        response_error,
        next_success,
        in_data_with_return_data
    )
    return 0
