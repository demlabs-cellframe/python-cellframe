"""
DAP JSON utilities - thin Python wrapper over dap_json C API

This module provides a modern Pythonic interface to the DAP SDK's JSON API,
which is designed to replace json-c with a cleaner, more maintainable interface.

Example:
    >>> from CellFrame import DapJSON
    >>>
    >>> # Create object
    >>> obj = DapJSON()
    >>> obj.add_string("name", "Alice")
    >>> obj.add_int("age", 30)
    >>> obj.add_bool("active", True)
    >>>
    >>> # Convert to string
    >>> json_str = obj.to_string()
    >>> print(json_str)
    {"name":"Alice","age":30,"active":true}
    >>>
    >>> # Parse from string
    >>> obj2 = DapJSON.parse('{"name":"Bob","age":25}')
    >>> print(obj2.get_string("name"))
    Bob
    >>>
    >>> # Arrays
    >>> arr = DapJSON.array_new()
    >>> item1 = DapJSON()
    >>> item1.add_string("id", "1")
    >>> arr.array_add(item1)
    >>> print(arr.array_length())
    1

Note:
    This is a thin wrapper over the C dap_json API. For maximum performance
    in tight loops, consider using the C API directly via python_cellframe module.
"""

from typing import Optional, Union, Any
import python_cellframe

# Re-export the C type for direct access
_DapJSON = python_cellframe.DapJSON


class DapJSON:
    """
    Pythonic wrapper for DAP JSON objects
    
    This class provides a clean interface to DAP's JSON API, which will
    replace json-c in the SDK. The API is designed to be simple, efficient,
    and familiar to Python developers.
    
    Attributes:
        _obj: Internal C DapJSON object (do not access directly)
    """
    
    __slots__ = ('_obj',)
    
    def __init__(self, _internal_obj=None):
        """
        Create a new JSON object
        
        Args:
            _internal_obj: Internal C object (for internal use only)
        """
        if _internal_obj is not None:
            self._obj = _internal_obj
        else:
            self._obj = _DapJSON()
    
    @classmethod
    def parse(cls, json_str: str) -> Optional['DapJSON']:
        """
        Parse JSON from string
        
        Args:
            json_str: JSON string to parse
            
        Returns:
            DapJSON object or None on parse error
            
        Example:
            >>> obj = DapJSON.parse('{"name":"Alice","age":30}')
            >>> print(obj.get_string("name"))
            Alice
        """
        c_obj = _DapJSON.parse(json_str)
        if c_obj is None:
            return None
        return cls(_internal_obj=c_obj)
    
    @classmethod
    def array_new(cls) -> 'DapJSON':
        """
        Create a new JSON array
        
        Returns:
            DapJSON array object
            
        Example:
            >>> arr = DapJSON.array_new()
            >>> item = DapJSON()
            >>> item.add_string("id", "1")
            >>> arr.array_add(item)
        """
        c_obj = _DapJSON.array_new()
        return cls(_internal_obj=c_obj)
    
    def to_string(self) -> str:
        """
        Convert JSON to string
        
        Returns:
            Compact JSON string
            
        Example:
            >>> obj = DapJSON()
            >>> obj.add_string("name", "Alice")
            >>> print(obj.to_string())
            {"name":"Alice"}
        """
        return self._obj.to_string()
    
    def to_string_pretty(self) -> str:
        """
        Convert JSON to pretty-formatted string
        
        Returns:
            Pretty-formatted JSON string with indentation
            
        Example:
            >>> obj = DapJSON()
            >>> obj.add_string("name", "Alice")
            >>> print(obj.to_string_pretty())
            {
              "name": "Alice"
            }
        """
        return self._obj.to_string_pretty()
    
    # ========== Object field setters ==========
    
    def add_string(self, key: str, value: str) -> None:
        """Add string field to object"""
        self._obj.add_string(key, value)
    
    def add_int(self, key: str, value: int) -> None:
        """Add integer field to object"""
        self._obj.add_int(key, value)
    
    def add_int64(self, key: str, value: int) -> None:
        """Add int64 field to object"""
        self._obj.add_int64(key, value)
    
    def add_uint64(self, key: str, value: int) -> None:
        """Add uint64 field to object"""
        self._obj.add_uint64(key, value)
    
    def add_double(self, key: str, value: float) -> None:
        """Add double field to object"""
        self._obj.add_double(key, value)
    
    def add_bool(self, key: str, value: bool) -> None:
        """Add boolean field to object"""
        self._obj.add_bool(key, value)
    
    def add_null(self, key: str) -> None:
        """Add null field to object"""
        self._obj.add_null(key)
    
    def add_object(self, key: str, value: 'DapJSON') -> None:
        """
        Add object field to object
        
        Args:
            key: Field key
            value: DapJSON object to add
            
        Example:
            >>> obj = DapJSON()
            >>> nested = DapJSON()
            >>> nested.add_string("city", "New York")
            >>> obj.add_object("address", nested)
        """
        self._obj.add_object(key, value._obj)
    
    # ========== Object field getters ==========
    
    def get_string(self, key: str) -> Optional[str]:
        """
        Get string field from object
        
        Args:
            key: Field key
            
        Returns:
            String value or None if not found
        """
        return self._obj.get_string(key)
    
    def get_int(self, key: str) -> int:
        """
        Get integer field from object
        
        Args:
            key: Field key
            
        Returns:
            Integer value or 0 if not found
        """
        return self._obj.get_int(key)
    
    def get_int64(self, key: str) -> int:
        """Get int64 field from object"""
        return self._obj.get_int64(key)
    
    def get_uint64(self, key: str) -> int:
        """Get uint64 field from object"""
        return self._obj.get_uint64(key)
    
    def get_double(self, key: str) -> float:
        """Get double field from object"""
        return self._obj.get_double(key)
    
    def get_bool(self, key: str) -> bool:
        """Get boolean field from object"""
        return self._obj.get_bool(key)
    
    def get_object(self, key: str) -> Optional['DapJSON']:
        """
        Get object field from object
        
        Args:
            key: Field key
            
        Returns:
            DapJSON object or None if not found
        """
        c_obj = self._obj.get_object(key)
        if c_obj is None:
            return None
        return DapJSON(_internal_obj=c_obj)
    
    # ========== Object utilities ==========
    
    def has_key(self, key: str) -> bool:
        """
        Check if key exists in object
        
        Args:
            key: Field key
            
        Returns:
            True if key exists
        """
        return self._obj.has_key(key)
    
    def del_key(self, key: str) -> None:
        """
        Delete key from object
        
        Args:
            key: Field key
            
        Raises:
            KeyError: If key not found
        """
        self._obj.del_key(key)
    
    def get_type(self) -> str:
        """
        Get JSON type
        
        Returns:
            Type name: "null", "bool", "int", "double", "string", "object", "array"
        """
        return self._obj.get_type()
    
    # ========== Array methods ==========
    
    def array_add(self, item: 'DapJSON') -> None:
        """
        Add item to array
        
        Args:
            item: DapJSON object to add
            
        Example:
            >>> arr = DapJSON.array_new()
            >>> item = DapJSON()
            >>> item.add_string("name", "Alice")
            >>> arr.array_add(item)
        """
        self._obj.array_add(item._obj)
    
    def array_length(self) -> int:
        """
        Get array length
        
        Returns:
            Number of elements in array
        """
        return self._obj.array_length()
    
    def array_get(self, index: int) -> Optional['DapJSON']:
        """
        Get array item by index
        
        Args:
            index: Item index
            
        Returns:
            DapJSON object or None if index out of bounds
            
        Raises:
            IndexError: If index is negative or out of range
        """
        c_obj = self._obj.array_get(index)
        if c_obj is None:
            return None
        return DapJSON(_internal_obj=c_obj)
    
    # ========== Python magic methods ==========
    
    def __str__(self) -> str:
        """String representation (compact JSON)"""
        return self.to_string()
    
    def __repr__(self) -> str:
        """Developer representation"""
        return f"DapJSON({self.to_string()})"
    
    def __getitem__(self, key: str) -> Any:
        """
        Dictionary-style access: obj["key"]
        
        Automatically detects type and returns appropriate value
        """
        if not self.has_key(key):
            raise KeyError(f"Key '{key}' not found in JSON object")
        
        # Try to determine type and return appropriate value
        # This is a convenience method - for type-safe access, use get_* methods
        obj = self.get_object(key)
        if obj is not None:
            return obj
        
        string_val = self.get_string(key)
        if string_val is not None:
            return string_val
        
        # Try numeric types
        try:
            return self.get_int(key)
        except:
            pass
        
        try:
            return self.get_double(key)
        except:
            pass
        
        try:
            return self.get_bool(key)
        except:
            pass
        
        # Fallback
        return None
    
    def __setitem__(self, key: str, value: Any) -> None:
        """
        Dictionary-style assignment: obj["key"] = value
        
        Automatically detects type and calls appropriate add_* method
        """
        if isinstance(value, str):
            self.add_string(key, value)
        elif isinstance(value, bool):  # Check bool before int (bool is subclass of int)
            self.add_bool(key, value)
        elif isinstance(value, int):
            self.add_int64(key, value)
        elif isinstance(value, float):
            self.add_double(key, value)
        elif isinstance(value, DapJSON):
            self.add_object(key, value)
        elif value is None:
            self.add_null(key)
        else:
            raise TypeError(f"Unsupported type for JSON: {type(value)}")
    
    def __contains__(self, key: str) -> bool:
        """Support for 'in' operator: "key" in obj"""
        return self.has_key(key)


# Export for convenience
__all__ = ['DapJSON']
