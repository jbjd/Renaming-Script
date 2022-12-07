// gcc renamer.c -municode
#include <stdio.h>  // printf
#include <stdlib.h>  // exit
#include <fileapi.h>
#include <wchar.h>
#include <minwinbase.h> // _WIN32_FIND_DATAW

// program to rename all files in directory
// accepts a path, new name, and optionally a number to start at
// files will be named like: name1, name2, name3...

int wmain(int argc, wchar_t *argv[]){
	if(argc < 3 || argc > 4){
		printf("Renames all files in provided directory like so: name1, name2, name3...\nRun with inputs of path to directory and new name\nAn optional third input is the number to start at\n");
		exit(0);
	}
	// Max length of path in windows is 260 characters
	if(wcslen(argv[1]) > 259){
		printf("Path provided is too long! Must be below 260.");
		exit(1);
	}
	if(wcslen(argv[2]) > 256){
		printf("New name provided is too long! Must be below 256.");
		exit(1);
	}
	
	wchar_t dest[264];
	wcscpy(dest, argv[1]);
	wchar_t* wildcard = L"*.*\0";
	wcsncat(dest, wildcard, 5);

	printf("Checking that '%ls' exists...\n", dest);
	struct _WIN32_FIND_DATAW dirData;
	HANDLE fileHandle = FindFirstFileW(dest, &dirData);
	// if handle is equivalent to -1, doesn't exist
	if(fileHandle == ((HANDLE)(long long)-1)){
		printf("%ls doesn't exist, exiting script.", argv[1]);
		exit(1);
	}
 	printf("Renaming files...\n");
	// we know path exists so start calling next file to get all files in dir

	unsigned shouldRename;
	const wchar_t* newName = argv[2];
	unsigned count = 1, i;
	if(argc > 3){
		// not yet implemented, set count to argv[3]
	}

	wcscpy(dest, argv[1]);
	const unsigned long long nameLen = wcslen(newName);
	const unsigned long long pathLen = wcslen(dest)+nameLen;
	wchar_t ext[256];
	const wchar_t dot = L'.';
	unsigned long long extLen;
	unsigned long long countLen;
	unsigned long long totalLen;
	wchar_t oldPath[261];
	int ret;

	while(FindNextFileW(fileHandle, &dirData)){
		// the docs use & not == since multiple attributes may be present, but in this case we don't want files with anything else set besides one of these
		// it would imply they are hidden/a directory/used by kernel/etc otherwise
		if(dirData.dwFileAttributes == FILE_ATTRIBUTE_NORMAL || dirData.dwFileAttributes == FILE_ATTRIBUTE_ARCHIVE){
			wchar_t newNameBuf[261];
			wchar_t numBuf[256];
			wchar_t* dotPosition = wcsrchr(dirData.cFileName, dot);
			extLen = wcslen(dotPosition);
			swprintf(numBuf, 256, L"%d", count);
			countLen = wcslen(numBuf);
			totalLen = countLen+extLen+pathLen;
			if(totalLen > 260){
				printf("Skippipng %ls due to full path exceeding 260 characters\n", dirData.cFileName);
			}else{
				// copy old
				wcscpy(oldPath, dest);
				wcsncat(oldPath, dirData.cFileName, wcslen(dirData.cFileName));
				// copy new
				wcscpy(newNameBuf, dest);
				wcsncat(newNameBuf, newName, nameLen);
				wcsncat(newNameBuf, numBuf, countLen);
				wcsncat(newNameBuf, dotPosition, extLen);
				ret = _wrename(oldPath, newNameBuf);
				if(ret != 0){
					printf("Failed to rename file %ls\n", oldPath);
				}else{
					printf("Renamed %ls to %ls\n", oldPath, newNameBuf);
					++count;
				}
			}			
		}
	}
}