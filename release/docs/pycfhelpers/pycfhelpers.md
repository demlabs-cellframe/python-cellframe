# pycfhelpers package

---

## pycfhelpers.cellframenet module

### *class* pycfhelpers.cellframenet.TSD

Bases: `object`

Types of Typed section data.

#### TYPE_UNKNOWN

Unknown type.

#### TYPE_TIMESTAMP

Timestamp type.

#### TYPE_ADDRESS

Address type.

#### TYPE_VALUE

Value type.

#### TYPE_CONTRACT

Contract type.

#### TYPE_NET_ID

Network ID type.

#### TYPE_BLOCK_NUM

Block number type.

#### TYPE_TOKEN_SYM

Token symbol type.

#### TYPE_OUTER_TX_HASH

Transaction output hash type.

#### TYPE_SOURCE

Source type

#### TYPE_SOURCE_SUBTYPE

Source subtype type.

#### TYPE_DATA

Data type.

#### TYPE_SENDER

Sender type.

#### TYPE_TOKEN_ADDRESS

Token adress type.

#### TYPE_SIGNATURS

Signaturs type.

#### TYPE_UNIQUE_ID

Unique ID type.

#### TYPE_BASE_TX_HASH

Base trancaction hash type.

#### TYPE_EMISSION_CENTER_UID

type of the unique identifier of
the emission center.

#### TYPE_EMISSION_CENTER_VER

type of the verificator/version of
the emission center.

#### TYPE_UNKNOWN *= 0*

#### TYPE_TIMESTAMP *= 1*

#### TYPE_ADDRESS *= 2*

#### TYPE_VALUE *= 3*

#### TYPE_CONTRACT *= 4*

#### TYPE_NET_ID *= 5*

#### TYPE_BLOCK_NUM *= 6*

#### TYPE_TOKEN_SYM *= 7*

#### TYPE_OUTER_TX_HASH *= 8*

#### TYPE_SOURCE *= 9*

#### TYPE_SOURCE_SUBTYPE *= 10*

#### TYPE_DATA *= 11*

#### TYPE_SENDER *= 12*

#### TYPE_TOKEN_ADDRESS *= 13*

#### TYPE_SIGNATURS *= 14*

#### TYPE_UNIQUE_ID *= 15*

#### TYPE_BASE_TX_HASH *= 16*

#### TYPE_EMISSION_CENTER_UID *= 17*

#### TYPE_EMISSION_CENTER_VER *= 18*

### *class* pycfhelpers.cellframenet.CellframeEmission(datum: CellFrame.Common.DatumEmission, event: CellFrame.Consensus.DAG | None = None)

Bases: `object`

Represents a Cellframe emission.

#### datum

The emission datum.

* **Type:**
  DatumEmission

#### event

The DAG event associated with
the emission. Defaults to None.

* **Type:**
  DAG | None, optional

#### hash

The hash of the emission.

* **Type:**
  str

#### uid

The unique ID of the emission.

* **Type:**
  int

#### \_\_init_\_(datum: CellFrame.Common.DatumEmission, event: CellFrame.Consensus.DAG | None = None)

Initialize a CellframeEmission instance.

* **Parameters:**
  * **datum** (*DatumEmission*) – The emission datum.
  * **event** (*DAG* *|* *None* *,* *optional*) – The DAG event associated with the emission. Defaults to None.

#### getTSD(type)

Get the TSD value for a given type.

* **Parameters:**
  **type** (*int*) – The type of TSD to retrieve.
* **Returns:**
  The TSD value if found decoded in
  : the format utf-8, otherwise None.
* **Return type:**
  (str | None, optional)

#### setTSD(type, data)

Set the TSD value for a given type.

* **Parameters:**
  * **type** (*int*) – The type of TSD to set.
  * **data** (*str*) – The data to set for the TSD.

### *class* pycfhelpers.cellframenet.CellframeBaseTransactionDatum(datum, net, block=None)

Bases: `object`

The Cellframe base transaction datum.

#### datum

The base transaction datum.

* **Type:**
  DatumEmission

#### block

The block associated with the datum.

* **Type:**
  Any

#### hash

The hash of the datum.

* **Type:**
  str

#### created

The creation date of the datum.

* **Type:**
  datetime

#### net

The Cellframe network associated with the datum.

* **Type:**
  Net

#### to_address

The recipient address of the transaction.

#### amount

The amount of the transaction.

* **Type:**
  int

#### emission_hash

The hash of the associated emission.

* **Type:**
  str

#### emission_

The CellframeEmission instance associated
with the emission.

#### \_\_init_\_(datum, net, block=None)

Initialize a CellframeBaseTransactionDatum instance.

* **Parameters:**
  * **datum** (*DatumEmission*) – The base transaction datum.
  * **net** (*Net*) – The Cellframe network associated with the datum.
  * **block** (*Any* *|* *None* *,* *optional*) – The block associated with the
    datum. Defaults to None.
* **Raises:**
  **RuntimeError** – If datum not base of transaction.

#### tx_out()

Retrieve the transaction output associated with the base transaction datum.

* **Returns:**
  The transaction output, if present,
  : otherwise None.
* **Return type:**
  (TxOut | None, optional)

#### tx_in()

Retrieve the transaction input associated with the base transaction datum.

* **Returns:**
  The transaction input, if present,
  : otherwise None.
* **Return type:**
  (TxIn | None, optional)

#### tx_token()

Retrieve the token associated with the base transaction datum.

* **Returns:**
  The token, if present, otherwise None.
* **Return type:**
  (TxToken | None, optional)

#### tx_sig()

Retrieve the signature associated with the base transaction datum.

* **Returns:**
  The signature, if present,
  : otherwise None.
* **Return type:**
  (TxSig | None, optional)

#### emission()

Retrieve the emission associated with the base transaction datum.

* **Returns:**
  The emission, if present,
  : otherwise None.
* **Return type:**
  ([CellframeEmission](#pycfhelpers.cellframenet.CellframeEmission) | None, optional)

### *class* pycfhelpers.cellframenet.CellframeNetwork(name, chains, group_alias=None, commision_wallet=None)

Bases: `object`

The Cellframe network.

#### main

The main chain of the network.

* **Type:**
  Chain

#### zerochain

The zerochain of the network.

* **Type:**
  Chain

#### name

The name of the network.

* **Type:**
  str

#### net

The Cellframe network instance.

* **Type:**
  Net

#### commision_wallet

The commission wallet associated
with the network.

* **Type:**
  Any

#### main *: CellFrame.Chain.Chain*

#### zerochain *: CellFrame.Chain.Chain*

#### \_\_init_\_(name, chains, group_alias=None, commision_wallet=None)

Initializes a CellframeNetwork instance.

* **Parameters:**
  * **name** (*str*) – The name of the network.
  * **chains** (*List* *[**str* *]*) – The list of chain names associated
    with the network.
  * **group_alias** (*Any* *|* *None* *,* *optional*) – The group alias of the network.
    Defaults to None.
  * **commision_wallet** (*Any* *|* *None* *,* *optional*) – The commission wallet
    associated with the network. Defaults to None.
* **Raises:**
  **RuntimeError** – If network with specified name does not exist.

#### *property* group_alias *: str*

Retrieve the group alias of the network.

* **Returns:**
  The group alias.
* **Return type:**
  str

#### *static* wallet_from_signature(sigbytes)

Retrieve the wallet address from the given signature bytes.

* **Parameters:**
  **sigbytes** (*bytes*) – The signature bytes.
* **Returns:**
  The wallet address.
* **Return type:**
  str

#### *static* netid_from_wallet(wallet)

Retrieve the net ID from the given wallet address.

* **Parameters:**
  **wallet** (*str*) – The wallet address.
* **Returns:**
  The net ID.
* **Return type:**
  int

#### tx_sender_wallet(tx)

Retrieve the sender wallet address from the given transaction.

* **Parameters:**
  **tx** (*Any*) – The transaction.
* **Returns:**
  The sender wallet address,
  : if found, otherwise None.
* **Return type:**
  (Sign | None, optional)

#### ledger_tx_by_hash(txh)

Retrieve the ledger transaction by its hash.

* **Parameters:**
  **txh** (*str*) – The transaction hash.
* **Returns:**
  The ledger transaction.
* **Return type:**
  tx (str)

#### netid()

Retrieve the net ID of the network.

* **Returns:**
  The net ID.
* **Return type:**
  int

#### set_mempool_notification_callback(chain, callback)

Set the mempool notification callback for the given chain.

* **Parameters:**
  * **chain** (*Chain*) – The chain.
  * **callback** (*Callable*) – The callback function.

#### set_gdbsync_notification_callback(callback)

Set the Global data base synchronization notification callback.

* **Parameters:**
  **callback** (*Callable*) – The callback function.

#### set_atom_notification_callback(chain, callback)

Set the atom notification callback for the given chain.

* **Parameters:**
  * **chain** (*Chain*) – The chain.
  * **callback** (*Callable*) – The callback function.

#### set_ledger_tx_notification_callback(callback)

Set the ledger transaction notification callback.

* **Parameters:**
  **callback** (*Callable*) – The callback function.

#### set_ledger_bridge_tx_notification_callback(callback)

Set the ledger bridge transaction notification callback.

* **Parameters:**
  **callback** (*Callable*) – The callback function.

#### load_cert()

Load the certificate with the given name.

* **Parameters:**
  **certname** (*str*) – The name of the certificate.
* **Returns:**
  The loaded certificate.
* **Return type:**
  Any

#### extract_emission_from_mempool_nofitication(chain, value)

Extract the emission from the mempool notification.

* **Parameters:**
  * **chain** (*Chain*) – The chain.
  * **value** (*Any*) – The value.
* **Returns:**
  The extracted emission,
  : if found, otherwise None.
* **Return type:**
  ([CellframeEmission](#pycfhelpers.cellframenet.CellframeEmission) | None, optional)

#### create_base_transaction(emission, certs, fee, native_tw=None)

Create a base transaction.

* **Parameters:**
  * **emission** ([*CellframeEmission*](#pycfhelpers.cellframenet.CellframeEmission)) – The emission associated with
    the transaction.
  * **certs** (*str*) – The certificates to use for the transaction.
  * **fee** (*int*) – The fee for the transaction.
  * **native_tw** (*str* *|* *None* *,* *optional*) – The native transaction wallet.
    Defaults to None.
* **Returns:**
  The created base transaction.

#### get_emission_by_tsd(tsd_dict)

Retrieve the emission by the given TSD dictionary.

* **Parameters:**
  **tsd_dict** (*Dict* *[**str* *,* *Any* *]*) – The TSD dictionary.
* **Returns:**
  The retrieved emissions.
* **Return type:**
  Dict[str, [CellframeEmission](#pycfhelpers.cellframenet.CellframeEmission)]

#### get_emission_from_mempool_by_tsd(tsd_dict)

Retrieve the emission from the mempool by the given TSD dictionary.

* **Parameters:**
  **tsd_dict** (*Dict* *[**str* *,* *Any* *]*) – The TSD dictionary.
* **Returns:**
  The retrieved emissions.
* **Return type:**
  Dict[str, [CellframeEmission](#pycfhelpers.cellframenet.CellframeEmission)]

#### base_transactions_from_blocks(emission_hash=None)

Retrieve base transactions from the blocks.

* **Parameters:**
  **emission_hash** (*str* *|* *None* *,* *optional*) – The emission hash to filter
  the transactions, defaults to None.

Yelds:
: Iterator[CellframeBaseTransactionDatum]: An iterator over the
  : base transactions.

Example:

#### get_transactions_to_wallet_from_blocks(address)

Retrieve transactions to the given wallet address from the blocks.

* **Parameters:**
  **address** (*str*) – The wallet address.
* **Returns:**
  The list of transactions
  : to the wallet address.
* **Return type:**
  List[[CellframeBaseTransactionDatum](#pycfhelpers.cellframenet.CellframeBaseTransactionDatum)]

#### create_emission(wallet, token_symbol, value, tsd)

Create an emission.

* **Parameters:**
  * **wallet** (*str*) – The wallet address.
  * **token_symbol** (*str*) – The token symbol.
  * **value** (*str*) – The emission value.
  * **tsd** (*Dict* *[**str* *,* *Any* *]*) – The TSD dictionary.
* **Returns:**
  The created emission.
* **Return type:**
  [CellframeEmission](#pycfhelpers.cellframenet.CellframeEmission)

#### place_emission(ems, chain)

Place the emission in the mempool of the given chain.

* **Parameters:**
  * **ems** (*DatumEmission*) – The emission.
  * **chain** (*Chain*) – The chain.
* **Returns:**
  ???

#### place_datum(datum, chain)

Place the datum in the mempool of the given chain.

* **Parameters:**
  * **datum** (*DatumEmission*) – The datum.
  * **chain** (*Chain*) – The chain.
* **Returns:**
  True if successful, False otherwise.
* **Return type:**
  bool

#### remove_key_from_mempool(key, chain)

Remove the key from the mempool of the given chain.

* **Parameters:**
  * **key** (*Any*) – The key.
  * **chain** (*Chain*) – The chain.

#### mempool_list(chain)

List the mempool of the given chain.

* **Parameters:**
  **chain** (*Chain*) – The chain.
* **Returns:**
  The mempool.
* **Return type:**
  List[???]

#### mempool_get_emission(key)

Retrieve the emission from the mempool by the given key.

* **Parameters:**
  **key** (*Any*) – The key.
* **Returns:**
  The retrieved emission.
* **Return type:**
  (CellframeEmission ???)

#### mempool_proc(hash, chain)

Process the mempool of the given chain.

* **Parameters:**
  * **hash** (*str*) – The hash.
  * **chain** (*Chain*) – The chain.

#### all_tx_from_ledger()

Retrieve all transactions from the ledger.

* **Returns:**
  A tuple containing the list of
  : transactions and the ledger.
* **Return type:**
  Tuple[List, Any]

#### *static* get_datums_from_atom(chain: CellFrame.Chain.Chain, atom: CellFrame.Chain.ChainAtomPtr)

Retrieve datums from the atom.

* **Parameters:**
  * **chain** (*Chain*) – The chain.
  * **atom** (*Any*) – The atom.
* **Returns:**
  The retrieved datums.
* **Return type:**
  List

#### get_datums_from_chains(chains: tuple[str] = ('main', 'zerochain'))

Get datums from multiple chains.

* **Parameters:**
  **chains** (*Tuple* *[**str* *,*  *...* *]*) – The chains to retrieve datums from,
  defaults to (“main”, “zerochain”).
* **Yields:**
  *Iterator* – An iterator over the retrieved datums.

Example:

#### all_tx_from_blocks()

Retrieve all transactions from the blocks.

Yelds:
: Iterator: An iterator over the retrieved transactions.

Example:

## pycfhelpers.contract module

## pycfhelpers.globaldb module

### *class* pycfhelpers.globaldb.GDBTable(table)

Bases: `object`

The table in the global database.

Provides methods to interact with a specific table in the global database.

#### table

The name of the table.

* **Type:**
  str

#### \_\_init_\_(table)

Initialize a GDBTable object with the given table name.

* **Parameters:**
  **table** (*str*) – The name of the table.

#### set(key, value)

Set a key-value pair in the table.

* **Parameters:**
  * **key** (*str*) – The key to set.
  * **value** – The value to set.

#### get(key, default)

Get the value associated with the given key from the table.

* **Parameters:**
  * **key** (*str*) – The key to get the value for.
  * **default** – The default value to return if the key is not found.
* **Returns:**
  The value associated with the key, or the default value if the key is not found.

#### delete(key)

Delete the key-value pair associated with the given key from the table.

* **Parameters:**
  **key** (*str*) – The key to delete.

#### group_list()

Load the list of groups associated with the table.

* **Returns:**
  The list of groups associated with the table.
* **Return type:**
  list

### pycfhelpers.globaldb.sync_group(netname, grp, callback, cbname)

Synchronize a group with a callback.

This function synchronizes a group with a callback function.

* **Parameters:**
  * **netname** (*str*) – The name of the network.
  * **grp** – The group to synchronize.
  * **callback** – The callback function to use for synchronization.
  * **cbname** – The name of the callback function.

## pycfhelpers.helpers module

### pycfhelpers.helpers.json_dump(data)

Serialize Python object to a JSON formatted string.

* **Parameters:**
  **data** – The Python object to be serialized.
* **Returns:**
  The JSON formatted string representing the serialized object.
* **Return type:**
  str

### pycfhelpers.helpers.json_load(data)

Deserialize JSON formatted string to a Python object.

* **Parameters:**
  **data** (*str*) – The JSON formatted string to be deserialized.
* **Returns:**
  The Python object deserialized from the JSON string, or None if deserialization fails.
* **Return type:**
  Any

### pycfhelpers.helpers.find_tx_out(tx, out_type)

Find a transaction output of the specified type in a transaction.

* **Parameters:**
  * **tx** – The transaction to search.
  * **out_type** – The type of transaction output to find.
* **Returns:**
  The transaction output of the specified type, or None if not found.
* **Return type:**
  TxOut or None

### pycfhelpers.helpers.get_tx_items(tx, out_type)

Get a list of transaction items of the specified type from a transaction.

* **Parameters:**
  * **tx** – The transaction to retrieve items from.
  * **out_type** – The type of transaction items to retrieve.
* **Returns:**
  A list of transaction items of the specified type,
  : or None if not found.
* **Return type:**
  list or None

### pycfhelpers.helpers.get_tsd_data(tsd_list, type_tsd)

Get data of a specific type from a list of TSDs.

* **Parameters:**
  * **tsd_list** (*list*) – The list of TSDs.
  * **type_tsd** – The type of TSD data to retrieve.
* **Returns:**
  The data of the specified type from the list of TSDs,
  : or None if not found.
* **Return type:**
  Any

### pycfhelpers.helpers.get_tx_outs(tx, out_type)

Get a list of transaction outputs of the specified type from a transaction.

* **Parameters:**
  * **tx** – The transaction to retrieve outputs.
  * **out_type** – The type of transaction outputs to retrieve.
* **Returns:**
  A list of transaction outputs of the specified type,
  : or None if not found.
* **Return type:**
  list or None

### *class* pycfhelpers.helpers.ListCache(capacity)

Bases: `object`

A cache implementation using a list.

This class provides a caching mechanism using a list with a specified capacity.

#### capacity

The maximum capacity of the cache.

* **Type:**
  int

#### data

The list to store cached items.

* **Type:**
  list

#### \_\_init_\_(capacity)

Initialize a ListCache object with the given capacity.

* **Parameters:**
  **capacity** (*int*) – The maximum capacity of the cache.

#### add(data)

Add data to the cache.

* **Parameters:**
  **data** – The data to add to the cache.

#### data()

Get the data stored in the cache.

* **Returns:**
  The data stored in the cache.
* **Return type:**
  list

### pycfhelpers.helpers.net_by_name(NETS, netname)

Get network by name from a list of networks.

* **Parameters:**
  * **NETS** (*list*) – A list of networks.
  * **netname** (*str*) – The name of the network to find.
* **Returns:**
  The network object with the specified name, or None if not found.
* **Return type:**
  Any

## pycfhelpers.logger module

## pycfhelpers.tornado_http module

### pycfhelpers.tornado_http.init_tornado_http(url, handler, port)

Initialize Tornado HTTP server.

* **Parameters:**
  * **url** (*str*) – URL pattern to listen to.
  * **handler** (*callable*) – Function to handle incoming requests.
  * **port** (*int*) – Port number to listen on.
* **Raises:**
  **BaseException** – If Tornado is not installed, external http-server will be unavailable.

## Module contents
