from __future__ import annotations
from typing import TYPE_CHECKING
import traceback

from CellFrame.Chain import ChainAtomPtr
from CellFrame.Consensus import DAG, Block
from CellFrame.Services import StakePosDelegate
from DAP.Core import logIt

if TYPE_CHECKING:
    from .net import CFChain
    from ..net import CFNet
    from DAP.Core import Math

from .datums import CFDatum
from .crypto import CFSign
from ..common.types import ChainTypes


class CFBlock:
    """The CellFrame block.

    Attributes:
        chain (CFChain): The CFChain object associated with the block.
        hash (str): The hash of the block.
        created_at (datetime): The time when the block was created.
        version (int): The version of the block.
        cell_id (str): The cell ID associated with the block.
        chainid (str): The chain ID associated witgh the block.
        signs (list[CFSign]): A list of CFSign objects representing the signatures of the block.
        meta (dict): The dictionary containing metadata of the block.
        size (int): The total size of the block in bytes.

            The dictionary includes the following keys:
            - "blockPrevHash" (str): The hash of the previous block.
            - "blockAnchorHash" (str): The hash of the anchor block.
            - "merkle" (str): The root hash of the Merkle tree representing the block's transactions.
            - "links" (list): A list of hashes representing links to other blocks.
            - "isGenesis" (bool): A boolean value indicating whether the block is a genesis block.
    """

    def __init__(self, atom: ChainAtomPtr, chain: CFChain, size: int = 0):
        """Initialize a CFBlock instance with the given atom and chain.

        Args:
            atom (ChainAtomPtr): The ChainAtomPtr object representing the block.
            chain (CFChain): The CFChain object associated with the block.
            size (int): The total size of the block in bytes. Defaults to 0.

        Raises:
            TypeError: If the chain type does not contain blocks.
            RuntimeError: If the block is not found from the atom.
        """
        self._origin_atom = atom
        if chain.type != ChainTypes.esbocs:
            raise TypeError(f"Chain type={chain.type} does not contain blocks")
        block = Block.fromAtom(self._origin_atom)
        if not block:
            raise RuntimeError("Not found block from atom")
        self._origin_block = block

        self.chain = chain
        self.hash = str(block.hash)
        self.created_at = block.created
        self.version = block.version
        self.cell_id = str(block.cellId)
        # TODO: move chain_id CFBlock --> CFChain
        self.chainid = str(block.chainId)
        self.signs = [CFSign(sign, self.chain.net) for sign in block.signs]
        self.meta = block.metaData
        self.number = block.getNumber(self.chain.net._origin_net, self.chain._origin_chain)
        self.size = size  # Total block size in bytes

    @property
    def rewards(self) -> dict[str, int]:
        """Return validators rewards for this block.

        Returns:
            dict[str, int]: A dict where each key is a pubkey hash and 
            each value is the corresponding reward.
        """
        raw_rewards = self._origin_block.getBlockSignersRewards(
            self.chain._origin_chain
        )
        if not raw_rewards:
            return {}

        rewards_dict: dict[str, int] = {}
        for reward_entry in raw_rewards:
            if not isinstance(reward_entry, dict):
                continue
            for raw_hash_obj, reward_obj in reward_entry.items():
                hex_hash = str(raw_hash_obj)
                amount = int(str(reward_obj))
                rewards_dict[hex_hash] = amount

        return rewards_dict

    def get_datums(self) -> list[CFDatum]:
        """Get datums associated with the block.

        Returns:
            list[CFDatum]: A list of CFDatum objects associated with the block.
        """
        if self._origin_block.datums is None:
            return []
        return [CFDatum(self, datum, self.chain) for datum in self._origin_block.datums]


class CFEvent:
    """The CellFrame event.

    Provides methods to work with CellFrame events.

    Attributes:
        chain (CFChain): The CFChain object associated with the event.
        hash (str): The hash of the event.
        created_at (datetime): The time when the event was created.
        version (int): The version of the event.
        cell_id (str): The cell ID associated with the event.
        round_id (int): The round ID associated with the event.
        hashes (list): A list of hashes associated with the event.
        size (int): The total size of the event in bytes.
    """

    def __init__(self, atom: ChainAtomPtr, chain: CFChain, size: int = 0):
        """Initialize a CFEvent object with the given atom and chain.

        Args:
            atom (ChainAtomPtr): The ChainAtomPtr object representing the event.
            chain (CFChain): The CFChain object associated with the event.
            size (int): The total size of the event in bytes. Defaults to 0.

        Raises:
            TypeError: If the chain type does not contain events.
            RuntimeError: If the event is not found from the atom.
        """
        self._origin_atom = atom
        if chain.type != ChainTypes.dag_poa:
            raise TypeError(f"Chain type={chain.type} does not contain events")
        event = DAG.fromAtom(self._origin_atom)
        if not event:
            raise RuntimeError("Not found event from atom")
        self._origin_event = event

        self.chain = chain
        self.hash = str(event.hash)
        self.created_at = event.created
        self.version = event.version
        self.cell_id = str(event.cellId)
        self.signs = [CFSign(sign, self.chain.net) for sign in event.signs]
        self.round_id = event.roundId
        self.hashes = list(map(str, event.links))
        self.size = size  # Total event size in bytes

    def get_datums(self) -> list[CFDatum]:
        """Get datum associated with the event.

        Returns:
            list[CFDatum]: A list of CFDatum object associated with the event. Event has only one Datum.
        """
        # Event has only one Datum
        return [CFDatum(self, self._origin_event.datum, self.chain)]


class CFValidatorInfo:
    """
    Represent information about validators in the CellFrame network.

    Attributes:
        total (int): Total number of validators.
        active (int): Number of active validators.
        total_weight (int): Total stake weight in the network.
    """

    def __init__(self, total: int, active: int,  total_weight: Math, native_ticker: str):
        self._total = total
        self._active = active
        self._total_weight = total_weight
        self._ticker_delegate = f"m{native_ticker}"

    @property
    def total(self) -> int:
        """
        Get the total number of validators.

        Returns:
            int: Total number of validators.
        """
        return self._total

    @property
    def active(self) -> int:
        """
        Get the number of active validators.

        Returns:
            int: Number of active validators.
        """
        return self._active

    @property
    def total_weight(self) -> Math:
        """
        Get the total stake weight in the network.

        Returns:
            Math: Total stake weight.
        """
        return self._total_weight

    @property
    def total_weight_in_coin(self) -> str:
        """
        Convert total weight to a human-readable format.

        Returns:
            str: Total weight in "coin" format.
        """
        coins_value = self._total_weight.coins
        return coins_value

    @property
    def ticker_delegate(self) -> str:
        """
        Get delegate token ticker

        Returns:
            string: Delegate token ticker.
        """

        return self._ticker_delegate

    def __repr__(self) -> str:
        return f"CFValidatorInfo(total={self.total}, active={self.active}, total_weight={self.total_weight})"

    def serialize(self) -> dict[str, str]:
        """
        Serialize the validator info to a dictionary.

        Returns:
            dict[str, str]: The serialized validator info.
        """
        return {
            "total": self.total,
            "active": self.active,
            "total_weight": str(self.total_weight),
            "total_weight_in_coin": self.total_weight_in_coin,
            "ticker_delegate": self._ticker_delegate,
        }


class CFStakePosDelegate:
    """
    Represent a StakePosDelegate service in CellFrame.

    Attributes:
        net (CFNet): The network instance.
    """

    def __init__(self, net: CFNet):
        """
        Initialize StakePosDelegate with the given network.

        Args:
            net (CFNet): The network to interact with.
        """
        self._net = net
        self._stake_delegate = StakePosDelegate(self._net._origin_net)
        self._native_ticker = self._net.fee_data.native_ticker

    @property
    def net(self) -> CFNet:
        """
        Get the network instance.

        Returns:
            CFNet: The network instance.
        """
        return self._net

    def get_validator_info(self) -> CFValidatorInfo:
        """
        Get information about the validators, including total, active, and total weight.

        Returns:
            CFValidatorInfo: Object containing validator information.
        """
        try:
            count_validators = self._stake_delegate.CountValidators
            total_validators = count_validators[0]
            active_validators = count_validators[1]
            total_weight = self._stake_delegate.TotalWeight

            return CFValidatorInfo(
                total_validators, active_validators, total_weight, self._native_ticker
            )
        except Exception as e:
            logIt.error(
                f"Error retrieving validator info for network {self._net.name}: {traceback.format_exc()}"
            )
            raise e

    def __repr__(self) -> str:
        return f"CFStakeDelegate(net='{self._net.name}')"
