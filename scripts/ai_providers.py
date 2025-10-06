#!/usr/bin/env python3
"""
AI Provider System for DemiEngine Auto Growth Scripts
Supports multiple AI providers: Gemini, OpenAI, and others
"""

import os
import time
from typing import Optional, Dict, Any
from abc import ABC, abstractmethod

class AIProvider(ABC):
    @abstractmethod
    def generate_content(self, prompt: str, max_tokens: int = 1000, **kwargs) -> Optional[str]:
        pass
    
    @abstractmethod
    def is_available(self) -> bool:
        pass

class GeminiProvider(AIProvider):
    def __init__(self):
        self.model = None
        self.last_request_time = 0
        self.setup()
    
    def setup(self):
        try:
            import google.generativeai as genai
            
            api_key = os.getenv('GEMINI_API_KEY') or os.getenv('GOOGLE_API_KEY')
            if not api_key or api_key == "your_gemini_api_key_here":
                print("Warning: GEMINI_API_KEY or GOOGLE_API_KEY not found or not set")
                return
            
            genai.configure(api_key=api_key)
            # Use an available model - gemini-2.5-flash is the latest fast model
            self.model = genai.GenerativeModel('gemini-2.5-flash')
            print("✅ Gemini AI provider initialized successfully")
            
        except ImportError:
            print("❌ google-generativeai library not installed. Run: pip install google-generativeai")
        except Exception as e:
            print(f"❌ Failed to initialize Gemini: {e}")
    
    def is_available(self) -> bool:
        return self.model is not None
    
    def generate_content(self, prompt: str, max_tokens: int = 1000, **kwargs) -> Optional[str]:
        if not self.is_available():
            return None
        
        try:
            # Rate limiting: wait if needed (free tier: 15 requests per minute)
            current_time = time.time()
            time_since_last = current_time - self.last_request_time
            if time_since_last < 4:  # 4 seconds between requests to be safe
                time.sleep(4 - time_since_last)
            
            response = self.model.generate_content(prompt)
            self.last_request_time = time.time()
            
            # Check if response has valid content
            if hasattr(response, 'candidates') and response.candidates:
                candidate = response.candidates[0]
                if hasattr(candidate, 'finish_reason'):
                    print(f"Finish reason: {candidate.finish_reason}")
                if hasattr(candidate, 'content') and candidate.content:
                    if hasattr(candidate.content, 'parts') and candidate.content.parts:
                        return candidate.content.parts[0].text
            
            # Fallback: try to get text directly
            if hasattr(response, 'text'):
                return response.text
            
            print(f"No valid content in response: {response}")
            return None
            
        except Exception as e:
            print(f"❌ Gemini generation error: {e}")
            return None

class OpenAIProvider(AIProvider):
    def __init__(self):
        self.client = None
        self.setup()
    
    def setup(self):
        try:
            import openai
            
            api_key = os.getenv('OPENAI_API_KEY')
            if not api_key or api_key.startswith('your_'):
                print("Warning: OPENAI_API_KEY not found or not set")
                return
            
            self.client = openai.OpenAI(api_key=api_key)
            print("✅ OpenAI provider initialized successfully")
            
        except ImportError:
            print("❌ openai library not installed. Run: pip install openai")
        except Exception as e:
            print(f"❌ Failed to initialize OpenAI: {e}")
    
    def is_available(self) -> bool:
        return self.client is not None
    
    def generate_content(self, prompt: str, max_tokens: int = 1000, **kwargs) -> Optional[str]:
        if not self.is_available():
            return None
        
        try:
            response = self.client.chat.completions.create(
                model="gpt-3.5-turbo",
                messages=[{"role": "user", "content": prompt}],
                max_tokens=max_tokens,
                temperature=kwargs.get('temperature', 0.7)
            )
            
            return response.choices[0].message.content
            
        except Exception as e:
            print(f"❌ OpenAI generation error: {e}")
            return None

class AIProviderManager:
    def __init__(self):
        self.providers = {}
        self.current_provider = None
        self.setup_providers()
    
    def setup_providers(self):
        # Initialize available providers
        self.providers['gemini'] = GeminiProvider()
        self.providers['openai'] = OpenAIProvider()
        
        # Select provider based on environment variable or availability
        preferred_provider = os.getenv('AI_PROVIDER', 'gemini').lower()
        
        if preferred_provider in self.providers and self.providers[preferred_provider].is_available():
            self.current_provider = self.providers[preferred_provider]
            self.current_provider_name = preferred_provider
            print(f"🎯 Using {preferred_provider} as AI provider")
        else:
            # Fallback to any available provider
            for name, provider in self.providers.items():
                if provider.is_available():
                    self.current_provider = provider
                    self.current_provider_name = name
                    print(f"🔄 Falling back to {name} as AI provider")
                    break
        
        if not self.current_provider:
            self.current_provider_name = "none"
            print("❌ No AI providers available. Please configure API keys.")
    
    def get_current_provider_name(self) -> str:
        return getattr(self, 'current_provider_name', 'unknown')
    
    def generate_content(self, prompt: str, max_tokens: int = 1000, **kwargs) -> Optional[str]:
        if not self.current_provider:
            print("❌ No AI provider available")
            return None
        
        return self.current_provider.generate_content(prompt, max_tokens, **kwargs)
    
    def is_available(self) -> bool:
        return self.current_provider is not None

# Global instance
ai_manager = AIProviderManager()