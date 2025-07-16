"""
Module for CellFrame Voting.
"""
from __future__ import annotations

from typing import TYPE_CHECKING
from DAP.Core import logIt
from CellFrame.Services import Vote
import traceback
if TYPE_CHECKING:
    from ..net import CFNet
    from DAP.Crypto import HashFast

class CFVotingOption:
    """
    Represent a voting option in a CellFrame voting.

    Attributes:
        description (str): The description of the voting.
        votes (int): The number of votes.
        weights (int): The weight of the votes.
        hash_txs (list[str]): The list of transaction hashes.
    """

    def __init__(self, description: str, votes: int, weights: int, hash_txs: list[HashFast]):
        self.description = description
        self.votes = votes
        self.weights = weights
        self.hash_txs = [str(tx) for tx in hash_txs]

    def __repr__(self) -> str:
        return (
            f"CFVotingOption(description='{self.description}', votes={self.votes}, "
            f"weights={self.weights}, hash_txs={self.hash_txs})"
        )

    def serialize(self) -> dict[str, str | int | list[str]]:
        """
        Serialize the voting option to a dictionary.

        Returns:
            dict[str, str | int | list[str]]: The serialized voting option.
        """
        return {
            "description": self.description,
            "votes": self.votes,
            "weights": self.weights,
            "hash_txs": self.hash_txs
        }

class CFVoting:
    """
    Represent a voting in the CellFrame.

    Attributes:
        net (CFNet): The network instance.
        hash (str): The hash of the voting.
        question (str): The question of the voting.
        options (list[CFVotingOption]): The list of voting options.
        expire (int): The expiration time of the voting.
        is_delegate_key_required (bool): Whether a delegate key is required.
        is_vote_changing_allowed (bool): Whether vote changing is allowed.
    """

    def __init__(
        self,
        net: CFNet,
        hash: str,
        question: str,
        options: list[CFVotingOption],
        expire: int,
        is_delegate_key_required: bool,
        is_vote_changing_allowed: bool
    ):
        if not net:
            raise ValueError("Network (net) cannot be None")
        if not hash:
            raise ValueError("Hash cannot be empty")
        if not question:
            raise ValueError("Question cannot be empty")
        if not options:
            raise ValueError("Options cannot be empty")

        self._net = net
        self._hash = hash
        self._question = question
        self._options = options
        self._expire = expire
        self._is_delegate_key_required = is_delegate_key_required
        self._is_vote_changing_allowed = is_vote_changing_allowed

    @property
    def net(self) -> CFNet:
        """
        Get the network instance.

        Returns:
            CFNet: The network instance.
        """
        return self._net

    @property
    def hash(self) -> str:
        """
        Get the hash of the voting.

        Returns:
            str: The hash of the voting.
        """
        return self._hash

    @property
    def question(self) -> str:
        """
        Get the question of the voting.

        Returns:
            str: The question of the voting.
        """
        return self._question

    @property
    def options(self) -> list[CFVotingOption]:
        """
        Get the list of voting options.

        Returns:
            list[CFVotingOption]: The list of voting options.
        """
        return self._options

    @property
    def expire(self) -> int:
        """
        Get the expiration time of the voting.

        Returns:
            int: The expiration time of the voting.
        """
        return self._expire

    @property
    def is_delegate_key_required(self) -> bool:
        """
        Check if a delegate key is required.

        Returns:
            bool: True if a delegate key is required, False otherwise.
        """
        return self._is_delegate_key_required

    @property
    def is_vote_changing_allowed(self) -> bool:
        """
        Check if vote changing is allowed.

        Returns:
            bool: True if vote changing is allowed, False otherwise.
        """
        return self._is_vote_changing_allowed

    def __repr__(self) -> str:
        return (
            f"CFVoting(hash='{self.hash}', question='{self.question}', options={self.options}, "
            f"expire={self.expire}, is_delegate_key_required={self.is_delegate_key_required}, "
            f"is_vote_changing_allowed={self.is_vote_changing_allowed})"
        )

    def serialize(self) -> dict[str, str | int | bool | list[dict[str, str | int | list[str]]]]:
        """
        Serialize the voting to a dictionary.

        Returns:
            dict[str, str | int | bool | list[dict[str, str | int | list[str]]]]: The serialized voting.
        """
        return {
            "net": self.net.name,
            "hash": self.hash,
            "question": self.question,
            "options": [option.serialize() for option in self.options],
            "expire": self.expire,
            "is_delegate_key_required": self.is_delegate_key_required,
            "is_vote_changing_allowed": self.is_vote_changing_allowed
        }

    @staticmethod
    def list(net: CFNet) -> list[CFVoting]:
        """
        List all votings for the given network.

        Args:
            net (CFNet): The network instance.

        Returns:
            list[CFVoting]: The list of votings.
        """
        votings = Vote.list(net._origin_net)
        if votings is None:
            logIt.notice(f"In {net.name} votings not found")
            return []

        logIt.notice(f"Net {net.name} has {len(votings)} votings:")
        result: list[CFVoting] = []
        for voting in votings:
            options = []
            for op in voting.options:
                try:
                    options.append(CFVotingOption(
                                                    op.description,
                                                    op.votes,
                                                    op.weights,
                                                    op.hashTxs
                    ))
                except:
                    logIt.error(traceback.format_exc())

            try:
                cf_voting = CFVoting(
                    net,
                    voting.hash,
                    voting.question,
                    options,
                    voting.expire,
                    voting.isDelegateKeyRequired,
                    voting.isVoteChangingAllowed
                )
                result.append(cf_voting)
                logIt.notice(f"Added voting: {cf_voting}")
            except ValueError as e:
                logIt.error(f"Error creating CFVoting: {e}")

        return result
