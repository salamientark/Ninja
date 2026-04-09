#!/usr/bin/env python3
import re, sys, os

def amalgamate(main_file):
    seen = set()
    output = []

    def process(filepath):
        if filepath in seen:
            return
        seen.add(filepath)
        directory = os.path.dirname(filepath)

        with open(filepath, encoding='utf-8') as f:
            for line in f:
                # Skip #pragma once and local #include "..."
                if line.strip() == '#pragma once':
                    continue
                m = re.match(r'#include\s+"(.+)"', line.strip())
                if m:
                    local = os.path.join(directory, m.group(1))
                    if os.path.exists(local):
                        process(local)          # inline it
                        # also process the matching .cpp if it exists
                        base, ext = os.path.splitext(local)
                        if ext == '.h':
                            cpp = base + '.cpp'
                            if os.path.exists(cpp):
                                process(cpp)
                        continue
                output.append(line)

    process(main_file)
    return ''.join(output)

if len(sys.argv) < 2:
    print(f"Usage: {sys.argv[0]} <main_file.ino>", file=sys.stderr)
    sys.exit(1)

if not os.path.exists(sys.argv[1]):
    print(f"Error: file not found: {sys.argv[1]}", file=sys.stderr)
    sys.exit(1)

print(amalgamate(sys.argv[1]))
