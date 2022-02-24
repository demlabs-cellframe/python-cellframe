from API_CellFrame import ChainNetSrvClient, logItNotice, logItInfo, ChainNet, ChainNetSrv, ChainNetSrvUID

def callback_connected(serviceClient, arg):
    print("Python client connected")
    ch_uid = ChainNetSrvUID(156)
    serviceClient.write(1, 301, ch_uid, "HELLO I TEST SERV".encode('utf-8'))

def callback_read(serviceClient, data):
    print("Python client read back \'{:s}\'".format(data))

def callback_disconnected(serviceClient, arg):
    print("Python client disconnected")

def callback_deleted(serviceClient, arg):
    print("Python client deleted")

#def clientCMD(argv, indexStrReply):
#    reply = "Connected to ..."
#    AppCliServer.setReplyText(reply, indexStrReply)

def init():
    logItNotice("Init demoServer")
#    AppCliServer.cmdItemCreate("myClient", clientCMD, "Command for working cmd",
#
#    Command for working cmd client
#"""
#)
    net = ChainNet.byName("subzero")
    client = ChainNetSrvClient(net, "192.168.1.10", 8079, callback_connected, callback_read, callback_disconnected, callback_deleted, net)
#    client.test()
#    ch_uid = ChainNetSrvUID(156)
#    ch_uid2 = ChainNetSrvUID.fromStr("0x1101")
#    srv = ChainNetSrv(
#        ch_uid, 
#        requested, 
#        response_success,
#        response_error,
#        next_success
#    )
    return 0
