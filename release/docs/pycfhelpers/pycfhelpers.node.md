# pycfhelpers.node package

## Subpackages

* [pycfhelpers.node.http package](pycfhelpers.node.http.md)
  * [Submodules](pycfhelpers.node.http.md#submodules)
  * [pycfhelpers.node.http.simple module](pycfhelpers.node.http.md#module-pycfhelpers.node.http.simple)
    * [`CFSimpleHTTPRequest`](pycfhelpers.node.http.md#pycfhelpers.node.http.simple.CFSimpleHTTPRequest)
      * [`CFSimpleHTTPRequest.http`](pycfhelpers.node.http.md#pycfhelpers.node.http.simple.CFSimpleHTTPRequest.http)
      * [`CFSimpleHTTPRequest.__init__()`](pycfhelpers.node.http.md#pycfhelpers.node.http.simple.CFSimpleHTTPRequest.__init__)
      * [`CFSimpleHTTPRequest.body`](pycfhelpers.node.http.md#pycfhelpers.node.http.simple.CFSimpleHTTPRequest.body)
      * [`CFSimpleHTTPRequest.client_address`](pycfhelpers.node.http.md#pycfhelpers.node.http.simple.CFSimpleHTTPRequest.client_address)
      * [`CFSimpleHTTPRequest.method`](pycfhelpers.node.http.md#pycfhelpers.node.http.simple.CFSimpleHTTPRequest.method)
      * [`CFSimpleHTTPRequest.query`](pycfhelpers.node.http.md#pycfhelpers.node.http.simple.CFSimpleHTTPRequest.query)
      * [`CFSimpleHTTPRequest.url`](pycfhelpers.node.http.md#pycfhelpers.node.http.simple.CFSimpleHTTPRequest.url)
    * [`CFSimpleHTTPRequestHandler`](pycfhelpers.node.http.md#pycfhelpers.node.http.simple.CFSimpleHTTPRequestHandler)
      * [`CFSimpleHTTPRequestHandler.methods`](pycfhelpers.node.http.md#pycfhelpers.node.http.simple.CFSimpleHTTPRequestHandler.methods)
      * [`CFSimpleHTTPRequestHandler.handler`](pycfhelpers.node.http.md#pycfhelpers.node.http.simple.CFSimpleHTTPRequestHandler.handler)
      * [`CFSimpleHTTPRequestHandler.__call__()`](pycfhelpers.node.http.md#pycfhelpers.node.http.simple.CFSimpleHTTPRequestHandler.__call__)
      * [`CFSimpleHTTPRequestHandler.__init__()`](pycfhelpers.node.http.md#pycfhelpers.node.http.simple.CFSimpleHTTPRequestHandler.__init__)
    * [`CFSimpleHTTPServer`](pycfhelpers.node.http.md#pycfhelpers.node.http.simple.CFSimpleHTTPServer)
      * [`CFSimpleHTTPServer.__init__()`](pycfhelpers.node.http.md#pycfhelpers.node.http.simple.CFSimpleHTTPServer.__init__)
      * [`CFSimpleHTTPServer.handler()`](pycfhelpers.node.http.md#pycfhelpers.node.http.simple.CFSimpleHTTPServer.handler)
      * [`CFSimpleHTTPServer.register_uri_handler()`](pycfhelpers.node.http.md#pycfhelpers.node.http.simple.CFSimpleHTTPServer.register_uri_handler)
  * [Module contents](pycfhelpers.node.http.md#module-pycfhelpers.node.http)

## Submodules

## pycfhelpers.node.address module

### *class* pycfhelpers.node.address.CFWalletAddress(address: str)

The CellFrame wallet address.

Provides methods to work with CellFrame wallet addresses.

#### Attributes:

##### \_original_addr

The original ChainAddr object representing the wallet address.

* **Type:** *ChainAddr*

#### Methods:

##### \_\_init_\_(address: str)

Initialize a CFWalletAddress instance with the given address.

* **Parameters:**
	* **address** (*str*) – The wallet address string.

##### net_id()

Get the CellFrame network ID of the wallet address.

* **Returns:** The CellFrame network ID of the wallet address.
* **Return type:** *[CFNetID](#pycfhelpers.node.net.CFNetID)*

## pycfhelpers.node.cli module

### *class* pycfhelpers.node.cli.CFCliCommand(command: str, callback: [CliCallback](#pycfhelpers.node.cli.CliCallback), help_text: str = '')

The CellFrame CLI command.

Provides methods to register and handle CLI commands.

#### Attributes:

##### command

The CLI command.
* **Type:** *str*

##### callback

The callback function to be executed whe the command is invoked.

* **Type:** *[CliCallback](#pycfhelpers.node.cli.CliCallback)*

##### help_text

The help text for the command.

* **Type:** *str*

##### parser

The option parser for the command.

* **Type:** *OptionParser*

#### Methods:

##### \_\_init_\_(command: str, callback: [CliCallback](#pycfhelpers.node.cli.CliCallback), help_text: str = '')

Initialize a CFCliCommand object with the given parametrs.

* **Parameters:**
  * **command** (*str*) – The alias for CLI command.
  * **callback** ([*CliCallback*](#pycfhelpers.node.cli.CliCallback)) – The callback function to be executed when the command is invoked.
  * **help_text** (*str* *,* *optional*) – The help text for the command. Defaults to “”.

##### register()

Register the CLI command.

### *class* pycfhelpers.node.cli.CliCallback(\*args, \*\*kwargs)

Bases: `Protocol`

A protocol for CLI command callbacks.


#### Methods:

##### \_\_call_\_(\*args, reply_object: [ReplyObject](#pycfhelpers.node.cli.ReplyObject), \*\*kwargs)

Call method signature for CLI command callbacks.

### *class* pycfhelpers.node.cli.ReplyObject(reply_id: ReplyId)

An object representing a reply for CLI commands.

Provides a mechanism to manage replies for CLI commands.

#### Attributes:

##### reply_id

The ID of the reply.

* **Type:** *ReplyId*

#### Methods:

##### \_\_init_\_(reply_id: ReplyId)

Initialize a ReplyObject instance with the specified reply ID.

* **Parameters:**
	* **reply_id** (*ReplyId*) – The unique identifier for the reply.

##### reply(message: str)

Send a reply message.

* **Parameters:**
	* **message** (*str*) – The message to be sent as a reply.

## pycfhelpers.node.consensus module

### *class* pycfhelpers.node.consensus.CFBlock(atom: ChainAtomPtr, chain: [CFChain](#pycfhelpers.node.net.CFChain))

The CellFrame block.

#### Attributes:

##### chain

The CFChain object associated with the block.

* **Type:** *[CFChain](#pycfhelpers.node.net.CFChain)*

##### hash

The hash of the block.

* **Type:** *str*

##### created_at

The time when the block was created.

* **Type:** *datetime*

##### version

The version of the block.

* **Type:** *int*

##### cell_id

The cell ID associated with the block.

* **Type:** *str*

##### chainid

The chain ID associated with the block.

* **Type:** *str*

##### signs

A list of CFSign objects representing the signatures of the block.

* **Type:** *List[[CFSign](#pycfhelpers.node.crypto.CFSign)]*

##### meta

The dictionary containing metadata of the block.

The dictionary includes the following keys:
- “blockPrevHash” (str): The hash of the previous block.
- “blockAnchorHash” (str): The hash of the anchor block.
- “merkle” (str): The root hash of the Merkle tree representing the block’s transactions.
- “links” (list): A list of hashes representing links to other blocks.
- “isGenesis” (bool): A boolean value indicating whether the block is a genesis block.

* **Type:** *dict*

#### Methods:

##### \_\_init_\_(atom: ChainAtomPtr, chain: [CFChain](#pycfhelpers.node.net.CFChain))

Initialize a CFBlock instance with the given atom and chain.

* **Parameters:**
  * **atom** (*ChainAtomPtr*) – The ChainAtomPtr object
    representing the block.
  * **chain** ([*CFChain*](#pycfhelpers.node.net.CFChain)) – The CFChain object associated with the block.
* **Raises:**
  * **TypeError** – If the chain type does not contain blocks.
  * **RuntimeError** – If the block is not found from the atom.

##### get_datums()

Get datums associated with the block.

* **Returns:** A list of CFDatum objects associated with the block.
* **Return type:** *list[[CFDatum](#pycfhelpers.node.datums.CFDatum)]*

### *class* pycfhelpers.node.consensus.CFEvent(atom: ChainAtomPtr, chain: [CFChain](#pycfhelpers.node.net.CFChain))

The CellFrame event.

Provides methods to work with CellFrame events.

#### Attributes:

##### chain

The CFChain object associated with the event.
* **Type:** *[CFChain](#pycfhelpers.node.net.CFChain)*

##### hash

The hash of the event.

* **Type:** *str*

##### created_at

The timestamp when the event was created.

* **Type:** *float*

##### version

The version of the event.

* **Type:** *int*

##### cell_id

The cell ID associated with the event.

* **Type:** *str*

##### round_id

The round ID associated with the event.

* **Type:** *int*

##### hashes

A list of hashes associated with the event.

* **Type:** *list*

#### Methods:

##### \_\_init_\_(atom: ChainAtomPtr, chain: [CFChain](#pycfhelpers.node.net.CFChain))

Initialize a CFEvent object with the given atom and chain.

* **Parameters:**
  * **atom** (*ChainAtomPtr*) – The ChainAtomPtr object representing the event.
  * **chain** ([*CFChain*](#pycfhelpers.node.net.CFChain)) – The CFChain object associated with the event.
* **Raises:**
  * **TypeError** – If the chain type does not contain events.
  * **RuntimeError** – If the event is not found from the atom.

##### get_datums()

Get datum associated with the event.

* **Returns:** A list of CFDatum object associated with the event. Event has only one Datum.
* **Return type:** *List[[CFDatum](#pycfhelpers.node.datums.CFDatum)]*

## pycfhelpers.node.crypto module

### *class* pycfhelpers.node.crypto.CFCertificate(certificate_name: str)

The CellFrame certificate.

Provides methods to work with CellFrame certificates.

#### Attributes:

##### pkey

The public key associated with the certificate.
* **Type:** *[CFPkey](#pycfhelpers.node.crypto.CFPkey)*

##### key

The private key associated with the certificate.

* **Type:** *[CFKey](#pycfhelpers.node.crypto.CFKey)*

##### \_origin_certificate

The origin certificate.

* **Type:** *Cert*

#### Methods:

##### \_\_init_\_(certificate_name: str)

Initialize a CFCertificate object with the given certificate name.

* **Parameters:**
	* **certificate_name** (*str*) – The name of the certificate.
* **Raises:**
	* **ValueError** – If the certificate with the given name is not found.

##### delete()

Delete the certificate.

##### *property* private_key *: [CFKey](#pycfhelpers.node.crypto.CFKey) | None*

The private key associated with the certificate.

* **Returns:** The private key, or None if not available.
* **Return type:** *[CFKey](#pycfhelpers.node.crypto.CFKey) | None*

##### *property* public_key *: [CFPkey](#pycfhelpers.node.crypto.CFPkey)*

The public key associated with the certificate.
* **Returns:** The public key.
* **Return type:** *[CFPkey](#pycfhelpers.node.crypto.CFPkey)*

### *class* pycfhelpers.node.crypto.CFKey(key: Key)

The CellFrame key.

Provides methods to work with CellFrame keys..


#### Methods:

##### \_\_init_\_(key: Key)

Initialize a CFKey object with the given key.
* **Parameters:**
	* **key** (*Key*) – The Key object.

### *class* pycfhelpers.node.crypto.CFPkey(pkey: DAP.Crypto.Pkey)

The CellFrame public key.

Provides methods to work with CellFrame public keys.

#### Attributes:

##### type

The type of the public key.

* **Type:** *str*

##### size

The size of the public key.

* **Type:** *int*

##### hash

The hash of the public key.

* **Type:** *str*

#### Methods:

##### \_\_init_\_(pkey: DAP.Crypto.Pkey)

Initialize a CFPkey object with the given pkey.

* **Parameters:**
	* **pkey** (*Pkey*) – The Pkey object representing the public key.

##### *static* \_from_bytes(data: bytes, /)

Create a CFPkey object from bytes.

* **Parameters:**
	* **data** (*bytes*) – The bytes representation of the public key.
* **Returns:** The CFPkey object created from bytes.
* **Return type:** *[CFPkey](#pycfhelpers.node.crypto.CFPkey)*

##### \_to_bytes()

Convert the public key to bytes.
* **Returns:** The bytes representation of the public key.
* **Return type:** *bytes*

### *class* pycfhelpers.node.crypto.CFSign(sign: Sign, net: [CFNet](#pycfhelpers.node.net.CFNet))

The CellFrame signature.

Provides methods to work with CellFrame signatures.

#### Attributes:

##### net

The CFNet object associated with the signature.
* **Type:** *[CFNet](#pycfhelpers.node.net.CFNet)*

##### type

The type of the signature.

* **Type:** *str*

##### pkey

The public key associated with the signature.

* **Type:** *[CFPkey](#pycfhelpers.node.crypto.CFPkey)*

##### size

The size of the signature.

* **Type:** *int*

##### pkey_hash

The hash of the public key associated with the signature.

* **Type:** *str*

##### address

The address derived from the signature.

* **Type:** *str*

#### Methods:

##### \_\_init_\_(sign: Sign, net: [CFNet](#pycfhelpers.node.net.CFNet))

Initialize a CFSign object with the given sign and net.

* **Parameters:**
  * **sign** (*Sign*) – The Sign object representing the signature.
  * **net** ([*CFNet*](#pycfhelpers.node.net.CFNet)) – The CFNet object associated with the signature.

##### *static* deserialize(data: bytes, net: [CFNet](#pycfhelpers.node.net.CFNet))

Deserialize the signature.

* **Parameters:**
  * **data** (*bytes*) – The serialized signature data.
  * **net** ([*CFNet*](#pycfhelpers.node.net.CFNet)) – The CFNet object associated with the signature.
* **Returns:** The deserialized CFSign object.
* **Return type:** *[CFSign](#pycfhelpers.node.crypto.CFSign)*

##### serialize()

Serialize the signature.
* **Returns:** The serialized signature.
* **Return type:** *bytes*

## pycfhelpers.node.datums module

### *class* pycfhelpers.node.datums.CFDatum(atom: [CFBlock](#pycfhelpers.node.consensus.CFBlock) | [CFEvent](#pycfhelpers.node.consensus.CFEvent) | None, datum: Datum, net: [CFNet](#pycfhelpers.node.net.CFNet) | None = None)

The Datum in the CellFrame.

#### Attributes:

##### hash

The hash of the datum.
* **Type:** *str*

##### type

The type of the datum.

* **Type:** *str*

##### version

The version of the datum.

* **Type:** *str*

##### size

The size of the datum.

* **Type:** *int*

##### created_at

The creation timestamp of the datum.

* **Type:** *datetime*

##### atom

The parent atom of the datum.

* **Type:** *[CFBlock](#pycfhelpers.node.consensus.CFBlock) | [CFEvent](#pycfhelpers.node.consensus.CFEvent) | None*

##### net

The CFNet instance.

* **Type:** *[CFNet](#pycfhelpers.node.net.CFNet) | None*

#### Methods:

##### \_\_init_\_(atom: [CFBlock](#pycfhelpers.node.consensus.CFBlock) | [CFEvent](#pycfhelpers.node.consensus.CFEvent) | None, datum: Datum, net: [CFNet](#pycfhelpers.node.net.CFNet) | None = None)

Initialize a new CFDatum instance.

* **Parameters:**
  * **atom** ([*CFBlock*](#pycfhelpers.node.consensus.CFBlock) *|* [*CFEvent*](#pycfhelpers.node.consensus.CFEvent) *|* *None*) – The parent atom of the datum.
  * **datum** (*Datum*) – The datum object.
  * **net** ([*CFNet*](#pycfhelpers.node.net.CFNet) *|* *None* *,* *optional*) – The CFNet instance. Defaults to None.
* **Raises:**
	* **AttributeError** – If atom is None and net is None.

##### \_\_repr_\_()

Return a string representation of the datum.

* **Returns:** The string representation of the datum containing
  type and hash.
* **Return type:** *str*

##### get_sub_datum()

Retrieve the sub-datum associated with this datum.
* **Returns:** The sub-datum object.
* **Return type:** *Union[[CFDatumTX](#pycfhelpers.node.datums.CFDatumTX), [CFDatumToken](#pycfhelpers.node.datums.CFDatumToken), [CFDatumEmission](#pycfhelpers.node.datums.CFDatumEmission), [CFDatumAnchor](#pycfhelpers.node.datums.CFDatumAnchor), [CFDatumDecree](#pycfhelpers.node.datums.CFDatumDecree), [CFDatumCustom](#pycfhelpers.node.datums.CFDatumCustom)]*

##### serialize()

Serialize the datum to bytes.
* **Returns:** The serialized datum.
* **Return type:** *bytes*

### *class* pycfhelpers.node.datums.CFDatumAnchor(parent_datum: [CFDatum](#pycfhelpers.node.datums.CFDatum) | None, sub_datum: CellFrame.Common.DatumAnchor)

Bases: [`CFSubDatum`](#pycfhelpers.node.datums.CFSubDatum)

Represents an anchor sub-datum in CellFrame.

#### Attributes:

##### hash

The hash value of the anchor sub-datum.
* **Type:** *str*

##### created_at

The creation timestamp of the anchor sub-datum.

* **Type:** *datetime*

##### signs

List of cryptographic signatures associated with the anchor.

* **Type:** *List[[CFSign](#pycfhelpers.node.crypto.CFSign)]*

#### Methods:

##### \_\_init_\_(parent_datum: [CFDatum](#pycfhelpers.node.datums.CFDatum) | None, sub_datum: CellFrame.Common.DatumAnchor)

Initialize a new CFDatumAnchor instance.

* **Parameters:**
  * **parent_datum** ([*CFDatum*](#pycfhelpers.node.datums.CFDatum) *|* *None*) – The parent datum associated with the sub-datum, or None if there’s no parent datum.
  * **sub_datum** (*DatumAnchor*) – The anchor sub-datum object.

### *class* pycfhelpers.node.datums.CFDatumCustom(parent_datum: [CFDatum](#pycfhelpers.node.datums.CFDatum) | None, sub_datum: bytes)

Bases: [`CFSubDatum`](#pycfhelpers.node.datums.CFSubDatum)

Represents a custom sub-datum in CellFrame.

#### Attributes:

##### hash

The hash value of the custom sub-datum.

* **Type:** *str*

##### type

The type of the custom sub-datum.

* **Type:** *str*

##### data

The raw data associated with the custom sub-datum, represented as hexadecimal string.

* **Type:** *str*

#### Methods:

##### \_\_init_\_(parent_datum: [CFDatum](#pycfhelpers.node.datums.CFDatum) | None, sub_datum: bytes)

Initialize a new CFDatumCustom instance.

* **Parameters:**
  * **parent_datum** ([*CFDatum*](#pycfhelpers.node.datums.CFDatum) *|* *None*) – The parent datum associated with the sub-datum, or None if there’s no parent datum.
  * **sub_datum** (*bytes*) – The raw data of the custom sub-datum.

### *class* pycfhelpers.node.datums.CFDatumDecree(parent_datum: [CFDatum](#pycfhelpers.node.datums.CFDatum) | None, sub_datum: CellFrame.Common.DatumDecree)

Bases: [`CFSubDatum`](#pycfhelpers.node.datums.CFSubDatum)

Represents a decree sub-datum in CellFrame.

#### Attributes:

##### hash

The hash value of the decree sub-datum.

* **Type:** *str*

##### type

The type of the decree sub-datum.

* **Type:** *str*

##### subtype

The subtype of the decree sub-datum.

* **Type:** *str*

##### created_at

The creation timestamp of the decree sub-datum.

* **Type:** *datetime*

##### signs

List of cryptographic signatures associated with the decree.

* **Type:** *List[[CFSign](#pycfhelpers.node.crypto.CFSign)]*

#### Methods:

##### \_\_init_\_(parent_datum: [CFDatum](#pycfhelpers.node.datums.CFDatum) | None, sub_datum: CellFrame.Common.DatumDecree)

Initialize a new CFDatumDecree instance.

* **Parameters:**
  * **parent_datum** ([*CFDatum*](#pycfhelpers.node.datums.CFDatum) *|* *None*) – The parent datum associated with the sub-datum, or None if there’s no parent datum.
  * **sub_datum** (*DatumDecree*) – The decree sub-datum object.

### *class* pycfhelpers.node.datums.CFDatumEmission(parent_datum: [CFDatum](#pycfhelpers.node.datums.CFDatum) | None, sub_datum: DatumEmission, net: [CFNet](#pycfhelpers.node.net.CFNet) | None = None)

Bases: [`CFSubDatum`](#pycfhelpers.node.datums.CFSubDatum)

The emission sub-datum in CellFrame.

#### Attributes:

##### hash

The hash value of the emission sub-datum.

* **Type:** *str*

##### type

The type of the emission sub-datum.

* **Type:** *str*

##### ticker

The ticker symbol associated with the token.

* **Type:** *str*

##### version

The version of the emission.

* **Type:** *str*

##### address

The address associated with the emission.

* **Type:** *str*

##### value

The value of the emission, representing a quantity of some unit.

* **Type:** *int*

##### data

Additional data associated with the emission, including cryptographic signatures, dictionaries, or strings.

* **Type:** *List[Union[[CFSign](#pycfhelpers.node.crypto.CFSign), dict, str]]*

##### tsd

Typed section data associated with the emission, containing various information in a structured format.

* **Type:** *dict[str, Union[str, dict[str, Any]]]*

#### Methods:

##### \_\_init_\_(parent_datum: [CFDatum](#pycfhelpers.node.datums.CFDatum) | None, sub_datum: DatumEmission, net: [CFNet](#pycfhelpers.node.net.CFNet) | None = None)

Initialize a new CFDatumEmission instance.

* **Parameters:**
  * **parent_datum** ([*CFDatum*](#pycfhelpers.node.datums.CFDatum) *|* *None*) – The parent datum associated with the sub-datum, or None if there’s no parent datum.
  * **sub_datum** (*DatumEmission*) – The emission sub-datum object.
  * **net** ([*CFNet*](#pycfhelpers.node.net.CFNet) *|* *None* *,* *optional*) – The CFNet instance representing the network context, defaults to None.

##### \_\_repr_\_()

Return a string representation of the datum.

* **Returns:** The string representation of the datum containing type and hash.
* **Return type:** *str*

##### add_sign(certificate: [CFCertificate](#pycfhelpers.node.crypto.CFCertificate))

Add a cryptographic signature to the emission.
* **Parameters:**
	* **certificate** ([*CFCertificate*](#pycfhelpers.node.crypto.CFCertificate)) – The certificate containing the signature.
* **Returns:** The cryptographic signature object.
* **Return type:** *[CFSign](#pycfhelpers.node.crypto.CFSign)*

##### *property* signs *: List[[CFSign](#pycfhelpers.node.crypto.CFSign)]*

Get the list of signatures associated with the emission.
* **Returns:** The list of signatures.
* **Return type:** *List[[CFSign](#pycfhelpers.node.crypto.CFSign)]*

##### *property* valid_sign_hashes *: List[str]*

Get the list of valid signature hashes associated with the emission.
* **Returns:** The list of valid signature hashes.
* **Return type:** *List[str]*

### *class* pycfhelpers.node.datums.CFDatumTX(parent_datum: [CFDatum](#pycfhelpers.node.datums.CFDatum) | None, sub_datum: DatumTx, net: [CFNet](#pycfhelpers.node.net.CFNet) | None = None)

Bases: [`CFSubDatum`](#pycfhelpers.node.datums.CFSubDatum)

The transaction sub-datum in CellFrame.

#### Attributes:

##### created_at

The datetime when the sub-datum was created.
* **Type:** *datetime*

##### ticker

The ticker associated with the transaction sub-datum.

* **Type:** *str*

##### ledger_rc

The response code from the ledger cache.

* **Type:** *[CFLedgerCacheResponse](#pycfhelpers.node.types.CFLedgerCacheResponse)*

##### accepted

Indicates whether the transaction is accepted or not.

* **Type:** *bool

* **Parameters:**
	* **CFSubDatum** ([*CFSubDatum*](#pycfhelpers.node.datums.CFSubDatum)) – a sub-datum in CellFrame*

#### Methods:

##### \_\_init_\_(parent_datum: [CFDatum](#pycfhelpers.node.datums.CFDatum) | None, sub_datum: DatumTx, net: [CFNet](#pycfhelpers.node.net.CFNet) | None = None)

Initialize a new CFDatumTX instance.

* **Parameters:**
  * **parent_datum** ([*CFDatum*](#pycfhelpers.node.datums.CFDatum) *|* *None*) – The parent datum associated with the sub-datum, or None if there’s no parent datum.
  * **sub_datum** (*DatumTx*) – The transaction sub-datum object.
  * **net** ([*CFNet*](#pycfhelpers.node.net.CFNet) *|* *None* *,* *optional*) – The CFNet instance representing the
  * **context** (*network*)
  * **None.** (*defaults to*)

##### get_items(filter_type: Type[T] | None = None)

Get the list of items associated with transaction.

* **Parameters:**
	* **filter_type** (*Type* *[**T* *]*  *|* *None* *,* *optional*) – The type to filter items. Defaults to None.
* **Returns:** The list of filtered items.
* **Return type:** *List[T]*

##### *property* items *: List[[CFItem](#pycfhelpers.node.items.CFItem)]*

Get the list of items associated with this transaction.
* **Returns:** The list of items.
* **Return type:** *List[[CFItem](#pycfhelpers.node.items.CFItem)]*

### *class* pycfhelpers.node.datums.CFDatumToken(parent_datum: [CFDatum](#pycfhelpers.node.datums.CFDatum) | None, sub_datum: CellFrame.Common.DatumToken)

Bases: [`CFSubDatum`](#pycfhelpers.node.datums.CFSubDatum)

Represents a token sub-datum in CellFrame.

#### Attributes:

##### hash

The hash value of the token sub-datum.
* **Type:** *str*

##### type

The type of the token sub-datum.

* **Type:** *str*

##### ticker

The ticker symbol of the token.

* **Type:** *str*

##### data

Additional data associated with the token.

* **Type:** *str*

##### signs

List of cryptographic signatures
associated with the token.

* **Type:** *List[[CFSign](#pycfhelpers.node.crypto.CFSign)]*

#### Methods:

##### \_\_init_\_(parent_datum: [CFDatum](#pycfhelpers.node.datums.CFDatum) | None, sub_datum: CellFrame.Common.DatumToken)

Initialize a new CFDatumToken instance.

* **Parameters:**
  * **parent_datum** ([*CFDatum*](#pycfhelpers.node.datums.CFDatum) *|* *None*) – The parent datum associated with the sub-datum, or None if there’s no parent datum.
  * **sub_datum** (*DatumToken*) – The token sub-datum object.

### *class* pycfhelpers.node.datums.CFSubDatum(parent_datum: [CFDatum](#pycfhelpers.node.datums.CFDatum) | None, sub_datum: DatumTx | DatumToken | DatumEmission | DatumDecree | DatumAnchor | bytes, net: [CFNet](#pycfhelpers.node.net.CFNet) | None = None)

Represent a sub-datum in CellFrame.

#### Attributes:

##### hash

The hash value of the sub-datum.

* **Type:** *str*

##### type

The type of the sub-datum, defaults to “DEFAULT”

* **Type:** *str*

#### Methods:

##### \_\_init_\_(parent_datum: [CFDatum](#pycfhelpers.node.datums.CFDatum) | None, sub_datum: DatumTx | DatumToken | DatumEmission | DatumDecree | DatumAnchor | bytes, net: [CFNet](#pycfhelpers.node.net.CFNet) | None = None)

Initialize a new CFSubDatum instance.

* **Parameters:**
  * **parent_datum** ([*CFDatum*](#pycfhelpers.node.datums.CFDatum) *|* *None*) – The parent datum associated with the sub-datum, or None if there’s no parent datum.
  * **sub_datum** (*Union* *[**DatumTx* *,* *DatumToken* *,* *DatumEmission* *,* *DatumDecree* *,* *DatumAnchor* *,* *bytes* *]*)
  * **representation.** (*The sub-datum object* *or* *its bytes*)
  * **net** ([*CFNet*](#pycfhelpers.node.net.CFNet) *|* *None* *,* *optional*) – The CFNet instance representing the network, defaults to None.

##### *property* net *: [CFNet](#pycfhelpers.node.net.CFNet)*

Get the CFNet instance representing the network.

* **Returns:** The CFNet instance.
* **Return type:** *[CFNet](#pycfhelpers.node.net.CFNet)*

## pycfhelpers.node.gdb module

### *class* pycfhelpers.node.gdb.GDBGroup(group)

Bases: [`GDBGroupBase`](#pycfhelpers.node.gdb.GDBGroupBase)

Represents a group in the GlobalDB.

#### Attributes:

##### group

The name of the group.
* **Type:** *str*

#### Methods:

##### delete(key: str)

Delete the value associated with the given key from the group.

* **Parameters:**
	* **key** (*str*) – The key to delete.
* **Returns:** None

##### get(key: str, net: [CFNet](#pycfhelpers.node.net.CFNet) | None = None, default: D | None = None)

Retrieve the value associated with the given key from the group.
* **Parameters:**
  * **key** (*str*) – The key.
  * **net** ([*CFNet*](#pycfhelpers.node.net.CFNet) *|* *None*) – The CFNet instance. Defaults to None.
  * **default** (*D* *|* *None*) – The default value to return if
  * **None.** (*the key is not found. Defaults to*)
* **Returns:** The value associated with the key, or the default value
  if the key is not found.
* **Return type:** *bytes | D*

##### group_list()

Retrieve the list of key-value pairs in the group.
* **Returns:** The list of key-value pairs.
* **Return type:** *list[dict[str, bytes]]*

##### set(key: str, value: bytes)

Set the value associated with the given key in the group.
* **Parameters:**
  * **key** (*str*) – The key.
  * **value** (*bytes*) – The value.
* **Returns:** True if successful, False otherwise.
* **Return type:** *bool*

### *class* pycfhelpers.node.gdb.GDBGroupBase(group)

Bases: `ABC`

Abstract base class representing a group in the GlobalDB.

#### Attributes:

##### group

The name of the group.
* **Type:** *str*

#### Methods:

##### \_\_init_\_(group)

Initialize a new GDBGroupBase instance.

* **Parameters:**
	* **group** (*str*) – The name of the group.

##### *static* add_to_white_list(net_name: str, group_mask: str, callback: Callable, callback_args: list)

Add the specified group to the GlobalDB white list.

* **Parameters:**
  * **net_name** (*str*) – The name of the network.
  * **group_mask** (*str*) – The group mask pattern.
    Example: “raiden.emission-center.\*”
  * **callback** (*Callable*) – The callback function.
  * **callback_args** (*list*) – The list of callback arguments.
* **Returns:** None

##### *abstract* delete(key: str)

Delete the value associated with the given key from the group.
* **Parameters:**
	* **key** (*str*) – The key to delete.
* **Returns:** None

##### *abstract* get(key: str, net: [CFNet](#pycfhelpers.node.net.CFNet), default=None)

Retrieve the value associated with the given key from the group.
* **Parameters:**
  * **key** (*str*) – The key.
  * **net** ([*CFNet*](#pycfhelpers.node.net.CFNet)) – The CFNet instance.
  * **default** (*Any* *|* *None*) – The default value to return if the
  * **None.** (*key is not found. Defaults to*)
* **Returns:** The value associated with the key, or the default value    if the key is not found.
* **Return type:** *Any*

##### *abstract* group_list()

Retrieve the list of key-value pairs in the group.
* **Returns:** The list of key-value pairs.
* **Return type:** *list[dict[str, Any]]*

##### *abstract* set(key: str, value: Any)

Set the value associated with the given key in the group.
* **Parameters:**
  * **key** (*str*) – The key.
  * **value** (*Any*) – The value.
* **Returns:** None

### *class* pycfhelpers.node.gdb.GDBGroupSign(group)

Bases: [`GDBGroupBase`](#pycfhelpers.node.gdb.GDBGroupBase)

Represents a group in the GlobalDB for signed data.

#### Attributes:

##### group

The name of the group.
* **Type:** *str*

#### Methods:

##### delete(key: str)

Delete the value associated with the given key from the group.

* **Parameters:**
	* **key** (*str*) – The key to delete.
* **Returns:** None

##### get(key: str, net: [CFNet](#pycfhelpers.node.net.CFNet), default: D | None = None)

Retrieve the value associated with the given key from the group.
* **Parameters:**
  * **key** (*str*) – The key.
  * **net** ([*CFNet*](#pycfhelpers.node.net.CFNet)) – The CFNet instance.
  * **default** (*D* *|* *None*) – The default value to return if the key is not found. Defaults to None.
* **Returns:** The value associated with the key, or
  the default value if the key is not found.
* **Return type:** *[SerializableObject](#pycfhelpers.node.gdb.SerializableObject) | D*

##### group_list()

Retrieve the list of key-value pairs in the group.
* **Returns:** The list of key-value pairs.
* **Return type:** *list[dict[str, bytes]]*

##### set(key: str, value: [SerializableObject](#pycfhelpers.node.gdb.SerializableObject))

Set the value associated with the given key in the group.
* **Parameters:**
  * **key** (*str*) – The key.
  * **value** ([*SerializableObject*](#pycfhelpers.node.gdb.SerializableObject)) – The value.
* **Returns:** True if successful, False otherwise.
* **Return type:** *bool*

### *class* pycfhelpers.node.gdb.SerializableObject(\*args, \*\*kwargs)

Bases: `Protocol`

Protocol representing a serializable object.


#### Methods:

##### deserialize()

Deserialize the object.
* **Returns:** The deserialized object.
* **Return type:** *object*

##### serialize()

Serialize the object to bytes.
* **Returns:** The serialized object.
* **Return type:** *bytes*

## pycfhelpers.node.items module

### *class* pycfhelpers.node.items.CFItem(origin_item: OriginItemTypes, type: [ItemTypes](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes), net: [CFNet](#pycfhelpers.node.net.CFNet))

The item in CellFrame.

#### Attributes:

##### type

The type of the item.
* **Type:** *str*

##### net

The CFNet instance.

* **Type:** *[CFNet](#pycfhelpers.node.net.CFNet)*

#### Methods:

##### \_\_init_\_(origin_item: OriginItemTypes, type: [ItemTypes](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes), net: [CFNet](#pycfhelpers.node.net.CFNet))

Initialize a new CFItem instance.

* **Parameters:**
  * **origin_item** (*OriginItemTypes*) – The original item.
  * **type** ([*ItemTypes*](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes)) – The type of the item.
  * **net** ([*CFNet*](#pycfhelpers.node.net.CFNet)) – The CFNet instance.

### *class* pycfhelpers.node.items.CFTxIn(\*args, \*\*kwargs)

Bases: [`CFItem`](#pycfhelpers.node.items.CFItem)

The transaction input in CellFrame.

#### Attributes:

##### prev_hash

The hash of the previous transaction.

* **Type:** *str*

##### prev_idx

The index of the previous output in the previous transaction.

* **Type:** *int*

#### Methods:

##### \_\_init_\_(\*args, \*\*kwargs)

Initialize a new CFTxIn instance.

### *class* pycfhelpers.node.items.CFTxInCond(origin_item: OriginItemTypes, type: [ItemTypes](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes), net: [CFNet](#pycfhelpers.node.net.CFNet))

Bases: [`CFItem`](#pycfhelpers.node.items.CFItem)

The conditional transaction input in CellFrame.

### *class* pycfhelpers.node.items.CFTxOut(\*args, \*\*kwargs)

Bases: [`CFItem`](#pycfhelpers.node.items.CFItem)

The transaction output in CellFrame.

#### Attributes:

##### address

The address of the recipient.

* **Type:** *str*

##### value

The value of the output.

* **Type:** *int*

#### Methods:

##### \_\_init_\_(\*args, \*\*kwargs)

Initialize a new CFTxOut instance.

### *class* pycfhelpers.node.items.CFTxOutCond(\*args, \*\*kwargs)

Bases: [`CFItem`](#pycfhelpers.node.items.CFItem)

The conditional transaction output in CellFrame.

#### Attributes:

##### expires

The expiration timestamp of the conditional output.

* **Type:** *int*

##### value

The value of the output.

* **Type:** *int*

##### subtype

The subtype of the output.

* **Type:** *str*

#### Methods:

##### \_\_init_\_(\*args, \*\*kwargs)

Initialize a new CFTxOutCond instance.

### *class* pycfhelpers.node.items.CFTxOutCondSubtypeSrvPay(origin_item: OriginItemTypes, type: [ItemTypes](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes), net: [CFNet](#pycfhelpers.node.net.CFNet))

Bases: [`CFItem`](#pycfhelpers.node.items.CFItem)

The server payment subtype of a conditional transaction output in CellFrame.

### *class* pycfhelpers.node.items.CFTxOutCondSubtypeSrvStakeLock(\*args, \*\*kwargs)

Bases: [`CFItem`](#pycfhelpers.node.items.CFItem)

The server stake lock subtype of a conditional transaction output in CellFrame.

#### Attributes:

##### unlock

The unlock time for the stake.

* **Type:** *int*

##### value

The value of the stake.

* **Type:** *int*

##### reinvest_percent

The reinvestment percentage for the stake.

* **Type:** *int*

#### Methods:

##### \_\_init_\_(\*args, \*\*kwargs)

Initialize a new CFTxOutCondSubtypeSrvStakeLock instance.

### *class* pycfhelpers.node.items.CFTxOutCondSubtypeSrvStakePosDelegate(origin_item: OriginItemTypes, type: [ItemTypes](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes), net: [CFNet](#pycfhelpers.node.net.CFNet))

Bases: [`CFItem`](#pycfhelpers.node.items.CFItem)

The server stake pos delegate subtype of a conditional transaction output in CellFrame.

### *class* pycfhelpers.node.items.CFTxOutCondSubtypeSrvXchange(origin_item: OriginItemTypes, type: [ItemTypes](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes), net: [CFNet](#pycfhelpers.node.net.CFNet))

Bases: [`CFItem`](#pycfhelpers.node.items.CFItem)

The server exchange subtype of a conditional transaction output in CellFrame.

### *class* pycfhelpers.node.items.CFTxOutExt(\*args, \*\*kwargs)

Bases: [`CFItem`](#pycfhelpers.node.items.CFItem)

The extended transaction output in CellFrame.

#### Attributes:

##### address

The address of the recipient.

* **Type:** *str*

##### ticker

The ticker symbol of the token.

* **Type:** *str*

##### value

The value of the output.

* **Type:** *int*

#### Methods:

##### \_\_init_\_(\*args, \*\*kwargs)

Initialize a new CFTxOutExt instance.

### *class* pycfhelpers.node.items.CFTxPkey(origin_item: OriginItemTypes, type: [ItemTypes](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes), net: [CFNet](#pycfhelpers.node.net.CFNet))

Bases: [`CFItem`](#pycfhelpers.node.items.CFItem)

The transaction public key in CellFrame.

### *class* pycfhelpers.node.items.CFTxReceipt(origin_item: OriginItemTypes, type: [ItemTypes](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes), net: [CFNet](#pycfhelpers.node.net.CFNet))

Bases: [`CFItem`](#pycfhelpers.node.items.CFItem)

The transaction receipt in CellFrame.

### *class* pycfhelpers.node.items.CFTxSig(\*args, \*\*kwargs)

Bases: [`CFItem`](#pycfhelpers.node.items.CFItem)

The transaction signature in CellFrame.

#### Attributes:

##### size

The size of the signature.

* **Type:** *int*

##### sign

The cryptographic signature.

* **Type:** *[CFSign](#pycfhelpers.node.crypto.CFSign)*

#### Methods:

##### \_\_init_\_(\*args, \*\*kwargs)

Initialize a new CFTxSig instance.

### *class* pycfhelpers.node.items.CFTxTSD(origin_item: OriginItemTypes, type: [ItemTypes](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes), net: [CFNet](#pycfhelpers.node.net.CFNet))

Bases: [`CFItem`](#pycfhelpers.node.items.CFItem)

The transaction TSD  in CellFrame.

### *class* pycfhelpers.node.items.CFTxToken(\*args, \*\*kwargs)

Bases: [`CFItem`](#pycfhelpers.node.items.CFItem)

The token transaction in CellFrame.

#### Attributes:

##### ticker

The ticker symbol of the token.

* **Type:** *str*

##### emission_hash

The hash of the token emission.

* **Type:** *str*

##### emission_chain_id

The chain ID of the token emission.

* **Type:** *str*

#### Methods:

##### \_\_init_\_(\*args, \*\*kwargs)

Initialize a new CFTxToken instance.

### *class* pycfhelpers.node.items.CFTxVote(origin_item: OriginItemTypes, type: [ItemTypes](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes), net: [CFNet](#pycfhelpers.node.net.CFNet))

Bases: [`CFItem`](#pycfhelpers.node.items.CFItem)

The transaction vote in CellFrame.

### *class* pycfhelpers.node.items.CFTxVoting(origin_item: OriginItemTypes, type: [ItemTypes](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes), net: [CFNet](#pycfhelpers.node.net.CFNet))

Bases: [`CFItem`](#pycfhelpers.node.items.CFItem)

The transaction voting in CellFrame.

## pycfhelpers.node.logging module

### *class* pycfhelpers.node.logging.CFLog

Provides logging functionality for CellFrame.

#### Attributes:

##### \_logger

The logger instance.

#### Methods:

##### \_\_init_\_()

Initialize a new CFLog instance.

##### att(message: str)

Log a message with the ATTENTION level.

* **Parameters:**
	* **message** (*str*) – The attention message.

##### critical(message: str)

Log a message with the CRITICAL level

* **Parameters:**
	* **message** (*str*) – The critical error message.

##### dap(message: str)

Log a message with the DAP level

* **Parameters:**
	* **message** (*str*) – The DAP-specific message.

##### debug(message: str)

Log a message with the DEBUG level.

* **Parameters:**
	* **message** (*str*) – The debug message.

##### error(message: str)

Log a message with the ERROR level.

* **Parameters:**
	* **message** (*str*) – The error message.

##### info(message: str)

Log a message with the INFO level

* **Parameters:**
	* **message** (*str*) – The informational message.

##### message(message: str)

Log a message with the MESSAGE level

* **Parameters:**
	* **message** (*str*) – The generic message.

##### notice(message: str)

Log a message with the NOTICE level.

* **Parameters:**
	* **message** (*str*) – The notice message.

##### warning(message: str)

Log a message with the WARNING level.

* **Parameters:**
	* **message** (*str*) – The warning message.

## pycfhelpers.node.mappings module

### *class* pycfhelpers.node.mappings.CFItemMapper

Maps item types to corresponding CellFrame item wrapper classes.


#### Methods:

##### *static* build(origin_item, net)

Build a CellFrame item based on the provided origin item.

* **Parameters:**
  * **origin_item** – The original item.
  * **net** – The CFNet instance representing the network context.
* **Returns:** The CellFrame item.
* **Return type:** *[CFItem](#pycfhelpers.node.items.CFItem)*

### *class* pycfhelpers.node.mappings.CFSubDatumBuilder(type)

Build sub-datums for CellFrame based on datum type.


#### Methods:

##### \_\_init_\_(type)

Initialize a new CFSubDatumBuilder instance.
* **Parameters:**
	* **type** ([*DatumTypes*](pycfhelpers.common.md#pycfhelpers.common.types.DatumTypes)) – The type of datum for which sub-datum is being built. Should be one of the enumeration values from DatumTypes.

##### build(parent_datum)

Build a sub-datum for CellFrame based on the parent datum.

* **Parameters:**
	* **parent_datum** – The parent datum.
* **Returns:** The sub-datum.
* **Return type:** *[CFDatum](#pycfhelpers.node.datums.CFDatum)*

### pycfhelpers.node.mappings.ItemMeta

alias of `ItemMapping`

### pycfhelpers.node.mappings.SubDatumMeta

alias of `SubDatumMapping`

## pycfhelpers.node.math module

## pycfhelpers.node.net module

### *class* pycfhelpers.node.net.CFChain(net: [CFNet](#pycfhelpers.node.net.CFNet), chain_name: str)

Represents a CellFrame chain.

#### Attributes:

##### net

The CFNet object.
* **Type:** *[CFNet](#pycfhelpers.node.net.CFNet)*

##### type

The type of the chain.

* **Type:** *str*

##### name

The name of the chain.

* **Type:** *str*

#### Methods:

##### \_\_init_\_(net: [CFNet](#pycfhelpers.node.net.CFNet), chain_name: str)

Initialize CFChain object.

* **Parameters:**
  * **net** ([*CFNet*](#pycfhelpers.node.net.CFNet)) – The CFNet object.
  * **chain_name** (*str*) – The name of the chain.
* **Raises:**
	* **RuntimeError** – If there is no net with specified chain name.

##### get_atoms()

Return an iterator over CFBlock or CFEvent objects (atoms) contained in the chain.

* **Yields:**
  *Iterator[CFBlock | CFEvent]* – An iterator over CFBlock or CFEvent objects.
* **Raises:**
	* **TypeError** – If Chain type neither ChainTypes.esbocs nor ChainTypes.dag_poa

##### get_datums(type: [CFSubDatum](#pycfhelpers.node.datums.CFSubDatum) | None = None)

> Return an iterator over CFDatum objects (datums) in the chain, optionally filtered by type.
* **Parameters:**
	* **type** ([*CFSubDatum*](#pycfhelpers.node.datums.CFSubDatum) *|* *None*) – The subdatum type to filter by.
* **Yields:**
  *Iterator[CFDatum]* – An iterator over CFDatum objects.

##### get_mempool()

> Return the CFMempool object associated with the chain.
* **Returns:** The CFMempool object.
* **Return type:** *[CFMempool](#pycfhelpers.node.net.CFMempool)*

##### get_transactions()

Return an iterator over CFDatumTX objects representing transactions in the chain.
* **Yields:**
  *Iterator[CFDatumTX]* – An iterator over CFDatumTX objects.

##### register_atom_notification_callback(callback, \*args, \*\*kwargs)

Register a callback function to be invoked when there are changes in the chain’s atoms.

* **Parameters:**
	* **callback** – The callback function.

##### register_mempool_notification_callback(callback: [MempoolCallback](#pycfhelpers.node.net.MempoolCallback), \*args, \*\*kwargs)

Register a callback function to be invoked when there are changes in the mempool.

* **Parameters:**
	* **callback** ([*MempoolCallback*](#pycfhelpers.node.net.MempoolCallback)) – The callback function.

### *class* pycfhelpers.node.net.CFLedger(net: [CFNet](#pycfhelpers.node.net.CFNet), ledger: CellFrame.Chain.Ledger | None = None)

Represents a CellFrame ledger.

#### Attributes:

##### net

The CFNet object.

* **Type:** *[CFNet](#pycfhelpers.node.net.CFNet)*

##### \_origin_ledger

The original ledger object.

* **Type:** *Ledger*

#### Methods:

##### \_\_init_\_(net: [CFNet](#pycfhelpers.node.net.CFNet), ledger: CellFrame.Chain.Ledger | None = None)

Initialize CFLedger object.

* **Parameters:**
  * **net** ([*CFNet*](#pycfhelpers.node.net.CFNet)) – The CFNet object.
  * **ledger** (*Ledger* *|* *None*) – The original ledger object. Defaults to None.

##### calc_address_balances(address: str)

Calculate the balances of an address.

* **Parameters:**
	* **address** (*str*) – The address.
* **Returns:** A dictionary containing tickers and balances.
* **Return type:** *dict[ticker, str]*

##### get_emission(emission_hash: str)

Return the emission from the ledger.
* **Parameters:**
	* **emission_hash** (*str*) – The hash of the emission.
* **Returns:** The CFDatumEmission object.
* **Return type:** *[CFDatumEmission](#pycfhelpers.node.datums.CFDatumEmission)*

##### get_tx_ledger_rc(datum: [CFDatumTX](#pycfhelpers.node.datums.CFDatumTX))

Return the transaction ledger Rc.
* **Parameters:**
	* **datum** ([*CFDatumTX*](#pycfhelpers.node.datums.CFDatumTX)) – The CFDatumTX object.
* **Returns:** The transaction ledger RC or None if not found.
* **Return type:** *ledger_cache_rc | None*

##### get_tx_ticker(datum: [CFDatumTX](#pycfhelpers.node.datums.CFDatumTX))

Return the transaction ticker.
* **Parameters:**
	* **datum** ([*CFDatumTX*](#pycfhelpers.node.datums.CFDatumTX)) – The CFDatumTX object.
* **Returns:** The transaction ticker or None if not found.
* **Return type:** *ticker | None*

##### has_emission(emission: [CFDatumEmission](#pycfhelpers.node.datums.CFDatumEmission))

Check if an emission exists in the ledger.
* **Parameters:**
	* **emission** ([*CFDatumEmission*](#pycfhelpers.node.datums.CFDatumEmission)) – The CFDatumEmission object.
* **Returns:** True if the emission exists, False otherwise.
* **Return type:** *bool*

##### register_ledger_tx_notification_callback(callback, \*args, \*\*kwargs)

Register the ledger transaction notification callback.
* **Parameters:**
	* **callback** – The callback function.

##### token_auth_signs_pkey_hashes(datum: [CFDatum](#pycfhelpers.node.datums.CFDatum))

Return the public keys hashes of token declaration for provided emission.

* **Parameters:**
	* **datum** ([*CFDatum*](#pycfhelpers.node.datums.CFDatum)) – The CFDatum object.
* **Returns:** A list of public key hashes.
* **Return type:** *List[str]*

##### token_auth_signs_total(datum: [CFDatum](#pycfhelpers.node.datums.CFDatum))

Return the total number of signature signs.
* **Parameters:**
	* **datum** ([*CFDatum*](#pycfhelpers.node.datums.CFDatum)) – The CFDatum object.
* **Returns:** The total number of signature signs.
* **Return type:** *int*

##### token_auth_signs_valid(datum: [CFDatum](#pycfhelpers.node.datums.CFDatum))

Return the number of valid signature signs.
* **Parameters:**
	* **datum** ([*CFDatum*](#pycfhelpers.node.datums.CFDatum)) – The CFDatum object.
* **Returns:** The number of valid signature signs.
* **Return type:** *int*

##### tx_by_hash(hash: str)

Return the transaction by its hash.
* **Parameters:**
	* **hash** (*str*) – The hash of the transaction.
* **Returns:** The CFDatumTX object.
* **Return type:** *[CFDatumTX](#pycfhelpers.node.datums.CFDatumTX)
* **Raises:**
	* **ValueError** – If the transaction with the specified hash is not found.*

### *class* pycfhelpers.node.net.CFMempool(chain: [CFChain](#pycfhelpers.node.net.CFChain))

Represents a CellFrame mempool.

#### Attributes:

##### chain

The CFChain object.
* **Type:** *[CFChain](#pycfhelpers.node.net.CFChain)*

#### Methods:

##### \_\_init_\_(chain: [CFChain](#pycfhelpers.node.net.CFChain))

Initialize CFMempool object.

* **Parameters:**
	* **chain** ([*CFChain*](#pycfhelpers.node.net.CFChain)) – The CFChain object.

##### get_datum_from_bytes(value: bytes)

Return a CFDatum object from bytes.

* **Parameters:**
	* **value** (*bytes*) – The bytes value.
* **Returns:** The CFDatum object or None if not found.
* **Return type:** *[CFDatum](#pycfhelpers.node.datums.CFDatum) | None*

##### get_datums()

Return a list of CFDatum objects in the mempool.
* **Returns:** A list of CFDatum objects.
* **Return type:** *List[[CFDatum](#pycfhelpers.node.datums.CFDatum)]*

##### reason()

##### remove(datum: [CFDatum](#pycfhelpers.node.datums.CFDatum) | [CFSubDatum](#pycfhelpers.node.datums.CFSubDatum))

Remove a datum from the mempool.
* **Parameters:**
	* **datum** ([*CFDatum*](#pycfhelpers.node.datums.CFDatum) *|* [*CFSubDatum*](#pycfhelpers.node.datums.CFSubDatum)) – The CFDatum or CFSubDatum object.
* **Returns:** True if removal is successful, False otherwise.
* **Return type:** *bool*

##### valid_signs_table()

### *class* pycfhelpers.node.net.CFNet(name_or_id: str | [CFNetID](#pycfhelpers.node.net.CFNetID))

Represents a CellFrame network.

#### Attributes:

##### \_origin_net

The original Net object.
* **Type:** *Net*

##### name

The name of the network.

* **Type:** *str*

##### address

The address of the network.

* **Type:** *str*

##### main

The main chain of the network.

* **Type:** *[CFChain](#pycfhelpers.node.net.CFChain)*

##### zerochain

The zerochain of the network.

* **Type:** *[CFChain](#pycfhelpers.node.net.CFChain)*

##### fee_data

The fee data of the network.

* **Type:** *[NetFee](#pycfhelpers.node.net.NetFee)*

#### Methods:

##### \_\_init_\_(name_or_id: str | [CFNetID](#pycfhelpers.node.net.CFNetID))

Initialize CFNet object.

* **Parameters:**
	* **name_or_id** (*str* *|* [*CFNetID*](#pycfhelpers.node.net.CFNetID)) – The name or CFNetID object.
* **Raises:**
	* **RuntimeError** – If there is no network with such name and ID.

##### *static* active_nets()

Return the list of active CFNet objects.

* **Returns:** The list of CFNet objects.
* **Return type:** *List[[CFNet](#pycfhelpers.node.net.CFNet)]*

##### *property* chains *: list[[CFChain](#pycfhelpers.node.net.CFChain)]*

Return the list of CFChain objects.
* **Returns:** The list of CFChain objects containing mainchain and zerochain.
* **Return type:** *List[[CFChain](#pycfhelpers.node.net.CFChain)]*

##### change_state(state: [CFNetState](#pycfhelpers.node.types.CFNetState))

Change the state of CFNet.
* **Parameters:**
	* **state** ([*CFNetState*](#pycfhelpers.node.types.CFNetState)) – The state to change to.
* **Raises:**
	* **NotImplemented** – If there is no network with such name and ID.

##### get_ledger()

Return the CFLedger object.

* **Returns:** The CFLedger object.
* **Return type:** *[CFLedger](#pycfhelpers.node.net.CFLedger)*

##### *property* group_alias *: str*

Return the group alias.
* **Returns:** The group alias.
* **Return type:** *str*

##### *property* id *: [CFNetID](#pycfhelpers.node.net.CFNetID)*

Return the CFNetID object.
* **Returns:** The CFNetID object.
* **Return type:** *[CFNetID](#pycfhelpers.node.net.CFNetID)*

##### *static* net_id_from_wallet_str(wallet: str)

Return the net ID from the wallet string.
* **Parameters:**
	* **wallet** (*str*) – The wallet string.
* **Returns:** The net ID.
* **Return type:** *int*

##### register_gdbsync_notification_callback(callback: Callable, \*args, \*\*kwargs)

Register the gdbsync notification callback.
* **Parameters:**
	* **callback** (*Callable*) – The callback function.

### *class* pycfhelpers.node.net.CFNetID(str_or_origin: str | CellFrame.Network.NetID)

Represents the ID of a CellFrame network.

#### Attributes:

##### \_origin_net_id

The original NetID object.

* **Type:** *NetID*

#### Methods:

##### \_\_init_\_(str_or_origin: str | CellFrame.Network.NetID)

Initialize CFNetID object.

* **Parameters:**
	* **str_or_origin** (*Union* *[**str* *,* *NetID* *]*) – The string or NetID object.

##### *property* long

Return the long value of CFNetID.

* **Returns:** The long value.
* **Return type:** *int*

### *class* pycfhelpers.node.net.MempoolCallback(\*args, \*\*kwargs)

Bases: `Protocol`

Represents the protocol for mempool callback functions.


#### Methods:

##### \_\_call_\_(op_code: Literal['a', 'd'], datum: [CFDatum](#pycfhelpers.node.datums.CFDatum) | datum_hash, \*args, chain: [CFChain](#pycfhelpers.node.net.CFChain), \*\*kwargs)

Call the mempool callback function.
* **Parameters:**
  * **op_code** (*Literal* *[* *"a"* *,*  *"d"* *]*) – The operation code.
  * **datum** (*Union* *[*[*CFDatum*](#pycfhelpers.node.datums.CFDatum) *,* *datum_hash* *]*) – The datum or datum hash.
  * **chain** ([*CFChain*](#pycfhelpers.node.net.CFChain)) – The CFChain object.

### *class* pycfhelpers.node.net.NetFee(net: [CFNet](#pycfhelpers.node.net.CFNet))

Represents network fee data.

#### Attributes:

##### net

The CFNet object.

* **Type:** *[CFNet](#pycfhelpers.node.net.CFNet)*

##### tx_fee

The transaction fee.

* **Type:** *str*

##### tx_fee_addr

The transaction fee adress.

* **Type:** *int*

##### validator_avg_fee

The average validator fee

* **Type:** *str*

##### validator_max_fee

The maximum validator fee

* **Type:** *str*

##### validator_min_fee

The minimum validator fee

* **Type:** *str*

##### native_ticker

The native ticker of the network.

* **Type:** *str*

#### Methods:

##### \_\_init_\_(net: [CFNet](#pycfhelpers.node.net.CFNet))

Initialize NetFee object.

* **Parameters:**
	* **net** ([*CFNet*](#pycfhelpers.node.net.CFNet)) – The CFNet object.

## pycfhelpers.node.notificators module

### *class* pycfhelpers.node.notificators.CFNets(nets: list[[CFNet](#pycfhelpers.node.net.CFNet)])

Represent a set of CFNet objects.

#### Attributes:

##### nets

The list of CFNet objects.

* **Type:** *List[[CFNet](#pycfhelpers.node.net.CFNet)]*

#### Methods:

##### \_\_init_\_(nets: list[[CFNet](#pycfhelpers.node.net.CFNet)])

Initialize CFNets object.

* **Parameters:**
	* **nets** (*List* *[*[*CFNet*](#pycfhelpers.node.net.CFNet) *]*) – The list of CFNet objects.

##### atom_notificator(\*args, chain_name: Literal['main', 'zerochain', 'all'] = 'all', \*\*kwargs)

Decorator for registering atom notification callbacks.

* **Parameters:**
  * **\*args** – Positional arguments.
  * **chain_name** (*Literal* *[* *"main"* *,*  *"zerochain"* *,*  *"all"* *]*) – The chain name to specify where to register the callback. Defaults to “all”.
  * **\*\*kwargs** – Keyword arguments.

### Example

```pycon
>>> @CFNets(NETS).atom_notificator("to_args", chain_name="main", key="to_kwargs")
>>> def on_new_atom(atom: CFBlock | CFEvent, size: int, *args, chain: CFChain, **kwargs):
>>>     pass
```

#### gdbsync_notificator(\*args, \*\*kwargs)

Decorator for registering global database sync notification callbacks.

* **Parameters:**
  * **\*args** – Positional arguments.
  * **\*\*kwargs** – Keyword arguments.

### Example

```pycon
>>> @CFNets(NETS).gdbsync_notificator("to_args", key="to_kwargs")
>>> def on_new_table_record(op_code, group, key, value, *args, net:CFNet, **kwargs):
>>>     pass
```

#### ledger_tx_notificator(\*args, \*\*kwargs)

Decorator for registering ledger transaction notification callbacks.

* **Parameters:**
  * **\*args** – Positional arguments.
  * **\*\*kwargs** – Keyword arguments.

### Example

```pycon
>>> @CFNets(NETS).ledger_tx_notificator("to_args", key="to_kwargs")
>>> def on_new_ledger_transaction(ledger, tx, *args, net: CFNet, **kwargs):
>>>     pass
```

#### mempool_notificator(\*args, chain_name: Literal['main', 'zerochain', 'all'] = 'all', \*\*kwargs)

Decorator for registering mempool notification callbacks.

* **Parameters:**
  * **\*args** – Positional arguments.
  * **chain_name** (*Literal* *[* *"main"* *,*  *"zerochain"* *,*  *"all"* *]*) – The chain name to register the callback. Defaults to “all”.
  * **\*\*kwargs** – Keyword arguments.

### Example

```pycon
>>> @CFNets(NETS).mempool_notificator("to_args", chain_name="main", key="to_kwargs")
>>> def on_mempool_change(op_code: Literal["a", "d"], datum: CFDatum | datum_hash, *args, chain: 'CFChain', **kwargs):
>>>     pass
```

## pycfhelpers.node.types module

### *class* pycfhelpers.node.types.CFLedgerCacheResponse(value)

Bases: `str`, `Enum`

Enum representing responses from the CFLedger cache.

#### Attributes:

##### DAP_LEDGER_TX_CHECK_OK

Transaction check passed.

##### DAP_LEDGER_TX_CHECK_NULL_TX

Null transaction

##### DAP_LEDGER_TX_CHECK_INVALID_TX_SIZE

Invalid transaction size.

##### DAP_LEDGER_TX_ALREADY_CACHED

Transaction has already been cached.

##### DAP_LEDGER_TX_CHECK_INVALID_TX_SIGN

Invalid transaction signature.

##### DAP_LEDGER_TX_CHECK_IN_EMS_ALREADY_USED

##### DAP_LEDGER_TX_CHECK_STAKE_LOCK_IN_EMS_ALREADY_USED

##### DAP_LEDGER_TX_CHECK_EMISSION_NOT_FOUND

##### DAP_LEDGER_TX_CHECK_TX_NO_VALID_INPUTS

##### DAP_LEDGER_TX_CHECK_TICKER_NOT_FOUND

##### DAP_LEDGER_TX_CHECK_STAKE_LOCK_INVALID_TOKEN

##### DAP_LEDGER_TX_CHECK_STAKE_LOCK_NO_OUT_COND_FOR_IN_EMS

##### DAP_LEDGER_TX_CHECK_MULT256_OVERFLOW_EMS_LOCKED_X_RATE

##### DAP_LEDGER_TX_CHECK_NO_OUT_EXT_FOR_GIRDLED_IN_EMS

##### DAP_LEDGER_TX_CHECK_NO_OUT_ITEMS_FOR_BASE_TX

##### DAP_LEDGER_TX_CHECK_TOKEN_EMS_VALUE_EXEEDS_CUR_SUPPLY

##### DAP_LEDGER_TX_CHECK_STAKE_LOCK_UNEXPECTED_VALUE

##### DAP_LEDGER_TX_CHECK_STAKE_LOCK_TICKER_NOT_FOUND

##### DAP_LEDGER_TX_CHECK_STAKE_LOCK_OTHER_TICKER_EXPECTED

##### DAP_LEDGER_TX_CHECK_OUT_ITEM_ALREADY_USED

##### DAP_LEDGER_TX_CHECK_PREV_TX_NOT_FOUND

##### DAP_LEDGER_TX_CHECK_PREV_OUT_ITEM_NOT_FOUND

##### DAP_LEDGER_TX_CHECK_PKEY_HASHES_DONT_MATCH

##### DAP_LEDGER_TX_CHECK_PREV_OUT_ALREADY_USED_IN_CURRENT_TX

##### DAP_LEDGER_TX_CHECK_NO_VERIFICATOR_SET

Verificator is not setted.

##### DAP_LEDGER_TX_CHECK_VERIFICATOR_CHECK_FAILURE

Verificator check failure.

##### DAP_LEDGER_TX_CHECK_PREV_TICKER_NOT_FOUND

Previous ticker not found.

##### DAP_LEDGER_TX_CHECK_PREV_TOKEN_NOT_FOUND

Previous token not found.

##### DAP_LEDGER_PERMISSION_CHECK_FAILED

Permission failed.

##### DAP_LEDGER_TX_CHECK_SUM_INS_NOT_EQUAL_SUM_OUTS

##### DAP_LEDGER_TX_CHECK_REWARD_ITEM_ALREADY_USED

Transaction reward item has alredy been used.

##### DAP_LEDGER_TX_CHECK_REWARD_ITEM_ILLEGAL

Transaction reward item is illegal.

##### DAP_LEDGER_PERMISSION_CHECK_FAILED *= 'DAP_LEDGER_PERMISSION_CHECK_FAILED'*

##### DAP_LEDGER_TX_ALREADY_CACHED *= 'DAP_LEDGER_TX_ALREADY_CACHED'*

##### DAP_LEDGER_TX_CHECK_EMISSION_NOT_FOUND *= 'DAP_LEDGER_TX_CHECK_EMISSION_NOT_FOUND'*

##### DAP_LEDGER_TX_CHECK_INVALID_TX_SIGN *= 'DAP_LEDGER_TX_CHECK_INVALID_TX_SIGN'*

##### DAP_LEDGER_TX_CHECK_INVALID_TX_SIZE *= 'DAP_LEDGER_TX_CHECK_INVALID_TX_SIZE'*

##### DAP_LEDGER_TX_CHECK_IN_EMS_ALREADY_USED *= 'DAP_LEDGER_TX_CHECK_IN_EMS_ALREADY_USED'*

##### DAP_LEDGER_TX_CHECK_MULT256_OVERFLOW_EMS_LOCKED_X_RATE *= 'DAP_LEDGER_TX_CHECK_MULT256_OVERFLOW_EMS_LOCKED_X_RATE'*

##### DAP_LEDGER_TX_CHECK_NO_OUT_EXT_FOR_GIRDLED_IN_EMS *= 'DAP_LEDGER_TX_CHECK_NO_OUT_EXT_FOR_GIRDLED_IN_EMS'*

##### DAP_LEDGER_TX_CHECK_NO_OUT_ITEMS_FOR_BASE_TX *= 'DAP_LEDGER_TX_CHECK_NO_OUT_ITEMS_FOR_BASE_TX'*

##### DAP_LEDGER_TX_CHECK_NO_VERIFICATOR_SET *= 'DAP_LEDGER_TX_CHECK_NO_VERIFICATOR_SET'*

##### DAP_LEDGER_TX_CHECK_NULL_TX *= 'DAP_LEDGER_TX_CHECK_NULL_TX'*

##### DAP_LEDGER_TX_CHECK_OK *= 'DAP_LEDGER_TX_CHECK_OK'*

##### DAP_LEDGER_TX_CHECK_OUT_ITEM_ALREADY_USED *= 'DAP_LEDGER_TX_CHECK_OUT_ITEM_ALREADY_USED'*

##### DAP_LEDGER_TX_CHECK_PKEY_HASHES_DONT_MATCH *= 'DAP_LEDGER_TX_CHECK_PKEY_HASHES_DONT_MATCH'*

##### DAP_LEDGER_TX_CHECK_PREV_OUT_ALREADY_USED_IN_CURRENT_TX *= 'DAP_LEDGER_TX_CHECK_PREV_OUT_ALREADY_USED_IN_CURRENT_TX'*

##### DAP_LEDGER_TX_CHECK_PREV_OUT_ITEM_NOT_FOUND *= 'DAP_LEDGER_TX_CHECK_PREV_OUT_ITEM_NOT_FOUND'*

##### DAP_LEDGER_TX_CHECK_PREV_TICKER_NOT_FOUND *= 'DAP_LEDGER_TX_CHECK_PREV_TICKER_NOT_FOUND'*

##### DAP_LEDGER_TX_CHECK_PREV_TOKEN_NOT_FOUND *= 'DAP_LEDGER_TX_CHECK_PREV_TOKEN_NOT_FOUND'*

##### DAP_LEDGER_TX_CHECK_PREV_TX_NOT_FOUND *= 'DAP_LEDGER_TX_CHECK_PREV_TX_NOT_FOUND'*

##### DAP_LEDGER_TX_CHECK_REWARD_ITEM_ALREADY_USED *= 'DAP_LEDGER_TX_CHECK_REWARD_ITEM_ALREADY_USED'*

##### DAP_LEDGER_TX_CHECK_REWARD_ITEM_ILLEGAL *= 'DAP_LEDGER_TX_CHECK_REWARD_ITEM_ILLEGAL'*

##### DAP_LEDGER_TX_CHECK_STAKE_LOCK_INVALID_TOKEN *= 'DAP_LEDGER_TX_CHECK_STAKE_LOCK_INVALID_TOKEN'*

##### DAP_LEDGER_TX_CHECK_STAKE_LOCK_IN_EMS_ALREADY_USED *= 'DAP_LEDGER_TX_CHECK_STAKE_LOCK_IN_EMS_ALREADY_USED'*

##### DAP_LEDGER_TX_CHECK_STAKE_LOCK_NO_OUT_COND_FOR_IN_EMS *= 'DAP_LEDGER_TX_CHECK_STAKE_LOCK_NO_OUT_COND_FOR_IN_EMS'*

##### DAP_LEDGER_TX_CHECK_STAKE_LOCK_OTHER_TICKER_EXPECTED *= 'DAP_LEDGER_TX_CHECK_STAKE_LOCK_OTHER_TICKER_EXPECTED'*

##### DAP_LEDGER_TX_CHECK_STAKE_LOCK_TICKER_NOT_FOUND *= 'DAP_LEDGER_TX_CHECK_STAKE_LOCK_TICKER_NOT_FOUND'*

##### DAP_LEDGER_TX_CHECK_STAKE_LOCK_UNEXPECTED_VALUE *= 'DAP_LEDGER_TX_CHECK_STAKE_LOCK_UNEXPECTED_VALUE'*

##### DAP_LEDGER_TX_CHECK_SUM_INS_NOT_EQUAL_SUM_OUTS *= 'DAP_LEDGER_TX_CHECK_SUM_INS_NOT_EQUAL_SUM_OUTS'*

##### DAP_LEDGER_TX_CHECK_TICKER_NOT_FOUND *= 'DAP_LEDGER_TX_CHECK_TICKER_NOT_FOUND'*

##### DAP_LEDGER_TX_CHECK_TOKEN_EMS_VALUE_EXEEDS_CUR_SUPPLY *= 'DAP_LEDGER_TX_CHECK_TOKEN_EMS_VALUE_EXEEDS_CUR_SUPPLY'*

##### DAP_LEDGER_TX_CHECK_TX_NO_VALID_INPUTS *= 'DAP_LEDGER_TX_CHECK_TX_NO_VALID_INPUTS'*

##### DAP_LEDGER_TX_CHECK_VERIFICATOR_CHECK_FAILURE *= 'DAP_LEDGER_TX_CHECK_VERIFICATOR_CHECK_FAILURE'*

#### Methods:

##### \_\_format_\_(format_spec)

Returns format using actual value type unless \_\_str_\_ has been overridden.

### *class* pycfhelpers.node.types.CFNetState(value)

Bases: `str`, `Enum`

Enum representing the state of a CFNet.

#### Attributes:

##### NET_STATE_OFFLINE

Network is offline.

##### NET_STATE_ONLINE

Network is online.

##### NET_STATE_OFFLINE *= 'NET_STATE_OFFLINE'*

##### NET_STATE_ONLINE *= 'NET_STATE_ONLINE'*

#### Methods:

##### \_\_format_\_(format_spec)

Returns format using actual value type unless \_\_str_\_ has been overridden.

### *class* pycfhelpers.node.types.TSD(value)

Bases: `Enum`

Enum representing different types of TSD.

#### Attributes:

##### TYPE_UNKNOWN

Unknown type.

##### TYPE_TIMESTAMP

Timestamp type.

##### TYPE_ADDRESS

Address type.

##### TYPE_VALUE

Value type.

##### TYPE_CONTRACT

Contract type.

##### TYPE_NET_ID

Network ID type.

##### TYPE_BLOCK_NUM

Block number type.

##### TYPE_TOKEN_SYM

Token symbol type.

##### TYPE_OUTER_TX_HASH

Transaction output hash type.

##### TYPE_SOURCE

Source type

##### TYPE_SOURCE_SUBTYPE

Source subtype type.

##### TYPE_DATA

Data type.

##### TYPE_SENDER

Sender type.

##### TYPE_TOKEN_ADDRESS

Token adress type.

##### TYPE_SIGNATURS

Signaturs type.

##### TYPE_UNIQUE_ID

Unique ID type.

##### TYPE_BASE_TX_HASH

Base trancaction hash type.

##### TYPE_EMISSION_CENTER_UID

type of the unique identifier of the emission center.

##### TYPE_EMISSION_CENTER_VER

type of the verificator/version of the emission center.

##### TYPE_ADDRESS *= 2*

##### TYPE_BASE_TX_HASH *= 16*

##### TYPE_BLOCK_NUM *= 6*

##### TYPE_CONTRACT *= 4*

##### TYPE_DATA *= 11*

##### TYPE_EMISSION_CENTER_UID *= 17*

##### TYPE_EMISSION_CENTER_VER *= 18*

##### TYPE_NET_ID *= 5*

##### TYPE_OUTER_TX_HASH *= 8*

##### TYPE_SENDER *= 12*

##### TYPE_SIGNATURS *= 14*

##### TYPE_SOURCE *= 9*

##### TYPE_SOURCE_SUBTYPE *= 10*

##### TYPE_TIMESTAMP *= 1*

##### TYPE_TOKEN_ADDRESS *= 13*

##### TYPE_TOKEN_SYM *= 7*

##### TYPE_UNIQUE_ID *= 15*

##### TYPE_UNKNOWN *= 0*

##### TYPE_VALUE *= 3*

## Module contents
