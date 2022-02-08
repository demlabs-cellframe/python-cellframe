from API_CellFrame import Client, ClientStage, ClientError, ClientStageStatus, logItNotice, Events

#def cmdPluginClient(argv, reply)
cl = None

def iniStage(client, buf):
    print("INIT STAGE")

def iniStageError(client, buf):
    print("ERR STAGE")

def response_data(client, data):
    print("RSP DATA")

def rsponse_error(client, code):
    print("RSP ERROR")

def stageConnected(client):
    logItNotice("STAGE CONNECTED")
    #client.encRequestUnsafe("/", "hello".encode('utf-8'), response_data, response_error)

def init():
    logItNotice("Running plugin client")
    ev = Events()
    cl = Client(ev, iniStage, iniStageError)
    addr = "127.0.0.1"
    print(type(addr))
    print(addr)
    cl.setUplinkUnsafe(addr) #default port 8079
    cl.setActiveChannelsUnsafe("R")
    cl.goStage(ClientStage.STAGE_STREAM_CONNECTED(), stageConnected)
    return 0

def deinit():
    logItNotice("Plugin client deinit")
