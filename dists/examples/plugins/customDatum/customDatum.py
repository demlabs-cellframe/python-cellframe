from API_CellFrame import logItNotice, AppCliServer, ChainNetSrvDatum, ChainNet

def datumCMD(argv, indexStrReply):
    if (len(argv) == 3 or len(argv) == 5):
        if (argv[1] == 'create'):
            net = ChainNet.byName(argv[2])
            chain = net.getChainByName(argv[3])
            res = ChainNetSrvDatum.create(chain, argv[4].encode('utf-8'))
            AppCliServer.setReplyText("Return hash datum: "+str(res), indexStrReply)
        elif(argv[1] == 'load'):
            data = ChainNetSrvDatum.read(argv[2])
            AppCliServer.setReplyText("Return data: "+data, indexStrReply)
    else:
        logitNotice("This command can take three or four arguments.")

def init():
    logItNotice("Start plugin customDatum")
    AppCliServer.cmdItemCreate("pyDatum", datumCMD, "Command for creation datum", """
Command for created and get info from custom datum:
    create <net name> <chain name> <date in datum>
    load <path for file>
    """)
    return 0
