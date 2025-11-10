# DemiEngine Version Management

This directory contains version management and changelog generation tools for DemiEngine.

## Files

- `generate_changelog.sh` - Main changelog generation and version management script
- `VERSION` - Current version number (semantic versioning: MAJOR.MINOR.PATCH)
- `CHANGELOG.md` - Generated changelog from git history

## Usage

### Check Current Version
```bash
./generate_changelog.sh version
```

### Generate Changelog
```bash
# Generate full changelog from git history
./generate_changelog.sh generate

# Generate changelog since specific tag
./generate_changelog.sh generate v0.9.0
```

### Create a New Release
```bash
# Patch release (1.0.0 -> 1.0.1) - bug fixes
./generate_changelog.sh release patch

# Minor release (1.0.0 -> 1.1.0) - new features
./generate_changelog.sh release minor  

# Major release (1.0.0 -> 2.0.0) - breaking changes
./generate_changelog.sh release major
```

## Release Process

The `release` command performs these actions automatically:
1. Increments version number in `VERSION` file
2. Updates version constants in `src/config.hpp`
3. Generates changelog entry for the new version
4. Commits version changes with conventional commit format
5. Creates a git tag for the release

## Commit Categories

The changelog generator automatically categorizes commits:

| Pattern | Category | Icon |
|---------|----------|------|
| `feat:` or Feature keywords | ✨ Features | |
| `fix:` or Fix keywords | 🐛 Bug Fixes | |
| `perf:` or Performance keywords | ⚡ Performance Improvements | |
| `docs:` or Documentation keywords | 📚 Documentation | |
| `test:` or Test keywords | 🧪 Tests | |
| `refactor:` | ♻️ Code Refactoring | |
| `style:` | 💅 Code Style | |
| `build:` | 🔧 Build System | |
| `ci:` | 👷 Continuous Integration | |
| `chore:` | 🔨 Chores | |
| Others | 🔄 Other Changes | |

## Version Constants

Version information is automatically updated in `src/config.hpp`:

```cpp
#define DEMI_VERSION_MAJOR 1
#define DEMI_VERSION_MINOR 0  
#define DEMI_VERSION_PATCH 0
#define DEMI_VERSION_STRING "1.0.0"
```

These can be used in the application code to display version information.

## Semantic Versioning

This project follows [Semantic Versioning](https://semver.org/):

- **MAJOR** version when you make incompatible API changes
- **MINOR** version when you add functionality in a backwards compatible manner  
- **PATCH** version when you make backwards compatible bug fixes

## Examples

### Creating a patch release for bug fixes:
```bash
./generate_changelog.sh release patch
# Output: 1.0.0 -> 1.0.1
```

### Creating a minor release for new features:
```bash  
./generate_changelog.sh release minor
# Output: 1.0.1 -> 1.1.0
```

### Creating a major release for breaking changes:
```bash
./generate_changelog.sh release major  
# Output: 1.1.0 -> 2.0.0
```

## Integration with Build System

The Makefile can use the version information:

```makefile
VERSION := $(shell cat VERSION)
CPPFLAGS += -DDEMI_VERSION=\"$(VERSION)\"
```

## Automation

For CI/CD pipelines, you can automate releases:

```bash
# Automatically determine release type based on commit messages
# (This would require additional scripting)

# Manual release in CI
./generate_changelog.sh release patch
git push origin main --tags
```