from API_CellFrame import Client, ClientStage, ClientError, ClientStageStatus, logItNotice, Events

#def cmdPluginClient(argv, reply)
cl = None

def iniStage(client, buf):
    print("INIT STAGE")

def iniStageError(client, buf):
    print("ERR STAGE")

def stageBegin(client):
    logItNotice("STAGE BEGIN")

def stageEncInit(client, i):
    logItNotice("STAGE ENC INIT code: "+ str(i))

def stageConnected(client):
    logItNotice("STAGE CONNECTED")

def init():
    logItNotice("Running plugin client")
    ev = Events()
    cl = Client(ev, iniStage, iniStageError)
    addr = "127.0.0.1"
    print(type(addr))
    print(addr)
    cl.setUplinkUnsafe() #default 127.0.0.1:8079
#    cl.goStage(ClientStage.STAGE_BEGIN(), stageBegin)
#    cl.goStage(ClientStage.STAGE_STREAM_STREAMING(), stageBegin)
#    cl.goStage(ClientStage.STAGE_ENC_INIT(), stageEncInit)
#    cl.goStage(ClientStage.STAGE_STREAM_CONNECTED(), stageConnected)
    return 0

def deinit():
    logItNotice("Plugin client deinit")
