from typing import Literal, Callable

from .net import CFNet, MempoolCallback


class CFNets:
    """
    Represent a set of CFNet objects.

    Attributes:
        nets (List[CFNet]): The list of CFNet objects.
    """

    def __init__(self, nets: list[CFNet]):
        """
        Initialize CFNets object.

        Args:
            nets (List[CFNet]): The list of CFNet objects.
        """
        self.nets = nets

    def mempool_notificator(self, *args, chain_name: Literal["main", "zerochain", "all"] = "all", **kwargs):
        """
        Decorator for registering mempool notification callbacks.

        Args:
            *args: Positional arguments.
            chain_name (Literal["main", "zerochain", "all"]): The chain name to register the callback. Defaults to "all".
            **kwargs: Keyword arguments.

        Example:
            >>> @CFNets(NETS).mempool_notificator("to_args", chain_name="main", key="to_kwargs")
            >>> def on_mempool_change(op_code: Literal["a", "d"], datum: CFDatum | datum_hash, *args, chain: 'CFChain', **kwargs):
            >>>     pass
        """

        def wrapper(callback: MempoolCallback):
            for net in self.nets:
                if chain_name == "all":
                    for chain in net.chains:
                        chain.register_mempool_notification_callback(callback, *args, **kwargs)
                else:
                    getattr(net, chain_name).register_mempool_notification_callback(callback, *args, **kwargs)

        return wrapper

    def atom_notificator(self, *args, chain_name: Literal["main", "zerochain", "all"] = "all", **kwargs):
        """
        Decorator for registering atom notification callbacks.

        Args:
            *args: Positional arguments.
            chain_name (Literal["main", "zerochain", "all"]): The chain name to specify where to register the callback. Defaults to "all".
            **kwargs: Keyword arguments.

        Example:
            >>> @CFNets(NETS).atom_notificator("to_args", chain_name="main", key="to_kwargs")
            >>> def on_new_atom(atom: CFBlock | CFEvent, size: int, *args, chain: CFChain, **kwargs):
            >>>     pass

        """

        def wrapper(callback: Callable[[], None]):
            for net in self.nets:
                if chain_name == "all":
                    for chain in net.chains:
                        chain.register_atom_notification_callback(callback, *args, **kwargs)
                else:
                    getattr(net, chain_name).register_atom_notification_callback(callback, *args, **kwargs)

        return wrapper
    

    def atom_confirmed_notificator(self, *args, chain_name: Literal["main", "zerochain", "all"] = "all", conf_cnt=0,  **kwargs):
        """
        Decorator for registering atom confirmed notification callbacks.

        Args:
            *args: Positional arguments to pass to the callback.
            chain_name (Literal["main", "zerochain", "all"]): The chain name where to register the callback. Defaults to "all".
            **kwargs: Keyword arguments to pass to the callback.

        Example:
            >>> @CFNets(NETS).atom_confirmed_notificator("to_args", chain_name="main", key="to_kwargs")
            >>> def on_atom_confirmed(atom: CFBlock | CFEvent, *args, chain: CFChain, **kwargs):
            >>>     pass

        """

        def wrapper(callback: Callable):
            for net in self.nets:
                if chain_name == "all":
                    for chain in net.chains:
                        chain.register_atom_confirmed_notification_callback(callback, conf_cnt, *args, **kwargs)
                else:
                    chain = getattr(net, chain_name)
                    chain.register_atom_confirmed_notification_callback(callback, conf_cnt,  *args, **kwargs)

        return wrapper


    def gdbsync_notificator(self, *args, **kwargs):
        """
        Decorator for registering global database sync notification callbacks.

        Args:
            *args: Positional arguments.
            **kwargs: Keyword arguments.

        Example:
            >>> @CFNets(NETS).gdbsync_notificator("to_args", key="to_kwargs")
            >>> def on_new_table_record(op_code, group, key, value, *args, net:CFNet, **kwargs):
            >>>     pass

        """

        def wrapper(callback: Callable[[], None]):
            for net in self.nets:
                net.register_gdbsync_notification_callback(callback, *args, **kwargs)

        return wrapper

    def ledger_tx_notificator(self):
        """
        Decorator for registering ledger transaction notification callbacks.

        Args:
            *args: Positional arguments.
            **kwargs: Keyword arguments.

        Example:
            >>> @CFNets(NETS).ledger_tx_notificator("to_args", key="to_kwargs")
            >>> def on_new_ledger_transaction(ledger, tx, *args, net: CFNet, **kwargs):
            >>>     pass

        """

        def wrapper(callback: Callable[[], None]):
            for net in self.nets:
                ledger = net.get_ledger()
                ledger.register_ledger_tx_notification_callback(callback)
            return callback

        return wrapper


    def ledger_bridged_tx_notificator(self):
        """
        Decorator for registering crosschain (bridged) transaction notification callbacks.

        Args:
            *args: Positional arguments.
            **kwargs: Keyword arguments.

        Example:
            >>> @CFNets(NETS).bridged_tx_notificator("to_args", key="to_kwargs")
            >>> def on_new_ledger_transaction(ledger, tx, *args, net: CFNet, **kwargs):
            >>>     pass

        """

        def wrapper(callback: Callable[[], None]):
            for net in self.nets:
                ledger = net.get_ledger()
                ledger.register_ledger_bridged_tx_notification_callback(callback)
            return callback
        return wrapper
