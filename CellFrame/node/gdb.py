from collections.abc import MutableMapping
from enum import Enum
from typing import Any, Callable, Iterator, Optional, List, Tuple, Dict

try:
    from CellFrame.legacy.DAP.GlobalDB import DB, Cluster, Instance, MemberRoles
    from CellFrame.legacy.DAP.Network import ClusterRoles
except:
    print("No DAP.GlobalDB, use stabs")
    from CellFrame.chain import GlobalDB as DB
    class Cluster:
        pass

    class Instance:
        pass
    class MemberRoles:
        INVALID = "invalid"
        NOBODY = "nobody"
        GUEST = "guest"
        USER = "user"
        ROOT = "root"
        DEFAULT = "default"
        pass
    class ClusterRoles:
        INVALID = "invalid"
        EMBEDDED="embedded"
        AUTONOMIC = "autonomic"
        ISOLATED = "isolated"
        VIRTUAL = "virtual"
        pass


from CellFrame.legacy.pycfhelpers.node.crypto import CFGUUID

from .net import CFNodeAddress, CFNet


class CFGDBCluster:
    """
    Represents a CFGDBCluster which manages cluster configurations and roles.

    Attributes:
        _cluster (Cluster): The underlying Cluster object.
    """

    class MemberRole(Enum):
        """
       Enumeration of member roles in the cluster.
        """
        INVALID = MemberRoles.INVALID
        NOBODY = MemberRoles.NOBODY
        GUEST = MemberRoles.GUEST
        USER = MemberRoles.USER
        ROOT = MemberRoles.ROOT
        DEFAULT = MemberRoles.DEFAULT

    class ClusterRole(Enum):
        """
        Enumeration of cluster roles.
        """
        INVALID = ClusterRoles.INVALID
        EMBEDDED = ClusterRoles.EMBEDDED
        AUTONOMIC = ClusterRoles.AUTONOMIC
        ISOLATED = ClusterRoles.ISOLATED
        VIRTUAL = ClusterRoles.VIRTUAL

    def __init__(self,
                 name: str,
                 guuid: CFGUUID,
                 grp_mask: str,
                 ttl_hour: int,
                 root_access_for_owner: bool,
                 defalt_member_role: MemberRole,
                 cluster_role: ClusterRole):
        """
        Initialize CFGDBCluster object.

        Args:
            name (str): The name of the cluster.
            guuid (CFGUUID): The globally unique user identifier for the cluster.
            grp_mask (str): The group mask.
            ttl_hour (int): Time-to-live in hours.
            root_access_for_owner (bool): Whether the owner has root access.
            defalt_member_role (CFGDBCluster.MemberRole): The default role for members.
            cluster_role (CFGDBCluster.ClusterRole): The role of the cluster.
        """

        instance = Instance()
        self._cluster = Cluster(instance,
                                name,
                                guuid._guuid,
                                grp_mask,
                                ttl_hour,
                                root_access_for_owner,
                                defalt_member_role.value,
                                cluster_role.value)

    def add_notify(self, func: Callable[[str, str, bytes, Any], Any], arg):
        """
        Add a notification callback to the cluster.

        Args:
            func (Callable[[str, str, bytes, Any], Any]): The callback function.
            arg (Any): Additional argument for the callback function.
        """
        self._cluster.notifyAdd(func, arg)

    def member_add(self, addr: CFNodeAddress, role: MemberRole):
        """
        Add a member to the cluster with a specified role.

        Args:
            addr (CFNodeAddress): The address of the member.
            role (CFGDBCluster.MemberRole): The role of the member.
        """
        self._cluster.memberAdd(addr._addr, role.value)

    def member_delete(self, addr: CFNodeAddress, role: MemberRole):
        """
        Delete a member from the cluster.

        Args:
            addr (CFNodeAddress): The address of the member to delete.
        """
        self._cluster.memberDelete(addr)
 
    def add_net_associate(self, net: CFNet):
        self._cluster.AddNetAssociate(net.id.long)



class CFGDBGroup(MutableMapping):
    """
    Represents a group in the GlobalDB, behaving like a dictionary.
    """

    def __init__(self, group: str):
        """
        Initialize a new GDBGroupBase instance.

        Args:
            group (str): The name of the group.
        """
        self.group = group

    def __setitem__(self, key: str, value: bytes) -> None:
        """
        Set the value associated with the given key in the group.

        Args:
            key (str): The key.
            value (bytes): The value.
        """
        DB.set(key, self.group, value)

    def get(self, key: str, default: Optional[bytes] = None) -> Optional[bytes]:
        """
        Retrieve the value associated with the given key from the group.

        Args:
            key (str): The key.
            default (Optional[bytes]): The default value to return if the key
                is not found. Defaults to None.

        Returns:
            Optional[bytes]: The value associated with the key, or the default
                value if the key is not found.
        """
        value = DB.get(key, self.group)
        if value is None:
            return default
        return value
    
    def set(self, key: str, value: bytes) -> None:
        """
        Save the value associated with the given key to the group.

        Args:
            key (str): The key.
            value (bytes) : Value

        Returns:
            None
        """
        DB.set(key, self.group, value)
        
    def __getitem__(self, key: str) -> bytes:
        """
        Retrieve the value associated with the given key from the group.

        Args:
            key (str): The key.

        Returns:
            bytes: The value associated with the key.

        Raises:
            KeyError: If the key not found in the group.
        """
        value = DB.get(key, self.group)
        if value is None:
            raise KeyError(f"Key '{key}' not found in group '{self.group}'")
        return value

    def __delitem__(self, key: str) -> None:
        """
        Delete the value associated with the given key from the group.

        Args:
            key (str): The key to delete.

        Raises:
            KeyError: If the key not found in the group.
        """
        if not DB.delete(key, self.group):
            raise KeyError(f"Key '{key}' not found in group '{self.group}'")

    def delete(self, key: str) -> None:
        """
        Delete the value associated with the given key from the group.

        Args:
            key (str): The key to delete.

        Raises:
            KeyError: If the key not found in the group.
        """
        self.__delitem__(key)


    def __iter__(self) -> Iterator[str]:
        """
        Return an iterator over the keys of the group.

        Returns:
            Iterator[str]: An iterator over the keys.
        """
        return (element.key for element in DB.grLoad(self.group))

    def __len__(self) -> int:
        """
        Return the number of items in the group.

        Returns:
            int: The number of items.
        """
        return len(DB.grLoad(self.group))

    def keys(self) -> "set[str]":
        """
        Return a new view of the group's keys.

        Returns:
            KeysView[str]: A set of the keys.
        """
        return {element.key for element in DB.grLoad(self.group)}

    def values(self) -> List[bytes]:
        """
        Return a new view of the group's values.

        Returns:
            ValuesView[bytes]: A set of the values encoded in bytes.
        """
        return [element.value for element in DB.grLoad(self.group)]

    def items(self) -> List[Tuple[str, bytes]]:
        """
        Return a new view of the group's items (key, value pairs).

        Returns:
            ItemsView[str, bytes]: A view of the items.
        """
        return [(element.key, element.value) for element in DB.grLoad(self.group)]

    def __contains__(self, key: str) -> bool:
        """
        Check if the group contains the given key.

        Args:
            key (str): The key to check.

        Returns:
            bool: True if the key is found, False otherwise.
        """
        return DB.get(key, self.group) is not None

    def __repr__(self) -> str:
        """
        Return a string representation of the group.

        Returns:
            str: A string representation of the group.
        """
        return repr({element.key: element.value for element in DB.grLoad(self.group)})
