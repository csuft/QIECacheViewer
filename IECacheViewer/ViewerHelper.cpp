#include "ViewerHelper.h"

ViewerHelper::ViewerHelper(void)
{
	// pointer to memory allocated in somewhere, so we have to free
	// the memory manually at last.
	LPINTERNET_CACHE_ENTRY_INFO lpCacheEntry = NULL;  
	HANDLE hCacheDir;
	CacheEntry tmp;
	// To get the first cache entry

	hCacheDir = getStartCacheEntry(&lpCacheEntry);
	if (hCacheDir)
	{
		setAllFields(&tmp, lpCacheEntry);
		transformTimeFormat(&tmp, lpCacheEntry);
		delete [] lpCacheEntry;
		// push the first cache record
		m_recordsVec.push_back(tmp);  
		while (getNextCacheEntry(hCacheDir, &lpCacheEntry))
		{
			if (lpCacheEntry->lpszLocalFileName == NULL)
			{
				continue;
			}
			memset(&tmp, 0, sizeof(CacheEntry));
			setAllFields(&tmp, lpCacheEntry);
			transformTimeFormat(&tmp, lpCacheEntry);
			delete [] lpCacheEntry;
			m_recordsVec.push_back(tmp);
		}
		// close the file handle.
		FindCloseUrlCache(hCacheDir);  
	}
}

ViewerHelper::~ViewerHelper(void)
{

}

HANDLE ViewerHelper::getStartCacheEntry(LPINTERNET_CACHE_ENTRY_INFO* startEnt)
{
	HANDLE hCacheEnt;
	DWORD dwEntrySize;
	DWORD MAX_CACHE_ENTRY_INFO_SIZE = 4096;
	dwEntrySize = MAX_CACHE_ENTRY_INFO_SIZE;
	*startEnt = (LPINTERNET_CACHE_ENTRY_INFO) new char[dwEntrySize];
	(*startEnt)->dwStructSize = dwEntrySize;
HeadAgain:
	if (!(hCacheEnt = FindFirstUrlCacheEntry(NULL, *startEnt, &dwEntrySize)))
	{
		delete [] *startEnt;
		switch (GetLastError())
		{
		case ERROR_INSUFFICIENT_BUFFER:  
			// To allocate more memory to store the entry information
			*startEnt = (LPINTERNET_CACHE_ENTRY_INFO) new char[dwEntrySize];
			(*startEnt)->dwStructSize = dwEntrySize;
			goto HeadAgain;
			break;
		default:
			return (HANDLE)0;
		}
	}

	return hCacheEnt;

}

bool ViewerHelper::getNextCacheEntry(HANDLE hDir, LPINTERNET_CACHE_ENTRY_INFO* next)
{
	DWORD dwEntrySize;
	DWORD MAX_CACHE_ENTRY_INFO_SIZE = 4096;
	dwEntrySize = MAX_CACHE_ENTRY_INFO_SIZE;

	*next = (LPINTERNET_CACHE_ENTRY_INFO) new char[dwEntrySize];
	(*next)->dwStructSize = dwEntrySize;
HeadAgain:
	if (!FindNextUrlCacheEntry(hDir, *next, &dwEntrySize))
	{
		delete [] *next;
		switch(GetLastError())
		{
		case ERROR_INSUFFICIENT_BUFFER:
			*next = (LPINTERNET_CACHE_ENTRY_INFO)new char[dwEntrySize];
			(*next)->dwStructSize = dwEntrySize;
			goto HeadAgain;
			break;
		default:
			FindCloseUrlCache(hDir);
			return false;
		}
	}
	return true;
}

void ViewerHelper::setAllFields(pCacheEntry entry, const LPINTERNET_CACHE_ENTRY_INFO record)
{
	char *ptr = NULL;
	char chTemp[512] = {'\0'};
	char localPath[256] = {'\0'};

	// some cache entry present without this field. We just ignore it.
	if (record->lpszLocalFileName == NULL || record->lpszSourceUrlName == NULL)  
	{
		return ;
	}
	entry->m_hits = record->dwHitRate;
	wcstombs(chTemp, record->lpszSourceUrlName, 256);
	entry->m_urlStr = chTemp;
	wcstombs(localPath, record->lpszLocalFileName, 256);
	entry->m_localPath = localPath;
	entry->m_entrySize = record->dwSizeLow;
	memset(chTemp, 0, 256);
	if (record->lpHeaderInfo)
	{
		wcstombs(chTemp, record->lpHeaderInfo, 256);
		entry->m_headerInfo = chTemp;
	}
	else
	{
		entry->m_headerInfo = "None exists";
	}

	ptr = strrchr(localPath, '\\');
	// skip to the next character to copy.
	entry->m_fileName = ptr+1; 

	// set the name of sub folder into which we store the cache file.
	// truncate the path
	*ptr = '\0';  
	ptr = strrchr(localPath, '\\');
	entry->m_subFolder = ptr + 1; 
}

void ViewerHelper::transformTimeFormat(pCacheEntry entry, const LPINTERNET_CACHE_ENTRY_INFO lpCacheEnt)
{
	SYSTEMTIME st;
	FILETIME ft;
	const int TIMEBUFFER = 32;
	char chDateTime[TIMEBUFFER];

	// transform the last access time format.
	memset(chDateTime, 0, TIMEBUFFER);
	FileTimeToLocalFileTime(&lpCacheEnt->LastAccessTime, &ft);
	FileTimeToSystemTime(&ft, &st);
	_snprintf(chDateTime, TIMEBUFFER, "%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	entry->m_lastAccess = chDateTime;

	// transform the last modified time format
	memset(chDateTime, 0, TIMEBUFFER);
	if (lpCacheEnt->LastModifiedTime.dwHighDateTime != 0 && lpCacheEnt->LastModifiedTime.dwLowDateTime != 0)
	{
		FileTimeToLocalFileTime(&lpCacheEnt->LastModifiedTime, &ft);
		FileTimeToSystemTime(&ft, &st);
		_snprintf(chDateTime, TIMEBUFFER, "%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	}
	else
	{
		_snprintf(chDateTime, TIMEBUFFER, "N/A");
	}
	entry->m_lastModified = chDateTime;

	// transform the format of expiration time
	memset(chDateTime, 0, TIMEBUFFER);
	if (lpCacheEnt->ExpireTime.dwHighDateTime != 0 && lpCacheEnt->ExpireTime.dwLowDateTime != 0)
	{
		FileTimeToLocalFileTime(&lpCacheEnt->ExpireTime, &ft);
		FileTimeToSystemTime(&ft, &st);
		_snprintf(chDateTime, TIMEBUFFER, "%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	}
	else
	{
		_snprintf(chDateTime, TIMEBUFFER, "N/A");
	}
	entry->m_expiration = chDateTime;
}

IndexParser::IndexParser()
{
	const char* fileName = "C:\\Documents and Settings\\Administrator\\Local Settings\\Temporary Internet Files\\Content.IE5\\index.dat";
	// create a kernel file object
	m_hFile = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_READONLY, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		MessageBoxA(NULL, "Error", "Can't open the index.dat file.", MB_OKCANCEL);
		return;
	}

	// create a kernel file mapping object
	m_hMapping = CreateFileMappingA(m_hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (m_hMapping == NULL)
	{
		MessageBoxA(NULL, "Error", "Can't create file mapping object.", MB_OKCANCEL);
		CloseHandle(m_hFile);
		return ;
	}
	// get the start address of memory file mapping
	m_startAddr = (LPSTR)MapViewOfFile(m_hMapping, FILE_MAP_READ, 0, 0, 0);
	if (m_startAddr == NULL)
	{
		MessageBoxA(NULL, "Error", "Can't mappping the index.dat file.", MB_OKCANCEL);
		CloseHandle(m_hFile);
		CloseHandle(m_hMapping);
		return;
	}
	// get all entries and store into the vector.
	initializeEntriesVec();
}

IndexParser::~IndexParser()
{
	// To free all resources we get when we do file mapping
	CloseHandle(m_startAddr);
	CloseHandle(m_hMapping);
	CloseHandle(m_hFile);
}

void IndexParser::initializeEntriesVec()
{
	const int PATH_LEN = 256;
	const char* prefix = "C:\\Documents and Settings\\Administrator\\Local Settings\\Temporary Internet Files\\Content.IE5\\";
	char path_buf[PATH_LEN] = {'\0'};
	
	// retrieve the index.dat header
	LPMEMMAP_HEADER_SMALL lpsmallHead = (LPMEMMAP_HEADER_SMALL)m_startAddr;

	if (lpsmallHead->dwHashTableOffset == 0) // None hash header exists in this .dat file.
	{
		return ;
	}
	char subdirNames[32][8] = {'\0'};
	int subdirCount = 0;
	// Jump to the beginning of the subdir entries.
	// That is to say we have have to move 76 bytes to skip other fields of small head
	LPSTR subdirBeginAddr = (LPSTR)lpsmallHead + 76; 
	while (subdirCount < lpsmallHead->nDirCount)
	{
		subdirBeginAddr += 4; // skip the items count in the subdir, 4 byte 
		strncpy(subdirNames[subdirCount], subdirBeginAddr, 8);
		subdirBeginAddr += 8; // skip the next subdirectory entry.
		subdirCount++;
	}
	int ordNum;
	const int SIGNATURE_LEN = 5;
	const int NAME_LEN = 9;
	char signature[SIGNATURE_LEN] = {'\0'};
	char namebuf[NAME_LEN] = {'\0'};
	LPHASH_ITEM lphashItem = NULL;
	HASH_ITEM hashItem;
	HASH_FILEMAP_ENTRY hashEntry;
	LPIE5_URL_FILEMAP_ENTRY lpie5Record = NULL;
	IE5_URL_FILEMAP_ENTRY ie5Record;
	CacheEntry record;
	// get the first hash table.
	LPHASH_FILEMAP_ENTRY lphashHeader = (LPHASH_FILEMAP_ENTRY)(m_startAddr + lpsmallHead->dwHashTableOffset);
	while (lphashHeader)
	{
		ordNum++;
		memset(path_buf, 0, PATH_LEN);
		memcpy(&hashEntry, lphashHeader, sizeof(HASH_FILEMAP_ENTRY));
		// iterate through all hash items
		lphashItem = (LPHASH_ITEM)(lphashHeader + 1);
		memcpy(&hashItem, lphashItem, sizeof(HASH_ITEM));
		// Terminated in 3 conditions: 0x0, 0x3, 0xDEADBEEF
		while (ordNum <= (hashEntry.nBlocks*0x80-16)/8 && hashItem.dwOffset != 0xDEADBEEF)
		{
			// skip the holes in hash sections
			if (hashItem.dwHash != 0x0 && hashItem.dwHash != 0x3)
			{
				lpie5Record = (LPIE5_URL_FILEMAP_ENTRY)(m_startAddr + hashItem.dwOffset);
				memcpy(&ie5Record, lpie5Record, sizeof(IE5_URL_FILEMAP_ENTRY));
				memcpy(signature, &ie5Record, 4);
				// We just ignore all entries except that with type of URL
				if (!strcmp(signature,"URL "))
				{
					// fulfill all fields of the cache entry
					record.m_expiration = transformTimeFormat(ie5Record.dostExpireTime);
					record.m_lastAccess = transformTimeFormat(ie5Record.LastAccessedTime);
					record.m_lastModified = transformTimeFormat(ie5Record.LastModifiedTime);
					record.m_hits = ie5Record.NumAccessed;
					record.m_entrySize = ie5Record.dwFileSize;
					record.m_subFolder = string(subdirNames[ie5Record.DirIndex]);
					strcat(path_buf, prefix);
					strncat(path_buf, subdirNames[ie5Record.DirIndex], 8);
					record.m_localPath = string(path_buf);
					record.m_fileName = string((char*)lpie5Record + lpie5Record->InternalFileNameOffset);
					record.m_urlStr = string((char*)lpie5Record + lpie5Record->UrlNameOffset);
					record.m_headerInfo = string((char*)lpie5Record + lpie5Record->HeaderInfoOffset);
					m_recordsVec.push_back(record);
					memset(path_buf, 0, PATH_LEN);
				}
			}
			// skip to the next hash item.
			lphashItem += 1;  
			memcpy(&hashItem, lphashItem, sizeof(HASH_ITEM));
		}
		lphashHeader = hashEntry.dwNext?(LPHASH_FILEMAP_ENTRY)(m_startAddr + hashEntry.dwNext):NULL;
	}
	 
}

const string IndexParser::transformTimeFormat(LONGLONG dtime)
{
	SYSTEMTIME st;
	FILETIME ft;
	FILETIME tmp;
	const int TIMEBUFFER = 32;
	char chDateTime[TIMEBUFFER] = {'\0'};
	// transform the time format to system time format
	memcpy(&tmp, &dtime, sizeof(LONGLONG)); 
	if (tmp.dwHighDateTime == 0 && tmp.dwLowDateTime == 0)
	{
		_snprintf(chDateTime, TIMEBUFFER, "N/A");
	}
	else
	{
		FileTimeToLocalFileTime(&tmp, &ft);
		FileTimeToSystemTime(&ft, &st);
		_snprintf(chDateTime, TIMEBUFFER, "%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	}

	return string(chDateTime);
}