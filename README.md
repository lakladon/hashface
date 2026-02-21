# hashface

GitHub-style avatar generator in C++

by lakladon

## Description

hashface generates unique 12x12 pixel avatars similar to GitHub's identicons. Each avatar is symmetric and deterministic - the same input always produces the same avatar.

## Features

- Deterministic generation from any string input
- Symmetric avatars (mirrored pattern)
- 5-color palette matching GitHub's style
- ASCII art output in terminal
- PNG export with 10x scaling (120x120 pixels)
- No external image libraries required (uses zlib only)

## Building

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install zlib1g-dev

# Build
make

# Clean build files
make clean
```

## Usage

```bash
# Generate from name
./hashface --name "John"

# Generate from text
./hashface --text "hello world"

# Generate from email
./hashface --email "user@example.com"

# Generate random avatar
./hashface --random

# Specify output file
./hashface --name "John" --output "john.png"

# Show help
./hashface --help
```

## Command Line Options

| Option | Description |
|--------|-------------|
| `--name <string>` | Generate avatar from name |
| `--text <string>` | Generate avatar from text |
| `--email <string>` | Generate avatar from email |
| `--random` | Generate random avatar |
| `--output <file>` | Output filename (default: avatar.png) |
| `--help` | Show help message |

## Output

The program outputs:
1. ASCII art representation in terminal
2. PNG file (120x120 pixels)

Example terminal output:
```
+------------------------+
|▒▒▓▓▓▓  ▒▒████▒▒  ▓▓▓▓▒▒|
|██    ░░▒▒▒▒▒▒▒▒░░    ██|
|░░▒▒      ▓▓▓▓      ▒▒░░|
|░░░░░░░░▒▒░░░░▒▒░░░░░░░░|
|▓▓▒▒▒▒▒▒▒▒    ▒▒▒▒▒▒▒▒▓▓|
|▓▓░░██▓▓  ▒▒▒▒  ▓▓██░░▓▓|
|▓▓  ▒▒██▓▓▓▓▓▓▓▓██▒▒  ▓▓|
|░░▓▓██            ██▓▓░░|
|▓▓  ▓▓  ▓▓▓▓▓▓▓▓  ▓▓  ▓▓|
|████  ▒▒░░████░░▒▒  ████|
|░░▓▓  ▓▓██▓▓▓▓██▓▓  ▓▓░░|
|▓▓██  ▓▓██▓▓▓▓██▓▓  ██▓▓|
+------------------------+
```

## Color Palette

| Level | Color | Description |
|-------|-------|-------------|
| 0 | `#EBEDF0` | Empty (light gray) |
| 1 | `#9BE9A8` | Level 1 (light green) |
| 2 | `#40C463` | Level 2 (green) |
| 3 | `#30A14E` | Level 3 (dark green) |
| 4 | `#216E39` | Level 4 (very dark green) |

## Project Structure

```
hashface/
├── gridmaker.h      # Grid generator header
├── gridmaker.cpp    # Grid generator implementation
├── pngdrawer.h      # PNG writer header
├── pngdrawer.cpp    # PNG writer implementation
├── zapuskator.cpp   # Main program
├── Makefile         # Build configuration
└── README.md        # This file
```

## License

MIT License

## Author

by lakladon