"""
🏭 Mock Factories for Complex Test Objects
Factory functions for creating consistent mock objects across tests.
"""

from unittest.mock import Mock, MagicMock
from decimal import Decimal
from datetime import datetime, timedelta
from typing import Dict, Any, List, Optional
import secrets
import string


class MockWalletFactory:
    """Factory for creating mock wallets with consistent data."""
    
    @staticmethod
    def create_simple_wallet(name: str = None, balance: Decimal = None) -> Mock:
        """Create a simple mock wallet."""
        name = name or f"wallet_{secrets.token_hex(4)}"
        balance = balance or Decimal("1000.0")
        
        wallet = Mock()
        wallet.name = name
        wallet.type = "simple"
        wallet.get_address.return_value = f"addr_{name}_{secrets.token_hex(8)}"
        wallet.get_balance.return_value = balance
        wallet.save.return_value = True
        wallet.is_valid = True
        wallet.created_at = datetime.now()
        
        return wallet
    
    @staticmethod
    def create_hd_wallet(name: str = None, balance: Decimal = None) -> Mock:
        """Create an HD mock wallet."""
        name = name or f"hd_wallet_{secrets.token_hex(4)}"
        balance = balance or Decimal("5000.0")
        
        wallet = Mock()
        wallet.name = name
        wallet.type = "hd"
        wallet.get_address.return_value = f"hd_addr_{name}_{secrets.token_hex(8)}"
        wallet.get_balance.return_value = balance
        wallet.can_derive_addresses = True
        wallet.max_addresses = 1000
        wallet.save.return_value = True
        wallet.is_valid = True
        wallet.created_at = datetime.now()
        
        # HD-specific methods
        def derive_address(index: int = None):
            index = index or secrets.randbelow(1000)
            return f"derived_{name}_{index}_{secrets.token_hex(6)}"
        
        wallet.derive_address.side_effect = derive_address
        
        return wallet
    
    @staticmethod
    def create_wallet_list(count: int = 5, prefix: str = "test") -> List[Mock]:
        """Create a list of mock wallets."""
        wallets = []
        for i in range(count):
            wallet_type = "hd" if i % 2 == 0 else "simple"
            if wallet_type == "hd":
                wallet = MockWalletFactory.create_hd_wallet(f"{prefix}_hd_{i}")
            else:
                wallet = MockWalletFactory.create_simple_wallet(f"{prefix}_simple_{i}")
            wallets.append(wallet)
        return wallets


class MockTransactionFactory:
    """Factory for creating mock transactions with realistic data."""
    
    @staticmethod
    def create_pending_transaction(amount: Decimal = None, fee: Decimal = None) -> Mock:
        """Create a pending transaction."""
        amount = amount or Decimal("100.0")
        fee = fee or Decimal("1.0")
        
        tx = Mock()
        tx.hash = f"pending_tx_{secrets.token_hex(16)}"
        tx.type = "transfer"
        tx.status = "pending"
        tx.amount = amount
        tx.fee = fee
        tx.timestamp = datetime.now()
        tx.confirmations = 0
        
        # Mock methods
        tx.add_input.return_value = True
        tx.add_output.return_value = True
        tx.sign.return_value = True
        tx.verify.return_value = True
        tx.get_size.return_value = secrets.randbelow(500) + 200  # 200-700 bytes
        
        return tx
    
    @staticmethod
    def create_confirmed_transaction(amount: Decimal = None, confirmations: int = 6) -> Mock:
        """Create a confirmed transaction."""
        amount = amount or Decimal("200.0")
        
        tx = Mock()
        tx.hash = f"confirmed_tx_{secrets.token_hex(16)}"
        tx.type = "transfer"
        tx.status = "confirmed"
        tx.amount = amount
        tx.fee = Decimal("2.0")
        tx.timestamp = datetime.now() - timedelta(hours=confirmations)
        tx.confirmations = confirmations
        
        # Mock methods
        tx.add_input.return_value = True
        tx.add_output.return_value = True
        tx.sign.return_value = True
        tx.verify.return_value = True
        tx.get_size.return_value = secrets.randbelow(300) + 300  # 300-600 bytes
        
        return tx
    
    @staticmethod
    def create_failed_transaction(reason: str = "Insufficient balance") -> Mock:
        """Create a failed transaction."""
        tx = Mock()
        tx.hash = f"failed_tx_{secrets.token_hex(16)}"
        tx.type = "transfer"
        tx.status = "failed"
        tx.amount = Decimal("0.0")
        tx.fee = Decimal("0.0")
        tx.timestamp = datetime.now()
        tx.error_reason = reason
        tx.confirmations = 0
        
        # Mock methods that fail
        tx.add_input.return_value = False
        tx.add_output.return_value = False
        tx.sign.return_value = False
        tx.verify.return_value = False
        tx.get_size.return_value = 0
        
        return tx
    
    @staticmethod
    def create_transaction_batch(count: int = 10, status_mix: bool = True) -> List[Mock]:
        """Create a batch of transactions with mixed statuses."""
        transactions = []
        
        for i in range(count):
            if status_mix:
                if i % 3 == 0:
                    tx = MockTransactionFactory.create_confirmed_transaction(
                        amount=Decimal(str(100 + i * 10))
                    )
                elif i % 3 == 1:
                    tx = MockTransactionFactory.create_pending_transaction(
                        amount=Decimal(str(50 + i * 5))
                    )
                else:
                    tx = MockTransactionFactory.create_failed_transaction()
            else:
                tx = MockTransactionFactory.create_confirmed_transaction(
                    amount=Decimal(str(100 + i * 10))
                )
                
            transactions.append(tx)
        
        return transactions


class MockNetworkFactory:
    """Factory for creating mock network objects."""
    
    @staticmethod
    def create_network(name: str = "testnet", status: str = "active") -> Dict[str, Any]:
        """Create a mock network configuration."""
        return {
            "name": name,
            "chain_id": f"0x{secrets.randbelow(999) + 1}",
            "status": status,
            "nodes": [f"node{i}.{name}.com" for i in range(1, 6)],
            "consensus": "proof_of_stake",
            "block_time": 5,  # seconds
            "created_at": datetime.now().isoformat(),
            "total_supply": str(Decimal("1000000000")),  # 1B tokens
            "circulating_supply": str(Decimal("500000000"))  # 500M tokens
        }
    
    @staticmethod
    def create_node_info(node_id: str = None) -> Dict[str, Any]:
        """Create mock node information."""
        node_id = node_id or f"node_{secrets.token_hex(8)}"
        
        return {
            "id": node_id,
            "address": f"{node_id}.cellframe.net",
            "port": secrets.randbelow(9000) + 1000,
            "version": "2.0.0",
            "status": "online",
            "uptime": secrets.randbelow(86400),  # Up to 24 hours
            "connections": secrets.randbelow(100),
            "last_seen": datetime.now().isoformat(),
            "stake": str(Decimal(str(secrets.randbelow(100000) + 1000))),
            "is_validator": secrets.choice([True, False])
        }


class MockServiceFactory:
    """Factory for creating mock service objects."""
    
    @staticmethod
    def create_staking_service() -> Mock:
        """Create a mock staking service."""
        service = Mock()
        
        def mock_stake(amount: Decimal, validator: str):
            return {
                "stake_id": f"stake_{secrets.token_hex(8)}",
                "amount": amount,
                "validator": validator,
                "timestamp": datetime.now().isoformat(),
                "annual_rate": Decimal("0.05"),
                "expected_rewards": amount * Decimal("0.05") / 365 * 30  # 30 days
            }
        
        def mock_unstake(stake_id: str):
            return {
                "stake_id": stake_id,
                "unstaked": True,
                "amount": Decimal(str(secrets.randbelow(10000) + 1000)),
                "timestamp": datetime.now().isoformat(),
                "cooldown_ends": (datetime.now() + timedelta(days=7)).isoformat()
            }
        
        def mock_get_rewards(stake_id: str):
            return {
                "stake_id": stake_id,
                "rewards": Decimal(str(secrets.randbelow(100) + 10)),
                "period_days": 30,
                "annual_rate": Decimal("0.05"),
                "last_reward": datetime.now().isoformat()
            }
        
        service.stake.side_effect = mock_stake
        service.unstake.side_effect = mock_unstake
        service.get_rewards.side_effect = mock_get_rewards
        service.get_validators.return_value = [
            {"id": f"validator_{i}", "stake": Decimal(str(100000 + i * 10000))}
            for i in range(1, 6)
        ]
        
        return service
    
    @staticmethod
    def create_exchange_service() -> Mock:
        """Create a mock exchange service."""
        service = Mock()
        
        def mock_create_order(order_type: str, amount: Decimal, price: Decimal, token_pair: str):
            return {
                "order_id": f"order_{secrets.token_hex(8)}",
                "type": order_type,
                "amount": amount,
                "price": price,
                "token_pair": token_pair,
                "status": "open",
                "created_at": datetime.now().isoformat()
            }
        
        def mock_cancel_order(order_id: str):
            return {
                "order_id": order_id,
                "status": "cancelled",
                "cancelled_at": datetime.now().isoformat()
            }
        
        service.create_order.side_effect = mock_create_order
        service.cancel_order.side_effect = mock_cancel_order
        service.get_order_book.return_value = {
            "bids": [{"price": Decimal("1.0"), "amount": Decimal("100.0")}],
            "asks": [{"price": Decimal("1.1"), "amount": Decimal("200.0")}]
        }
        
        return service


class MockContextFactory:
    """Factory for creating mock context objects."""
    
    @staticmethod
    def create_lib_context(app_name: str = "test_app") -> Mock:
        """Create a mock library context."""
        context = Mock()
        context.is_plugin_mode = False
        context.is_library_mode = True
        context.app_name = app_name
        context.execution_mode = "library"
        context.initialize.return_value = True
        context.shutdown.return_value = None
        context.get_resource.return_value = None
        context.get_data_dir.return_value = f"/tmp/{app_name}/data"
        context.get_config_dir.return_value = f"/tmp/{app_name}/config"
        context.get_config.return_value = {"debug": True}
        return context
    
    @staticmethod
    def create_plugin_context(plugin_name: str = "test_plugin") -> Mock:
        """Create a mock plugin context."""
        context = Mock()
        context.is_plugin_mode = True
        context.is_library_mode = False
        context.app_name = plugin_name
        context.execution_mode = "plugin"
        context.initialize.return_value = True
        context.shutdown.return_value = None
        context.get_resource.return_value = None
        context.get_plugin_manifest.return_value = {
            "name": plugin_name,
            "version": "1.0.0",
            "author": "Test Author",
            "description": "Test plugin"
        }
        context.get_node_config.return_value = {
            "debug": True,
            "network": "testnet"
        }
        return context


class MockDataFactory:
    """Factory for creating mock test data."""
    
    @staticmethod
    def create_address(prefix: str = "addr") -> str:
        """Create a mock address."""
        return f"{prefix}_{secrets.token_hex(20)}"
    
    @staticmethod
    def create_hash(prefix: str = "hash") -> str:
        """Create a mock hash."""
        return f"{prefix}_{secrets.token_hex(32)}"
    
    @staticmethod
    def create_token_data(symbol: str = "TEST") -> Dict[str, Any]:
        """Create mock token data."""
        return {
            "symbol": symbol,
            "name": f"{symbol} Token",
            "decimals": 18,
            "total_supply": str(Decimal("1000000000")),
            "contract_address": MockDataFactory.create_address("token"),
            "created_at": datetime.now().isoformat()
        }
    
    @staticmethod
    def create_block_data(height: int = None) -> Dict[str, Any]:
        """Create mock block data."""
        height = height or secrets.randbelow(1000000)
        
        return {
            "height": height,
            "hash": MockDataFactory.create_hash("block"),
            "parent_hash": MockDataFactory.create_hash("parent"),
            "timestamp": datetime.now().isoformat(),
            "transactions": [
                MockDataFactory.create_hash("tx") 
                for _ in range(secrets.randbelow(10) + 1)
            ],
            "validator": f"validator_{secrets.token_hex(4)}",
            "size": secrets.randbelow(1000000) + 100000  # 100KB - 1MB
        }
    
    @staticmethod
    def create_random_string(length: int = 10) -> str:
        """Create a random string."""
        return ''.join(secrets.choice(string.ascii_letters + string.digits) for _ in range(length))
    
    @staticmethod
    def create_random_decimal(min_val: float = 0.0, max_val: float = 10000.0) -> Decimal:
        """Create a random decimal value."""
        return Decimal(str(round(secrets.randbelow(int(max_val - min_val) * 100) / 100.0 + min_val, 2)))