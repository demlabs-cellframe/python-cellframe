# pycfhelpers.common package

## Submodules

## pycfhelpers.common.parsers module

### pycfhelpers.common.parsers.parse_cf_v1_address(address: str)

Parse a CF v1 format address and returns its various components.

* **Parameters:**
  **address** (*str*) – The CF v1 address string to parse. It should be a Base58 encoded string.
* **Returns:**
  A tuple containing the following components:
  : - version (int): The address version.
    - net_id (int): The network identifier.
    - sign_id (int): The signature identifier.
    - public_hash (bytes): The public hash.
    - summary_hash (bytes): The summary hash.
    - control_hash (bytes): The control hash.
* **Return type:**
  tuple
* **Raises:**
  **ValueError** – If the address is invalid.

## pycfhelpers.common.types module

### *class* pycfhelpers.common.types.ChainTypes(value)

Bases: `str`, `Enum`

An enumeration.

#### esbocs *= 'esbocs'*

#### dag_poa *= 'dag_poa'*

#### \_\_format_\_(format_spec)

Returns format using actual value type unless \_\_str_\_ has been overridden.

### *class* pycfhelpers.common.types.DatumTypes(value)

Bases: `str`, `Enum`

An enumeration.

#### DATUM_TX *= 'DATUM_TX'*

#### DATUM_TOKEN_DECL *= 'DATUM_TOKEN_DECL'*

#### DATUM_TOKEN_EMISSION *= 'DATUM_TOKEN_EMISSION'*

#### DATUM_CUSTOM *= 'DATUM_CUSTOM'*

#### DATUM_DECREE *= 'DATUM_DECREE'*

#### DATUM_ANCHOR *= 'DATUM_ANCHOR'*

#### \_\_format_\_(format_spec)

Returns format using actual value type unless \_\_str_\_ has been overridden.

### *class* pycfhelpers.common.types.ItemTypes(value)

Bases: `str`, `Enum`

An enumeration.

#### TX_ITEM_TYPE_IN *= 'TX_ITEM_TYPE_IN'*

#### TX_ITEM_TYPE_IN_COND *= 'TX_ITEM_TYPE_IN_COND'*

#### TX_ITEM_TYPE_OUT *= 'TX_ITEM_TYPE_OUT'*

#### TX_ITEM_TYPE_OUT_COND *= 'TX_ITEM_TYPE_OUT_COND'*

#### TX_ITEM_TYPE_PKEY *= 'TX_ITEM_TYPE_PKEY'*

#### TX_ITEM_TYPE_SIG *= 'TX_ITEM_TYPE_SIG'*

#### TX_ITEM_TYPE_IN_EMS *= 'TX_ITEM_TYPE_IN_EMS'*

#### TX_ITEM_TYPE_RECEIPT *= 'TX_ITEM_TYPE_RECEIPT'*

#### TX_ITEM_TYPE_OUT_EXT *= 'TX_ITEM_TYPE_OUT_EXT'*

#### TX_ITEM_TYPE_TSD *= 'TX_ITEM_TYPE_TSD'*

#### TX_ITEM_TYPE_VOTING *= 'TX_ITEM_TYPE_VOTING'*

#### TX_ITEM_TYPE_VOTE *= 'TX_ITEM_TYPE_VOTE'*

#### DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_PAY *= 'DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_PAY'*

#### DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_LOCK *= 'DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_LOCK'*

#### DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_XCHANGE *= 'DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_XCHANGE'*

#### DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_POS_DELEGATE *= 'DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_POS_DELEGATE'*

#### \_\_format_\_(format_spec)

Returns format using actual value type unless \_\_str_\_ has been overridden.

## Module contents
