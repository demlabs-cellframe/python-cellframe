from API_CellFrame import Mempool, ChainWallet, logItNotice, configGetItem, ChainNet, ChainAddr

def init():
    wallet_path = configGetItem("resources", "wallets_path")
    logItNotice("wallet path: "+wallet_path)
    wallet = ChainWallet.open("mywallet1", wallet_path)
    chain_net = ChainNet.byName("subzero")
    chain = chain_net.getChainByName("support")
    key = wallet.getKey(0)
    addr_from = wallet.getAddr(chain_net.id)
    addr_to = ChainAddr.fromStr("mJUUJk6Yk2gBSTjcDdb9R9gJh5SjcLn6DPc5etfBhPSJfNCdzedocd9ZBUFk33HmBbMTZSGpxtn4mgK31MvadR57LUHNcnoqX2F7G2rA")
    addr_fee = None
    value = 500
    value_fee = 0
    tx_hash = Mempool.txCreate(chain, key, addr_from, addr_to, addr_fee, "tCELL", value, value_fee)
    tx_hash_str = str(tx_hash)
    print(tx_hash_str)
    logItNotice("Created transaction with hash: " + tx_hash_str)
    return 0
