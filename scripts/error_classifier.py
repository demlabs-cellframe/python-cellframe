#!/usr/bin/env python3
"""
🏷️ Error Classification System for Python Cellframe SDK
Automatic classification and prioritization of detected issues.
"""

import json
import argparse
from pathlib import Path
from typing import Dict, List, Any, Tuple
from datetime import datetime
from dataclasses import dataclass, asdict
from enum import Enum


class Priority(Enum):
    """Priority levels for issues."""
    CRITICAL = "critical"
    HIGH = "high"
    MEDIUM = "medium"
    LOW = "low"
    INFO = "info"


class IssueType(Enum):
    """Types of issues."""
    SECURITY = "security"
    RELIABILITY = "reliability"
    PERFORMANCE = "performance"
    MAINTAINABILITY = "maintainability"
    COMPATIBILITY = "compatibility"
    STYLE = "style"
    DOCUMENTATION = "documentation"


@dataclass
class ClassifiedIssue:
    """Classified issue with priority and fix strategy."""
    original_issue: Dict[str, Any]
    priority: Priority
    issue_type: IssueType
    fix_strategy: str
    estimated_effort: str  # "low", "medium", "high"
    auto_fixable: bool
    breaking_change: bool
    affects_api: bool
    component: str
    fix_command: str = ""
    notes: str = ""


class ErrorClassifier:
    """Intelligent error classification system."""
    
    def __init__(self):
        self.classification_rules = self._build_classification_rules()
        self.component_map = self._build_component_map()
    
    def _build_classification_rules(self) -> Dict[str, Dict[str, Any]]:
        """Build rules for classifying different types of issues."""
        return {
            # Security issues
            'hardcoded_password': {
                'priority': Priority.CRITICAL,
                'type': IssueType.SECURITY,
                'strategy': 'Use environment variables or secure key management',
                'effort': 'medium',
                'auto_fixable': False,
                'breaking': False,
                'api_impact': False
            },
            'sql_injection': {
                'priority': Priority.CRITICAL,
                'type': IssueType.SECURITY,
                'strategy': 'Use parameterized queries',
                'effort': 'high',
                'auto_fixable': False,
                'breaking': False,
                'api_impact': False
            },
            'hardcoded_bind_all_interfaces': {
                'priority': Priority.HIGH,
                'type': IssueType.SECURITY,
                'strategy': 'Bind to specific interfaces only',
                'effort': 'medium',
                'auto_fixable': False,
                'breaking': True,
                'api_impact': False
            },
            
            # Reliability issues  
            'undefined_variable': {
                'priority': Priority.HIGH,
                'type': IssueType.RELIABILITY,
                'strategy': 'Define variable before use',
                'effort': 'low',
                'auto_fixable': False,
                'breaking': False,
                'api_impact': False
            },
            'attribute_error': {
                'priority': Priority.HIGH,
                'type': IssueType.RELIABILITY,
                'strategy': 'Check attribute existence or fix typo',
                'effort': 'low',
                'auto_fixable': False,
                'breaking': False,
                'api_impact': True
            },
            'import_error': {
                'priority': Priority.HIGH,
                'type': IssueType.RELIABILITY,
                'strategy': 'Fix import path or add missing dependency',
                'effort': 'medium',
                'auto_fixable': False,
                'breaking': False,
                'api_impact': True
            },
            
            # Performance issues
            'unused_import': {
                'priority': Priority.LOW,
                'type': IssueType.PERFORMANCE,
                'strategy': 'Remove unused import',
                'effort': 'low',
                'auto_fixable': True,
                'breaking': False,
                'api_impact': False
            },
            'unused_variable': {
                'priority': Priority.LOW,
                'type': IssueType.MAINTAINABILITY,
                'strategy': 'Remove unused variable or use it',
                'effort': 'low',
                'auto_fixable': True,
                'breaking': False,
                'api_impact': False
            },
            
            # Style issues
            'line_too_long': {
                'priority': Priority.LOW,
                'type': IssueType.STYLE,
                'strategy': 'Break line or use black formatter',
                'effort': 'low',
                'auto_fixable': True,
                'breaking': False,
                'api_impact': False
            },
            'missing_whitespace': {
                'priority': Priority.LOW,
                'type': IssueType.STYLE,
                'strategy': 'Add appropriate whitespace',
                'effort': 'low',
                'auto_fixable': True,
                'breaking': False,
                'api_impact': False
            },
            
            # Type issues
            'type_error': {
                'priority': Priority.MEDIUM,
                'type': IssueType.RELIABILITY,
                'strategy': 'Fix type annotation or casting',
                'effort': 'medium',
                'auto_fixable': False,
                'breaking': False,
                'api_impact': True
            },
            
            # Complexity issues
            'too_complex': {
                'priority': Priority.MEDIUM,
                'type': IssueType.MAINTAINABILITY,
                'strategy': 'Refactor into smaller functions',
                'effort': 'high',
                'auto_fixable': False,
                'breaking': False,
                'api_impact': False
            }
        }
    
    def _build_component_map(self) -> Dict[str, str]:
        """Map file patterns to components."""
        return {
            'CellFrame/core/': 'Core',
            'CellFrame/chain/': 'Chain',
            'CellFrame/services/': 'Services', 
            'CellFrame/network/': 'Network',
            'CellFrame/node/': 'Node',
            'CellFrame/legacy/': 'Legacy',
            'src/': 'C Bindings',
            'include/': 'Headers',
            'tests/unit/': 'Unit Tests',
            'tests/integration/': 'Integration Tests',
            'tests/e2e/': 'E2E Tests',
            'tests/fixtures/': 'Test Fixtures',
            'tests/': 'Tests'
        }
    
    def _identify_component(self, file_path: str) -> str:
        """Identify which component a file belongs to."""
        for pattern, component in self.component_map.items():
            if pattern in file_path:
                return component
        return 'Unknown'
    
    def _classify_single_issue(self, issue: Dict[str, Any]) -> ClassifiedIssue:
        """Classify a single issue."""
        
        # Extract issue details
        tool = issue.get('tool', '')
        message = issue.get('message', '').lower()
        symbol = issue.get('symbol', '').lower()
        code = issue.get('code', '').lower()
        file_path = issue.get('file', '')
        
        # Determine component
        component = self._identify_component(file_path)
        
        # Default classification
        priority = Priority.MEDIUM
        issue_type = IssueType.MAINTAINABILITY
        strategy = 'Manual review required'
        effort = 'medium'
        auto_fixable = False
        breaking = False
        api_impact = False
        fix_command = ""
        
        # Security classifications
        if any(term in message for term in ['password', 'secret', 'key', 'token']) and 'hardcoded' in message:
            rule = self.classification_rules['hardcoded_password']
        elif 'sql' in message and 'injection' in message:
            rule = self.classification_rules['sql_injection']
        elif 'bind_all_interfaces' in symbol or 'hardcoded_bind_all_interfaces' in code:
            rule = self.classification_rules['hardcoded_bind_all_interfaces']
            
        # Reliability classifications
        elif 'undefined' in message and 'variable' in message:
            rule = self.classification_rules['undefined_variable']
        elif 'has no attribute' in message or 'attribute_error' in symbol:
            rule = self.classification_rules['attribute_error']
        elif 'import' in message and ('error' in message or 'not found' in message):
            rule = self.classification_rules['import_error']
        elif symbol in ['undefined-variable', 'used-before-assignment']:
            rule = self.classification_rules['undefined_variable']
            
        # Performance/Maintainability classifications  
        elif 'unused' in message and 'import' in message:
            rule = self.classification_rules['unused_import']
            fix_command = f"isort {file_path} --remove-unused-imports"
        elif 'unused' in message and 'variable' in message:
            rule = self.classification_rules['unused_variable']
        elif symbol == 'unused-variable' or code == 'F841':
            rule = self.classification_rules['unused_variable']
        elif symbol == 'unused-import' or code == 'F401':
            rule = self.classification_rules['unused_import']
            fix_command = f"autoflake --remove-unused-variables --in-place {file_path}"
            
        # Style classifications
        elif 'line too long' in message or code == 'E501':
            rule = self.classification_rules['line_too_long']
            fix_command = f"black {file_path}"
        elif 'whitespace' in message or code.startswith('E2') or code.startswith('W2'):
            rule = self.classification_rules['missing_whitespace']
            fix_command = f"autopep8 --in-place {file_path}"
            
        # Type issues
        elif tool == 'mypy' or 'type' in message:
            rule = self.classification_rules['type_error']
            
        # Complexity issues
        elif 'complex' in message and tool == 'radon':
            rule = self.classification_rules['too_complex']
            
        else:
            # Default rule for unclassified issues
            rule = {
                'priority': Priority.MEDIUM,
                'type': IssueType.MAINTAINABILITY,
                'strategy': 'Manual review and fix required',
                'effort': 'medium',
                'auto_fixable': False,
                'breaking': False,
                'api_impact': False
            }
        
        # Apply rule
        priority = rule['priority']
        issue_type = rule['type']
        strategy = rule['strategy']
        effort = rule['effort']
        auto_fixable = rule['auto_fixable']
        breaking = rule['breaking']
        api_impact = rule['api_impact']
        
        # Severity overrides for specific components
        if component == 'C Bindings' or component == 'Headers':
            if priority == Priority.LOW:
                priority = Priority.MEDIUM  # C code issues are more critical
        
        if component == 'Core':
            if priority == Priority.MEDIUM:
                priority = Priority.HIGH  # Core issues are more critical
        
        return ClassifiedIssue(
            original_issue=issue,
            priority=priority,
            issue_type=issue_type,
            fix_strategy=strategy,
            estimated_effort=effort,
            auto_fixable=auto_fixable,
            breaking_change=breaking,
            affects_api=api_impact,
            component=component,
            fix_command=fix_command,
            notes=""
        )
    
    def classify_issues(self, analysis_result: Dict[str, Any]) -> Dict[str, Any]:
        """Classify all issues from static analysis."""
        issues = analysis_result.get('issues', [])
        
        classified_issues = []
        for issue in issues:
            classified = self._classify_single_issue(issue)
            classified_issues.append(classified)
        
        # Generate classification summary
        classification_summary = self._generate_classification_summary(classified_issues)
        
        return {
            'metadata': {
                'classification_timestamp': datetime.now().isoformat(),
                'total_issues': len(classified_issues),
                'original_analysis': analysis_result
            },
            'classified_issues': [asdict(issue) for issue in classified_issues],
            'summary': classification_summary,
            'fix_plan': self._generate_fix_plan(classified_issues)
        }
    
    def _generate_classification_summary(self, issues: List[ClassifiedIssue]) -> Dict[str, Any]:
        """Generate summary of classified issues."""
        summary = {
            'by_priority': {},
            'by_type': {},
            'by_component': {},
            'auto_fixable_count': 0,
            'api_affecting_count': 0,
            'breaking_change_count': 0
        }
        
        for issue in issues:
            # Count by priority
            priority = issue.priority.value
            summary['by_priority'][priority] = summary['by_priority'].get(priority, 0) + 1
            
            # Count by type
            issue_type = issue.issue_type.value
            summary['by_type'][issue_type] = summary['by_type'].get(issue_type, 0) + 1
            
            # Count by component
            component = issue.component
            summary['by_component'][component] = summary['by_component'].get(component, 0) + 1
            
            # Count special flags
            if issue.auto_fixable:
                summary['auto_fixable_count'] += 1
            if issue.affects_api:
                summary['api_affecting_count'] += 1
            if issue.breaking_change:
                summary['breaking_change_count'] += 1
        
        return summary
    
    def _generate_fix_plan(self, issues: List[ClassifiedIssue]) -> Dict[str, Any]:
        """Generate a prioritized fix plan."""
        
        # Sort issues by priority and impact
        priority_order = [Priority.CRITICAL, Priority.HIGH, Priority.MEDIUM, Priority.LOW, Priority.INFO]
        
        sorted_issues = sorted(issues, key=lambda x: (
            priority_order.index(x.priority),
            not x.affects_api,  # API-affecting issues first
            not x.breaking_change,  # Breaking changes need careful planning
            x.estimated_effort  # Lower effort first within same priority
        ))
        
        # Group into phases
        phases = {
            'phase_1_critical': [],
            'phase_2_high_priority': [],
            'phase_3_medium_priority': [],
            'phase_4_low_priority': [],
            'auto_fixable': []
        }
        
        for issue in sorted_issues:
            if issue.auto_fixable:
                phases['auto_fixable'].append(asdict(issue))
            elif issue.priority == Priority.CRITICAL:
                phases['phase_1_critical'].append(asdict(issue))
            elif issue.priority == Priority.HIGH:
                phases['phase_2_high_priority'].append(asdict(issue))
            elif issue.priority == Priority.MEDIUM:
                phases['phase_3_medium_priority'].append(asdict(issue))
            else:
                phases['phase_4_low_priority'].append(asdict(issue))
        
        return {
            'phases': phases,
            'recommendations': {
                'start_with': 'auto_fixable',
                'immediate_attention': len(phases['phase_1_critical']),
                'breaking_changes_review': sum(1 for i in sorted_issues if i.breaking_change),
                'estimated_total_effort': self._estimate_total_effort(sorted_issues)
            }
        }
    
    def _estimate_total_effort(self, issues: List[ClassifiedIssue]) -> str:
        """Estimate total effort required to fix all issues."""
        effort_weights = {'low': 1, 'medium': 3, 'high': 8}
        
        total_weight = sum(effort_weights.get(issue.estimated_effort, 3) for issue in issues)
        
        if total_weight < 10:
            return "low"
        elif total_weight < 30:
            return "medium"
        elif total_weight < 80:
            return "high"
        else:
            return "very_high"
    
    def save_classification(self, classification_result: Dict[str, Any], output_file: str = None):
        """Save classification result to file."""
        if output_file is None:
            output_file = f"error_classification_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        
        with open(output_file, 'w') as f:
            json.dump(classification_result, f, indent=2, ensure_ascii=False)
        
        print(f"📝 Classification report saved to: {output_file}")
    
    def print_classification_summary(self, classification_result: Dict[str, Any]):
        """Print classification summary."""
        summary = classification_result['summary']
        fix_plan = classification_result['fix_plan']
        
        print("\n" + "="*60)
        print("🏷️  ERROR CLASSIFICATION SUMMARY")
        print("="*60)
        
        print(f"\n📊 Total Issues: {classification_result['metadata']['total_issues']}")
        
        print("\n🎯 By Priority:")
        priority_emojis = {
            'critical': '🔴',
            'high': '🟠', 
            'medium': '🟡',
            'low': '🟢',
            'info': '⚪'
        }
        for priority, count in summary['by_priority'].items():
            emoji = priority_emojis.get(priority, '⚪')
            print(f"  {emoji} {priority.capitalize()}: {count}")
        
        print("\n📋 By Type:")
        for issue_type, count in summary['by_type'].items():
            print(f"  • {issue_type.replace('_', ' ').title()}: {count}")
        
        print("\n🏗️  By Component:")
        for component, count in summary['by_component'].items():
            print(f"  • {component}: {count}")
        
        print(f"\n🤖 Auto-fixable: {summary['auto_fixable_count']}")
        print(f"⚠️  API affecting: {summary['api_affecting_count']}")
        print(f"💥 Breaking changes: {summary['breaking_change_count']}")
        
        print("\n📅 Fix Plan Phases:")
        for phase_name, phase_issues in fix_plan['phases'].items():
            if phase_issues:
                phase_display = phase_name.replace('_', ' ').title()
                print(f"  • {phase_display}: {len(phase_issues)} issues")
        
        recommendations = fix_plan['recommendations']
        print(f"\n💡 Recommendations:")
        print(f"  • Start with: {recommendations['start_with'].replace('_', ' ')}")
        print(f"  • Immediate attention needed: {recommendations['immediate_attention']} issues")
        print(f"  • Breaking changes to review: {recommendations['breaking_changes_review']}")
        print(f"  • Estimated total effort: {recommendations['estimated_total_effort']}")
        
        print("\n" + "="*60)


def main():
    """Main entry point for error classification."""
    parser = argparse.ArgumentParser(description='Error Classification for Python Cellframe SDK')
    parser.add_argument('input_file', help='Static analysis JSON report file')
    parser.add_argument('--output', '-o', help='Output file for classification report')
    parser.add_argument('--quiet', '-q', action='store_true', help='Suppress progress output')
    
    args = parser.parse_args()
    
    # Load static analysis report
    try:
        with open(args.input_file, 'r') as f:
            analysis_result = json.load(f)
    except (FileNotFoundError, json.JSONDecodeError) as e:
        print(f"❌ Error loading analysis file: {e}")
        return 1
    
    # Classify issues
    classifier = ErrorClassifier()
    classification_result = classifier.classify_issues(analysis_result)
    
    # Print summary unless quiet
    if not args.quiet:
        classifier.print_classification_summary(classification_result)
    
    # Save classification
    classifier.save_classification(classification_result, args.output)
    
    # Return exit code based on critical issues
    critical_count = classification_result['summary']['by_priority'].get('critical', 0)
    if critical_count > 0:
        print(f"\n❌ Found {critical_count} critical issues that need immediate attention!")
        return 1
    else:
        print(f"\n✅ No critical issues found!")
        return 0


if __name__ == '__main__':
    exit(main())