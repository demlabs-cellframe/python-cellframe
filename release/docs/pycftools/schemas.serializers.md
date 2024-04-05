# schemas.serializers package

## Submodules

## schemas.serializers.block module

### *class* schemas.serializers.block.Block(\*args: Any, \*\*kwargs: Any)

Bases: `SerializeModel`

Represents a block.

#### Attributes:

##### chain

The chain to which the block belongs.

* **Type:** *[Chain](#schemas.serializers.chain.Chain)*

##### hash

The hash of the block.

* **Type:** *str*

##### created_at

The timestamp indicating when the block was created.

* **Type:** *str*

##### version

The version of the block.

* **Type:** *int*

##### cell_id

The ID of the cell associated with the block.

* **Type:** *str*

##### signs

The list of signs associated with the block.

* **Type:** *list[[Sign](#schemas.serializers.sign.Sign)]*

##### meta

Additional metadata associated with the block.

* **Type:** *dict*

##### cell_id *: str*

#### Methods:

##### chain *: [Chain](#schemas.serializers.chain.Chain)*

##### created_at *: str*

##### date_format(date: datetime)

Format the date string.

* **Parameters:**
	* **date** (*datetime*) – The datetime object to format.
* **Returns:** The formatted date string.
* **Return type:** *str*

##### hash *: str*

##### meta *: dict*

##### signs *: list[[Sign](#schemas.serializers.sign.Sign)]*

##### version *: int*

## schemas.serializers.chain module

### *class* schemas.serializers.chain.Chain(\*args: Any, \*\*kwargs: Any)

Bases: `SerializeModel`

Represents a Chain in a blockchain network.

#### Attributes:

##### net

The network to which the chain belongs.
* **Type:** *[Net](#schemas.serializers.net.Net)*

##### name

The name of the chain.

* **Type:** *str*

##### type

The type of the chain.

* **Type:** *str*

##### name *: str*

#### Methods:

##### net *: [Net](#schemas.serializers.net.Net)*

##### type *: str*

## schemas.serializers.datums module

### *class* schemas.serializers.datums.Datum(\*args: Any, \*\*kwargs: Any)

Bases: `SerializeModel`

#### Attributes:

##### created_at *: datetime*

##### hash *: str*

#### Methods:

##### *classmethod* model_validate(\*args, \*\*kwargs)

##### size *: int*

##### sub_datum *: [DatumTX](#schemas.serializers.datums.DatumTX) | [DatumEmission](#schemas.serializers.datums.DatumEmission) | [SubDatum](#schemas.serializers.datums.SubDatum)*

##### type *: str*

##### version *: str*

### *class* schemas.serializers.datums.DatumAnchor(\*args: Any, \*\*kwargs: Any)

Bases: [`SubDatum`](#schemas.serializers.datums.SubDatum)

#### Attributes:

##### created_at *: str*

##### hash *: str*

#### Methods:

##### signs *: list[[Sign](#schemas.serializers.sign.Sign)]*

### *class* schemas.serializers.datums.DatumCorrupted(\*args: Any, \*\*kwargs: Any)

Bases: [`SubDatum`](#schemas.serializers.datums.SubDatum)

#### Attributes:

##### hash *: str*

##### type *: str*

### *class* schemas.serializers.datums.DatumCustom(\*args: Any, \*\*kwargs: Any)

Bases: [`SubDatum`](#schemas.serializers.datums.SubDatum)

#### Attributes:

##### data *: str*

### *class* schemas.serializers.datums.DatumDecree(\*args: Any, \*\*kwargs: Any)

Bases: [`SubDatum`](#schemas.serializers.datums.SubDatum)

#### Attributes:

##### created_at *: str*

##### hash *: str*

#### Methods:

##### signs *: list[[Sign](#schemas.serializers.sign.Sign)]*

##### subtype *: str*

##### type *: str*

### *class* schemas.serializers.datums.DatumEmission(\*args: Any, \*\*kwargs: Any)

Bases: [`SubDatum`](#schemas.serializers.datums.SubDatum)

#### Attributes:

##### address *: str*

#### Methods:

##### data *: dict | list[[Sign](#schemas.serializers.sign.Sign)]*

##### date_format(date: list[str])

##### hash *: str*

##### signs *: list[[Sign](#schemas.serializers.sign.Sign)]*

##### ticker *: str*

##### tsd *: dict*

##### type *: str*

##### value *: str*

##### version *: int*

### *class* schemas.serializers.datums.DatumTX(\*args: Any, \*\*kwargs: Any)

Bases: [`SubDatum`](#schemas.serializers.datums.SubDatum)

#### Attributes:

##### created_at *: datetime*

##### hash *: str*

#### Methods:

##### items *: list[[TxIn](#schemas.serializers.datums.TxIn) | [TxOutExt](#schemas.serializers.datums.TxOutExt) | [TxOut](#schemas.serializers.datums.TxOut) | [TxOutCond](#schemas.serializers.datums.TxOutCond) | [TxSig](#schemas.serializers.datums.TxSig) | [TxToken](#schemas.serializers.datums.TxToken) | [TxOutCondSubTypeSrvStakeLock](#schemas.serializers.datums.TxOutCondSubTypeSrvStakeLock) | [Item](#schemas.serializers.datums.Item)]*

##### *classmethod* model_validate(\*args, \*\*kwargs)

##### ticker *: str | None*

### *class* schemas.serializers.datums.DatumToken(\*args: Any, \*\*kwargs: Any)

Bases: [`SubDatum`](#schemas.serializers.datums.SubDatum)

#### Attributes:

##### data *: dict*

#### Methods:

##### signs *: list[[Sign](#schemas.serializers.sign.Sign)]*

##### ticker *: str*

##### type *: str*

### *class* schemas.serializers.datums.Item(\*args: Any, \*\*kwargs: Any)

Bases: `SerializeModel`

#### Attributes:

##### type *: str*

### *class* schemas.serializers.datums.SubDatum(\*args: Any, \*\*kwargs: Any)

Bases: `SerializeModel`

### *class* schemas.serializers.datums.TxIn(\*args: Any, \*\*kwargs: Any)

Bases: [`Item`](#schemas.serializers.datums.Item)

#### Attributes:

##### prev_hash *: str*

##### prev_idx *: int*

### *class* schemas.serializers.datums.TxOut(\*args: Any, \*\*kwargs: Any)

Bases: [`Item`](#schemas.serializers.datums.Item)

#### Attributes:

##### address *: str*

##### value *: str*

### *class* schemas.serializers.datums.TxOutCond(\*args: Any, \*\*kwargs: Any)

Bases: [`Item`](#schemas.serializers.datums.Item)

#### Attributes:

##### expires *: datetime*

##### subtype *: str*

##### value *: str*

### *class* schemas.serializers.datums.TxOutCondSubTypeSrvStakeLock(\*args: Any, \*\*kwargs: Any)

Bases: [`Item`](#schemas.serializers.datums.Item)

#### Attributes:

##### reinvest_percent *: int*

##### unlock *: datetime*

##### value *: str*

### *class* schemas.serializers.datums.TxOutExt(\*args: Any, \*\*kwargs: Any)

Bases: [`Item`](#schemas.serializers.datums.Item)

#### Attributes:

##### address *: str*

##### ticker *: str*

##### value *: str*

### *class* schemas.serializers.datums.TxSig(\*args: Any, \*\*kwargs: Any)

Bases: [`Item`](#schemas.serializers.datums.Item)


#### Methods:

##### sign *: [Sign](#schemas.serializers.sign.Sign)*

##### size *: int*

### *class* schemas.serializers.datums.TxToken(\*args: Any, \*\*kwargs: Any)

Bases: [`Item`](#schemas.serializers.datums.Item)

#### Attributes:

##### emission_chain_id *: str*

##### emission_hash *: str*

##### ticker *: str*

## schemas.serializers.event module

### *class* schemas.serializers.event.Event(\*args: Any, \*\*kwargs: Any)

Bases: `SerializeModel`

#### Attributes:

##### cell_id *: str*

#### Methods:

##### chain *: [Chain](#schemas.serializers.chain.Chain)*

##### created_at *: str*

##### date_format(date: datetime)

##### hash *: str*

##### hashes *: list[str]*

##### signs *: list[[Sign](#schemas.serializers.sign.Sign)]*

##### version *: int*

## schemas.serializers.net module

### *class* schemas.serializers.net.Net(\*args: Any, \*\*kwargs: Any)

Bases: `SerializeModel`

#### Attributes:

##### address *: str*

##### group_alias *: str*

##### id *: int*

##### name *: str*

## schemas.serializers.sign module

### *class* schemas.serializers.sign.Pkey(\*args: Any, \*\*kwargs: Any)

Bases: `SerializeModel`

#### Attributes:

##### size *: int*

##### type *: str*

### *class* schemas.serializers.sign.Sign(\*args: Any, \*\*kwargs: Any)

Bases: `SerializeModel`

#### Attributes:

##### address *: str*

#### Methods:

##### pkey *: [Pkey](#schemas.serializers.sign.Pkey)*

##### pkey_hash *: str*

##### size *: int*

##### type *: str*

## schemas.serializers.test_schema module

### *class* schemas.serializers.test_schema.TestSchema(\*args: Any, \*\*kwargs: Any)

Bases: `SerializeModel`

#### Attributes:

##### id *: int*

##### name *: str*

#### Methods:

##### values *: list[[TestValue](#schemas.serializers.test_schema.TestValue)]*

### *class* schemas.serializers.test_schema.TestValue(\*args: Any, \*\*kwargs: Any)

Bases: `SerializeModel`

#### Attributes:

##### value *: int*

## Module contents
