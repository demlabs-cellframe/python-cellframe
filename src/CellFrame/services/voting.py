"""
🗳️ Voting Service

High-level voting and governance operations for Cellframe blockchain.
Provides proposal creation, voting, and governance functionality.
"""

import logging
import threading
from typing import Dict, List, Optional, Any, Union
from decimal import Decimal
from datetime import datetime, timedelta
from enum import Enum
from dataclasses import dataclass

from ..core.exceptions import CellframeException

# Import C bindings
try:
    import python_cellframe as cf_native
    
    # Centralized check for all required voting functions - ONCE at import
    required_functions = [
        'dap_chain_net_srv_voting_init',
        'dap_chain_net_srv_voting_create',
        'dap_chain_net_srv_voting_create_proposal',
        'dap_chain_net_srv_voting_cast_vote',
        'dap_chain_net_srv_voting_delegate',
        'dap_chain_net_srv_voting_get_proposals',
        'dap_chain_net_srv_voting_get_results',
        'dap_chain_net_srv_voting_deinit'
    ]
    
    missing_functions = [func for func in required_functions if not hasattr(cf_native, func)]
    if missing_functions:
        # In test mode, allow missing functions for mock testing
        import os
        if os.environ.get('CELLFRAME_TEST_MODE') == '1':
            print(f"⚠️  TEST MODE: Missing native voting functions: {', '.join(missing_functions)}")
        else:
            raise ImportError(
                f"❌ CRITICAL: Missing native voting functions: {', '.join(missing_functions)}\n"
                "This is a Python bindings library - all voting functions must be implemented.\n"
                "Please implement these functions in src/cellframe_services.c"
            )
except ImportError as e:
    raise ImportError(
        "❌ CRITICAL: Native CellFrame voting functions not available!\n"
        "This is a Python bindings library - native C extension is required.\n"
        "Required: dap_chain_net_srv_voting_* functions must be implemented in native C extension.\n"
        f"Original error: {e}\n"
        "Please implement voting bindings in src/cellframe_services.c"
    ) from e

# No wrapper functions - use cf_native directly everywhere


class VotingError(CellframeException):
    """Voting-specific errors"""
    pass


class ProposalType(Enum):
    """Types of governance proposals"""
    SIMPLE = "simple"
    WEIGHTED = "weighted"
    QUADRATIC = "quadratic"
    DELEGATED = "delegated"
    MULTI_CHOICE = "multi_choice"
    RANKED_CHOICE = "ranked_choice"


class ProposalStatus(Enum):
    """Proposal status"""
    DRAFT = "draft"
    ACTIVE = "active"
    CLOSED = "closed"
    EXECUTED = "executed"
    CANCELLED = "cancelled"
    FAILED = "failed"


class VoteOption(Enum):
    """Standard voting options"""
    YES = "yes"
    NO = "no"
    ABSTAIN = "abstain"


@dataclass
class Proposal:
    """Governance proposal representation"""
    id: str
    title: str
    description: str
    type: ProposalType
    status: ProposalStatus
    created_at: datetime
    start_time: datetime
    end_time: datetime
    creator: str
    options: List[str]
    quorum: Optional[Decimal] = None
    threshold: Optional[Decimal] = None
    
    @property
    def is_active(self) -> bool:
        """Check if proposal is currently active"""
        now = datetime.now()
        return (self.status == ProposalStatus.ACTIVE and 
                self.start_time <= now <= self.end_time)
    
    @property
    def time_remaining(self) -> timedelta:
        """Calculate time remaining for voting"""
        if not self.is_active:
            return timedelta(0)
        return self.end_time - datetime.now()
    
    @property
    def has_ended(self) -> bool:
        """Check if voting period has ended"""
        return datetime.now() > self.end_time


@dataclass
class Vote:
    """Individual vote representation"""
    voter: str
    proposal_id: str
    option: Union[str, int]
    weight: Decimal
    timestamp: datetime
    tx_hash: Optional[str] = None
    delegated_from: Optional[str] = None


@dataclass
class VotingResults:
    """Voting results for a proposal"""
    proposal_id: str
    total_votes: Decimal
    total_weight: Decimal
    results: Dict[str, Decimal]
    participation_rate: Decimal
    has_quorum: bool
    winner: Optional[str] = None
    
    def get_percentage(self, option: str) -> Decimal:
        """Get percentage of votes for an option"""
        if self.total_weight == 0:
            return Decimal('0')
        return (self.results.get(option, Decimal('0')) / self.total_weight) * 100


class VotingService:
    """
    High-level voting service for governance operations.
    
    This service provides:
    - Proposal creation and management
    - Vote casting and delegation
    - Result calculation and tracking
    - Governance analytics
    
    Note: Service handles are created and initialized by C library.
    Python only wraps existing handles or creates custom extensions.
    """
    
    def __init__(self, voting_handle):
        """
        Initialize voting service with existing handle from C library
        
        Args:
            voting_handle: Native voting service handle from C library
        """
        if voting_handle is None:
            raise VotingError("Voting handle cannot be None - must be provided by C library")
            
        self.logger = logging.getLogger('CellFrame.VotingService')
        self._handle = voting_handle
        self._lock = threading.RLock()
        self._proposals: Dict[str, Proposal] = {}
        self._votes: Dict[str, List[Vote]] = {}
        self._delegations: Dict[str, str] = {}
    
    @classmethod
    def get_available_services(cls) -> List['VotingService']:
        """Get all available voting services from C library"""
        # Get service handles from C library
        try:
            service_handles = cf_native.dap_chain_net_srv_voting_get_all_services()
            return [cls(handle) for handle in service_handles if handle]
        except AttributeError:
            # Function not implemented yet in C library
            return []
    
    @classmethod  
    def create_custom_service(cls) -> 'VotingService':
        """Create custom voting service for extensions"""
        # This creates a new service handle for custom implementations
        voting_handle = cf_native.dap_chain_net_srv_voting_create(None)
        if voting_handle is None:
            raise VotingError("Failed to create custom voting service")
        
        return cls(voting_handle)
    

    def create_proposal(
        self,
        title: str,
        description: str,
        options: List[str],
        duration: timedelta,
        proposal_type: ProposalType = ProposalType.SIMPLE,
        quorum: Optional[Union[Decimal, float]] = None,
        threshold: Optional[Union[Decimal, float]] = None,
        **kwargs
    ) -> Proposal:
        """
        Create a new governance proposal
        
        Args:
            title: Proposal title
            description: Detailed description
            options: Voting options
            duration: Voting duration
            proposal_type: Type of proposal
            quorum: Required participation (optional)
            threshold: Required approval percentage (optional)
            **kwargs: Additional proposal parameters
            
        Returns:
            Created proposal object
            
        Raises:
            VotingError: If proposal creation fails
        """
        with self._lock:
            if not self._initialized:
                raise VotingError("Voting service not initialized")
            
            # Validate inputs
            if not title or not description:
                raise VotingError("Title and description are required")
            
            if len(options) < 2:
                raise VotingError("At least 2 options required")
            
            if duration <= timedelta(0):
                raise VotingError("Duration must be positive")
            
            try:
                # Create proposal via C binding
                proposal_id = cf_native.dap_chain_net_srv_voting_create_proposal(
                    self._handle,
                    title,
                    description,
                    options,
                    int(duration.total_seconds())
                )
                
                # Create proposal object
                now = datetime.now()
                proposal = Proposal(
                    id=proposal_id,
                    title=title,
                    description=description,
                    type=proposal_type,
                    status=ProposalStatus.DRAFT,
                    created_at=now,
                    start_time=now,
                    end_time=now + duration,
                    creator=kwargs.get('creator', 'system'),
                    options=options,
                    quorum=Decimal(str(quorum)) if quorum else None,
                    threshold=Decimal(str(threshold)) if threshold else None
                )
                
                self._proposals[proposal_id] = proposal
                self._votes[proposal_id] = []
                
                self.logger.info(f"Created proposal {proposal_id}: {title}")
                return proposal
                
            except Exception as e:
                self.logger.error(f"Failed to create proposal: {e}")
                raise VotingError(f"Proposal creation failed: {e}")
    
    def cast_vote(
        self,
        proposal_id: str,
        option: Union[str, int],
        weight: Optional[Union[Decimal, float]] = None,
        voter: Optional[str] = None
    ) -> Vote:
        """
        Cast a vote on a proposal
        
        Args:
            proposal_id: ID of proposal to vote on
            option: Option to vote for (string or index)
            weight: Vote weight (optional)
            voter: Voter address (optional)
            
        Returns:
            Vote object
            
        Raises:
            VotingError: If voting fails
        """
        with self._lock:
            if not self._initialized:
                raise VotingError("Voting service not initialized")
            
            if proposal_id not in self._proposals:
                raise VotingError(f"Proposal {proposal_id} not found")
            
            proposal = self._proposals[proposal_id]
            if not proposal.is_active:
                raise VotingError("Proposal is not active")
            
            # Resolve option index
            if isinstance(option, str):
                if option not in proposal.options:
                    raise VotingError(f"Invalid option: {option}")
                option_index = proposal.options.index(option)
            else:
                option_index = int(option)
                if option_index >= len(proposal.options):
                    raise VotingError(f"Invalid option index: {option_index}")
            
            # Set default weight
            if weight is None:
                weight = Decimal('1')
            else:
                weight = Decimal(str(weight))
            
            try:
                # Cast vote via C binding
                result = cf_native.dap_chain_net_srv_voting_cast_vote(
                    self._handle,
                    proposal_id,
                    option_index,
                    float(weight)
                )
                
                # Create vote object
                vote = Vote(
                    voter=voter or 'anonymous',
                    proposal_id=proposal_id,
                    option=proposal.options[option_index],
                    weight=weight,
                    timestamp=datetime.now(),
                    tx_hash=result.get('tx_hash')
                )
                
                self._votes[proposal_id].append(vote)
                self.logger.info(f"Cast vote on proposal {proposal_id}: {vote.option}")
                
                return vote
                
            except Exception as e:
                self.logger.error(f"Failed to cast vote: {e}")
                raise VotingError(f"Vote casting failed: {e}")
    
    def delegate_voting_power(
        self,
        delegate_address: str,
        weight: Optional[Union[Decimal, float]] = None
    ) -> Dict[str, Any]:
        """
        Delegate voting power to another address
        
        Args:
            delegate_address: Address to delegate to
            weight: Weight to delegate (optional)
            
        Returns:
            Delegation result
            
        Raises:
            VotingError: If delegation fails
        """
        with self._lock:
            if not self._initialized:
                raise VotingError("Voting service not initialized")
            
            if not delegate_address:
                raise VotingError("Delegate address required")
            
            weight = Decimal(str(weight)) if weight else Decimal('1')
            
            try:
                result = cf_native.dap_chain_net_srv_voting_delegate(
                    self._handle,
                    delegate_address,
                    float(weight)
                )
                
                if result.get('success'):
                    self._delegations[delegate_address] = str(weight)
                    self.logger.info(f"Delegated voting power to {delegate_address}")
                
                return result
                
            except Exception as e:
                self.logger.error(f"Failed to delegate voting power: {e}")
                raise VotingError(f"Delegation failed: {e}")
    
    def get_proposal(self, proposal_id: str) -> Proposal:
        """
        Get proposal by ID
        
        Args:
            proposal_id: Proposal ID
            
        Returns:
            Proposal object
            
        Raises:
            VotingError: If proposal not found
        """
        with self._lock:
            if proposal_id not in self._proposals:
                raise VotingError(f"Proposal {proposal_id} not found")
            return self._proposals[proposal_id]
    
    def get_proposals(
        self,
        status: Optional[ProposalStatus] = None,
        active_only: bool = False
    ) -> List[Proposal]:
        """
        Get all proposals or filter by status
        
        Args:
            status: Optional status filter
            active_only: Only return active proposals
            
        Returns:
            List of proposals
        """
        with self._lock:
            proposals = list(self._proposals.values())
            
            if status:
                proposals = [p for p in proposals if p.status == status]
            
            if active_only:
                proposals = [p for p in proposals if p.is_active]
            
            return proposals
    
    def get_results(self, proposal_id: str) -> VotingResults:
        """
        Get voting results for a proposal
        
        Args:
            proposal_id: Proposal ID
            
        Returns:
            Voting results
            
        Raises:
            VotingError: If proposal not found
        """
        with self._lock:
            if proposal_id not in self._proposals:
                raise VotingError(f"Proposal {proposal_id} not found")
            
            try:
                # Get results from C binding
                result_data = cf_native.dap_chain_net_srv_voting_get_results(
                    self._handle, proposal_id
                )
                
                # Calculate results
                proposal = self._proposals[proposal_id]
                votes = self._votes[proposal_id]
                
                # Aggregate votes by option
                results = {}
                total_weight = Decimal('0')
                
                for vote in votes:
                    if vote.option not in results:
                        results[vote.option] = Decimal('0')
                    results[vote.option] += vote.weight
                    total_weight += vote.weight
                
                # Determine winner
                winner = None
                if results:
                    winner = max(results.items(), key=lambda x: x[1])[0]
                
                # Check quorum
                has_quorum = True
                if proposal.quorum:
                    participation_rate = (len(votes) / 100) * 100  # Simplified
                    has_quorum = participation_rate >= proposal.quorum
                
                voting_results = VotingResults(
                    proposal_id=proposal_id,
                    total_votes=Decimal(str(len(votes))),
                    total_weight=total_weight,
                    results=results,
                    participation_rate=Decimal(str(len(votes))),  # Simplified
                    has_quorum=has_quorum,
                    winner=winner
                )
                
                return voting_results
                
            except Exception as e:
                self.logger.error(f"Failed to get results for {proposal_id}: {e}")
                raise VotingError(f"Result calculation failed: {e}")
    
    def close_proposal(self, proposal_id: str) -> bool:
        """
        Close a proposal and finalize results
        
        Args:
            proposal_id: Proposal ID
            
        Returns:
            True if closed successfully
            
        Raises:
            VotingError: If closing fails
        """
        with self._lock:
            if proposal_id not in self._proposals:
                raise VotingError(f"Proposal {proposal_id} not found")
            
            proposal = self._proposals[proposal_id]
            
            # Check if can be closed
            if proposal.status != ProposalStatus.ACTIVE:
                raise VotingError("Proposal is not active")
            
            if not proposal.has_ended and not self._context:
                raise VotingError("Voting period has not ended")
            
            try:
                # Get final results
                results = self.get_results(proposal_id)
                
                # Update status based on results
                if results.has_quorum:
                    proposal.status = ProposalStatus.CLOSED
                else:
                    proposal.status = ProposalStatus.FAILED
                
                self.logger.info(f"Closed proposal {proposal_id} with status {proposal.status}")
                return True
                
            except Exception as e:
                self.logger.error(f"Failed to close proposal: {e}")
                raise VotingError(f"Proposal closing failed: {e}")
    
    def __enter__(self):
        """Context manager entry"""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit"""
        self.close()
    
    def close(self):
        """Close voting service and cleanup resources"""
        with self._lock:
            if self._initialized and self._handle:
                try:
                    cf_native.dap_chain_net_srv_voting_deinit(self._handle)
                    self.logger.info("Voting service closed")
                except Exception as e:
                    self.logger.error(f"Error closing voting service: {e}")
                finally:
                    self._initialized = False
                    self._handle = None
                    self._proposals.clear()
                    self._votes.clear()
                    self._delegations.clear()
