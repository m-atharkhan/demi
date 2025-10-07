#!/usr/bin/env python3
"""
Enhanced Daily Post Generator for DemiEngine
Creates varied, engaging content automatically every day
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

# Try to import Gemini AI provider
try:
    import google.generativeai as genai
    GEMINI_AVAILABLE = True
except ImportError:
    GEMINI_AVAILABLE = False

class DailyPostGenerator:
    def __init__(self):
        # Setup AI providers with fallback support
        self.setup_ai_providers()
        self.setup_social_apis()
        
        # Daily content themes for variety
        self.daily_themes = {
            'monday': ['motivation', 'week_planning', 'goals'],
            'tuesday': ['technical_deep_dive', 'architecture', 'design_decisions'],
            'wednesday': ['community_spotlight', 'contributor_highlights', 'open_source'],
            'thursday': ['tutorials', 'how_to', 'learning_resources'],
            'friday': ['progress_recap', 'achievements', 'weekend_plans'],
            'saturday': ['fun_facts', 'behind_scenes', 'casual_updates'],
            'sunday': ['reflection', 'roadmap', 'future_vision']
        }
        
        # Content variety templates
        self.post_types = [
            'thread', 'single_tweet', 'poll', 'image_post', 
            'code_snippet', 'tip_of_day', 'question', 'announcement'
        ]
    
    def setup_ai_providers(self):
        """Setup Gemini AI provider only"""
        self.ai_providers = []
        
        # Setup Gemini with free model
        if GEMINI_AVAILABLE:
            gemini_key = os.getenv('GEMINI_API_KEY')
            if gemini_key:
                try:
                    genai.configure(api_key=gemini_key)
                    # Use the correct free Gemini model name
                    self.gemini_model = genai.GenerativeModel('gemini-2.5-flash')  # Free model
                    self.ai_providers.append('gemini')
                    print(f"✅ Gemini (free model gemini-2.5-flash) configured successfully")
                except Exception as e:
                    print(f"❌ Gemini setup failed: {e}")
                    self.gemini_model = None
            else:
                print("⚠️  No GEMINI_API_KEY found in environment")
                self.gemini_model = None
        else:
            print("⚠️  google-generativeai package not available")
            self.gemini_model = None
        
        print(f"🤖 AI provider configured: {'Gemini (free)' if self.ai_providers else 'None (using fallback content)'}")
    
    
    def get_recent_work_context(self):
        """Get context about recent work for AI content generation"""
        try:
            context = {}
            
            # Get recent commits with more detail
            result = subprocess.run(
                ['git', 'log', '--oneline', '--since="3 days ago"', '--pretty=format:%h %s'], 
                capture_output=True, text=True
            )
            if result.stdout.strip():
                recent_commits = result.stdout.strip().split('\n')[:5]  # Last 5 commits
                context['recent_commits'] = recent_commits
            
            # Get modified files recently
            result = subprocess.run(
                ['git', 'diff', '--name-only', 'HEAD~5..HEAD'], 
                capture_output=True, text=True
            )
            if result.stdout.strip():
                modified_files = result.stdout.strip().split('\n')[:10]  # Last 10 files
                context['modified_files'] = modified_files
            
            # Get current branch and any active work
            result = subprocess.run(['git', 'status', '--porcelain'], capture_output=True, text=True)
            if result.stdout.strip():
                context['working_changes'] = len(result.stdout.strip().split('\n'))
            
            # Check for recent test activity
            result = subprocess.run(['find', 'tests', '-name', '*.asm', '-mtime', '-3'], capture_output=True, text=True)
            if result.stdout.strip():
                context['recent_test_activity'] = len(result.stdout.strip().split('\n'))
            
            # Check documentation updates
            result = subprocess.run(['find', 'docs', '-name', '*.md', '-mtime', '-7'], capture_output=True, text=True)
            if result.stdout.strip():
                context['recent_docs'] = len(result.stdout.strip().split('\n'))
            
            return context
            
        except Exception as e:
            print(f"Error getting work context: {e}")
            return {}
    
    def setup_social_apis(self):
        """Setup social media APIs"""
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
        
        try:
            self.reddit = praw.Reddit(
                client_id=os.getenv('REDDIT_CLIENT_ID'),
                client_secret=os.getenv('REDDIT_CLIENT_SECRET'),
                username=os.getenv('REDDIT_USERNAME'),
                password=os.getenv('REDDIT_PASSWORD'),
                user_agent='DemiEngine Daily Bot v2.0 by /u/Significant_Copy2108'
            )
        except Exception as e:
            print(f"Reddit API setup failed: {e}")
            self.reddit = None
    
    def get_dynamic_project_stats(self):
        """Get real-time project statistics for dynamic content"""
        try:
            stats = {}
            
            # Git statistics
            result = subprocess.run(['git', 'log', '--oneline', '--since="1 week ago"'], 
                                  capture_output=True, text=True)
            stats['commits_this_week'] = len(result.stdout.strip().split('\n')) if result.stdout.strip() else 0
            
            # Code statistics
            result = subprocess.run(['find', 'src', '-name', '*.cpp', '-o', '-name', '*.hpp'], 
                                  capture_output=True, text=True)
            stats['source_files'] = len(result.stdout.strip().split('\n')) if result.stdout.strip() else 0
            
            # Test statistics
            result = subprocess.run(['find', 'tests', '-name', '*.asm'], 
                                  capture_output=True, text=True)
            stats['test_files'] = len(result.stdout.strip().split('\n')) if result.stdout.strip() else 0
            
            # Lines of code
            try:
                result = subprocess.run(['wc', '-l'] + subprocess.run(['find', 'src', '-name', '*.cpp', '-o', '-name', '*.hpp'], 
                                       capture_output=True, text=True).stdout.strip().split('\n'), 
                                      capture_output=True, text=True)
                lines = result.stdout.strip().split('\n')[-1]  # Last line has total
                stats['lines_of_code'] = int(lines.split()[0]) if lines else 0
            except:
                stats['lines_of_code'] = 'Unknown'
            
            # Recent activity
            result = subprocess.run(['git', 'log', '--oneline', '-1'], 
                                  capture_output=True, text=True)
            stats['latest_commit'] = result.stdout.strip()
            
            # Branch info
            result = subprocess.run(['git', 'branch', '--show-current'], 
                                  capture_output=True, text=True)
            stats['current_branch'] = result.stdout.strip()
            
            return stats
            
        except Exception as e:
            print(f"Error getting project stats: {e}")
            return {}
    
    def generate_daily_content(self):
        """Generate content based on current day and theme"""
        today = datetime.now()
        day_name = today.strftime('%A').lower()
        themes = self.daily_themes.get(day_name, ['general', 'update'])
        theme = random.choice(themes)
        
        stats = self.get_dynamic_project_stats()
        
        # Choose content type based on day
        if day_name in ['monday', 'friday']:
            content_type = 'twitter_thread'
        elif day_name in ['tuesday', 'thursday']:
            content_type = 'technical_post'
        elif day_name == 'wednesday':
            content_type = 'community_post'
        else:
            content_type = random.choice(['single_tweet', 'tip_of_day', 'poll'])
        
        return self.create_themed_content(theme, content_type, stats, day_name)
    
    def generate_ai_content(self, prompt, provider='gemini'):
        """Generate content using Gemini AI only"""
        if provider != 'gemini' or not self.gemini_model:
            return None
        
        try:
            # Use Gemini free model to generate content
            response = self.gemini_model.generate_content(prompt)
            return response.text
                
        except Exception as e:
            print(f"Gemini generation failed: {e}")
            return None
    
    def create_context_aware_content(self, theme, content_type, stats, day_name):
        """Create content that's aware of recent work context"""
        work_context = self.get_recent_work_context()
        
        # Build a context-aware prompt
        context_info = ""
        if work_context.get('recent_commits'):
            context_info += f"Recent commits:\n"
            for commit in work_context['recent_commits'][:3]:
                context_info += f"- {commit}\n"
        
        if work_context.get('modified_files'):
            context_info += f"\nRecently modified files: {', '.join(work_context['modified_files'][:5])}\n"
        
        if work_context.get('working_changes'):
            context_info += f"\nActive changes: {work_context['working_changes']} files modified\n"
        
        prompt = f"""
        Create a {content_type} for {day_name} with a {theme} theme about DemiEngine development.
        
        Recent project activity:
        {context_info}
        
        Project stats:
        - Commits this week: {stats.get('commits_this_week', 0)}
        - Source files: {stats.get('source_files', 0)}
        - Lines of code: {stats.get('lines_of_code', 0)}
        - Latest commit: {stats.get('latest_commit', 'Active development')}
        
        Make the content:
        1. Relevant to recent development work
        2. Engaging for developers interested in systems programming
        3. Include specific technical details when appropriate
        4. Use appropriate hashtags for the platform
        5. Keep it under 280 characters if it's a single tweet
        
        Focus on the recent work context to make it feel current and authentic.
        """
        
        # Try AI generation first
        ai_content = self.generate_ai_content(prompt)
        if ai_content:
            return {
                'content': ai_content,
                'type': content_type,
                'theme': theme,
                'day': day_name,
                'context_aware': True
            }
        
        # Fallback to context-aware mock content
        return self.create_context_aware_mock_content(theme, content_type, stats, day_name, work_context)
    
    def create_context_aware_mock_content(self, theme, content_type, stats, day_name, work_context):
        """Create context-aware mock content based on recent work"""
        
        # Extract recent work info
        recent_work = ""
        if work_context.get('recent_commits'):
            latest_commit = work_context['recent_commits'][0] if work_context['recent_commits'] else ""
            recent_work = f"Latest: {latest_commit}"
        
        modified_areas = ""
        if work_context.get('modified_files'):
            files = work_context['modified_files'][:3]
            if any('automation' in f.lower() or 'daily' in f.lower() for f in files):
                modified_areas = "automation system"
            elif any('fpu' in f.lower() or 'float' in f.lower() for f in files):
                modified_areas = "FPU operations"
            elif any('test' in f.lower() for f in files):
                modified_areas = "testing framework"
            elif any('doc' in f.lower() for f in files):
                modified_areas = "documentation"
            else:
                modified_areas = "core engine"
        
        # Create content based on actual recent work
        if theme == 'technical_deep_dive' and 'automation' in recent_work.lower():
            content = f"""🤖 Deep dive into automation architecture!

Just finished implementing multi-provider AI content generation for DemiEngine:

🔧 OpenAI + Gemini + Claude fallback system
📊 Context-aware posting based on git activity  
🎯 Smart content themes tied to development phases
⚡ {stats.get('commits_this_week', 0)} commits this week pushing the boundaries

The goal: Build systems that build themselves while I focus on the virtual machine core.

{recent_work}

#Automation #SystemsProgramming #AI #DemiEngine"""

        elif theme == 'progress_recap':
            content = f"""📈 This Week in DemiEngine Development

Recent achievements:
✅ {stats.get('commits_this_week', 0)} commits across {stats.get('source_files', 0)} source files
✅ Enhanced {modified_areas} implementation  
✅ {work_context.get('recent_test_activity', 0)} new tests added
✅ {stats.get('lines_of_code', 0)} total lines of battle-tested code

{recent_work}

Building the future of configurable programming languages, one commit at a time! 🚀

Weekend plans: More {modified_areas} improvements and documentation updates.

#Progress #OpenSource #VirtualMachine"""

        elif theme == 'motivation':
            content = f"""� Monday Motivation: Why I Build DemiEngine

Every commit in our {modified_areas} reminds me why systems programming matters:
🧠 Direct control over every instruction
⚡ Performance optimizations that actually count  
🎯 Building tools others will use to build amazing things

This week: {stats.get('commits_this_week', 0)} commits, focusing on {modified_areas}

{recent_work}

What low-level challenge are you tackling this week? 🔥

#MondayMotivation #SystemsProgramming #DemiEngine"""

        else:
            # Generic context-aware content
            content = f"""🔬 Working on {modified_areas} in DemiEngine today!

Recent focus areas:
• {modified_areas.title()} enhancements
• {stats.get('commits_this_week', 0)} commits this week
• {stats.get('source_files', 0)} source files maintained

{recent_work}

Building a virtual machine that adapts to YOUR project needs. The future of programming is customizable! 

#DemiEngine #SystemsProgramming #OpenSource"""
        
        return {
            'content': content,
            'type': content_type,
            'theme': theme,
            'day': day_name,
            'context_aware': True,
            'work_context': modified_areas
        }
    
    def generate_daily_content(self):
        """Generate content based on current day and recent work context"""
        today = datetime.now()
        day_name = today.strftime('%A').lower()
        themes = self.daily_themes.get(day_name, ['general', 'update'])
        theme = random.choice(themes)
        
        stats = self.get_dynamic_project_stats()
        
        # Choose content type based on day
        if day_name in ['monday', 'friday']:
            content_type = 'twitter_thread'
        elif day_name in ['tuesday', 'thursday']:
            content_type = 'technical_post'
        elif day_name == 'wednesday':
            content_type = 'community_post'
        else:
            content_type = random.choice(['single_tweet', 'tip_of_day', 'poll'])
        
        # Use context-aware generation
        return self.create_context_aware_content(theme, content_type, stats, day_name)
    
    def create_poll_content(self):
        """Create engaging poll content"""
        polls = [
            {
                'question': "What's most important in a programming language?",
                'options': ["Performance", "Ease of use", "Customizability", "Community"]
            },
            {
                'question': "Which DemiEngine feature interests you most?",
                'options': ["FPU operations", "Custom syntax", "x86-64 codegen", "SIMD support"]
            },
            {
                'question': "Best way to learn systems programming?",
                'options': ["Build a VM", "Write an OS", "Assembly projects", "Compiler design"]
            }
        ]
        
        poll = random.choice(polls)
        
        content = f"🗳️ Quick poll for fellow developers:\n\n{poll['question']}\n\n"
        for i, option in enumerate(poll['options'], 1):
            content += f"{i}. {option}\n"
        
        content += "\nDrop your vote and thoughts below! 🤔\n\n#Programming #Poll #DemiEngine"
        
        return {
            'content': content,
            'type': 'poll',
            'theme': 'engagement',
            'day': datetime.now().strftime('%A').lower()
        }
    
    def post_daily_content(self):
        """Generate and post daily content"""
        print(f"🚀 Generating daily content for {datetime.now().strftime('%A, %B %d')}")
        
        # Occasionally create polls for engagement
        if random.random() < 0.2:  # 20% chance
            content_data = self.create_poll_content()
        else:
            content_data = self.generate_daily_content()
        
        content = content_data['content']
        content_type = content_data['type']
        theme = content_data['theme']
        
        print(f"📝 Content type: {content_type}, Theme: {theme}")
        
        # Post to Twitter
        if self.twitter and content_type in ['twitter_thread', 'single_tweet', 'poll', 'tip_of_day']:
            try:
                if '\n\n' in content and len(content) > 280:
                    # Thread
                    self.post_twitter_thread(content)
                else:
                    # Single tweet
                    self.post_single_tweet(content)
            except Exception as e:
                print(f"Error posting to Twitter: {e}")
        
        # Post to Reddit (less frequently, only for substantial content)
        if content_type in ['technical_post', 'community_post', 'tutorials'] and random.random() < 0.6:
            try:
                self.post_to_reddit_smart(content, theme)
            except Exception as e:
                print(f"Error posting to Reddit: {e}")
    
    def post_twitter_thread(self, content):
        """Post a Twitter thread"""
        tweets = [tweet.strip() for tweet in content.split('\n\n') if tweet.strip()]
        tweet_ids = []
        
        for i, tweet in enumerate(tweets):
            if len(tweet) > 280:
                # Split long tweets
                tweet = tweet[:277] + "..."
            
            if i == 0:
                response = self.twitter.create_tweet(text=tweet)
                tweet_ids.append(response.data['id'])
            else:
                response = self.twitter.create_tweet(
                    text=tweet,
                    in_reply_to_tweet_id=tweet_ids[-1]
                )
                tweet_ids.append(response.data['id'])
        
        print(f"✅ Posted Twitter thread with {len(tweet_ids)} tweets")
    
    def post_single_tweet(self, content):
        """Post a single tweet"""
        if len(content) > 280:
            content = content[:277] + "..."
        
        response = self.twitter.create_tweet(text=content)
        print(f"✅ Posted single tweet: {response.data['id']}")
    
    def post_to_reddit_smart(self, content, theme):
        """Post to Reddit with smart subreddit selection"""
        # Choose subreddit based on theme and content
        subreddit_map = {
            'technical_deep_dive': ['programming', 'systems', 'cpp'],
            'tutorials': ['learnprogramming', 'programming', 'systems'],
            'community_spotlight': ['opensource', 'programming'],
            'motivation': ['programming', 'getmotivated'],
            'progress_recap': ['programming', 'opensource']
        }
        
        possible_subs = subreddit_map.get(theme, ['programming'])
        target_sub = random.choice(possible_subs)
        
        # Extract title and body
        lines = content.split('\n', 1)
        title = lines[0].replace('#', '').replace('*', '').strip()
        body = lines[1] if len(lines) > 1 else content
        
        # Clean up title
        if len(title) > 150:
            title = title[:147] + "..."
        
        try:
            subreddit_obj = self.reddit.subreddit(target_sub)
            submission = subreddit_obj.submit(title=title, selftext=body)
            print(f"✅ Posted to r/{target_sub}: {submission.url}")
            return True
        except Exception as e:
            print(f"❌ Failed to post to r/{target_sub}: {e}")
            return False

def main():
    """Run daily content generation"""
    generator = DailyPostGenerator()
    generator.post_daily_content()

if __name__ == "__main__":
    main()