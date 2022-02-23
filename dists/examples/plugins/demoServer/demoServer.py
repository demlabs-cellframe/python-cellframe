from API_CellFrame import logItNotice, logItInfo, ChainNet, ChainNetSrv, ChainNetSrvUID

server_object = None

def requested(usage_id, client_remote, data):
    logItInfo("[server] func requested")

def response_success(usage_id, client_remote, data):
    logItInfo("[server] func response success")

def response_error(usage_id, client_remote, data):
    logItInfo("[server] func response error")

def next_success(usage_id, client_remote, data):
    logItInfo("[server] func next success")

def in_data(usage_id, client_remote, data):
    logItNotice("[server] Reading data")
    logItNotice("[server] USAGE ID: "+str(usage_id)+" Client remote:"+str(client_remote))
    logItNotice("[server] Input data: " + str(data))
    return data

def in_data_with_return_data(usage_id, client_remote, data):
    return data


def init():
    logItNotice("Init demoServer")
    ch_uid = ChainNetSrvUID(156)
    srv_object = ChainNetSrv(
        ch_uid,
        requested,
        response_success,
        response_error,
        next_success,
        in_data,
        in_data_with_return_data
    )
    return 0
