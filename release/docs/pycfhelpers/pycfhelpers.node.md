# pycfhelpers.node package
---

---

## pycfhelpers.node.address module
---

### *class* pycfhelpers.node.address.CFWalletAddress(address: str) ^CFWalletAddress

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
  **address** (*str*) – The wallet address string.

##### net_id()

Get the CellFrame network ID of the wallet address.

* **Returns:** The CellFrame network ID of the wallet address.
* **Return type:** *[[#^CFNetID|CFNetID]]*

## pycfhelpers.node.cli module
---

### *class* pycfhelpers.node.cli.ReplyObject(reply_id: ReplyId) ^ReplyObject

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
  **reply_id** (*ReplyId*) – The unique identifier for the reply.

##### reply(message: str)

Send a reply message.

* **Parameters:**
  **message** (*str*) – The message to be sent as a reply.

### *class* pycfhelpers.node.cli.CliCallback(\*args, \*\*kwargs) ^CliCallback

Bases: Protocol

A protocol for CLI command callbacks.


#### Methods:

##### \_\_call_\_(\*args, reply_object: [[#^ReplyObject|ReplyObject]], \*\*kwargs)

Call method signature for CLI command callbacks.

### *class* pycfhelpers.node.cli.CFCliCommand(command: str, callback: [[#^CliCallback|CliCallback]], help_text: str = '') ^CFCliCommand

The CellFrame CLI command.

Provides methods to register and handle CLI commands.

#### Attributes:

##### command

The CLI command.

* **Type:** *str*

##### callback

The callback function to be executed whe the command is invoked.

* **Type:** *[[#^CliCallback|CliCallback]]*

##### help_text

The help text for the command.

* **Type:** *str*

##### parser

The option parser for the command.

* **Type:** *OptionParser*

#### Methods:

##### \_\_init_\_(command: str, callback: [[#^CliCallback|CliCallback]], help_text: str = '')

Initialize a CFCliCommand object with the given parametrs.

* **Parameters:**
  * **command** (*str*) – The alias for CLI command.
  * **callback** ([[#^CliCallback|CliCallback]]) – The callback function to be executed when the command is invoked.
  * **help_text** (*str* *,* *optional*) – The help text for the command. Defaults to “”.

##### register()

Register the CLI command.

## pycfhelpers.node.consensus module
---

### *class* pycfhelpers.node.consensus.CFBlock(atom: ChainAtomPtr, chain: [[#^CFChain|CFChain]]) ^CFBlock

The CellFrame block.

#### Attributes:

##### chain

The CFChain object associated with the block.

* **Type:** *[[#^CFChain|CFChain]]*

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

* **Type:** *List\[[[#^CFSign|CFSign]]]*

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

##### \_\_init_\_(atom: ChainAtomPtr, chain: [[#^CFChain|CFChain]])

Initialize a CFBlock instance with the given atom and chain.

* **Parameters:**
  * **atom** (*ChainAtomPtr*) – The ChainAtomPtr object
    representing the block.
  * **chain** ([[#^CFChain|CFChain]]) – The CFChain object associated with the block.
* **Raises:**
  * **TypeError** – If the chain type does not contain blocks.
  * **RuntimeError** – If the block is not found from the atom.

##### get_datums()

Get datums associated with the block.

* **Returns:** A list of CFDatum objects associated with the block.
* **Return type:** *list\[[[#^CFDatum|CFDatum]]]*

### *class* pycfhelpers.node.consensus.CFEvent(atom: ChainAtomPtr, chain: [[#^CFChain|CFChain]]) ^CFEvent

The CellFrame event.

Provides methods to work with CellFrame events.

#### Attributes:

##### chain

The CFChain object associated with the event.
* **Type:** *[[#^CFChain|CFChain]]*

##### hash

The hash of the event.

* **Type:** *str*

##### created_at

The time when the event was created.

* **Type:** *datetime*

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

##### \_\_init_\_(atom: ChainAtomPtr, chain: [[#^CFChain|CFChain]])

Initialize a CFEvent object with the given atom and chain.

* **Parameters:**
  * **atom** (*ChainAtomPtr*) – The ChainAtomPtr object representing the event.
  * **chain** ([[#^CFChain|CFChain]]) – The CFChain object associated with the event.
* **Raises:**
  * **TypeError** – If the chain type does not contain events.
  * **RuntimeError** – If the event is not found from the atom.

##### get_datums()

Get datum associated with the event.

* **Returns:** A list of CFDatum object associated with the event. Event has only one Datum.
* **Return type:** *List\[[[#^CFDatum|CFDatum]]]*

## pycfhelpers.node.crypto module
---

### *class* pycfhelpers.node.crypto.CFSign(sign: Sign, net: [[#^CFNet|CFNet]]) ^CFSign

The CellFrame signature.

Provides methods to work with CellFrame signatures.

#### Attributes:

##### net

The CFNet object associated with the signature.
* **Type:** *[[#^CFNet|CFNet]]*

##### type

The type of the signature.

* **Type:** *str*

##### pkey

The public key associated with the signature.

* **Type:** *[[#^CFPkey|CFPkey]]*

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

##### \_\_init_\_(sign: Sign, net: [[#^CFNet|CFNet]])

Initialize a CFSign object with the given sign and net.

* **Parameters:**
  * **sign** (*Sign*) – The Sign object representing the signature.
  * **net** ([[#^CFNet|CFNet]]) – The CFNet object associated with the signature.

##### serialize()

Serialize the signature.

* **Returns:** The serialized signature.
* **Return type:** *bytes*

##### *static* deserialize(data: bytes, net: [[#^CFNet|CFNet]])

Deserialize the signature.
* **Parameters:**
  * **data** (*bytes*) – The serialized signature data.
  * **net** ([[#^CFNet|CFNet]]) – The CFNet object associated with the signature.
* **Returns:** The deserialized CFSign object.
* **Return type:** *[[#^CFSign|CFSign]]*

### *class* pycfhelpers.node.crypto.CFPkey(pkey: DAP.Crypto.Pkey) ^CFPkey

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
  **pkey** (*Pkey*) – The Pkey object representing the public key.

### *class* pycfhelpers.node.crypto.CFKey(key: Key) ^CFKey

The CellFrame key.

Provides methods to work with CellFrame keys..


#### Methods:

##### \_\_init_\_(key: Key)

Initialize a CFKey object with the given key.

* **Parameters:**
  **key** (*Key*) – The Key object.

### *class* pycfhelpers.node.crypto.CFCertificate(certificate_name: str) ^CFCertificate

The CellFrame certificate.

Provides methods to work with CellFrame certificates.

#### Attributes:

##### pkey

The public key associated with the certificate.

* **Type:** *[[#^CFPkey|CFPkey]]*

##### key

The private key associated with the certificate.

* **Type:** *[[#^CFKey|CFKey]]*

##### \_origin_certificate

The origin certificate.

* **Type:** *Cert*

#### Methods:

##### \_\_init_\_(certificate_name: str)

Initialize a CFCertificate object with the given certificate name.

* **Parameters:**
  **certificate_name** (*str*) – The name of the certificate.
* **Raises:**
  **ValueError** – If the certificate with the given name is not found.

##### *property* private_key *: [[#^CFKey|CFKey]] | None*

The private key associated with the certificate.

* **Returns:** The private key, or None if not available.
* **Return type:** *[[#^CFKey|CFKey]] | None*

##### *property* public_key *: [[#^CFPkey|CFPkey]]*

The public key associated with the certificate.
* **Returns:** The public key.
* **Return type:** *[[#^CFPkey|CFPkey]]*

##### delete()

Delete the certificate.

## pycfhelpers.node.datums module
---

### *class* pycfhelpers.node.datums.CFDatum(atom: [[#^CFBlock|CFBlock]] | [[#^CFEvent|CFEvent]] | None, datum: Datum, net: [[#^CFNet|CFNet]] | None = None) ^CFDatum

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

* **Type:** *[[#^CFBlock|CFBlock]] | [[#^CFEvent|CFEvent]] | None*

##### net

The CFNet instance.

* **Type:** *[[#^CFNet|CFNet]] | None*

#### Methods:

##### \_\_init_\_(atom: [[#^CFBlock|CFBlock]] | [[#^CFEvent|CFEvent]] | None, datum: Datum, net: [[#^CFNet|CFNet]] | None = None)

Initialize a new CFDatum instance.

* **Parameters:**
  * **atom** ([[#^CFBlock|CFBlock]] *|* [[#^CFEvent|CFEvent]] *|* *None*) – The parent atom of the datum.
  * **datum** (*Datum*) – The datum object.
  * **net** ([[#^CFNet|CFNet]] *|* *None*) – The CFNet instance. Defaults to None.
* **Raises:**
  **AttributeError** – If atom is None and net is None.

##### get_sub_datum()

Retrieve the sub-datum associated with this datum.

* **Returns:** The sub-datum object.
* **Return type:** *Union\[[[#^CFDatumTX|CFDatumTX]], [[#^CFDatumToken|CFDatumToken]], [[#^CFDatumEmission|CFDatumEmission]], [[#^CFDatumAnchor|CFDatumAnchor]], [[#^CFDatumDecree|CFDatumDecree]], [[#^CFDatumCustom|CFDatumCustom]]]*

##### serialize()

Serialize the datum to bytes.
* **Returns:** The serialized datum.
* **Return type:** *bytes*

##### \_\_repr_\_()

Return a string representation of the datum.
* **Returns:** The string representation of the datum containing
  type and hash.
* **Return type:** *str*

### *class* pycfhelpers.node.datums.CFSubDatum(parent_datum: [[#^CFDatum|CFDatum]] | None, sub_datum: DatumTx | DatumToken | DatumEmission | DatumDecree | DatumAnchor | bytes, net: [[#^CFNet|CFNet]] | None = None) ^CFSubDatum

Represent a sub-datum in CellFrame.

#### Attributes:

##### hash

The hash value of the sub-datum.
* **Type:** *str*

##### type

The type of the sub-datum, defaults to “DEFAULT”

* **Type:** *str*

#### Methods:

##### \_\_init_\_(parent_datum: [[#^CFDatum|CFDatum]] | None, sub_datum: DatumTx | DatumToken | DatumEmission | DatumDecree | DatumAnchor | bytes, net: [[#^CFNet|CFNet]] | None = None)

Initialize a new CFSubDatum instance.

* **Parameters:**
  * **parent_datum** ([[#^CFDatum|CFDatum]] *|* *None*) – The parent datum associated with the sub-datum, or None if there’s no parent datum.
  * **sub_datum** (*Union* *[* *DatumTx* *,* *DatumToken* *,* *DatumEmission* *,* *DatumDecree* *,* *DatumAnchor* *,* *bytes* *]*)
  * **representation.** (*The sub-datum object* *or* *its bytes*)
  * **net** ([[#^CFNet|CFNet]] *|* *None* *,* *optional*) – The CFNet instance representing the network, defaults to None.

##### *property* net *: [[#^CFNet|CFNet]]*

Get the CFNet instance representing the network.

* **Returns:** The CFNet instance.
* **Return type:** *[[#^CFNet|CFNet]]*

### *class* pycfhelpers.node.datums.CFDatumTX(parent_datum: [[#^CFDatum|CFDatum]] | None, sub_datum: DatumTx, net: [[#^CFNet|CFNet]] | None = None) ^CFDatumTX

Bases: [[#^CFSubDatum|CFSubDatum]]

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

* **Type:** *[[#^CFLedgerCacheResponse|CFLedgerCacheResponse]]*

##### accepted

Indicates whether the transaction is accepted or not.

* **Type:** *bool

* **Parameters:**
  **CFSubDatum** ([[#^CFSubDatum|CFSubDatum]]) – a sub-datum in CellFrame*

#### Methods:

##### \_\_init_\_(parent_datum: [[#^CFDatum|CFDatum]] | None, sub_datum: DatumTx, net: [[#^CFNet|CFNet]] | None = None)

Initialize a new CFDatumTX instance.

* **Parameters:**
  * **parent_datum** ([[#^CFDatum|CFDatum]] *|* *None*) – The parent datum associated with the sub-datum, or None if there’s no parent datum.
  * **sub_datum** (*DatumTx*) – The transaction sub-datum object.
  * **net** ([[#^CFNet|CFNet]] *|* *None* *,* *optional*) – The CFNet instance representing the
  * **context** (*network*)
  * **None.** (*defaults to*)

##### *property* items *: List\[[[#^CFItem|CFItem]]]*

Get the list of items associated with this transaction.

* **Returns:** The list of items.
* **Return type:** *List\[[[#^CFItem|CFItem]]]*

##### get_items(filter_type: Type[T] | None = None)

Get the list of items associated with transaction.
* **Parameters:**
  **filter_type** (*Type [T]*  *|* *None* *,* *optional*) – The type to filter items. Defaults to None.
* **Returns:** The list of filtered items.
* **Return type:** *List[T]*

### *class* pycfhelpers.node.datums.CFDatumToken(parent_datum: [[#^CFDatum|CFDatum]] | None, sub_datum: CellFrame.Common.DatumToken) ^CFDatumToken

Bases: [[#^CFSubDatum|CFSubDatum]]

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

* **Type:** *List\[[[#^CFSign|CFSign]]]*

#### Methods:

##### \_\_init_\_(parent_datum: [[#^CFDatum|CFDatum]] | None, sub_datum: CellFrame.Common.DatumToken)

Initialize a new CFDatumToken instance.

* **Parameters:**
  * **parent_datum** ([[#^CFDatum|CFDatum]] *|* *None*) – The parent datum associated with the sub-datum, or None if there’s no parent datum.
  * **sub_datum** (*DatumToken*) – The token sub-datum object.

### *class* pycfhelpers.node.datums.CFDatumEmission(parent_datum: [[#^CFDatum|CFDatum]] | None, sub_datum: DatumEmission, net: [[#^CFNet|CFNet]] | None = None) ^CFDatumEmission

Bases: [[#^CFSubDatum|CFSubDatum]]

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

* **Type:** *List[Union\[[[#^CFSign|CFSign]], dict, str]]*

##### tsd

Typed section data associated with the emission, containing various information in a structured format.

* **Type:** *dict[str, Union\[str, dict[str, Any]]]*

#### Methods:

##### \_\_init_\_(parent_datum: [[#^CFDatum|CFDatum]] | None, sub_datum: DatumEmission, net: [[#^CFNet|CFNet]] | None = None)

Initialize a new CFDatumEmission instance.

* **Parameters:**
  * **parent_datum** ([[#^CFDatum|CFDatum]] *|* *None*) – The parent datum associated with the sub-datum, or None if there’s no parent datum.
  * **sub_datum** (*DatumEmission*) – The emission sub-datum object.
  * **net** ([[#^CFNet|CFNet]] *|* *None* *,* *optional*) – The CFNet instance representing the network context, defaults to None.

##### *property* signs *: List\[[[#^CFSign|CFSign]]]*

Get the list of signatures associated with the emission.

* **Returns:** The list of signatures.
* **Return type:** *List\[[[#^CFSign|CFSign]]]*

##### \_\_repr_\_()

Return a string representation of the datum.
* **Returns:** The string representation of the datum containing type and hash.
* **Return type:** *str*

##### *property* valid_sign_hashes *: List[str]*

Get the list of valid signature hashes associated with the emission.
* **Returns:** The list of valid signature hashes.
* **Return type:** *List[str]*

##### add_sign(certificate: [[#^CFCertificate|CFCertificate]])

Add a cryptographic signature to the emission.
* **Parameters:**
  **certificate** ([[#^CFCertificate|CFCertificate]]) – The certificate containing the signature.
* **Returns:** The cryptographic signature object.
* **Return type:** *[[#^CFSign|CFSign]]*

### *class* pycfhelpers.node.datums.CFDatumDecree(parent_datum: [[#^CFDatum|CFDatum]] | None, sub_datum: CellFrame.Common.DatumDecree) ^CFDatumDecree

Bases: [[#^CFSubDatum|CFSubDatum]]

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

* **Type:** *List\[[[#^CFSign|CFSign]]]*

#### Methods:

##### \_\_init_\_(parent_datum: [[#^CFDatum|CFDatum]] | None, sub_datum: CellFrame.Common.DatumDecree)

Initialize a new CFDatumDecree instance.

* **Parameters:**
  * **parent_datum** ([[#^CFDatum|CFDatum]] *|* *None*) – The parent datum associated with the sub-datum, or None if there’s no parent datum.
  * **sub_datum** (*DatumDecree*) – The decree sub-datum object.

### *class* pycfhelpers.node.datums.CFDatumAnchor(parent_datum: [[#^CFDatum|CFDatum]] | None, sub_datum: CellFrame.Common.DatumAnchor) ^CFDatumAnchor

Bases: [[#^CFSubDatum|CFSubDatum]]

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

* **Type:** *List\[[[#^CFSign|CFSign]]]*

#### Methods:

##### \_\_init_\_(parent_datum: [[#^CFDatum|CFDatum]] | None, sub_datum: CellFrame.Common.DatumAnchor)

Initialize a new CFDatumAnchor instance.

* **Parameters:**
  * **parent_datum** ([[#^CFDatum|CFDatum]] *|* *None*) – The parent datum associated with the sub-datum, or None if there’s no parent datum.
  * **sub_datum** (*DatumAnchor*) – The anchor sub-datum object.

### *class* pycfhelpers.node.datums.CFDatumCustom(parent_datum: [[#^CFDatum|CFDatum]] | None, sub_datum: bytes) ^CFDatumCustom

Bases: [[#^CFSubDatum|CFSubDatum]]

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

##### \_\_init_\_(parent_datum: [[#^CFDatum|CFDatum]] | None, sub_datum: bytes)

Initialize a new CFDatumCustom instance.

* **Parameters:**
  * **parent_datum** ([[#^CFDatum|CFDatum]] *|* *None*) – The parent datum associated with the sub-datum, or None if there’s no parent datum.
  * **sub_datum** (*bytes*) – The raw data of the custom sub-datum.

## pycfhelpers.node.gdb module
---

### *class* pycfhelpers.node.gdb.GDBGroupBase(group) ^GDBGroupBase

Bases: ABC

Abstract base class representing a group in the GlobalDB.

#### Attributes:

##### group

The name of the group.

* **Type:** *str*

#### Methods:

##### \_\_init_\_(group)

Initialize a new GDBGroupBase instance.

* **Parameters:**
  **group** (*str*) – The name of the group.

##### *abstract* set(key: str, value: Any)

Set the value associated with the given key in the group.

* **Parameters:**
  * **key** (*str*) – The key.
  * **value** (*Any*) – The value.
* **Returns:** True if successful, False otherwise.
* **Return type:** *bool*

##### *abstract* get(key: str, net: [[#^CFNet|CFNet]], default=None)

Retrieve the value associated with the given key from the group.
* **Parameters:**
  * **key** (*str*) – The key.
  * **net** ([[#^CFNet|CFNet]]) – The CFNet instance.
  * **default** (*Any* *|* *None*) – The default value to return if the
  * **None.** (*key is not found. Defaults to*)
* **Returns:** The value associated with the key, or the default value if the key is not found.
* **Return type:** *Any*

##### *abstract* delete(key: str)

Delete the value associated with the given key from the group.
* **Parameters:**
  **key** (*str*) – The key to delete.
* **Returns:** True if successful, False otherwise.
* **Return type:** *bool*

##### *abstract* group_list()

Retrieve the list of key-value pairs in the group.
* **Returns:** The list of key-value pairs.
* **Return type:** *list[dict[str, Any]]*

##### *static* add_to_white_list(net_name: str, group_mask: str, callback: Callable, callback_args: list)

Add the specified group to the GlobalDB white list.
* **Parameters:**
  * **net_name** (*str*) – The name of the network.
  * **group_mask** (*str*) – The group mask pattern.
    Example: “raiden.emission-center.\*”
  * **callback** (*Callable*) – The callback function.
  * **callback_args** (*list*) – The list of callback arguments.
* **Returns:** None

### *class* pycfhelpers.node.gdb.GDBGroup(group) ^GDBGroup

Bases: [[#^GDBGroupBase|GDBGroupBase]]

Represents a group in the GlobalDB.

#### Attributes:

##### group

The name of the group.
* **Type:** *str*

#### Methods:

##### set(key: str, value: bytes)

Set the value associated with the given key in the group.

* **Parameters:**
  * **key** (*str*) – The key.
  * **value** (*bytes*) – The value.
* **Returns:** True if successful, False otherwise.
* **Return type:** *bool*

##### get(key: str, net: [[#^CFNet|CFNet]] | None = None, default: D | None = None)

Retrieve the value associated with the given key from the group.
* **Parameters:**
  * **key** (*str*) – The key.
  * **net** ([[#^CFNet|CFNet]] *|* *None*) – The CFNet instance. Defaults to None.
  * **default** (*D* *|* *None*) – The default value to return if the key is not found. Defaults to None.
* **Returns:** The value associated with the key, or the default value
  if the key is not found.
* **Return type:** *bytes | D*

##### delete(key: str)

Delete the value associated with the given key from the group.
* **Parameters:**
  **key** (*str*) – The key to delete.
* **Returns:** True if successfull, overwise False.
* **Return type:** *bool*

##### group_list()

Retrieve the list of key-value pairs in the group.
* **Returns:** The list of key-value pairs.
* **Return type:** *list[dict[str, bytes]]*

### *class* pycfhelpers.node.gdb.SerializableObject(\*args, \*\*kwargs) ^SerializableObject

Bases: Protocol

Protocol representing a serializable object.


#### Methods:

##### serialize()

Serialize the object to bytes.
* **Returns:** The serialized object.
* **Return type:** *bytes*

##### deserialize()

Deserialize the object.
* **Returns:** The deserialized object.
* **Return type:** *object*

### *class* pycfhelpers.node.gdb.GDBGroupSign(group) ^GDBGroupSign

Bases: [[#^GDBGroupBase|GDBGroupBase]]

Represents a group in the GlobalDB for signed data.

#### Attributes:

##### group

The name of the group.
* **Type:** *str*

#### Methods:

##### set(key: str, value: [[#^SerializableObject|SerializableObject]])

Set the value associated with the given key in the group.

* **Parameters:**
  * **key** (*str*) – The key.
  * **value** ([[#^SerializableObject|SerializableObject]]) – The value.

##### get(key: str, net: [[#^CFNet|CFNet]], default: D | None = None)

Retrieve the value associated with the given key from the group.

* **Parameters:**
  * **key** (*str*) – The key.
  * **net** ([[#^CFNet|CFNet]]) – The CFNet instance.
  * **default** (*D* *|* *None*) – The default value to return if the key is not found. Defaults to None.
* **Returns:** The value associated with the key, or
  the default value if the key is not found.
* **Return type:** *[[#^SerializableObject|SerializableObject]] | D*

##### delete(key: str)

Delete the value associated with the given key from the group.
* **Parameters:**
  **key** (*str*) – The key to delete.
* **Returns:** True if successful, False otherwise.
* **Return type:** *bool*

##### group_list()

Retrieve the list of key-value pairs in the group.
* **Returns:** The list of key-value pairs.
* **Return type:** *list[dict[str, bytes]]*

## pycfhelpers.node.items module
---

### *class* pycfhelpers.node.items.CFItem(origin_item: OriginItemTypes, type: [ItemTypes](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes), net: [[#^CFNet|CFNet]]) ^CFItem

The item in CellFrame.

#### Attributes:

##### type

The type of the item.
* **Type:** *str*

##### net

The CFNet instance.

* **Type:** *[[#^CFNet|CFNet]]*

#### Methods:

##### \_\_init_\_(origin_item: OriginItemTypes, type: [ItemTypes](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes), net: [[#^CFNet|CFNet]])

Initialize a new CFItem instance.

* **Parameters:**
  * **origin_item** (*OriginItemTypes*) – The original item.
  * **type** ([*ItemTypes*](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes)) – The type of the item.
  * **net** ([[#^CFNet|CFNet]]) – The CFNet instance.

### *class* pycfhelpers.node.items.CFTxIn(\*args, \*\*kwargs) ^CFTxIn

Bases: [[#^CFItem|CFItem]]

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

### *class* pycfhelpers.node.items.CFTxInCond(origin_item: OriginItemTypes, type: [ItemTypes](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes), net: [[#^CFNet|CFNet]]) ^CFTxInCond

Bases: [[#^CFItem|CFItem]]

The conditional transaction input in CellFrame.

### *class* pycfhelpers.node.items.CFTxOut(\*args, \*\*kwargs) ^CFTxOut

Bases: [[#^CFItem|CFItem]]

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

### *class* pycfhelpers.node.items.CFTxOutCond(\*args, \*\*kwargs) ^CFTxOutCond

Bases: [[#^CFItem|CFItem]]

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

### *class* pycfhelpers.node.items.CFTxPkey(origin_item: OriginItemTypes, type: [ItemTypes](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes), net: [[#^CFNet|CFNet]]) ^CFTxPkey

Bases: [[#^CFItem|CFItem]]

The transaction public key in CellFrame.

### *class* pycfhelpers.node.items.CFTxSig(\*args, \*\*kwargs) ^CFTxSig

Bases: [[#^CFItem|CFItem]]

The transaction signature in CellFrame.

#### Attributes:

##### size

The size of the signature.

* **Type:** *int*

##### sign

The cryptographic signature.

* **Type:** *[[#^CFSign|CFSign]]*

#### Methods:

##### \_\_init_\_(\*args, \*\*kwargs)

Initialize a new CFTxSig instance.

### *class* pycfhelpers.node.items.CFTxToken(\*args, \*\*kwargs) ^CFTxToken

Bases: [[#^CFItem|CFItem]]

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

### *class* pycfhelpers.node.items.CFTxReceipt(origin_item: OriginItemTypes, type: [ItemTypes](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes), net: [[#^CFNet|CFNet]]) ^CFTxReceipt

Bases: [[#^CFItem|CFItem]]

The transaction receipt in CellFrame.

### *class* pycfhelpers.node.items.CFTxOutExt(\*args, \*\*kwargs) ^CFTxOutExt

Bases: [[#^CFItem|CFItem]]

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

### *class* pycfhelpers.node.items.CFTxTSD(origin_item: OriginItemTypes, type: [ItemTypes](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes), net: [[#^CFNet|CFNet]]) ^CFTxTSD

Bases: [[#^CFItem|CFItem]]

The transaction TSD  in CellFrame.

### *class* pycfhelpers.node.items.CFTxOutCondSubtypeSrvPay(origin_item: OriginItemTypes, type: [ItemTypes](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes), net: [[#^CFNet|CFNet]]) ^CFTxOutCondSubtypeSrvPay

Bases: [[#^CFItem|CFItem]]

The server payment subtype of a conditional transaction output in CellFrame.

### *class* pycfhelpers.node.items.CFTxOutCondSubtypeSrvXchange(origin_item: OriginItemTypes, type: [ItemTypes](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes), net: [[#^CFNet|CFNet]]) ^CFTxOutCondSubtypeSrvXchange

Bases: [[#^CFItem|CFItem]]

The server exchange subtype of a conditional transaction output in CellFrame.

### *class* pycfhelpers.node.items.CFTxOutCondSubtypeSrvStakeLock(\*args, \*\*kwargs) ^CFTxOutCondSubtypeSrvStakeLock

Bases: [[#^CFItem|CFItem]]

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

### *class* pycfhelpers.node.items.CFTxOutCondSubtypeSrvStakePosDelegate(origin_item: OriginItemTypes, type: [ItemTypes](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes), net: [[#^CFNet|CFNet]]) ^CFTxOutCondSubtypeSrvStakePosDelegate

Bases: [[#^CFItem|CFItem]]

The server stake pos delegate subtype of a conditional transaction output in CellFrame.

### *class* pycfhelpers.node.items.CFTxVoting(origin_item: OriginItemTypes, type: [ItemTypes](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes), net: [[#^CFNet|CFNet]]) ^CFTxVoting

Bases: [[#^CFItem|CFItem]]

The transaction voting in CellFrame.

### *class* pycfhelpers.node.items.CFTxVote(origin_item: OriginItemTypes, type: [ItemTypes](pycfhelpers.common.md#pycfhelpers.common.types.ItemTypes), net: [[#^CFNet|CFNet]]) ^CFTxVote

Bases: [[#^CFItem|CFItem]]

The transaction vote in CellFrame.

## pycfhelpers.node.logging module
---

### *class* pycfhelpers.node.logging.CFLog ^CFLog

Provides logging functionality for CellFrame.

#### Attributes:

##### \_logger

The logger instance.

#### Methods:

##### \_\_init_\_()

Initialize a new CFLog instance.

##### debug(message: str)

Log a message with the DEBUG level.

* **Parameters:**
  **message** (*str*) – The debug message.

##### info(message: str)

Log a message with the INFO level

* **Parameters:**
  **message** (*str*) – The informational message.

##### notice(message: str)

Log a message with the NOTICE level.

* **Parameters:**
  **message** (*str*) – The notice message.

##### message(message: str)

Log a message with the MESSAGE level

* **Parameters:**
  **message** (*str*) – The generic message.

##### dap(message: str)

Log a message with the DAP level

* **Parameters:**
  **message** (*str*) – The DAP-specific message.

##### warning(message: str)

Log a message with the WARNING level.

* **Parameters:**
  **message** (*str*) – The warning message.

##### att(message: str)

Log a message with the ATTENTION level.

* **Parameters:**
  **message** (*str*) – The attention message.

##### error(message: str)

Log a message with the ERROR level.

* **Parameters:**
  **message** (*str*) – The error message.

##### critical(message: str)

Log a message with the CRITICAL level

* **Parameters:**
  **message** (*str*) – The critical error message.

## pycfhelpers.node.mappings module
---

### pycfhelpers.node.mappings.ItemMeta

alias of `ItemMapping`

### *class* pycfhelpers.node.mappings.CFItemMapper ^CFItemMapper

Maps item types to corresponding CellFrame item wrapper classes.


#### Methods:

##### *static* build(origin_item, net)

Build a CellFrame item based on the provided origin item.

* **Parameters:**
  * **origin_item** – The original item.
  * **net** – The CFNet instance representing the network context.
* **Returns:** The CellFrame item.
* **Return type:** *[[#^CFItem|CFItem]]*

### pycfhelpers.node.mappings.SubDatumMeta

alias of `SubDatumMapping`

### *class* pycfhelpers.node.mappings.CFSubDatumBuilder(type) ^CFSubDatumBuilder

Build sub-datums for CellFrame based on datum type.


#### Methods:

##### \_\_init_\_(type)

Initialize a new CFSubDatumBuilder instance.
* **Parameters:**
  **type** ([*DatumTypes*](pycfhelpers.common.md#pycfhelpers.common.types.DatumTypes)) – The type of datum for which sub-datum is being built. Should be one of the enumeration values from DatumTypes.

##### build(parent_datum)

Build a sub-datum for CellFrame based on the parent datum.

* **Parameters:**
  **parent_datum** – The parent datum.
* **Returns:** The sub-datum.
* **Return type:** *[[#^CFDatum|CFDatum]]*

## pycfhelpers.node.math module
---

## pycfhelpers.node.net module
---

### *class* pycfhelpers.node.net.NetFee(net: [[#^CFNet|CFNet]]) ^NetFee

Represents network fee data.

#### Attributes:

##### net

The CFNet object.
* **Type:** *[[#^CFNet|CFNet]]*

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

##### \_\_init_\_(net: [[#^CFNet|CFNet]])

Initialize NetFee object.

* **Parameters:**
  **net** ([[#^CFNet|CFNet]]) – The CFNet object.

### *class* pycfhelpers.node.net.CFNetID(str_or_origin: str | CellFrame.Network.NetID) ^CFNetID

Represents the ID of a CellFrame network.

#### Attributes:

##### \_origin_net_id

The original NetID object.

* **Type:** *NetID*

#### Methods:

##### \_\_init_\_(str_or_origin: str | CellFrame.Network.NetID)

Initialize CFNetID object.

* **Parameters:**
  **str_or_origin** (*Union* *[* *str* *,* *NetID* *]*) – The string or NetID object.

##### *property* long

Return the long value of CFNetID.

* **Returns:** The long value.
* **Return type:** *int*

### *class* pycfhelpers.node.net.CFNet(name_or_id: str | [[#^CFNetID|CFNetID]]) ^CFNet

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

* **Type:** *[[#^CFChain|CFChain]]*

##### zerochain

The zerochain of the network.

* **Type:** *[[#^CFChain|CFChain]]*

##### fee_data

The fee data of the network.

* **Type:** *[[#^NetFee|NetFee]]*

#### Methods:

##### \_\_init_\_(name_or_id: str | [[#^CFNetID|CFNetID]])

Initialize CFNet object.

* **Parameters:**
  **name_or_id** (*str* *|* [[#^CFNetID|CFNetID]]) – The name or CFNetID object.
* **Raises:**
  **RuntimeError** – If there is no network with such name and ID.

##### *static* active_nets()

Return the list of active CFNet objects.

* **Returns:** The list of CFNet objects.
* **Return type:** *List\[[[#^CFNet|CFNet]]]*

##### *static* net_id_from_wallet_str(wallet: str)

Return the net ID from the wallet string.
* **Parameters:**
  **wallet** (*str*) – The wallet string.
* **Returns:** The net ID.
* **Return type:** *int*

##### *property* id *: [[#^CFNetID|CFNetID]]*

Return the CFNetID object.
* **Returns:** The CFNetID object.
* **Return type:** *[[#^CFNetID|CFNetID]]*

##### *property* chains *: list\[[[#^CFChain|CFChain]]]*

Return the list of CFChain objects.
* **Returns:** The list of CFChain objects containing mainchain and zerochain.
* **Return type:** *List\[[[#^CFChain|CFChain]]]*

##### *property* group_alias *: str*

Return the group alias.
* **Returns:** The group alias.
* **Return type:** *str*

##### get_ledger()

Return the CFLedger object.
* **Returns:** The CFLedger object.
* **Return type:** *[[#^CFLedger|CFLedger]]*

##### register_gdbsync_notification_callback(callback: Callable, \*args, \*\*kwargs)

Register the gdbsync notification callback.
* **Parameters:**
  **callback** (*Callable*) – The callback function.

##### change_state(state: [[#^CFNetState|CFNetState]])

Change the state of CFNet.

* **Parameters:**
  **state** ([[#^CFNetState|CFNetState]]) – The state to change to.
* **Raises:**
  **NotImplemented** – If there is no network with such name and ID.

### *class* pycfhelpers.node.net.MempoolCallback(\*args, \*\*kwargs) ^MempoolCallback

Bases: Protocol

Represents the protocol for mempool callback functions.


#### Methods:

##### \_\_call_\_(op_code: Literal['a', 'd'], datum: [[#^CFDatum|CFDatum]] | datum_hash, \*args, chain: [[#^CFChain|CFChain]], \*\*kwargs)

Call the mempool callback function.

* **Parameters:**
  * **op_code** (*Literal* *[* *"a"* *,*  *"d"* *]*) – The operation code.
  * **datum** (*Union* *[*[[#^CFDatum|CFDatum]] *,* *datum_hash* *]*) – The datum or datum hash.
  * **chain** ([[#^CFChain|CFChain]]) – The CFChain object.

### *class* pycfhelpers.node.net.CFChain(net: [[#^CFNet|CFNet]], chain_name: str) ^CFChain

Represents a CellFrame chain.

#### Attributes:

##### net

The CFNet object.

* **Type:** *[[#^CFNet|CFNet]]*

##### type

The type of the chain.

* **Type:** *str*

##### name

The name of the chain.

* **Type:** *str*

#### Methods:

##### \_\_init_\_(net: [[#^CFNet|CFNet]], chain_name: str)

Initialize CFChain object.

* **Parameters:**
  * **net** ([[#^CFNet|CFNet]]) – The CFNet object.
  * **chain_name** (*str*) – The name of the chain.
* **Raises:**
  **RuntimeError** – If there is no net with specified chain name.

##### get_atoms()

Return an iterator over CFBlock or CFEvent objects (atoms) contained in the chain.

* **Yields:**
  *Iterator[CFBlock | CFEvent]* – An iterator over CFBlock or CFEvent objects.
* **Raises:**
  **TypeError** – If Chain type neither ChainTypes.esbocs nor ChainTypes.dag_poa

##### get_datums(type: [[#^CFSubDatum|CFSubDatum]] | None = None)

Return an iterator over CFDatum objects (datums) in the chain, optionally filtered by type.

* **Parameters:**
  **type** ([[#^CFSubDatum|CFSubDatum]] *|* *None*) – The subdatum type to filter by.
* **Yields:**
  *Iterator[CFDatum]* – An iterator over CFDatum objects.

##### get_transactions()

Return an iterator over CFDatumTX objects representing transactions in the chain.

* **Yields:**
  *Iterator[CFDatumTX]* – An iterator over CFDatumTX objects.

##### get_mempool()

Return the CFMempool object associated with the chain.

* **Returns:** The CFMempool object.
* **Return type:** *[[#^CFMempool|CFMempool]]*

##### register_mempool_notification_callback(callback: [[#^MempoolCallback|MempoolCallback]], \*args, \*\*kwargs)

Register a callback function to be invoked when there are changes in the mempool.
* **Parameters:**
  **callback** ([[#^MempoolCallback|MempoolCallback]]) – The callback function.

##### register_atom_notification_callback(callback, \*args, \*\*kwargs)

Register a callback function to be invoked when there are changes in the chain’s atoms.

* **Parameters:**
  **callback** – The callback function.

### *class* pycfhelpers.node.net.CFLedger(net: [[#^CFNet|CFNet]], ledger: CellFrame.Chain.Ledger | None = None) ^CFLedger

Represents a CellFrame ledger.

#### Attributes:

##### net

The CFNet object.

* **Type:** *[[#^CFNet|CFNet]]*

##### \_origin_ledger

The original ledger object.

* **Type:** *Ledger*

#### Methods:

##### \_\_init_\_(net: [[#^CFNet|CFNet]], ledger: CellFrame.Chain.Ledger | None = None)

Initialize CFLedger object.

* **Parameters:**
  * **net** ([[#^CFNet|CFNet]]) – The CFNet object.
  * **ledger** (*Ledger* *|* *None*) – The original ledger object. Defaults to None.

##### get_tx_ticker(datum: [[#^CFDatumTX|CFDatumTX]])

Return the transaction ticker.

* **Parameters:**
  **datum** ([[#^CFDatumTX|CFDatumTX]]) – The CFDatumTX object.
* **Returns:** The transaction ticker or None if not found.
* **Return type:** *ticker | None*

##### get_tx_ledger_rc(datum: [[#^CFDatumTX|CFDatumTX]])

Return the transaction ledger Rc.
* **Parameters:**
  **datum** ([[#^CFDatumTX|CFDatumTX]]) – The CFDatumTX object.
* **Returns:** The transaction ledger RC or None if not found.
* **Return type:** *ledger_cache_rc | None*

##### calc_address_balances(address: str)

Calculate the balances of an address.
* **Parameters:**
  **address** (*str*) – The address.
* **Returns:** A dictionary containing tickers and balances.
* **Return type:** *dict[ticker, str]*

##### token_auth_signs_pkey_hashes(datum: [[#^CFDatum|CFDatum]])

Return the public keys hashes of token declaration for provided emission.
* **Parameters:**
  **datum** ([[#^CFDatum|CFDatum]]) – The CFDatum object.
* **Returns:** A list of public key hashes.
* **Return type:** *List[str]*

##### token_auth_signs_valid(datum: [[#^CFDatum|CFDatum]])

Return the number of valid signature signs.
* **Parameters:**
  **datum** ([[#^CFDatum|CFDatum]]) – The CFDatum object.
* **Returns:** The number of valid signature signs.
* **Return type:** *int*

##### token_auth_signs_total(datum: [[#^CFDatum|CFDatum]])

Return the total number of signature signs.
* **Parameters:**
  **datum** ([[#^CFDatum|CFDatum]]) – The CFDatum object.
* **Returns:** The total number of signature signs.
* **Return type:** *int*

##### has_emission(emission: [[#^CFDatumEmission|CFDatumEmission]])

Check if an emission exists in the ledger.
* **Parameters:**
  **emission** ([[#^CFDatumEmission|CFDatumEmission]]) – The CFDatumEmission object.
* **Returns:** True if the emission exists, False otherwise.
* **Return type:** *bool*

##### get_emission(emission_hash: str)

Return the emission from the ledger.
* **Parameters:**
  **emission_hash** (*str*) – The hash of the emission.
* **Returns:** The CFDatumEmission object.
* **Return type:** *[[#^CFDatumEmission|CFDatumEmission]]*

##### tx_by_hash(hash: str)

Return the transaction by its hash.
* **Parameters:**
  **hash** (*str*) – The hash of the transaction.
* **Returns:** The CFDatumTX object.
* **Return type:** *[[#^CFDatumTX|CFDatumTX]]
* **Raises:**
  **ValueError** – If the transaction with the specified hash is not found.*

##### register_ledger_tx_notification_callback(callback, \*args, \*\*kwargs)

Register the ledger transaction notification callback.
* **Parameters:**
  **callback** – The callback function.

### *class* pycfhelpers.node.net.CFMempool(chain: [[#^CFChain|CFChain]]) ^CFMempool

Represents a CellFrame mempool.

#### Attributes:

##### chain

The CFChain object.

* **Type:** *[[#^CFChain|CFChain]]*

#### Methods:

##### \_\_init_\_(chain: [[#^CFChain|CFChain]])

Initialize CFMempool object.

* **Parameters:**
  **chain** ([[#^CFChain|CFChain]]) – The CFChain object.

##### get_datums()

Return a list of CFDatum objects in the mempool.

* **Returns:** A list of CFDatum objects.
* **Return type:** *List\[[[#^CFDatum|CFDatum]]]*

##### get_datum_from_bytes(value: bytes)

Return a CFDatum object from bytes.
* **Parameters:**
  **value** (*bytes*) – The bytes value.
* **Returns:** The CFDatum object or None if not found.
* **Return type:** *[[#^CFDatum|CFDatum]] | None*

##### valid_signs_table()

##### reason()

##### remove(datum: [[#^CFDatum|CFDatum]] | [[#^CFSubDatum|CFSubDatum]])

Remove a datum from the mempool.
* **Parameters:**
  **datum** ([[#^CFDatum|CFDatum]] *|* [[#^CFSubDatum|CFSubDatum]]) – The CFDatum or CFSubDatum object.
* **Returns:** True if removal is successful, False otherwise.
* **Return type:** *bool*

## pycfhelpers.node.notificators module
---

### *class* pycfhelpers.node.notificators.CFNets(nets: list\[[[#^CFNet|CFNet]]]) ^CFNets

Represent a set of CFNet objects.

#### Attributes:

##### nets

The list of CFNet objects.
* **Type:** *List\[[[#^CFNet|CFNet]]]*

#### Methods:

##### \_\_init_\_(nets: list\[[[#^CFNet|CFNet]]])

Initialize CFNets object.

* **Parameters:**
  **nets** (*List* *[*[[#^CFNet|CFNet]] *]*) – The list of CFNet objects.

##### mempool_notificator(\*args, chain_name: Literal['main', 'zerochain', 'all'] = 'all', \*\*kwargs)

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

#### atom_notificator(\*args, chain_name: Literal['main', 'zerochain', 'all'] = 'all', \*\*kwargs)

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

## pycfhelpers.node.types module
---

### *class* pycfhelpers.node.types.CFLedgerCacheResponse(value) ^CFLedgerCacheResponse

Bases: str, Enum

An enumeration.

#### Attributes:

##### DAP_LEDGER_TX_CHECK_OK *= 'DAP_LEDGER_TX_CHECK_OK'*

##### DAP_LEDGER_TX_CHECK_NULL_TX *= 'DAP_LEDGER_TX_CHECK_NULL_TX'*

##### DAP_LEDGER_TX_CHECK_INVALID_TX_SIZE *= 'DAP_LEDGER_TX_CHECK_INVALID_TX_SIZE'*

##### DAP_LEDGER_TX_ALREADY_CACHED *= 'DAP_LEDGER_TX_ALREADY_CACHED'*

##### DAP_LEDGER_TX_CHECK_INVALID_TX_SIGN *= 'DAP_LEDGER_TX_CHECK_INVALID_TX_SIGN'*

##### DAP_LEDGER_TX_CHECK_IN_EMS_ALREADY_USED *= 'DAP_LEDGER_TX_CHECK_IN_EMS_ALREADY_USED'*

##### DAP_LEDGER_TX_CHECK_STAKE_LOCK_IN_EMS_ALREADY_USED *= 'DAP_LEDGER_TX_CHECK_STAKE_LOCK_IN_EMS_ALREADY_USED'*

##### DAP_LEDGER_TX_CHECK_EMISSION_NOT_FOUND *= 'DAP_LEDGER_TX_CHECK_EMISSION_NOT_FOUND'*

##### DAP_LEDGER_TX_CHECK_TX_NO_VALID_INPUTS *= 'DAP_LEDGER_TX_CHECK_TX_NO_VALID_INPUTS'*

##### DAP_LEDGER_TX_CHECK_TICKER_NOT_FOUND *= 'DAP_LEDGER_TX_CHECK_TICKER_NOT_FOUND'*

##### DAP_LEDGER_TX_CHECK_STAKE_LOCK_INVALID_TOKEN *= 'DAP_LEDGER_TX_CHECK_STAKE_LOCK_INVALID_TOKEN'*

##### DAP_LEDGER_TX_CHECK_STAKE_LOCK_NO_OUT_COND_FOR_IN_EMS *= 'DAP_LEDGER_TX_CHECK_STAKE_LOCK_NO_OUT_COND_FOR_IN_EMS'*

##### DAP_LEDGER_TX_CHECK_MULT256_OVERFLOW_EMS_LOCKED_X_RATE *= 'DAP_LEDGER_TX_CHECK_MULT256_OVERFLOW_EMS_LOCKED_X_RATE'*

##### DAP_LEDGER_TX_CHECK_NO_OUT_EXT_FOR_GIRDLED_IN_EMS *= 'DAP_LEDGER_TX_CHECK_NO_OUT_EXT_FOR_GIRDLED_IN_EMS'*

##### DAP_LEDGER_TX_CHECK_NO_OUT_ITEMS_FOR_BASE_TX *= 'DAP_LEDGER_TX_CHECK_NO_OUT_ITEMS_FOR_BASE_TX'*

##### DAP_LEDGER_TX_CHECK_TOKEN_EMS_VALUE_EXEEDS_CUR_SUPPLY *= 'DAP_LEDGER_TX_CHECK_TOKEN_EMS_VALUE_EXEEDS_CUR_SUPPLY'*

##### DAP_LEDGER_TX_CHECK_STAKE_LOCK_UNEXPECTED_VALUE *= 'DAP_LEDGER_TX_CHECK_STAKE_LOCK_UNEXPECTED_VALUE'*

##### DAP_LEDGER_TX_CHECK_STAKE_LOCK_TICKER_NOT_FOUND *= 'DAP_LEDGER_TX_CHECK_STAKE_LOCK_TICKER_NOT_FOUND'*

##### DAP_LEDGER_TX_CHECK_STAKE_LOCK_OTHER_TICKER_EXPECTED *= 'DAP_LEDGER_TX_CHECK_STAKE_LOCK_OTHER_TICKER_EXPECTED'*

##### DAP_LEDGER_TX_CHECK_OUT_ITEM_ALREADY_USED *= 'DAP_LEDGER_TX_CHECK_OUT_ITEM_ALREADY_USED'*

##### DAP_LEDGER_TX_CHECK_PREV_TX_NOT_FOUND *= 'DAP_LEDGER_TX_CHECK_PREV_TX_NOT_FOUND'*

##### DAP_LEDGER_TX_CHECK_PREV_OUT_ITEM_NOT_FOUND *= 'DAP_LEDGER_TX_CHECK_PREV_OUT_ITEM_NOT_FOUND'*

##### DAP_LEDGER_TX_CHECK_PKEY_HASHES_DONT_MATCH *= 'DAP_LEDGER_TX_CHECK_PKEY_HASHES_DONT_MATCH'*

##### DAP_LEDGER_TX_CHECK_PREV_OUT_ALREADY_USED_IN_CURRENT_TX *= 'DAP_LEDGER_TX_CHECK_PREV_OUT_ALREADY_USED_IN_CURRENT_TX'*

##### DAP_LEDGER_TX_CHECK_NO_VERIFICATOR_SET *= 'DAP_LEDGER_TX_CHECK_NO_VERIFICATOR_SET'*

##### DAP_LEDGER_TX_CHECK_VERIFICATOR_CHECK_FAILURE *= 'DAP_LEDGER_TX_CHECK_VERIFICATOR_CHECK_FAILURE'*

##### DAP_LEDGER_TX_CHECK_PREV_TICKER_NOT_FOUND *= 'DAP_LEDGER_TX_CHECK_PREV_TICKER_NOT_FOUND'*

##### DAP_LEDGER_TX_CHECK_PREV_TOKEN_NOT_FOUND *= 'DAP_LEDGER_TX_CHECK_PREV_TOKEN_NOT_FOUND'*

##### DAP_LEDGER_PERMISSION_CHECK_FAILED *= 'DAP_LEDGER_PERMISSION_CHECK_FAILED'*

##### DAP_LEDGER_TX_CHECK_SUM_INS_NOT_EQUAL_SUM_OUTS *= 'DAP_LEDGER_TX_CHECK_SUM_INS_NOT_EQUAL_SUM_OUTS'*

##### DAP_LEDGER_TX_CHECK_REWARD_ITEM_ALREADY_USED *= 'DAP_LEDGER_TX_CHECK_REWARD_ITEM_ALREADY_USED'*

##### DAP_LEDGER_TX_CHECK_REWARD_ITEM_ILLEGAL *= 'DAP_LEDGER_TX_CHECK_REWARD_ITEM_ILLEGAL'*

#### Methods:

##### \_\_format_\_(format_spec)

Returns format using actual value type unless \_\_str_\_ has been overridden.

### *class* pycfhelpers.node.types.CFNetState(value) ^CFNetState

Bases: str, Enum

An enumeration.

#### Attributes:

##### NET_STATE_OFFLINE *= 'NET_STATE_OFFLINE'*

##### NET_STATE_ONLINE *= 'NET_STATE_ONLINE'*

#### Methods:

##### \_\_format_\_(format_spec)

Returns format using actual value type unless \_\_str_\_ has been overridden.

### *class* pycfhelpers.node.types.TSD(value) ^TSD

Bases: Enum

An enumeration.

#### Attributes:

##### TYPE_UNKNOWN *= 0*

##### TYPE_TIMESTAMP *= 1*

##### TYPE_ADDRESS *= 2*

##### TYPE_VALUE *= 3*

##### TYPE_CONTRACT *= 4*

##### TYPE_NET_ID *= 5*

##### TYPE_BLOCK_NUM *= 6*

##### TYPE_TOKEN_SYM *= 7*

##### TYPE_OUTER_TX_HASH *= 8*

##### TYPE_SOURCE *= 9*

##### TYPE_SOURCE_SUBTYPE *= 10*

##### TYPE_DATA *= 11*

##### TYPE_SENDER *= 12*

##### TYPE_TOKEN_ADDRESS *= 13*

##### TYPE_SIGNATURS *= 14*

##### TYPE_UNIQUE_ID *= 15*

##### TYPE_BASE_TX_HASH *= 16*

##### TYPE_EMISSION_CENTER_UID *= 17*

##### TYPE_EMISSION_CENTER_VER *= 18*


