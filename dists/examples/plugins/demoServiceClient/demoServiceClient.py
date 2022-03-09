from API_CellFrame import ChainNetSrvClient, logItNotice, logItInfo, logItWarning, ChainNet, ChainNetSrv, ChainNetSrvUID

def callback_connected(serviceClient, arg):
    logItInfo("Python client connected")
    ch_uid = ChainNetSrvUID(1234)
    serviceClient.write(0, ch_uid, "HELLO I TEST SERV".encode('utf-8'))

def callback_disconnected(serviceClient, arg):
    logItNotice("Python client disconnected")

def callback_deleted(serviceClient, arg):
    logItNotice("Python client deleted")

def callback_check(serviceClient, arg):
    logItNotice("Python client successfully checked the service")

def callback_sign(serviceClient, txCondReceipt, arg):
#    return txCondReceipt.sign
    return null

def callback_success(serviceClient, txCondHash, arg):
    logItNotice("Python client successfully requested the service")

def callback_error(serviceClient, errorNum, arg):
    logItWarning(f"Python client got error {errorNum:#x}")

def callback_data(serviceClient, data, arg):
    logItNotice(f"Python client custom data read back \'{data.decode('utf-8')}\'")

def init():
    logItNotice("Init demoServer")
#    AppCliServer.cmdItemCreate("myClient", clientCMD, "Command for working cmd",
#
#    Command for working cmd client
#"""
#) 
    net = ChainNet.byName("subzero")
    
    client = ChainNetSrvClient(net, "127.0.0.1", 8089, callback_connected,
                                                       callback_disconnected,
                                                       callback_deleted,
                                                       callback_check,
                                                       callback_sign,
                                                       callback_success,
                                                       callback_error,
                                                       callback_data,
                                                       net)
    return 0
