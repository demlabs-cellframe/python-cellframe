"""
🧪 Unit Tests for CellframeNode
Testing the main coordinator class with comprehensive coverage.
"""

import pytest
import sys
from unittest.mock import Mock, patch, MagicMock
from typing import Dict, Any

# Import the classes under test
try:
    from CellFrame.core import CellframeNode, CellframeComponent, ExecutionMode
    from CellFrame.core.context import AppContext, PluginContext, LibContext
    from CellFrame.core.exceptions import CellframeException, ConfigurationException
except ImportError as e:
    pytest.skip(f"CellFrame modules not available: {e}", allow_module_level=True)


class TestCellframeNode:
    """Comprehensive unit tests for CellframeNode class."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.mock_context = Mock(spec=AppContext)
        self.mock_context.get_resource.return_value = None
        self.mock_context.is_plugin_mode = False
        self.mock_context.is_library_mode = True
        self.mock_context.app_name = "test_app"
        self.mock_context.mode = Mock(value="library")  # Add mode attribute for get_status
        
    @pytest.mark.unit
    def test_node_creation_with_context(self):
        """Test CellframeNode creation with provided context."""
        node = CellframeNode(context=self.mock_context)
        
        assert node.context == self.mock_context
        assert hasattr(node, 'chain')
        assert node.name == "node"  # Component name attribute
        
    @pytest.mark.unit
    def test_node_creation_without_context(self):
        """Test CellframeNode creation with auto-generated context."""
        with patch('CellFrame.core.initialize_context') as mock_init:
            mock_init.return_value = self.mock_context
            
            node = CellframeNode()
            
            mock_init.assert_called_once()
            assert node.context == self.mock_context
            
    @pytest.mark.unit
    def test_node_creation_with_execution_mode(self):
        """Test CellframeNode creation with specific execution mode."""
        with patch('CellFrame.core.initialize_context') as mock_init:
            mock_init.return_value = self.mock_context
            
            node = CellframeNode(mode=ExecutionMode.LIBRARY)
            
            mock_init.assert_called_once_with(ExecutionMode.LIBRARY, None)
            
    @pytest.mark.unit
    def test_node_initialization_success(self):
        """Test successful node initialization."""
        node = CellframeNode(context=self.mock_context)
        
        # Mock the chain component initialization
        with patch.object(node.chain, 'initialize', return_value=True):
            result = node.initialize()
            
            assert result is True
            
    @pytest.mark.unit
    def test_node_initialization_failure(self):
        """Test node initialization failure."""
        node = CellframeNode(context=self.mock_context)
        
        # Mock the chain component initialization to fail
        with patch.object(node.chain, 'initialize', return_value=False):
            result = node.initialize()
            
            assert result is False
            
    @pytest.mark.unit
    def test_get_status_basic(self):
        """Test basic get_status functionality."""
        node = CellframeNode(context=self.mock_context)
        
        status = node.get_status()
        
        assert isinstance(status, dict)
        assert 'mode' in status
        assert 'context_type' in status
        assert 'app_name' in status
        assert status['app_name'] == "test_app"
        
    @pytest.mark.unit
    def test_get_chains_empty(self):
        """Test get_chains with no chains."""
        node = CellframeNode(context=self.mock_context)
        
        # CellframeNode stores chains in _chains dict
        assert node._chains == {}
        assert len(node._chains) == 0
        
    @pytest.mark.unit
    def test_get_chain_not_found(self):
        """Test get_chain with non-existent chain."""
        node = CellframeNode(context=self.mock_context)
        
        # Use the actual get_chain method
        chain = node.get_chain("nonexistent")
        assert chain is None
            
    @pytest.mark.unit
    def test_context_manager_success(self):
        """Test CellframeNode as context manager - success path."""
        with patch('CellFrame.core.initialize_context') as mock_init:
            mock_init.return_value = self.mock_context
            
            with CellframeNode() as node:
                assert node is not None
                assert node.context == self.mock_context
                
    @pytest.mark.unit
    def test_context_manager_exception(self):
        """Test CellframeNode as context manager with exception."""
        with patch('CellFrame.core.initialize_context') as mock_init:
            mock_init.return_value = self.mock_context
            
            with pytest.raises(ValueError):
                with CellframeNode() as node:
                    raise ValueError("Test exception")
                    
    @pytest.mark.unit
    def test_legacy_config_handling(self):
        """Test legacy configuration compatibility."""
        legacy_config = {
            "debug": True,
            "config_dir": "/test/path",
            "log_level": "DEBUG"
        }
        
        with patch('CellFrame.core.initialize_context') as mock_init:
            mock_init.return_value = self.mock_context
            
            node = CellframeNode(config=legacy_config)
            
            assert node._config == legacy_config
            
    @pytest.mark.unit
    def test_component_registration(self):
        """Test component registration in node."""
        node = CellframeNode(context=self.mock_context)
        
        # Check that chain component is registered
        assert 'chain' in node._components
        assert node.chain == node._components['chain']
        
    @pytest.mark.unit
    def test_resource_management(self):
        """Test resource management through context."""
        node = CellframeNode(context=self.mock_context)
        
        # Test getting resource
        self.mock_context.get_resource.return_value = "test_resource"
        
        resource = node.context.get_resource("test_key")
        assert resource == "test_resource"
        self.mock_context.get_resource.assert_called_with("test_key")
        
    @pytest.mark.unit
    def test_error_handling_in_initialization(self):
        """Test error handling during initialization."""
        self.mock_context.initialize.side_effect = Exception("Init error")
        
        node = CellframeNode(context=self.mock_context)
        
        result = node.initialize()
        assert result is False
        
    @pytest.mark.unit
    def test_node_shutdown(self):
        """Test node shutdown functionality."""
        node = CellframeNode(context=self.mock_context)
        
        # Initialize node first
        node._initialized = True
        
        # Test shutdown
        result = node.shutdown()
        
        assert result is True
        assert not node._initialized
            
    @pytest.mark.unit
    @pytest.mark.parametrize("mode,expected_type", [
        (ExecutionMode.PLUGIN, "plugin"),
        (ExecutionMode.LIBRARY, "library"),
        (None, "auto")
    ])
    def test_execution_modes(self, mode, expected_type):
        """Test different execution modes."""
        with patch('CellFrame.core.initialize_context') as mock_init:
            mock_init.return_value = self.mock_context
            
            node = CellframeNode(mode=mode)
            
            # Should call initialize_context with the provided mode
            if mode is not None:
                mock_init.assert_called_with(mode, None)
            else:
                mock_init.assert_called_with(None, None)


class TestCellframeNodeIntegration:
    """Integration tests for CellframeNode with its components."""
    
    @pytest.mark.integration
    def test_node_with_chain_component(self):
        """Test node integration with chain component."""
        mock_context = Mock(spec=AppContext)
        mock_context.is_plugin_mode = False
        mock_context.is_library_mode = True
        
        node = CellframeNode(context=mock_context)
        
        # Test chain component access
        assert hasattr(node, 'chain')
        assert node.chain is not None
        
    @pytest.mark.integration
    def test_node_initialization_sequence(self):
        """Test complete node initialization sequence."""
        mock_context = Mock(spec=AppContext)
        mock_context.initialize.return_value = True
        mock_context.is_plugin_mode = False
        mock_context.is_library_mode = True
        mock_context.mode = Mock(value="library")
        
        node = CellframeNode(context=mock_context)
        
        # Test initialization
        result = node.initialize()
        
        # Should return True for successful initialization
        assert result is True
        assert node.is_initialized()
        
    @pytest.mark.integration
    def test_node_error_propagation(self):
        """Test error propagation from components to node."""
        mock_context = Mock(spec=AppContext)
        mock_context.initialize.side_effect = CellframeException("Test error")
        
        node = CellframeNode(context=mock_context)
        
        # Should handle CellframeException gracefully
        result = node.initialize()
        assert result is False


class TestCellframeNodeEdgeCases:
    """Edge case tests for CellframeNode."""
    
    @pytest.mark.unit
    def test_node_with_invalid_mode(self):
        """Test node creation with invalid execution mode."""
        with patch('CellFrame.core.initialize_context') as mock_init:
            mock_init.side_effect = ValueError("Invalid mode")
            
            with pytest.raises(ValueError):
                CellframeNode(mode="invalid_mode")
                
    @pytest.mark.unit
    def test_node_multiple_initialization(self):
        """Test node behavior with multiple initialization calls."""
        mock_context = Mock(spec=AppContext)
        mock_context.initialize.return_value = True
        mock_context.mode = Mock(value="library")
        mock_context.is_plugin_mode = False
        mock_context.is_library_mode = True
        
        node = CellframeNode(context=mock_context)
        
        # Initialize twice
        result1 = node.initialize()
        result2 = node.initialize()
        
        assert result1 is True
        assert result2 is True  # Should be idempotent
        
    @pytest.mark.unit
    def test_node_with_none_context(self):
        """Test node behavior with None context."""
        # Node should create context automatically if None
        node = CellframeNode(context=None)
        assert node.context is not None
        assert hasattr(node, 'chain')
            
    @pytest.mark.unit
    def test_node_resource_cleanup(self):
        """Test node resource cleanup."""
        mock_context = Mock(spec=AppContext)
        mock_context.mode = Mock(value="library")
        node = CellframeNode(context=mock_context)
        
        # Add some state
        node._initialized = True
        node._chains = {"test": "chain"}
        
        # Cleanup
        node.shutdown()
        
        # Should clean up state
        assert not node._initialized
        
    @pytest.mark.unit
    def test_node_concurrent_access(self):
        """Test node thread safety."""
        import threading
        
        mock_context = Mock(spec=AppContext)
        node = CellframeNode(context=mock_context)
        
        results = []
        
        def access_node():
            try:
                status = node.get_status()
                results.append(status is not None)
            except Exception:
                results.append(False)
        
        # Create multiple threads
        threads = [threading.Thread(target=access_node) for _ in range(10)]
        
        # Start all threads
        for thread in threads:
            thread.start()
            
        # Wait for completion
        for thread in threads:
            thread.join()
            
        # All accesses should succeed
        assert all(results)
        assert len(results) == 10


if __name__ == "__main__":
    pytest.main([__file__, "-v"])