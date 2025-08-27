"""
📋 Order Service

High-level order management operations for Cellframe blockchain.
Provides order creation, matching, and fulfillment functionality.
"""

import logging
import threading
from typing import Dict, List, Optional, Any, Union, Callable
from decimal import Decimal
from datetime import datetime, timedelta
from enum import Enum
from dataclasses import dataclass

from ..core.exceptions import CellframeException

# Import C bindings
try:
    from python_cellframe import (
        dap_chain_net_srv_order_init,
        dap_chain_net_srv_order_create,
        dap_chain_net_srv_order_create_order,
        dap_chain_net_srv_order_accept_order,
        dap_chain_net_srv_order_reject_order,
        dap_chain_net_srv_order_complete_order,
        dap_chain_net_srv_order_get_orders,
        dap_chain_net_srv_order_get_matches,
        dap_chain_net_srv_order_deinit
    )
except ImportError as e:
    raise ImportError(
        "❌ CRITICAL: Native CellFrame order functions not available!\n"
        "This is a Python bindings library - native C extension is required.\n"
        "Required: dap_chain_net_srv_order_* functions must be implemented in native C extension.\n"
        f"Original error: {e}\n"
        "Please implement order bindings in src/cellframe_services.c"
    ) from e

# No wrapper functions - use cf_native directly everywhere


class OrderError(CellframeException):
    """Order-specific errors"""
    pass


class OrderType(Enum):
    """Types of orders"""
    BUY = "buy"
    SELL = "sell"
    SERVICE = "service"
    EXCHANGE = "exchange"
    ESCROW = "escrow"
    AUCTION = "auction"
    REQUEST = "request"


class OrderStatus(Enum):
    """Order status"""
    DRAFT = "draft"
    PENDING = "pending"
    MATCHED = "matched"
    ACCEPTED = "accepted"
    IN_PROGRESS = "in_progress"
    COMPLETED = "completed"
    CANCELLED = "cancelled"
    REJECTED = "rejected"
    DISPUTED = "disputed"


class FulfillmentStatus(Enum):
    """Order fulfillment status"""
    NOT_STARTED = "not_started"
    IN_PROGRESS = "in_progress"
    DELIVERED = "delivered"
    CONFIRMED = "confirmed"
    DISPUTED = "disputed"


@dataclass
class OrderCondition:
    """Condition for order execution"""
    type: str
    parameter: str
    value: Any
    operator: str = "eq"  # eq, gt, lt, gte, lte, ne
    
    def evaluate(self, context: Dict[str, Any]) -> bool:
        """Evaluate if condition is met"""
        actual_value = context.get(self.parameter)
        if actual_value is None:
            return False
        
        if self.operator == "eq":
            return actual_value == self.value
        elif self.operator == "gt":
            return actual_value > self.value
        elif self.operator == "lt":
            return actual_value < self.value
        elif self.operator == "gte":
            return actual_value >= self.value
        elif self.operator == "lte":
            return actual_value <= self.value
        elif self.operator == "ne":
            return actual_value != self.value
        else:
            return False


@dataclass
class Order:
    """Order representation"""
    id: str
    type: OrderType
    status: OrderStatus
    creator: str
    description: str
    amount: Decimal
    price: Optional[Decimal]
    token: str
    created_at: datetime
    updated_at: datetime
    expires_at: Optional[datetime] = None
    conditions: List[OrderCondition] = None
    metadata: Dict[str, Any] = None
    fulfillment_status: FulfillmentStatus = FulfillmentStatus.NOT_STARTED
    acceptor: Optional[str] = None
    escrow_amount: Optional[Decimal] = None
    
    @property
    def is_expired(self) -> bool:
        """Check if order has expired"""
        if self.expires_at is None:
            return False
        return datetime.now() > self.expires_at
    
    @property
    def total_value(self) -> Decimal:
        """Calculate total order value"""
        if self.price:
            return self.amount * self.price
        return self.amount
    
    def can_match(self, other: 'Order') -> bool:
        """Check if this order can match with another"""
        # Basic matching logic
        if self.type == OrderType.BUY and other.type == OrderType.SELL:
            return (self.token == other.token and 
                    self.price >= other.price and
                    not self.is_expired and 
                    not other.is_expired)
        elif self.type == OrderType.SELL and other.type == OrderType.BUY:
            return other.can_match(self)
        return False


@dataclass
class OrderMatch:
    """Order match representation"""
    order_id: str
    match_id: str
    match_score: float
    match_type: str
    conditions_met: bool
    timestamp: datetime


class OrderService:
    """
    High-level order service for order management.
    
    This service provides:
    - Order creation and management
    - Order matching and fulfillment
    - Escrow functionality
    - Condition evaluation
    """
    
    def __init__(self, context=None):
        """
        Initialize order service
        
        Args:
            context: Application context (optional)
        """
        self.logger = logging.getLogger('CellFrame.OrderService')
        self._context = context
        self._handle = None
        self._lock = threading.RLock()
        self._initialized = False
        self._orders: Dict[str, Order] = {}
        self._matches: Dict[str, List[OrderMatch]] = {}
        self._match_callbacks: List[Callable] = []
        
        # Initialize service
        self._initialize()
    
    def _initialize(self):
        """Initialize order service"""
        with self._lock:
            if self._initialized:
                return
                
            try:
                result = dap_chain_net_srv_order_init()
                if result != 0:
                    raise OrderError(f"Failed to initialize order service: {result}")
                
                self._handle = dap_chain_net_srv_order_create(self)
                self._initialized = True
                self.logger.info("Order service initialized successfully")
                
            except Exception as e:
                self.logger.error(f"Failed to initialize order service: {e}")
                raise OrderError(f"Order service initialization failed: {e}")
    
    def create_order(
        self,
        order_type: OrderType,
        description: str,
        amount: Union[Decimal, float, str],
        token: str,
        price: Optional[Union[Decimal, float, str]] = None,
        expires_in: Optional[timedelta] = None,
        conditions: Optional[List[OrderCondition]] = None,
        metadata: Optional[Dict[str, Any]] = None,
        **kwargs
    ) -> Order:
        """
        Create a new order
        
        Args:
            order_type: Type of order
            description: Order description
            amount: Order amount
            token: Token for the order
            price: Price per unit (optional)
            expires_in: Expiration duration (optional)
            conditions: Order conditions (optional)
            metadata: Additional metadata (optional)
            **kwargs: Additional order parameters
            
        Returns:
            Created order object
            
        Raises:
            OrderError: If order creation fails
        """
        with self._lock:
            if not self._initialized:
                raise OrderError("Order service not initialized")
            
            # Validate inputs
            amount = Decimal(str(amount))
            if amount <= 0:
                raise OrderError("Amount must be positive")
            
            if price is not None:
                price = Decimal(str(price))
                if price <= 0:
                    raise OrderError("Price must be positive")
            
            try:
                # Prepare conditions for C binding
                conditions_data = []
                if conditions:
                    for cond in conditions:
                        conditions_data.append({
                            'type': cond.type,
                            'parameter': cond.parameter,
                            'value': str(cond.value),
                            'operator': cond.operator
                        })
                
                # Create order via C binding
                order_id = dap_chain_net_srv_order_create_order(
                    self._handle,
                    order_type.value,
                    description,
                    float(amount),
                    float(price) if price else 0,
                    conditions_data
                )
                
                # Create order object
                now = datetime.now()
                order = Order(
                    id=order_id,
                    type=order_type,
                    status=OrderStatus.PENDING,
                    creator=kwargs.get('creator', 'system'),
                    description=description,
                    amount=amount,
                    price=price,
                    token=token,
                    created_at=now,
                    updated_at=now,
                    expires_at=now + expires_in if expires_in else None,
                    conditions=conditions or [],
                    metadata=metadata or {}
                )
                
                self._orders[order_id] = order
                self.logger.info(f"Created {order_type.value} order {order_id}: {description}")
                
                # Trigger matching
                self._find_matches(order)
                
                return order
                
            except Exception as e:
                self.logger.error(f"Failed to create order: {e}")
                raise OrderError(f"Order creation failed: {e}")
    
    def accept_order(self, order_id: str, acceptor: str) -> Dict[str, Any]:
        """
        Accept an order
        
        Args:
            order_id: ID of order to accept
            acceptor: Address of acceptor
            
        Returns:
            Acceptance result
            
        Raises:
            OrderError: If acceptance fails
        """
        with self._lock:
            if not self._initialized:
                raise OrderError("Order service not initialized")
            
            if order_id not in self._orders:
                raise OrderError(f"Order {order_id} not found")
            
            order = self._orders[order_id]
            if order.status != OrderStatus.PENDING:
                raise OrderError(f"Order is not pending: {order.status}")
            
            try:
                result = dap_chain_net_srv_order_accept_order(
                    self._handle, order_id, acceptor
                )
                
                if result.get('success'):
                    order.status = OrderStatus.ACCEPTED
                    order.acceptor = acceptor
                    order.updated_at = datetime.now()
                    self.logger.info(f"Order {order_id} accepted by {acceptor}")
                
                return result
                
            except Exception as e:
                self.logger.error(f"Failed to accept order: {e}")
                raise OrderError(f"Order acceptance failed: {e}")
    
    def complete_order(self, order_id: str) -> Dict[str, Any]:
        """
        Complete an order
        
        Args:
            order_id: ID of order to complete
            
        Returns:
            Completion result
            
        Raises:
            OrderError: If completion fails
        """
        with self._lock:
            if not self._initialized:
                raise OrderError("Order service not initialized")
            
            if order_id not in self._orders:
                raise OrderError(f"Order {order_id} not found")
            
            order = self._orders[order_id]
            if order.status != OrderStatus.ACCEPTED:
                raise OrderError(f"Order is not accepted: {order.status}")
            
            try:
                result = dap_chain_net_srv_order_complete_order(
                    self._handle, order_id
                )
                
                if result.get('success'):
                    order.status = OrderStatus.COMPLETED
                    order.fulfillment_status = FulfillmentStatus.CONFIRMED
                    order.updated_at = datetime.now()
                    self.logger.info(f"Order {order_id} completed")
                
                return result
                
            except Exception as e:
                self.logger.error(f"Failed to complete order: {e}")
                raise OrderError(f"Order completion failed: {e}")
    
    def get_order(self, order_id: str) -> Order:
        """
        Get order by ID
        
        Args:
            order_id: Order ID
            
        Returns:
            Order object
            
        Raises:
            OrderError: If order not found
        """
        with self._lock:
            if order_id not in self._orders:
                raise OrderError(f"Order {order_id} not found")
            return self._orders[order_id]
    
    def get_orders(
        self,
        status: Optional[OrderStatus] = None,
        order_type: Optional[OrderType] = None,
        creator: Optional[str] = None
    ) -> List[Order]:
        """
        Get orders with optional filters
        
        Args:
            status: Filter by status
            order_type: Filter by type
            creator: Filter by creator
            
        Returns:
            List of orders
        """
        with self._lock:
            orders = list(self._orders.values())
            
            if status:
                orders = [o for o in orders if o.status == status]
            
            if order_type:
                orders = [o for o in orders if o.type == order_type]
            
            if creator:
                orders = [o for o in orders if o.creator == creator]
            
            return orders
    
    def get_matches(self, order_id: str) -> List[OrderMatch]:
        """
        Get matches for an order
        
        Args:
            order_id: Order ID
            
        Returns:
            List of matches
        """
        with self._lock:
            return self._matches.get(order_id, [])
    
    def _find_matches(self, order: Order):
        """Find potential matches for an order"""
        matches = []
        
        for other_id, other_order in self._orders.items():
            if other_id == order.id:
                continue
            
            if order.can_match(other_order):
                match = OrderMatch(
                    order_id=order.id,
                    match_id=other_id,
                    match_score=self._calculate_match_score(order, other_order),
                    match_type="compatible",
                    conditions_met=self._check_conditions(order, other_order),
                    timestamp=datetime.now()
                )
                matches.append(match)
        
        if matches:
            self._matches[order.id] = matches
            # Notify callbacks
            for callback in self._match_callbacks:
                callback(order, matches)
    
    def _calculate_match_score(self, order1: Order, order2: Order) -> float:
        """Calculate match score between two orders"""
        score = 0.0
        
        # Token match
        if order1.token == order2.token:
            score += 0.3
        
        # Price compatibility
        if order1.price and order2.price:
            price_diff = abs(order1.price - order2.price)
            price_score = 1.0 - (price_diff / max(order1.price, order2.price))
            score += 0.4 * max(0, price_score)
        
        # Amount compatibility
        amount_ratio = min(order1.amount, order2.amount) / max(order1.amount, order2.amount)
        score += 0.3 * amount_ratio
        
        return min(1.0, score)
    
    def _check_conditions(self, order: Order, context_order: Order) -> bool:
        """Check if order conditions are met"""
        if not order.conditions:
            return True
        
        context = {
            'amount': context_order.amount,
            'price': context_order.price,
            'token': context_order.token,
            'type': context_order.type.value,
            'creator': context_order.creator
        }
        
        return all(cond.evaluate(context) for cond in order.conditions)
    
    def register_match_callback(self, callback: Callable):
        """Register callback for order matches"""
        with self._lock:
            self._match_callbacks.append(callback)
    
    def __enter__(self):
        """Context manager entry"""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit"""
        self.close()
    
    def close(self):
        """Close order service and cleanup resources"""
        with self._lock:
            if self._initialized and self._handle:
                try:
                    dap_chain_net_srv_order_deinit(self._handle)
                    self.logger.info("Order service closed")
                except Exception as e:
                    self.logger.error(f"Error closing order service: {e}")
                finally:
                    self._initialized = False
                    self._handle = None
                    self._orders.clear()
                    self._matches.clear()
                    self._match_callbacks.clear()
