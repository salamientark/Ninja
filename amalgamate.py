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

        with open(filepath) as f:
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
                        cpp = local.replace('.h', '.cpp')
                        if os.path.exists(cpp):
                            process(cpp)
                        continue
                output.append(line)

    process(main_file)
    return ''.join(output)

print(amalgamate(sys.argv[1]))
