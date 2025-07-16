from web3 import Web3, HTTPProvider
try:
    from web3.middleware import geth_poa_middleware
except ImportError:
    try:
        # For newer versions of web3
        from web3.middleware.geth_poa import geth_poa_middleware
    except ImportError:
        # If this doesn't work either, create a stub
        def geth_poa_middleware(w3):
            return lambda make_request, w3: make_request
try:
    from web3.auto import w3
except ImportError:
    # w3 removed in newer versions of web3
    w3 = None
from eth_account.messages import encode_defunct

from web3.exceptions import TransactionNotFound
from requests.exceptions import ConnectionError
from web3.datastructures import AttributeDict


class ContractProvider:
    """
    Contract provider class for interacting with smart contracts.

    Attributes:
        name (str): Name of the contract provider.
        provider_urls (list): List of provider URLs.
        contract_address (str): Address of the smart contract.
        abi (dict): ABI (Application Binary Interface) of the smart contract.
        commision_wallet (str, optional): Wallet address for commission.
            Defaults to None.
        cfnet (str, optional): CFNet. Defaults to None.
        chain_name (str, optional): Name of the chain. Defaults to None.
        event_keys_map (dict, optional): Mapping of event keys.
            Defaults to None.
        native_token_index (int, optional): Index of the native token.
            Defaults to None.
        pair_token_index (int, optional): Index of the pair token.
            Defaults to None.
        nft_contract (bool, optional): Flag indicating whether it's an
            NFT contract. Defaults to False.
        nft_token (str, optional): NFT token. Defaults to None.
        network_id (str, optional): Network ID. Defaults to None.
    """
    def __init__(self,
                 name,
                 provider_urls,
                 contract_address,
                 abi,
                 commision_wallet=None,
                 cfnet=None,
                 chain_name=None,
                 event_keys_map=None,
                 native_token_index=None,
                 pair_token_index=None,
                 nft_contract=False,
                 nft_token=None,
                 network_id=None):
        """
        Initialize the ContractProvider object.

        Args:
            name (str): Name of the contract provider.
            provider_urls (list): List of provider URLs.
            contract_address (str): Address of the smart contract.
            abi (dict): ABI (Application Binary Interface) of the
                smart contract.
            commision_wallet (str, optional): Wallet address for commission.
                Defaults to None.
            cfnet (str, optional): CFNet. Defaults to None.
            chain_name (str, optional): Name of the chain. Defaults to None.
            event_keys_map (dict, optional): Mapping of event keys.
                Defaults to None.
            native_token_index (int, optional): Index of the native token.
                Defaults to None.
            pair_token_index (int, optional): Index of the pair token.
                Defaults to None.
            nft_contract (bool, optional): Flag indicating whether it's an
                NFT contract. Defaults to False.
            nft_token (str, optional): NFT token. Defaults to None.
            network_id (str, optional): Network ID. Defaults to None.
        """
        self.name = name

        self.net = cfnet
        self.commision_wallet = commision_wallet
        self.contract_address = contract_address
        self.__last_attempt_block = "latest"
        self.abi = abi
        self.urls = provider_urls

        self.chain_name = chain_name
        self.event_keys_map = event_keys_map

        self.native_token_index = native_token_index
        self.pair_token_index = pair_token_index

        self.__reserves = None
        self.__supply = None
        self.__tokens = None
        self.__token_symbols = None

        self.is_nft = nft_contract
        self.nft_token = nft_token
        self.network_id = network_id

    @property
    def w3(self):
        """
        Property: web3 instance.

        Returns:
            Web3: Web3 instance connected to the provider URL.

        Raises:
            ConnectionError: If no valid links are provided and
                connection fails.
        """
        for url in self.urls:
            try:
                web3 = Web3(HTTPProvider(url))
                if web3.isConnected():
                    web3.middleware_onion.inject(geth_poa_middleware, layer=0)
                    return web3
            except Exception as e:
                #logIt.notice(f"Connection failed for URL {url}: {e}")
                continue
        raise ConnectionError("No valid links provided - failed to connect")

    @property
    def contract(self):
        """
        Property: Contract instance.

        Returns:
            Contract: Contract instance.
        """
        contract = self.w3.eth.contract(address=self.contract_address, abi=self.abi)
        return contract

    @property
    def functions(self):
        """
        Property: Contract functions.

        Returns:
            ContractFunctions: Contract functions.
        """
        return self.contract.functions

    @property
    def reserves(self):
        """
        Property: Reserves.

        Returns:
            int: Reserves if not an NFT contract, else 0.
        """
        if self.is_nft:
            return 0
        return self.functions.getReserves().call()

    @property
    def supply(self):
        """
        Property: Total supply.

        Returns:
            int: Total supply if not an NFT contract, else 0.
        """
        if self.is_nft:
            return 0
        return self.functions.totalSupply().call()

    @property
    def native_token_ticker(self):
        """
        Property: Native token ticker.

        Returns:
            str: Native token ticker if not an NFT contract, else
                symbol from ERC20 ABI.
        """
        #just nft - native ticker form ERC20 abi
        if self.is_nft:
            self.__token_symbols = [self.contract.functions.symbol().call(), "Unknown"]
            return self.__token_symbols[self.native_token_index]

        if not self.__tokens:
            self.__tokens =  [self.functions.token0().call(), self.functions.token1().call()]
        
        if not self.__token_symbols:
            t0provider = ContractProvider(name = "Token0-lp", provider_urls=self.urls, contract_address=self.__tokens[0], abi = self.abi)
            t1provider = ContractProvider(name = "Token1-lp", provider_urls=self.urls, contract_address=self.__tokens[1], abi = self.abi)

            t1 = t0provider.functions.symbol().call()
            t2 = t1provider.functions.symbol().call()

            self.__token_symbols = [t1, t2]

        return self.__token_symbols[self.native_token_index]

    @property
    def pair_token_ticker(self):
        """
        Property: Pair token ticker.

        Returns:
            str: Pair token ticker if not an NFT contract, else "NFT".
        """
        if self.is_nft:
            return "NFT"

        if not self.__tokens:
            self.__tokens =  [self.functions.token0().call(), self.functions.token1().call()]

        if not self.__token_symbols:
            t0provider = ContractProvider(name = "Token0-lp", provider_urls=self.urls, contract_address=self.__tokens[0], abi = self.abi)
            t1provider = ContractProvider(name = "Token1-lp", provider_urls=self.urls, contract_address=self.__tokens[1], abi = self.abi)

            t1 = t0provider.functions.symbol().call()
            t2 = t1provider.functions.symbol().call()

            self.__token_symbols = [t1, t2]

        return self.__token_symbols[self.pair_token_index]

    def all_events(self, event_name, **kwargs):
        """
        Get all events of a particular type.

        Args:
            event_name (str): Name of the event.
            **kwargs: Additional keyword arguments.

        Returns:
            list: List of all entries matching the event name
                and additional criteria.
        """
        return self.contract.events[event_name].createFilter(**kwargs).get_all_entries()

    def all_events_from_last_try(self, event_name):
        """
        Get all events from the last try.

        Args:
            event_name (str): Name of the event.

        Returns:
            list: List of all entries matching the event name
                from the last attempt.
        """
        block = self.w3.eth.blockNumber
        events = self.contract.events[event_name].createFilter(fromBlock=self.__last_attempt_block).get_all_entries()
        #logIt.notice("{} events from {} to {} block: {}".format(self.name, self.__last_attempt_block, block, len(events)))
        self.__last_attempt_block = block
        return events

    def event_from_tx_logs(self, tx_hash, event_name):
        """
        Get event from transaction logs.

        Args:
            tx_hash (str): Transaction hash.
            event_name (str): Name of the event.

        Returns:
            dict: Event data.
        """
        receipt = self.w3.eth.get_transaction_receipt(tx_hash)
        return self.contract.events[event_name]().processReceipt(receipt)

    def get_transaction(self, txhash):
        """
        Get transaction details.

        Args:
            txhash (str): Transaction hash.

        Returns:
            tuple: Tuple containing transaction details and contract data.
        """
        tx = self.w3.eth.get_transaction(txhash)
        contract_data = self.contract.decode_function_input(tx.input)[1]
        return tx, contract_data

    def normalized_event_data(self, ev):
        """
        Normalize event data.

        Args:
            ev (dict): Event data.

        Returns:
            dict: Normalized event data.

        Raises:
            RuntimeError: If no event keys mapping is provided.
        """
        if not self.event_keys_map: 
            raise RuntimeError("Can't normilize event data, no mapping")

        result = {}
        for key in self.event_keys_map.keys():
            result[key] = ev["args"][self.event_keys_map[key]]

        if not "_block" in ev["args"]:
            result["_block"] = self.w3.eth.blockNumber

        else:
            result["_block"] = ev["args"]["_block"]
        return result
