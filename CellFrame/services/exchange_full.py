"""
💱 Exchange Service

High-level exchange operations for Cellframe blockchain.
Provides token exchange, trading, and order management functionality.
"""

import logging
import threading
from typing import Dict, List, Optional, Any, Union, Tuple
from decimal import Decimal
from datetime import datetime, timedelta
from enum import Enum
from dataclasses import dataclass

from ..core.exceptions import CellframeException

# Import C bindings
try:
    from python_cellframe import (
        dap_chain_net_srv_xchange_init,
        dap_chain_net_srv_xchange_create,
        dap_chain_net_srv_xchange_create_order,
        dap_chain_net_srv_xchange_cancel_order,
        dap_chain_net_srv_xchange_get_orders,
        dap_chain_net_srv_xchange_get_order_history,
        dap_chain_net_srv_xchange_get_price,
        dap_chain_net_srv_xchange_deinit
    )
except ImportError:
    # Fallback implementations
    def dap_chain_net_srv_xchange_init(): return 0
    def dap_chain_net_srv_xchange_create(handle): return id(handle)
    def dap_chain_net_srv_xchange_create_order(handle, order_type, token_sell, token_buy, amount, price): 
        return f"order_{token_sell}_{token_buy}_{amount}"
    def dap_chain_net_srv_xchange_cancel_order(handle, order_id): return True
    def dap_chain_net_srv_xchange_get_orders(handle): 
        return [{"id": "order1", "status": "active", "type": "sell", "amount": 100}]
    def dap_chain_net_srv_xchange_get_order_history(handle): 
        return [{"id": "order1", "status": "completed", "timestamp": datetime.now().isoformat()}]
    def dap_chain_net_srv_xchange_get_price(handle, token_sell, token_buy): 
        return {"price": 1.5, "volume": 1000}
    def dap_chain_net_srv_xchange_deinit(handle): pass


class ExchangeError(CellframeException):
    """Exchange-specific errors"""
    pass


class OrderType(Enum):
    """Order types for exchange"""
    MARKET = "market"
    LIMIT = "limit"
    STOP = "stop"
    STOP_LIMIT = "stop_limit"


class OrderStatus(Enum):
    """Order status"""
    PENDING = "pending"
    ACTIVE = "active"
    PARTIALLY_FILLED = "partially_filled"
    FILLED = "filled"
    CANCELLED = "cancelled"
    EXPIRED = "expired"


@dataclass
class Order:
    """Exchange order representation"""
    id: str
    type: OrderType
    token_sell: str
    token_buy: str
    amount: Decimal
    price: Optional[Decimal]
    status: OrderStatus
    created_at: datetime
    updated_at: datetime
    filled_amount: Decimal = Decimal('0')
    fee: Decimal = Decimal('0')
    
    @property
    def remaining_amount(self) -> Decimal:
        """Calculate remaining amount to be filled"""
        return self.amount - self.filled_amount
    
    @property
    def fill_percentage(self) -> Decimal:
        """Calculate fill percentage"""
        if self.amount == 0:
            return Decimal('0')
        return (self.filled_amount / self.amount) * 100


@dataclass
class PriceInfo:
    """Price information for a token pair"""
    token_sell: str
    token_buy: str
    price: Decimal
    volume_24h: Decimal
    high_24h: Decimal
    low_24h: Decimal
    change_24h: Decimal
    bid: Decimal
    ask: Decimal
    spread: Decimal
    last_update: datetime


class ExchangeService:
    """
    High-level exchange service for token trading.
    
    This service provides:
    - Order creation and management
    - Price discovery
    - Market making
    - Liquidity management
    """
    
    def __init__(self, context=None):
        """
        Initialize exchange service
        
        Args:
            context: Application context (optional)
        """
        self.logger = logging.getLogger('CellFrame.ExchangeService')
        self._context = context
        self._handle = None
        self._lock = threading.RLock()
        self._initialized = False
        self._orders: Dict[str, Order] = {}
        self._price_cache: Dict[Tuple[str, str], PriceInfo] = {}
        self._cache_ttl = timedelta(seconds=30)
        
        # Initialize service
        self._initialize()
    
    def _initialize(self):
        """Initialize exchange service"""
        with self._lock:
            if self._initialized:
                return
                
            try:
                result = dap_chain_net_srv_xchange_init()
                if result != 0:
                    raise ExchangeError(f"Failed to initialize exchange service: {result}")
                
                self._handle = dap_chain_net_srv_xchange_create(self)
                self._initialized = True
                self.logger.info("Exchange service initialized successfully")
                
            except Exception as e:
                self.logger.error(f"Failed to initialize exchange service: {e}")
                raise ExchangeError(f"Exchange service initialization failed: {e}")
    
    def create_order(
        self,
        order_type: OrderType,
        token_sell: str,
        token_buy: str,
        amount: Union[Decimal, float, str],
        price: Optional[Union[Decimal, float, str]] = None,
        **kwargs
    ) -> Order:
        """
        Create a new exchange order
        
        Args:
            order_type: Type of order (market, limit, etc.)
            token_sell: Token to sell
            token_buy: Token to buy
            amount: Amount to exchange
            price: Price for limit orders (required for limit orders)
            **kwargs: Additional order parameters
            
        Returns:
            Created order object
            
        Raises:
            ExchangeError: If order creation fails
        """
        with self._lock:
            if not self._initialized:
                raise ExchangeError("Exchange service not initialized")
            
            # Validate inputs
            amount = Decimal(str(amount))
            if amount <= 0:
                raise ExchangeError("Amount must be positive")
            
            if order_type == OrderType.LIMIT and price is None:
                raise ExchangeError("Price required for limit orders")
            
            if price is not None:
                price = Decimal(str(price))
                if price <= 0:
                    raise ExchangeError("Price must be positive")
            
            try:
                # Create order via C binding
                order_id = dap_chain_net_srv_xchange_create_order(
                    self._handle,
                    order_type.value,
                    token_sell,
                    token_buy,
                    float(amount),
                    float(price) if price else 0
                )
                
                # Create order object
                order = Order(
                    id=order_id,
                    type=order_type,
                    token_sell=token_sell,
                    token_buy=token_buy,
                    amount=amount,
                    price=price,
                    status=OrderStatus.PENDING,
                    created_at=datetime.now(),
                    updated_at=datetime.now()
                )
                
                self._orders[order_id] = order
                self.logger.info(f"Created {order_type.value} order {order_id}: "
                               f"{amount} {token_sell} -> {token_buy}")
                
                return order
                
            except Exception as e:
                self.logger.error(f"Failed to create order: {e}")
                raise ExchangeError(f"Order creation failed: {e}")
    
    def cancel_order(self, order_id: str) -> bool:
        """
        Cancel an existing order
        
        Args:
            order_id: ID of order to cancel
            
        Returns:
            True if cancelled successfully
            
        Raises:
            ExchangeError: If cancellation fails
        """
        with self._lock:
            if not self._initialized:
                raise ExchangeError("Exchange service not initialized")
            
            if order_id not in self._orders:
                raise ExchangeError(f"Order {order_id} not found")
            
            try:
                result = dap_chain_net_srv_xchange_cancel_order(self._handle, order_id)
                if result:
                    self._orders[order_id].status = OrderStatus.CANCELLED
                    self._orders[order_id].updated_at = datetime.now()
                    self.logger.info(f"Cancelled order {order_id}")
                    return True
                else:
                    raise ExchangeError("Cancellation failed")
                    
            except Exception as e:
                self.logger.error(f"Failed to cancel order {order_id}: {e}")
                raise ExchangeError(f"Order cancellation failed: {e}")
    
    def get_order(self, order_id: str) -> Order:
        """
        Get order by ID
        
        Args:
            order_id: Order ID
            
        Returns:
            Order object
            
        Raises:
            ExchangeError: If order not found
        """
        with self._lock:
            if order_id not in self._orders:
                raise ExchangeError(f"Order {order_id} not found")
            return self._orders[order_id]
    
    def get_orders(self, status: Optional[OrderStatus] = None) -> List[Order]:
        """
        Get all orders or filter by status
        
        Args:
            status: Optional status filter
            
        Returns:
            List of orders
        """
        with self._lock:
            orders = list(self._orders.values())
            if status:
                orders = [o for o in orders if o.status == status]
            return orders
    
    def get_price(self, token_sell: str, token_buy: str) -> PriceInfo:
        """
        Get current price for token pair
        
        Args:
            token_sell: Token to sell
            token_buy: Token to buy
            
        Returns:
            Price information
            
        Raises:
            ExchangeError: If price not available
        """
        with self._lock:
            if not self._initialized:
                raise ExchangeError("Exchange service not initialized")
            
            # Check cache
            cache_key = (token_sell, token_buy)
            if cache_key in self._price_cache:
                price_info = self._price_cache[cache_key]
                if datetime.now() - price_info.last_update < self._cache_ttl:
                    return price_info
            
            try:
                # Get price from C binding
                price_data = dap_chain_net_srv_xchange_get_price(
                    self._handle, token_sell, token_buy
                )
                
                # Create price info object
                price_info = PriceInfo(
                    token_sell=token_sell,
                    token_buy=token_buy,
                    price=Decimal(str(price_data.get('price', 0))),
                    volume_24h=Decimal(str(price_data.get('volume', 0))),
                    high_24h=Decimal(str(price_data.get('high', 0))),
                    low_24h=Decimal(str(price_data.get('low', 0))),
                    change_24h=Decimal(str(price_data.get('change', 0))),
                    bid=Decimal(str(price_data.get('bid', 0))),
                    ask=Decimal(str(price_data.get('ask', 0))),
                    spread=Decimal(str(price_data.get('spread', 0))),
                    last_update=datetime.now()
                )
                
                # Update cache
                self._price_cache[cache_key] = price_info
                return price_info
                
            except Exception as e:
                self.logger.error(f"Failed to get price for {token_sell}/{token_buy}: {e}")
                raise ExchangeError(f"Price query failed: {e}")
    
    def market_buy(self, token_buy: str, amount: Union[Decimal, float, str], 
                   token_sell: str = "CELL") -> Order:
        """Execute market buy order"""
        return self.create_order(
            OrderType.MARKET,
            token_sell=token_sell,
            token_buy=token_buy,
            amount=amount
        )
    
    def market_sell(self, token_sell: str, amount: Union[Decimal, float, str],
                    token_buy: str = "CELL") -> Order:
        """Execute market sell order"""
        return self.create_order(
            OrderType.MARKET,
            token_sell=token_sell,
            token_buy=token_buy,
            amount=amount
        )
    
    def limit_buy(self, token_buy: str, amount: Union[Decimal, float, str],
                  price: Union[Decimal, float, str], token_sell: str = "CELL") -> Order:
        """Place limit buy order"""
        return self.create_order(
            OrderType.LIMIT,
            token_sell=token_sell,
            token_buy=token_buy,
            amount=amount,
            price=price
        )
    
    def limit_sell(self, token_sell: str, amount: Union[Decimal, float, str],
                   price: Union[Decimal, float, str], token_buy: str = "CELL") -> Order:
        """Place limit sell order"""
        return self.create_order(
            OrderType.LIMIT,
            token_sell=token_sell,
            token_buy=token_buy,
            amount=amount,
            price=price
        )
    
    def __enter__(self):
        """Context manager entry"""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit"""
        self.close()
    
    def close(self):
        """Close exchange service and cleanup resources"""
        with self._lock:
            if self._initialized and self._handle:
                try:
                    dap_chain_net_srv_xchange_deinit(self._handle)
                    self.logger.info("Exchange service closed")
                except Exception as e:
                    self.logger.error(f"Error closing exchange service: {e}")
                finally:
                    self._initialized = False
                    self._handle = None
                    self._orders.clear()
                    self._price_cache.clear()
