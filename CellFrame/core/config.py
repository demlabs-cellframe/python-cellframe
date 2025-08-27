from typing import Optional, Union, List
from CellFrame.legacy.DAP import configGetItem, configGetSysDir

ConfigValueType = Union[str, bool, int, List[str]]


class CfConfig:
    """
    A class to access configuration settings.
    """
    def __init__(self) -> None:
        """Initialize the CfConfig object."""
        pass

    def __getitem__(self, section: str) -> 'CfSection':
        """
        Retrieve a CfSection object for the given section.

        Args:
            section (str): The section of the configuration.

        Returns:
            CfSection: The CfSection object for the given section.
        """
        return CfSection(self, section)

    def get(self, section: str, item: str, default: Optional[ConfigValueType] = None) -> ConfigValueType:
        """
        Retrieve the value of the configuration item.

        Args:
            section (str): The section of the configuration.
            item (str): The item within the section.
            default (Optional[ConfigValueType], optional): The default value to return if the item is not found. Defaults to None.

        Returns:
            ConfigValueType: The value of the configuration item, or the default value if the item is not found.
        """
        try:
            return configGetItem(section, item, default)
        except ValueError:
            return default
        
    def storage_path(self) -> str:
        """
        Get the storage path of the configuration.
        """
        return configGetSysDir()


class CfSection:
    """
    A class to represent a section of the configuration.
    """
    def __init__(self, config: CfConfig, section: str) -> None:
        """
        Initialize the CfSection object.

        Args:
            config (CfConfig): The CfConfig object that this section belongs to.
            section (str): The name of the section.
        """
        self.config = config
        self.section = section

    def __getitem__(self, item: str) -> ConfigValueType:
        """
        Retrieve the value of the item in the section.

        Args:
            item (str): The name of the item.

        Retrieve:
            ConfigValueType: The value of the item.
        """
        return self.config.get(self.section, item)

    def get(self, item: str, default: Optional[ConfigValueType] = None) -> ConfigValueType:
        """
        Retrieve the value of the item in the section.

        Args:
            item (str): The name of the item.
            default (Optional[ConfigValueType], optional): The default value to return if the item is not found. Defaults to None.

        Returns:
            ConfigValueType: The value of the item, or the default value if the item is not found.
        """
        return self.config.get(self.section, item, default)
