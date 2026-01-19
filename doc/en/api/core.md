# Core Module Documentation

The core module provides the execution context system used by the SDK.
It is responsible for detecting plugin vs library mode and managing shared
resources. A high-level `CellframeNode` orchestrator is not implemented yet.

## ExecutionMode

Enum defining the execution mode:

```python
from CellFrame.common.context import ExecutionMode

ExecutionMode.PLUGIN
ExecutionMode.LIBRARY
```

## AppContext (abstract)

Base class for all contexts.

Key methods:
- `initialize() -> bool`
- `shutdown() -> bool`
- `get_config_value(key, default=None)`
- `set_config_value(key, value)`
- `get_data_dir() -> Path`
- `get_log_dir() -> Path`
- `get_resource(name)` / `set_resource(name, resource)` / `remove_resource(name)`

## PluginContext

Context for running inside `cellframe-node`.

Additional methods:
- `get_plugin_manifest() -> Optional[Dict[str, Any]]`
- `get_node_api() -> Optional[Any]`
- `get_node_config_path() -> Optional[Path]`

## LibContext

Context for standalone library mode.

Additional methods:
- `get_config_dir() -> Path`

## ContextFactory

Factory for creating contexts.

```python
from CellFrame.common.context import ContextFactory, ExecutionMode

context = ContextFactory.create_context(ExecutionMode.LIBRARY, app_name="my-app")
mode = ContextFactory.auto_detect_mode()
```

## Global context helpers

```python
import CellFrame

context = CellFrame.initialize_context()
# ... use the SDK ...
CellFrame.shutdown_context()
```

Available helpers:
- `initialize_context(mode=None, app_name=None, **kwargs)`
- `shutdown_context()`
- `get_context()`
- `set_context(context)`
