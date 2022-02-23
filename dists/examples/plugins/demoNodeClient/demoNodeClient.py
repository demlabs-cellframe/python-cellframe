from API_CellFrame import ChainNetSrvClient, logItNotice, logItInfo, ChainNet, ChainNetSrv, ChainNetSrvUID

def handler(nodeClient, type):
    print (type)
    print ("HANDLER OK")
    if type == 0:
        ch_uid = ChainNetSrvUID(156)
        nodeClient.write(1, 301, ch_uid, "HELLO I TEST SERV".encode('utf-8'))

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
    net = ChainNet.byName("kelvin-testnet")
    client = ChainNetSrvClient(net, "127.0.0.1", 8079, handler)
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
