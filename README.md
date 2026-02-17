# Sokkuri
"そっくり" means "indistinguishable".  
Sokkuri will find duplicate files through hashing.

## Current Stage: Working
- Getting file list
- SHA256 library tested
- Added multithreading
- Create a CSV file, out.txt, with path/name/hash
- Create a CSV file, sokkuri.txt, with only duplicate files
- Cleanup ok  
- Split files to 128MiB (see FILE_MAX_DATA_SIZE in file.h)
- Remove jpg/jpeg/png restriction
- Added Linux support

## Hashed files
Previously there was a restriction on only hashing jpg/jpeg/png files.  
Now all files should get hashed.  
  
Big files are split into pieces of 128MiB as to not overload the RAM.
I have the program set to 16 threads (see MAX_THREADS), which would equate to max 2GiB of data in RAM.  

## How to use
Run the program as follows:
> sokkuri.exe "C:\root\folder"

## Dependencies
This program uses the standard C libraries.  
For windows it uses WINAPI calls and for linux it uses linux calls.  
  
PS: I'm no expert when it comes to programming on linux so there's definitely better ways to add linux support.

## Tested on
I've tested this on Windows 10, Windows 11 and Linux Mint 22.2.  

## Credits
<a href="https://twitter.com/obskyr/">**@obskyr**</a> for his help finding a kick-ass repo name.  
dwyw, from discord, for finding the strlen issue on commit 67b433f.  

## License
CC Attribution-ShareAlike 4.0 International, see LICENSE.md
