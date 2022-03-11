from API_CellFrame import ChainNetSrvClient, logIt, NOTICE, WARNING, ChainNet, ChainNetSrvUID, Cert, ChainTxReceipt, ChainHashFast

def callback_connected(serviceClient, arg):
    logIt(NOTICE, "Python client connected")
    ch_uid = ChainNetSrvUID(123)
    net = ChainNet.byName("private")
    #serviceClient.write(ch_uid, "Greetings from test client".encode('utf-8'))
    #serviceClient.check(net, ch_uid, "Any data".encode('utf-8'))
    condHash = ChainHashFast.fromString("0x058EE1DDFAD16776A1B40A9C7AC69837D96F4B466F5D5EC973D04BE046F2E131");
    serviceClient.request(net, ch_uid, condHash)

def callback_disconnected(serviceClient, arg):
    logIt(NOTICE, "Python client disconnected")

def callback_deleted(serviceClient, arg):
    logIt(NOTICE, "Python client deleted")

def callback_check(serviceClient, arg):
    logIt(NOTICE, "Python client successfully checked the service")

def callback_sign(serviceClient, txCondRec, arg):
    logIt(NOTICE, "Sing receipt by python client")
    signCert = Cert.load("svc_client")
    return txCondRec.sign(signCert)

def callback_success(serviceClient, txCondHash, arg):
    logIt(NOTICE, "Python client successfully requested the service")

def callback_error(serviceClient, errorNum, arg):
    logIt(WARNING, f"Python client got error {errorNum:#x}")

def callback_data(serviceClient, data, arg):
    logIt(NOTICE, f"Python client custom data read back \'{data.decode('utf-8')}\'")

def init():
    logIt(NOTICE, "Init demoClient")
#    Command for working cmd client
#    AppCliServer.cmdItemCreate("myClient", clientCMD, "Command for working cmd",
    net = ChainNet.byName("private")
    client = ChainNetSrvClient(net, "127.0.0.1", 8089, callback_connected,
                                                       callback_disconnected,
                                                       callback_deleted,
                                                       callback_check,
                                                       callback_sign,
                                                       callback_success,
                                                       callback_error,
                                                       callback_data,
                                                       0)
    return 0
