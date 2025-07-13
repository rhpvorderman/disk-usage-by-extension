#!/usr/bin/env python3

# Copyright (c) 2025 Leiden University Medical Center
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

"""
Script to scan a directory and report the disk usage by extension.
"""

import argparse
import os
from collections import defaultdict
from typing import Iterator


def iter_files(directory) -> Iterator[os.DirEntry]:
    for entry in os.scandir(directory):  # type: os.DirEntry[str]
        if entry.is_file(follow_symlinks=False):
            yield entry
        elif entry.is_dir(follow_symlinks=False):
            yield from iter_files(entry.path)


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument("directory")
    args = parser.parse_args()
    compressed_exts = (".gz", ".bz2", ".xz")
    size_by_extension = defaultdict(lambda: 0)
    for entry in iter_files(args.directory):
        root, extension = os.path.splitext(entry.name)
        if extension == '':
            extension = "No extension"
        elif extension in compressed_exts:
            _, extension = os.path.splitext(root)
        size = entry.stat().st_size
        size_by_extension[extension] += size

    total_size = sum(size_by_extension.values())
    sorted_by_size = sorted(size_by_extension.items(),
                            key=lambda x: x[1],
                            reverse=True)
    remaining_index = len(sorted_by_size)
    for i, (extension, size) in enumerate(sorted_by_size):
        if (size / total_size) < 0.001:
            remaining_index = i
            break
    sorted_by_size_truncated = sorted_by_size[:remaining_index]
    sorted_by_size_truncated.append(
        ("other", sum(size for ext, size in sorted_by_size[remaining_index:])))
    print(f"Total\t{total_size / (1024 ** 3):.2f} GiB\t100.00%")
    for extension, size in sorted_by_size_truncated:
        print(f"{extension}\t{size / 1024 **3:.2f} GiB\t{size / total_size:.2%}")


if __name__ == "__main__":
    main()
