from typing import Literal, Callable
from pycfhelpers.node.notificators import CFNets
from pycfhelpers.node.net import CFNet
from pycfhelpers.node.logging import CFLog

NETS = [CFNet('Backbone'), CFNet('name_2')]
nets = CFNets(NETS)

#def on_mempool_change(op_code: Literal["a", "d"], datum: CFDatum | datum_hash, *args, chain: 'CFChain', **kwargs):
    #pass

# Используем декоратор mempool_notificator для регистрации обратного вызова
@nets.mempool_notificator("to_args", chain_name="main", key="to_kwargs")
def on_mempool_change(op_code: Literal["a", "d"], datum: CFDatum | datum_hash, *args, chain: 'CFChain', **kwargs):
    pass

#@CFNets(NETS).mempool_notificator("to_args", chain_name="main", key="to_kwargs")
#def on_mempool_change(op_code: Literal["a", "d"], datum: CFDatum | datum_hash, *args, chain: 'CFChain', **kwargs):
    #pass


def init():

    return 0
