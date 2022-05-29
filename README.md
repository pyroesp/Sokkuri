# Sokkuri
"そっくり" means "indistinguishable" or "dead ringer".  
Sokkuri will find duplicate files through hashing.

## Current Stage
- Getting file list
- SHA256 library tested
- Create a CSV file, out.txt, with path/name/hash
- Create a CSV file, sokkuri.txt, with only duplicate files
- Cleanup ok  
  
- TODO: Hash using multithreading

## Hashed files
For now I've hardcoded that only jpg/jpeg/png files would be hashed as that was my use case.  
I do intend to change that in the future to allow any file to be hashed.  

## Dependencies
This program uses the WIN API to get a list of files from folders and subfolders.  
I don't have/own mac/linux OS to try and implement it there. Feel free to make a PR for that.  

## Credits
<a href="https://twitter.com/obskyr/">**@obskyr**</a> for his help finding a kick-ass repo name.  
dwyw, from discord, for finding the strlen issue on commit 67b433f.  

## License
CC Attribution-ShareAlike 4.0 International, see LICENSE.md
