File paths:

C File/sdcard

import "C File/FS Project/sdcard"

Allowed chars: a-zA-Z0-9_-

file_new("name");




file_new()


- No Links allowed

fp = file_open("name");
file_read(fp, buf, start, len);
file_write(fp, buf, start, len);

64 bytes per file entry
4 bytes: start block
4 bytes: size in blocks
1 bytes: type (dir 0/file 1)
54 bytes: file name + 1 byte null terminator



Root Block:
- 4 bytes: next free block

Free Block:
- 4 bytes: next free block area
- 4 bytes: size of free area

Dir:
1 block -> 8 file entries

start_block = node_reserve(size)

all files are always continous!




