# pycfhelpers.common package

## Submodules

## pycfhelpers.common.parsers module

## pycfhelpers.common.types module

### *class* pycfhelpers.common.types.ChainTypes(value)

Bases: `str`, `Enum`

Enumeration of chain types.


#### Attributes:

##### esbocs

esbocs chain type.

* **Type:** *str*

dag_poa

dag_poa chain type.

* **Type:** *str*

##### \_generate_next_value_(start, count, last_values)

Generate the next value when not given.

name: the name of the member
start: the initial start value or None
count: the number of existing members
last_value: the last value assigned or None

##### dag_poa *= 'dag_poa'*

##### esbocs *= 'esbocs'*

### *class* pycfhelpers.common.types.DatumTypes(value)

Bases: `str`, `Enum`

Enumeration of datum types.


#### Attributes:

##### DATUM_TX

Datum transaction type.

* **Type:** *str*

DATUM_TOKEN_DECL

Datum token declaration type.

* **Type:** *str*

##### DATUM_TOKEN_EMISSION

Datum token emission type.

* **Type:** *str*

##### DATUM_CUSTOM

Custom datum type.

* **Type:** *str*

##### DATUM_DECREE

Datum decree type.

* **Type:** *str*

##### DATUM_ANCHOR

Datum anchor type.

* **Type:** *str*

##### DATUM_ANCHOR *= 'DATUM_ANCHOR'*

##### DATUM_CUSTOM *= 'DATUM_CUSTOM'*

##### DATUM_DECREE *= 'DATUM_DECREE'*

##### DATUM_TOKEN_DECL *= 'DATUM_TOKEN_DECL'*

##### DATUM_TOKEN_EMISSION *= 'DATUM_TOKEN_EMISSION'*

##### DATUM_TX *= 'DATUM_TX'*

##### \_generate_next_value_(start, count, last_values)

Generate the next value when not given.

name: the name of the member
start: the initial start value or None
count: the number of existing members
last_value: the last value assigned or None

### *class* pycfhelpers.common.types.ItemTypes(value)

Bases: `str`, `Enum`

Enumeration of item types.


#### Attributes:

##### DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_PAY *= 'DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_PAY'*

DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_LOCK *= 'DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_LOCK'*

##### DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_POS_DELEGATE *= 'DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_POS_DELEGATE'*

##### DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_XCHANGE *= 'DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_XCHANGE'*

##### TX_ITEM_TYPE_IN *= 'TX_ITEM_TYPE_IN'*

##### TX_ITEM_TYPE_IN_COND *= 'TX_ITEM_TYPE_IN_COND'*

##### TX_ITEM_TYPE_IN_EMS *= 'TX_ITEM_TYPE_IN_EMS'*

##### TX_ITEM_TYPE_OUT *= 'TX_ITEM_TYPE_OUT'*

##### TX_ITEM_TYPE_OUT_COND *= 'TX_ITEM_TYPE_OUT_COND'*

##### TX_ITEM_TYPE_OUT_EXT *= 'TX_ITEM_TYPE_OUT_EXT'*

##### TX_ITEM_TYPE_PKEY *= 'TX_ITEM_TYPE_PKEY'*

##### TX_ITEM_TYPE_RECEIPT *= 'TX_ITEM_TYPE_RECEIPT'*

##### TX_ITEM_TYPE_SIG *= 'TX_ITEM_TYPE_SIG'*

##### TX_ITEM_TYPE_TSD *= 'TX_ITEM_TYPE_TSD'*

##### TX_ITEM_TYPE_VOTE *= 'TX_ITEM_TYPE_VOTE'*

##### TX_ITEM_TYPE_VOTING *= 'TX_ITEM_TYPE_VOTING'*

##### \_generate_next_value_(start, count, last_values)

Generate the next value when not given.

name: the name of the member
start: the initial start value or None
count: the number of existing members
last_value: the last value assigned or None

## Module contents
