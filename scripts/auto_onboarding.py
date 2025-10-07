#!/usr/bin/env python3
"""
Auto-onboarding system for new contributors
Automatically guides new contributors through their first contribution
"""

import requests
import json
import os
from datetime import datetime
from dotenv import load_dotenv

# Load environment variables
load_dotenv('/workspaces/demi/config/auto_growth_config.env')

# Import our AI provider system
from ai_providers import AIProviderManager

class AutoOnboarding:
    def __init__(self):
        self.github_token = os.getenv('GITHUB_TOKEN')
        self.repo = "bobrossrtx/demi"
        self.headers = {
            'Authorization': f'token {self.github_token}',
            'Accept': 'application/vnd.github.v3+json'
        }
        
        # Initialize AI provider manager
        self.ai_manager = AIProviderManager()
        
        print(f"🤖 Using AI provider: {self.ai_manager.get_current_provider_name()}")
    
    def detect_new_contributors(self):
        """Detect new people commenting on issues or opening PRs"""
        # Get recent issue comments
        url = f"https://api.github.com/repos/{self.repo}/issues/comments"
        response = requests.get(url, headers=self.headers)
        
        new_contributors = []
        for comment in response.json()[:10]:  # Check last 10 comments
            user = comment['user']['login']
            if self.is_new_contributor(user):
                new_contributors.append({
                    'username': user,
                    'comment_url': comment['html_url'],
                    'type': 'comment'
                })
        
        return new_contributors
    
    def is_new_contributor(self, username):
        """Check if user is a new contributor"""
        # Check if user has any merged PRs
        url = f"https://api.github.com/repos/{self.repo}/pulls"
        params = {'state': 'closed', 'author': username}
        response = requests.get(url, headers=self.headers, params=params)
        
        merged_prs = [pr for pr in response.json() if pr.get('merged_at')]
        return len(merged_prs) == 0
    
    def auto_welcome_new_contributor(self, contributor):
        """Automatically welcome and guide new contributors"""
        
        # Generate personalized welcome message using AI
        prompt = f"""
        Generate a welcoming and helpful comment for a new contributor to DemiEngine (a virtual machine project).
        
        Contributor: @{contributor['username']}
        Context: {contributor['type']} - {contributor.get('comment_url', 'N/A')}
        
        Include:
        - Warm welcome
        - Quick setup instructions (git clone, make build, make test)
        - Links to contributing resources
        - Suggestion of good first issues (FPU opcodes, assembly examples, documentation)
        - Encouragement about mentorship and community support
        
        Keep it friendly but professional, around 200-300 words.
        Format as GitHub comment markdown.
        """
        
        try:
            welcome_message = self.ai_manager.generate_content(prompt)
        except Exception as e:
            print(f"Error generating welcome message: {e}")
            # Fallback to static message
            welcome_message = f"""
👋 Welcome to DemiEngine, @{contributor['username']}! 

Thanks for your interest in contributing! Since this might be your first contribution to DemiEngine, here are some helpful resources:

🚀 **Quick Setup:**
```bash
git clone https://github.com/bobrossrtx/demi.git
cd demi
make prereqs  # Installs dependencies
make build    # Should complete successfully
make test     # Should show 100% passing tests
```

📚 **New Contributor Resources:**
- [Contributing Quick Start](./CONTRIBUTING_QUICKSTART.md)
- [Good First Issues](https://github.com/bobrossrtx/demi/labels/good-first-issue)
- [Architecture Overview](./docs/QUICK_REFERENCE.md)

💡 **Perfect starter tasks:**
- Implement missing FPU opcodes (2-3 hours each)
- Add assembly examples (1-2 hours each)
- Improve documentation

🤝 **Get Help:**
- Comment on any issue if you need guidance
- We typically respond within 24 hours
- All contributions are welcome, no matter how small!

Looking forward to your contribution! 🎉
            """
        
        # Post welcome comment (this would be implemented with GitHub API)
        print(f"Would welcome {contributor['username']} with: {welcome_message}")
    
    def auto_assign_mentors(self, contributor):
        """Automatically assign mentors to new contributors"""
        # This would assign experienced contributors as mentors
        pass
    
    def track_contributor_progress(self, username):
        """Track and encourage contributor progress"""
        # Monitor their first PR progress
        # Send encouraging messages
        # Offer help if stuck
        pass

if __name__ == "__main__":
    onboarding = AutoOnboarding()
    new_contributors = onboarding.detect_new_contributors()
    
    for contributor in new_contributors:
        onboarding.auto_welcome_new_contributor(contributor)