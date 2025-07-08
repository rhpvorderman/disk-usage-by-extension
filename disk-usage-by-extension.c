// Copyright (c) 2025 Leiden University Medical Center

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to 
// deal in the Software without restriction, including without limitation the 
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#define  _DEFAULT_SOURCE
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int recurse_directory(
    char *path_buffer, 
    size_t path_length, 
    size_t path_buffer_length
) {
     DIR *dir_ptr = opendir(path_buffer);
     if (dir_ptr == NULL) {
        fprintf(stderr, "Failed to open directory with error code: %d\n", errno);
        return -1;
     }
     while (1) {
        struct dirent *entry = readdir(dir_ptr);
        if (entry == NULL) {
            break;
        }
        char *name = entry->d_name;
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
            continue;
        }
        if (entry->d_type == DT_LNK) {
            continue;
        }
        if (entry->d_type == DT_REG) {
            printf("%s/%s\n", path_buffer, entry->d_name);
            continue;
        }
        if (entry->d_type == DT_DIR) {
            size_t name_length = strlen(entry->d_name);
            size_t new_path_length = path_length + name_length + 1; 
            if (new_path_length > path_buffer_length) {
                return -1;
            }
            path_buffer[path_length] = '/';
            memcpy(path_buffer + path_length + 1, entry->d_name, name_length);
            path_buffer[new_path_length] = 0;
            recurse_directory(path_buffer, new_path_length, path_buffer_length);
            // Reset path again
            path_buffer[path_length] = 0;
        } 
    }
}


int main(int argc, char *argv []) {
    if (argc != 2) {
        fprintf(stderr, "Only one argument must be used.");
        return 1;
    }
    char *directory = argv[1];
    char path_buffer[1025];
    memset(path_buffer, 0, 1024);
    strncpy(path_buffer, directory, 1024);
    return recurse_directory(path_buffer, strlen(directory), 1024);
}