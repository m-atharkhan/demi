#!/usr/bin/env python3
"""
Automated Community Growth System for DemiEngine
Generates and posts content across multiple platforms automatically
"""

import tweepy
import praw
import requests
import json
import random
import time
from datetime import datetime, timedelta
import os
import subprocess
from dotenv import load_dotenv

# Load environment variables
load_dotenv('/workspaces/demi/config/auto_growth_config.env')

# Import our AI provider system
from ai_providers import AIProviderManager

class AutoCommunityGrowth:
    def __init__(self):
        # Initialize AI provider manager
        self.ai_manager = AIProviderManager()
        
        print(f"🤖 Using AI provider: {self.ai_manager.get_current_provider_name()}")
        
        self.setup_social_apis()
        
    def setup_social_apis(self):
        # Twitter API v2 setup
        try:
            self.twitter = tweepy.Client(
                bearer_token=os.getenv('TWITTER_BEARER_TOKEN'),
                consumer_key=os.getenv('TWITTER_API_KEY'),
                consumer_secret=os.getenv('TWITTER_API_SECRET'),
                access_token=os.getenv('TWITTER_ACCESS_TOKEN'),
                access_token_secret=os.getenv('TWITTER_ACCESS_SECRET')
            )
        except Exception as e:
            print(f"Twitter API setup failed: {e}")
            self.twitter = None
        
        # Reddit API setup (skip if credentials not available)
        reddit_client_id = os.getenv('REDDIT_CLIENT_ID')
        reddit_client_secret = os.getenv('REDDIT_CLIENT_SECRET')
        
        if reddit_client_id and reddit_client_secret and reddit_client_id.strip() and reddit_client_secret.strip():
            try:
                self.reddit = praw.Reddit(
                    client_id=reddit_client_id,
                    client_secret=reddit_client_secret,
                    username=os.getenv('REDDIT_USERNAME'),
                    password=os.getenv('REDDIT_PASSWORD'),
                    user_agent='DemiEngine Community Bot v1.0 by /u/YOUR_USERNAME'
                )
                print("✅ Reddit API configured")
            except Exception as e:
                print(f"❌ Reddit API setup failed: {e}")
                self.reddit = None
        else:
            print("⏭️  Skipping Reddit (no credentials configured - Twitter-only mode)")
            self.reddit = None
    
    def get_project_status(self):
        """Automatically analyze the codebase to generate current status"""
        try:
            # Get recent commits
            result = subprocess.run(['git', 'log', '--oneline', '-10'], 
                                  capture_output=True, text=True)
            recent_commits = result.stdout
            
            # Count files and lines
            result = subprocess.run(['find', 'src', '-name', '*.cpp', '-o', '-name', '*.hpp'], 
                                  capture_output=True, text=True)
            file_count = len(result.stdout.strip().split('\n'))
            
            # Get test status
            result = subprocess.run(['make', 'test'], capture_output=True, text=True)
            test_passing = 'All tests passed!' in result.stdout
            
            return {
                'recent_commits': recent_commits,
                'file_count': file_count,
                'test_status': test_passing,
                'timestamp': datetime.now().isoformat()
            }
        except Exception as e:
            print(f"Error getting project status: {e}")
            return {}
    
    def generate_technical_content(self, content_type, project_status):
        """Generate technical content using AI providers"""
        
        prompts = {
            'twitter_thread': f"""
            Create a Twitter thread about DemiEngine, a virtual machine for a revolutionary programming language.
            
            Project status: {project_status}
            
            Make it technical but accessible. Include:
            - Recent development progress
            - Why it's interesting for systems programmers
            - Call to action for contributors
            - Use relevant hashtags
            
            Format as numbered tweets (1/N format).
            Keep each tweet under 280 characters.
            """,
            
            'reddit_post': f"""
            Create a Reddit post for r/programming about DemiEngine.
            
            Project status: {project_status}
            
            Make it engaging and technical. Include:
            - What makes DemiEngine unique
            - Current development status
            - Why developers should contribute
            - Technical challenges we're solving
            
            Target audience: experienced developers interested in systems programming.
            """,
            
            'blog_post': f"""
            Write a technical blog post about a recent development in DemiEngine.
            
            Project status: {project_status}
            
            Choose an interesting technical topic from recent commits.
            Make it educational and showcase the project's sophistication.
            Target audience: systems programmers and compiler engineers.
            """
        }
        
        try:
            return self.ai_manager.generate_content(prompts[content_type])
        except Exception as e:
            print(f"Error generating content: {e}")
            return self.generate_mock_content(content_type, project_status)
    
    def generate_mock_content(self, content_type, project_status):
        """Generate mock content when OpenAI is not available"""
        
        if content_type == 'twitter_thread':
            return """🧵 1/5 Working on DemiEngine - a revolutionary virtual machine for the future of programming languages!

🎯 Building towards a language that's the "Vim of programming" - completely customizable per project.

#programming #systems #cpp #opensource

2/5 Currently expanding the assembly instruction set:
✅ 63 core opcodes implemented  
✅ FPU operations in progress
✅ 100% test coverage maintained
✅ 134 register architecture

The foundation is rock-solid! 🚀

3/5 What makes DemiEngine special:
🔧 Dual-mode execution (interpretation + native compilation)
🎭 Project-specific language customization  
⚡ x86-64 code generation coming soon
🧪 Comprehensive test framework

4/5 Looking for contributors! Perfect for developers wanting to learn:
- CPU architecture & instruction design
- Systems programming in C++
- Floating-point arithmetic
- Virtual machine implementation

5/5 Ready to help build the future of programming languages?

🔗 GitHub: https://github.com/bobrossrtx/demi
📚 Docs: Well-documented with examples
🤝 Mentorship: We help new contributors succeed

#DemiEngine #OpenSource #SystemsProgramming"""
        
        elif content_type == 'reddit_post':
            return """# DemiEngine: Building the "Vim of Programming Languages"

I'm building DemiEngine, a virtual machine that will power a revolutionary programming language with unprecedented customization capabilities.

## What makes it unique?

- **Complete customization**: Every aspect configurable via `demi.toml`
- **Project-specific dialects**: Different language behavior per project
- **Dual-mode execution**: Fast interpretation + native compilation
- **Solid foundation**: 63 opcodes, 134 registers, 100% test coverage

## Current status (Stage 2/8):

Currently expanding the assembly instruction set with FPU operations, SIMD instructions, and more. The core VM is complete and battle-tested.

Recent progress:
- ✅ FPU stack management implemented
- ✅ Floating-point arithmetic opcodes
- ✅ Comprehensive test framework
- ⚡ Working toward x86-64 code generation

## Why contribute?

Perfect opportunity to learn:
- Systems programming and CPU architecture
- Virtual machine implementation
- IEEE 754 floating-point arithmetic
- Assembly language design
- Compiler/interpreter development

We provide mentorship and have comprehensive documentation. Every contribution is protected by our 100% test coverage.

## Looking for contributors!

**Good first issues available:**
- Implement missing FPU opcodes (2-3 hours each)
- Add SIMD vector operations
- Create assembly examples
- Improve documentation

**GitHub**: https://github.com/bobrossrtx/demi

Help us build the future of programming languages! The vision is a language where every aspect can be customized per project - imagine configuring syntax, keywords, and behavior all through a simple config file.

Questions welcome! 🚀"""
        
        else:  # blog_post
            return """# Implementing FPU Operations in DemiEngine

Today I want to share our progress on implementing floating-point operations in DemiEngine, our virtual machine for a revolutionary customizable programming language.

## The Challenge

Building a complete FPU (Floating-Point Unit) emulation requires careful attention to IEEE 754 standards while maintaining performance and accuracy...

[Content continues with technical details about FPU implementation]

## What's Next

We're looking for contributors to help expand the instruction set. Perfect for developers interested in systems programming and CPU architecture.

**GitHub**: https://github.com/bobrossrtx/demi"""
    
    def post_to_twitter(self, content):
        """Post thread to Twitter"""
        try:
            # Split into tweets if it's a thread
            tweets = content.split('\n\n')
            tweet_ids = []
            
            for i, tweet in enumerate(tweets):
                if tweet.strip():
                    if i == 0:
                        # First tweet
                        response = self.twitter.create_tweet(text=tweet.strip())
                        tweet_ids.append(response.data['id'])
                    else:
                        # Reply to previous tweet
                        response = self.twitter.create_tweet(
                            text=tweet.strip(),
                            in_reply_to_tweet_id=tweet_ids[-1]
                        )
                        tweet_ids.append(response.data['id'])
            
            print(f"Posted Twitter thread with {len(tweet_ids)} tweets")
            return True
        except Exception as e:
            print(f"Error posting to Twitter: {e}")
            return False
    
    def post_to_reddit(self, title, content, subreddit='programming'):
        """Post to Reddit"""
        if not self.reddit:
            print("Reddit API not configured, skipping post")
            return False
            
        try:
            subreddit_obj = self.reddit.subreddit(subreddit)
            submission = subreddit_obj.submit(title=title, selftext=content)
            print(f"Posted to r/{subreddit}: {submission.url}")
            return True
        except Exception as e:
            print(f"Error posting to Reddit: {e}")
            return False
    
    def auto_respond_to_mentions(self):
        """Automatically respond to mentions and comments"""
        try:
            # Check GitHub issues for new comments
            self.respond_to_github_activity()
            
            # Check Twitter mentions
            self.respond_to_twitter_mentions()
            
            # Check Reddit replies
            self.respond_to_reddit_activity()
            
        except Exception as e:
            print(f"Error in auto-responses: {e}")
    
    def respond_to_github_activity(self):
        """Auto-respond to GitHub issues and PRs"""
        # This would use GitHub API to:
        # - Welcome new contributors
        # - Provide guidance on issues
        # - Thank people for PRs
        pass
    
    def respond_to_twitter_mentions(self):
        """Auto-respond to Twitter mentions and replies"""
        if not self.twitter:
            return
            
        try:
            # For now, just log that we're checking mentions
            # Twitter API v2 mentions require special permissions
            print("📭 Checking for Twitter mentions...")
            print("💡 Note: Twitter API v2 mentions require elevated access")
            
            # Placeholder for future implementation when permissions are available
            # This prevents the error and allows the automation to continue
            return
                
        except Exception as e:
            print(f"Error responding to Twitter mentions: {e}")
    
    def respond_to_reddit_activity(self):
        """Auto-respond to Reddit comments and messages"""
        if not self.reddit:
            return
            
        try:
            # Check inbox for mentions/replies
            for item in self.reddit.inbox.unread(limit=5):
                if isinstance(item, praw.models.Comment):
                    # It's a comment reply
                    comment_text = item.body
                    
                    # Generate helpful response
                    prompt = f"""Generate a helpful response to this Reddit comment about DemiEngine:
                    "{comment_text}"
                    
                    Make it:
                    - Informative and technical
                    - Encouraging for developers
                    - Include relevant links if helpful
                    - Professional tone
                    """
                    
                    response_text = self.ai_manager.generate_content(prompt)
                    
                    if response_text:
                        # Reply to the comment
                        item.reply(response_text)
                        item.mark_read()
                        print(f"✅ Replied to Reddit comment in r/{item.subreddit.display_name}")
                        
                        # Rate limiting
                        time.sleep(5)
                
        except Exception as e:
            print(f"Error responding to Reddit activity: {e}")
    
    def generate_good_first_issues(self):
        """Automatically generate good first issues based on codebase analysis"""
        try:
            # Analyze codebase for missing features
            missing_opcodes = self.find_missing_opcodes()
            missing_tests = self.find_missing_tests()
            missing_docs = self.find_missing_documentation()
            
            issues_to_create = []
            
            for opcode in missing_opcodes[:3]:  # Limit to 3 new issues per run
                prompt = f"Create a GitHub issue for implementing the {opcode} opcode in DemiEngine. Make it beginner-friendly with clear steps."
                issue_content = self.ai_manager.generate_content(prompt)
                
                issues_to_create.append({
                    'title': f"[GOOD FIRST ISSUE] Implement {opcode} opcode",
                    'body': issue_content,
                    'labels': ['good-first-issue', 'assembly', 'beginner-friendly']
                })
            
            return issues_to_create
            
        except Exception as e:
            print(f"Error generating issues: {e}")
            return []
    
    def find_missing_opcodes(self):
        """Analyze codebase to find missing opcodes"""
        # This would parse the opcodes enum and find unimplemented ones
        return ['FMUL', 'FDIV', 'FSQRT', 'FSIN', 'FCOS']  # Example
    
    def find_missing_tests(self):
        """Find features that need more test coverage"""
        return []
    
    def find_missing_documentation(self):
        """Find code that needs documentation"""
        return []
    
    def run_daily_automation(self):
        """Main function to run daily automation"""
        print("Starting daily automation...")
        
        # Get current project status
        project_status = self.get_project_status()
        
        # Decide what content to create today
        today = datetime.now().weekday()
        
        if today == 0:  # Monday - Twitter thread
            content = self.generate_technical_content('twitter_thread', project_status)
            if content:
                self.post_to_twitter(content)
                
        elif today == 2:  # Wednesday - Reddit post
            content = self.generate_technical_content('reddit_post', project_status)
            if content:
                lines = content.split('\n', 1)
                title = lines[0] if lines else "DemiEngine Development Update"
                body = lines[1] if len(lines) > 1 else content
                
                # Start with low-restriction subreddits for new accounts
                # Gradually expand as karma grows
                target_subs = ['test', 'programming', 'opensource', 'ProgrammingLanguages', 'cpp']
                
                for i, subreddit in enumerate(target_subs):
                    if i > 0:  # Don't delay the first post
                        time.sleep(600)  # 10 minute delay between posts (be safe)
                    
                    try:
                        success = self.post_to_reddit(title, body, subreddit)
                        if success:
                            print(f"✅ Successfully posted to r/{subreddit}")
                            break  # Only post to one subreddit per day initially
                        else:
                            print(f"❌ Failed to post to r/{subreddit}")
                    except Exception as e:
                        print(f"❌ Error posting to r/{subreddit}: {e}")
                        continue
                
        elif today == 4:  # Friday - Generate new issues + Reddit technical post
            issues = self.generate_good_first_issues()
            for issue in issues:
                self.create_github_issue(issue)
                
            # Also post a technical update to Reddit on Fridays
            tech_content = self.generate_technical_content('blog_post', project_status)
            if tech_content:
                title = "DemiEngine Technical Update: Recent Progress"
                self.post_to_reddit(title, tech_content, 'systems')
        
        # Always check for mentions and respond
        self.auto_respond_to_mentions()
        
        print("Daily automation completed!")

if __name__ == "__main__":
    growth_bot = AutoCommunityGrowth()
    growth_bot.run_daily_automation()