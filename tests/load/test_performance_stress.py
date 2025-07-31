"""
⚡ Load and Stress Tests for Python Cellframe SDK
Testing performance under high load and stress conditions.
"""

import pytest
import time
import threading
import multiprocessing
import psutil
import gc
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
from unittest.mock import Mock, patch
from memory_profiler import profile
import statistics

# Import the classes under test
try:
    from CellFrame.core import CellframeNode
    from CellFrame.chain import Wallet, TX, WalletType
    from CellFrame.core.context import LibContext
    from CellFrame.core.exceptions import CellframeException
except ImportError as e:
    pytest.skip(f"CellFrame modules not available: {e}", allow_module_level=True)


class TestLoadTesting:
    """Load testing for normal usage patterns under high volume."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.mock_context = Mock(spec=LibContext)
        self.mock_context.is_plugin_mode = False
        self.mock_context.is_library_mode = True
        self.mock_context.app_name = "load_test"
        self.mock_context.initialize.return_value = True
        
    @pytest.mark.load
    @pytest.mark.slow
    def test_concurrent_node_creation(self):
        """Test concurrent node creation under load."""
        num_threads = 50
        results = []
        errors = []
        
        def create_node():
            try:
                with patch('CellFrame.core.initialize_context') as mock_init:
                    mock_init.return_value = self.mock_context
                    node = CellframeNode()
                    results.append(node is not None)
                    time.sleep(0.01)  # Simulate some work
            except Exception as e:
                errors.append(str(e))
        
        # Create threads
        threads = [threading.Thread(target=create_node) for _ in range(num_threads)]
        
        # Measure time
        start_time = time.time()
        
        # Start all threads
        for thread in threads:
            thread.start()
        
        # Wait for completion
        for thread in threads:
            thread.join()
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Verify results
        assert len(results) == num_threads
        assert all(results)
        assert len(errors) == 0
        assert elapsed < 10.0  # Should complete within reasonable time
        
        print(f"Created {num_threads} nodes in {elapsed:.2f} seconds")
        print(f"Rate: {num_threads/elapsed:.2f} nodes/second")
        
    @pytest.mark.load
    @pytest.mark.slow
    @patch('CellFrame.chain.create_wallet')
    def test_high_volume_wallet_operations(self, mock_create):
        """Test high volume wallet operations."""
        # Setup mock
        def create_wallet_mock(name, *args):
            wallet = Mock(spec=Wallet)
            wallet.name = name
            wallet.get_address.return_value = f"addr_{name}"
            wallet.get_balance.return_value = 1000.0
            return wallet
        
        mock_create.side_effect = create_wallet_mock
        
        node = CellframeNode(context=self.mock_context)
        
        # High volume operations
        num_operations = 1000
        operations_times = []
        
        for i in range(num_operations):
            start = time.time()
            
            # Create wallet
            wallet = node.chain.create_wallet(f"wallet_{i}")
            
            # Get address and balance
            address = wallet.get_address()
            balance = wallet.get_balance("CELL")
            
            end = time.time()
            operations_times.append(end - start)
            
            # Basic assertions
            assert wallet is not None
            assert address.startswith("addr_")
            assert balance == 1000.0
        
        # Performance analysis
        total_time = sum(operations_times)
        avg_time = statistics.mean(operations_times)
        max_time = max(operations_times)
        min_time = min(operations_times)
        
        print(f"Completed {num_operations} operations in {total_time:.2f} seconds")
        print(f"Average time per operation: {avg_time*1000:.2f} ms")
        print(f"Min/Max time: {min_time*1000:.2f}/{max_time*1000:.2f} ms")
        print(f"Operations per second: {num_operations/total_time:.2f}")
        
        # Performance requirements
        assert avg_time < 0.01  # Average operation should be under 10ms
        assert total_time < 30.0  # Total should complete in under 30 seconds
        
    @pytest.mark.load
    @pytest.mark.slow
    def test_memory_usage_under_load(self):
        """Test memory usage under sustained load."""
        process = psutil.Process()
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        nodes = []
        memory_measurements = [initial_memory]
        
        # Create many nodes
        num_nodes = 100
        for i in range(num_nodes):
            with patch('CellFrame.core.initialize_context') as mock_init:
                mock_init.return_value = self.mock_context
                node = CellframeNode()
                nodes.append(node)
                
                # Measure memory every 10 nodes
                if i % 10 == 0:
                    current_memory = process.memory_info().rss / 1024 / 1024
                    memory_measurements.append(current_memory)
        
        # Final memory measurement
        final_memory = process.memory_info().rss / 1024 / 1024
        memory_measurements.append(final_memory)
        
        # Clean up
        del nodes
        gc.collect()
        
        cleanup_memory = process.memory_info().rss / 1024 / 1024
        
        print(f"Initial memory: {initial_memory:.2f} MB")
        print(f"Final memory: {final_memory:.2f} MB")
        print(f"After cleanup: {cleanup_memory:.2f} MB")
        print(f"Memory growth: {final_memory - initial_memory:.2f} MB")
        print(f"Per node: {(final_memory - initial_memory)/num_nodes:.3f} MB")
        
        # Memory requirements
        memory_growth = final_memory - initial_memory
        assert memory_growth < 500  # Should not use more than 500MB for 100 nodes
        assert memory_growth / num_nodes < 5  # Should not use more than 5MB per node


class TestStressTesting:
    """Stress testing for extreme conditions and edge cases."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.mock_context = Mock(spec=LibContext)
        self.mock_context.is_plugin_mode = False
        self.mock_context.is_library_mode = True
        self.mock_context.app_name = "stress_test"
        self.mock_context.initialize.return_value = True
        
    @pytest.mark.stress
    @pytest.mark.slow
    def test_extreme_concurrent_access(self):
        """Test extreme concurrent access patterns."""
        num_threads = 200
        operations_per_thread = 50
        results = []
        errors = []
        
        def stress_operations():
            try:
                with patch('CellFrame.core.initialize_context') as mock_init:
                    mock_init.return_value = self.mock_context
                    
                    for i in range(operations_per_thread):
                        # Create node
                        node = CellframeNode()
                        
                        # Get status multiple times
                        for _ in range(5):
                            status = node.get_status()
                            assert isinstance(status, dict)
                        
                        # Simulate cleanup
                        del node
                        
                    results.append(True)
                    
            except Exception as e:
                errors.append(str(e))
        
        # Create many threads
        threads = [threading.Thread(target=stress_operations) for _ in range(num_threads)]
        
        start_time = time.time()
        
        # Start all threads
        for thread in threads:
            thread.start()
        
        # Wait for completion
        for thread in threads:
            thread.join(timeout=30)  # 30 second timeout
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Results
        total_operations = num_threads * operations_per_thread
        print(f"Completed {total_operations} operations with {num_threads} threads")
        print(f"Total time: {elapsed:.2f} seconds")
        print(f"Operations per second: {total_operations/elapsed:.2f}")
        print(f"Errors: {len(errors)}")
        
        # Stress test requirements - more lenient than load tests
        assert len(results) >= num_threads * 0.95  # At least 95% success rate
        assert len(errors) < num_threads * 0.1  # Less than 10% error rate
        
    @pytest.mark.stress
    @pytest.mark.slow
    @patch('CellFrame.chain.create_wallet')
    def test_resource_exhaustion_resilience(self, mock_create):
        """Test resilience under resource exhaustion."""
        # Simulate resource exhaustion scenarios
        exhaustion_errors = [
            MemoryError("Out of memory"),
            OSError("Too many open files"),
            RuntimeError("Resource temporarily unavailable")
        ]
        
        # Mock that fails intermittently
        call_count = 0
        def failing_create_wallet(name, *args):
            nonlocal call_count
            call_count += 1
            
            # Fail every 10th call with different errors
            if call_count % 10 == 0:
                error_idx = (call_count // 10 - 1) % len(exhaustion_errors)
                raise exhaustion_errors[error_idx]
            
            wallet = Mock(spec=Wallet)
            wallet.name = name
            return wallet
        
        mock_create.side_effect = failing_create_wallet
        
        node = CellframeNode(context=self.mock_context)
        
        successful_operations = 0
        error_counts = {type(e).__name__: 0 for e in exhaustion_errors}
        
        # Attempt many operations
        num_attempts = 100
        for i in range(num_attempts):
            try:
                wallet = node.chain.create_wallet(f"stress_wallet_{i}")
                successful_operations += 1
            except (MemoryError, OSError, RuntimeError) as e:
                error_counts[type(e).__name__] += 1
            except Exception as e:
                # Unexpected error
                pytest.fail(f"Unexpected error: {e}")
        
        print(f"Successful operations: {successful_operations}/{num_attempts}")
        print(f"Error breakdown: {error_counts}")
        
        # Should handle errors gracefully
        assert successful_operations > 0
        assert successful_operations >= num_attempts * 0.8  # At least 80% success
        
    @pytest.mark.stress
    @pytest.mark.slow
    def test_rapid_node_lifecycle_stress(self):
        """Test rapid node creation and destruction."""
        num_cycles = 500
        cycle_times = []
        
        for i in range(num_cycles):
            start = time.time()
            
            # Create, use, and destroy node rapidly
            with patch('CellFrame.core.initialize_context') as mock_init:
                mock_init.return_value = self.mock_context
                
                node = CellframeNode()
                status = node.get_status()
                assert isinstance(status, dict)
                del node
            
            end = time.time()
            cycle_times.append(end - start)
            
            # Force garbage collection occasionally
            if i % 50 == 0:
                gc.collect()
        
        # Performance analysis
        avg_cycle_time = statistics.mean(cycle_times)
        max_cycle_time = max(cycle_times)
        total_time = sum(cycle_times)
        
        print(f"Completed {num_cycles} cycles in {total_time:.2f} seconds")
        print(f"Average cycle time: {avg_cycle_time*1000:.2f} ms")
        print(f"Max cycle time: {max_cycle_time*1000:.2f} ms")
        print(f"Cycles per second: {num_cycles/total_time:.2f}")
        
        # Stress requirements
        assert avg_cycle_time < 0.1  # Average cycle under 100ms
        assert max_cycle_time < 1.0  # No cycle over 1 second
        
    @pytest.mark.stress
    @pytest.mark.slow
    def test_multiprocess_stress(self):
        """Test stress across multiple processes."""
        num_processes = multiprocessing.cpu_count()
        operations_per_process = 20
        
        def process_stress_test(_):
            """Function to run in separate process."""
            try:
                with patch('CellFrame.core.initialize_context') as mock_init:
                    mock_context = Mock(spec=LibContext)
                    mock_context.is_plugin_mode = False
                    mock_context.is_library_mode = True
                    mock_context.app_name = "stress_process"
                    mock_context.initialize.return_value = True
                    mock_init.return_value = mock_context
                    
                    successful = 0
                    for i in range(operations_per_process):
                        node = CellframeNode()
                        status = node.get_status()
                        if isinstance(status, dict):
                            successful += 1
                        del node
                    
                    return successful
                    
            except Exception as e:
                return f"Error: {str(e)}"
        
        # Run across multiple processes
        with ProcessPoolExecutor(max_workers=num_processes) as executor:
            start_time = time.time()
            
            futures = [executor.submit(process_stress_test, i) for i in range(num_processes)]
            results = [future.result(timeout=30) for future in futures]
            
            end_time = time.time()
        
        elapsed = end_time - start_time
        
        # Analyze results
        successful_processes = sum(1 for r in results if isinstance(r, int))
        total_successful_ops = sum(r for r in results if isinstance(r, int))
        errors = [r for r in results if isinstance(r, str)]
        
        print(f"Processes: {num_processes}")
        print(f"Successful processes: {successful_processes}/{num_processes}")
        print(f"Total successful operations: {total_successful_ops}")
        print(f"Errors: {len(errors)}")
        print(f"Total time: {elapsed:.2f} seconds")
        
        # Multiprocess stress requirements
        assert successful_processes >= num_processes * 0.8  # 80% of processes succeed
        assert len(errors) <= num_processes * 0.2  # Max 20% error rate


class TestScalabilityTesting:
    """Scalability testing for large-scale operations."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.mock_context = Mock(spec=LibContext)
        self.mock_context.is_plugin_mode = False
        self.mock_context.is_library_mode = True
        self.mock_context.app_name = "scalability_test"
        self.mock_context.initialize.return_value = True
        
    @pytest.mark.scalability
    @pytest.mark.slow
    @patch('CellFrame.chain.get_all_wallets')
    def test_large_dataset_handling(self, mock_get_all):
        """Test handling of large datasets."""
        # Create large wallet dataset
        dataset_sizes = [100, 1000, 10000]
        performance_data = {}
        
        for size in dataset_sizes:
            # Generate large wallet list
            wallets = []
            for i in range(size):
                wallet = Mock(spec=Wallet)
                wallet.name = f"wallet_{i:06d}"
                wallet.get_address.return_value = f"addr_{i:06d}"
                wallet.get_balance.return_value = 1000.0
                wallets.append(wallet)
            
            mock_get_all.return_value = wallets
            
            node = CellframeNode(context=self.mock_context)
            
            # Measure retrieval time
            start_time = time.time()
            retrieved_wallets = node.chain.get_all_wallets()
            end_time = time.time()
            
            elapsed = end_time - start_time
            performance_data[size] = {
                'time': elapsed,
                'wallets_per_second': size / elapsed if elapsed > 0 else float('inf')
            }
            
            # Verify correctness
            assert len(retrieved_wallets) == size
            
            print(f"Dataset size {size}: {elapsed:.3f}s ({size/elapsed:.0f} wallets/sec)")
        
        # Scalability analysis
        # Performance should not degrade drastically with size
        for i in range(1, len(dataset_sizes)):
            prev_size = dataset_sizes[i-1]
            curr_size = dataset_sizes[i]
            
            prev_rate = performance_data[prev_size]['wallets_per_second']
            curr_rate = performance_data[curr_size]['wallets_per_second']
            
            # Rate should not drop by more than 50% when dataset grows 10x
            rate_ratio = curr_rate / prev_rate
            print(f"Rate ratio {prev_size}->{curr_size}: {rate_ratio:.2f}")
            
            assert rate_ratio > 0.5, f"Performance degraded too much: {rate_ratio}"
    
    @pytest.mark.scalability
    @pytest.mark.slow
    def test_memory_scalability(self):
        """Test memory usage scalability."""
        process = psutil.Process()
        base_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        node_counts = [10, 50, 100]
        memory_per_node = []
        
        for count in node_counts:
            nodes = []
            
            # Create nodes
            for i in range(count):
                with patch('CellFrame.core.initialize_context') as mock_init:
                    mock_init.return_value = self.mock_context
                    node = CellframeNode()
                    nodes.append(node)
            
            # Measure memory
            current_memory = process.memory_info().rss / 1024 / 1024
            memory_used = current_memory - base_memory
            memory_per_node.append(memory_used / count)
            
            print(f"{count} nodes: {memory_used:.2f} MB total, {memory_used/count:.3f} MB per node")
            
            # Cleanup
            del nodes
            gc.collect()
        
        # Memory usage should be relatively consistent per node
        avg_memory_per_node = statistics.mean(memory_per_node)
        max_deviation = max(abs(m - avg_memory_per_node) for m in memory_per_node)
        
        print(f"Average memory per node: {avg_memory_per_node:.3f} MB")
        print(f"Max deviation: {max_deviation:.3f} MB")
        
        # Memory usage should be consistent (within 50% deviation)
        assert max_deviation < avg_memory_per_node * 0.5


if __name__ == "__main__":
    pytest.main([__file__, "-v", "-s"])