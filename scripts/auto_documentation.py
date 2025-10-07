#!/usr/bin/env python3
"""
Auto-documentation system
Automatically generates and updates documentation based on code changes
"""

import os
import subprocess
import re
from pathlib import Path
from dotenv import load_dotenv

# Load environment variables
load_dotenv('/workspaces/demi/config/auto_growth_config.env')

# Import our AI provider system
from ai_providers import AIProviderManager

class AutoDocumentation:
    def __init__(self):
        # Initialize AI provider manager
        self.ai_manager = AIProviderManager()
        
        print(f"🤖 Using AI provider: {self.ai_manager.get_current_provider_name()}")
    
    def analyze_recent_changes(self):
        """Analyze recent code changes to update documentation"""
        try:
            # Get recent commits
            result = subprocess.run(
                ['git', 'log', '--oneline', '--name-only', '-10'], 
                capture_output=True, text=True
            )
            
            # Parse changed files
            changed_files = []
            for line in result.stdout.split('\n'):
                if line.endswith('.cpp') or line.endswith('.hpp'):
                    changed_files.append(line)
            
            return changed_files[:5]  # Limit to 5 most recent
            
        except Exception as e:
            print(f"Error analyzing changes: {e}")
            return []
    
    def generate_code_documentation(self, file_path):
        """Generate documentation for a specific file"""
        try:
            with open(file_path, 'r') as f:
                code_content = f.read()
            
            prompt = f"""
            Analyze this C++ code and generate comprehensive documentation:
            
            File: {file_path}
            Code:
            {code_content[:2000]}  # Limit content length
            
            Generate:
            1. High-level description of what this file does
            2. Key functions and their purposes
            3. Important data structures
            4. Usage examples if applicable
            5. Integration points with other components
            
            Format as Markdown suitable for technical documentation.
            """
            
            return self.ai_manager.generate_content(prompt)
            
        except Exception as e:
            print(f"Error generating documentation for {file_path}: {e}")
            return None
    
    def update_api_reference(self):
        """Automatically update API reference documentation"""
        try:
            # Find all header files
            header_files = list(Path('src').rglob('*.hpp'))
            
            api_docs = "# DemiEngine API Reference\n\n"
            api_docs += "*This documentation is automatically generated*\n\n"
            
            for header_file in header_files:
                if 'test' not in str(header_file):  # Skip test files
                    doc = self.generate_code_documentation(header_file)
                    if doc:
                        api_docs += f"\n## {header_file.stem}\n\n"
                        api_docs += doc + "\n\n"
            
            # Write to docs directory
            os.makedirs('docs/api', exist_ok=True)
            with open('docs/api/AUTO_GENERATED_API.md', 'w') as f:
                f.write(api_docs)
            
            print("API reference updated successfully")
            
        except Exception as e:
            print(f"Error updating API reference: {e}")
    
    def generate_contributor_guide_updates(self):
        """Update contributor guides based on current codebase"""
        try:
            # Analyze current project structure
            project_structure = self.analyze_project_structure()
            
            prompt = f"""
            Based on this project structure, update the contributor guide:
            
            {project_structure}
            
            Generate updated sections for:
            1. How to add new opcodes
            2. Testing guidelines
            3. Code style requirements
            4. Common patterns to follow
            
            Keep it practical and actionable for new contributors.
            """
            
            updated_guide = self.ai_manager.generate_content(prompt)
            
            with open('CONTRIBUTING_AUTO_UPDATED.md', 'w') as f:
                f.write(updated_guide)
            
            print("Contributor guide updated")
            
        except Exception as e:
            print(f"Error updating contributor guide: {e}")
    
    def analyze_project_structure(self):
        """Analyze current project structure"""
        try:
            result = subprocess.run(['find', 'src', '-type', 'f'], capture_output=True, text=True)
            files = result.stdout.strip().split('\n')
            
            structure = {}
            for file_path in files:
                parts = file_path.split('/')
                current = structure
                for part in parts[:-1]:
                    if part not in current:
                        current[part] = {}
                    current = current[part]
                current[parts[-1]] = "file"
            
            return str(structure)
            
        except Exception as e:
            print(f"Error analyzing project structure: {e}")
            return ""
    
    def generate_release_notes(self):
        """Auto-generate release notes from recent commits"""
        try:
            # Get commits since last tag
            result = subprocess.run(
                ['git', 'log', '--oneline', '--since="1 week ago"'], 
                capture_output=True, text=True
            )
            
            recent_commits = result.stdout
            
            prompt = f"""
            Generate release notes from these recent commits:
            
            {recent_commits}
            
            Organize into:
            - New Features
            - Bug Fixes  
            - Improvements
            - Breaking Changes (if any)
            
            Write for both technical and non-technical audiences.
            """
            
            release_notes = self.ai_manager.generate_content(prompt)
            
            # Save release notes
            with open('LATEST_RELEASE_NOTES.md', 'w') as f:
                f.write(release_notes)
            
            print("Release notes generated")
            
        except Exception as e:
            print(f"Error generating release notes: {e}")

if __name__ == "__main__":
    auto_docs = AutoDocumentation()
    
    # Run all documentation updates
    auto_docs.update_api_reference()
    auto_docs.generate_contributor_guide_updates()
    auto_docs.generate_release_notes()
    
    print("Auto-documentation completed!")