#!/usr/bin/env python3
"""
Generate compile_commands.json for clangd IntelliSense support
This script reads the Makefile configuration and generates proper compile commands.
"""

import json
import os
import glob
from pathlib import Path

def find_source_files(src_dir):
    """Find all C++ source files in the project."""
    cpp_files = []
    for root, dirs, files in os.walk(src_dir):
        # Skip build directories
        dirs[:] = [d for d in dirs if not d.startswith('build')]
        
        for file in files:
            if file.endswith(('.cpp', '.cc', '.cxx')):
                cpp_files.append(os.path.join(root, file))
    return cpp_files

def generate_compile_commands():
    """Generate compile_commands.json for the Demi project."""
    
    project_dir = os.path.abspath(os.path.dirname(__file__))
    src_dir = os.path.join(project_dir, "src")
    
    # Compiler flags from your Makefile (debug configuration)
    compile_flags = [
        "g++",
        "-Wall", "-Wextra", "-Wpedantic",
        "-std=c++17",
        "-g3", "-O0",
        "-DDEMI_DEBUG_BUILD",
        "-D_GLIBCXX_DEBUG", 
        "-DDEMI_DEBUG_MODE",
        "-DVM_DEBUG_BOUNDS",
        "-DVM_DEBUG_LOGGING", 
        "-DVM_FULL_VALIDATION",
        "-Iextern/fmt/include",
        "-Isrc",
        "-c"
    ]
    
    # Find all source files
    source_files = find_source_files(src_dir)
    
    # Generate compile commands
    compile_commands = []
    
    for src_file in source_files:
        # Convert to absolute path
        abs_src_file = os.path.abspath(src_file)
        
        # Create object file path
        rel_path = os.path.relpath(src_file, src_dir)
        obj_file = os.path.join("build", rel_path).replace('.cpp', '.o').replace('.cc', '.o').replace('.cxx', '.o')
        
        # Build command
        command = compile_flags + [src_file, "-o", obj_file]
        
        compile_commands.append({
            "directory": project_dir,
            "command": " ".join(command),
            "file": abs_src_file
        })
    
    # Add external library sources (fmt)
    fmt_src = os.path.join(project_dir, "extern/fmt/src/format.cc")
    if os.path.exists(fmt_src):
        compile_commands.append({
            "directory": project_dir,
            "command": " ".join(compile_flags + [fmt_src, "-o", "build/fmt/format.o"]),
            "file": os.path.abspath(fmt_src)
        })
    
    return compile_commands

def main():
    """Main function to generate and write compile_commands.json."""
    try:
        project_dir = os.path.abspath(os.path.dirname(__file__))
        os.chdir(project_dir)
        compile_commands = generate_compile_commands()
        
        # Write to file
        with open("compile_commands.json", "w") as f:
            json.dump(compile_commands, f, indent=2)
        
        print(f"✅ Generated compile_commands.json with {len(compile_commands)} entries")
        print("🛠️  Clangd should now have proper IntelliSense support")
        print("📝 Reload VS Code window to apply changes")
        
    except Exception as e:
        print(f"❌ Error generating compile_commands.json: {e}")
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())