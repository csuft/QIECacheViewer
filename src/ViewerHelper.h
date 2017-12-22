#ifndef _VIEWERHELPER_H_
#define _VIEWERHELPER_H_

#define WIN32_LEAN_AND_MEAN
#include <string>
#include <vector>
#include <string.h>
#include <Windows.h>
#include <WinInet.h>
#include <strsafe.h>
#include <QSysInfo>

#pragma comment(lib, "strsafe.lib")
#pragma comment(lib, "Wininet.lib")

using std::wstring;
using std::string;
using std::vector;

typedef struct _CacheEntry
{
	string m_fileName;
	string m_urlStr;
	string m_localPath;
	string m_subFolder;
	string m_headerInfo;
	long   m_entrySize;    // the size of the cache entry
	int    m_hits;
	string m_lastAccess;
	string m_lastModified;
	string m_expiration;
}CacheEntry, *pCacheEntry;

class ViewerHelper
{
public:
	explicit ViewerHelper(void);
	~ViewerHelper(void);

	const vector<CacheEntry>& getCacheEntVec() const { return m_recordsVec; }
private:
	ViewerHelper& operator=(const ViewerHelper& obj);
	ViewerHelper(const ViewerHelper& obj);

	HANDLE getStartCacheEntry(LPINTERNET_CACHE_ENTRY_INFO* startEnt);
	bool getNextCacheEntry(HANDLE hDir, LPINTERNET_CACHE_ENTRY_INFO* next);
	void setAllFields(pCacheEntry entry, const LPINTERNET_CACHE_ENTRY_INFO record);
	void transformTimeFormat(pCacheEntry entry, const LPINTERNET_CACHE_ENTRY_INFO lpCacheEnt);
private:
	vector<CacheEntry> m_recordsVec;
};

//////////////////////////////////////////////////////////////////////////
// Head structure
struct CacheDir
{
	DWORD nFileCount;
	char sDirName[8];
};

typedef struct _MEMMAP_HEADER_SMALL
{
	char    FileSignature[28];     // ”Client UrlCache MMF Ver 5.2”
	DWORD    FileSize;             // index.dat文件的大小
	DWORD    dwHashTableOffset;    // 第一个哈希表的偏移,相对于文件的开始地址
	DWORD    NumUrlInternalEntries;// 未知
	DWORD    NumUrlEntriesAlloced; // 未知
	DWORD    dwGarbage;            // 未知
	LONGLONG CacheLimit;           // 8Byte
	LONGLONG CacheSize;            // 8Byte
	LONGLONG ExemptUsage;          // 8Byte
	DWORD    nDirCount;            // 子目录个数
	CacheDir DirArray[32];         // 子目录名称
	DWORD    dwHeaderData[33];     // 未知
}MEMMAP_HEADER_SMALL, *LPMEMMAP_HEADER_SMALL;

typedef struct _MEMMAP_HEADER : _MEMMAP_HEADER_SMALL
{
	DWORD AllocationBitMap[3948];
}MEMMAP_HEADER, *LPMEMMAP_HEADER;

//////////////////////////////////////////////////////////////////////////
// Hash items
typedef struct _FILEMAP_ENTRY
{
	DWORD dwSig;      //条目标识
	DWORD nBlocks;    //条目占用多少个块（每块128字节）
}FILEMAP_ENTRY, *LPFILEMAP_ENTRY;

typedef struct _HASH_FILEMAP_ENTRY : _FILEMAP_ENTRY
{
	DWORD dwNext;     // 下一个哈希表偏移（0表示为最后一个）,偏移以index.dat文件第0字节为基地址。
	DWORD nOrder;     // 本哈希表的序列号。从0，1，2…….
}HASH_FILEMAP_ENTRY, *LPHASH_FILEMAP_ENTRY;

typedef struct _HASH_ITEM
{
	DWORD dwHash;     //哈希值，注意最后6位为0
	DWORD dwOffset;   //指向的实体中的记录部分的偏移, 偏移以index.dat文件第0字节为基地址。
}HASH_ITEM, *LPHASH_ITEM;

// LEAK is the same as URL
typedef struct _IE5_URL_FILEMAP_ENTRY : _FILEMAP_ENTRY
{
	LONGLONG LastModifiedTime;         // 最后修改时间
	LONGLONG LastAccessedTime;         // 最后访问时间
	DWORD    dostExpireTime;           // 到期时间
	DWORD    dostPostCheckTime;

	DWORD    dwFileSize;               // 硬盘缓存中的文件的大小
	DWORD    dwRedirHashItemOffset;    // ask DanpoZ
	DWORD    dwGroupOffset;
	union
	{
		DWORD  dwExemptDelta;          // for SIG_URL
		DWORD  dwNextLeak;             // for SIG_LEAK
	};

	DWORD    CopySize;                 // 好像总是0x60
	DWORD    UrlNameOffset;            // URL名称偏移。基地址是本URL条目的开始地址
	BYTE     DirIndex;                 // 属于的子文件夹索引
	BYTE     bSyncState;               // automatic sync mode state
	BYTE     bVerCreate;               // 建立本ENTRY的CACHE的版本
	BYTE     bVerUpdate;               // 升级本ENTRY的CACHE的版本

	DWORD    InternalFileNameOffset;   // 硬盘上文件名（不包括目录）字符串的偏移，基地址是本URL条目的开始地址。
	DWORD    CacheEntryType;           // 缓存类型
	DWORD    HeaderInfoOffset;         // 从WEB服务器中取本文件时的返回的HTTP头部信息
	DWORD    HeaderInfoSize;           // HTTP头部大小（注意包括最后的回车换行的）
	DWORD    FileExtensionOffset;      // should be WORD
	DWORD    dostLastSyncTime;      
	DWORD    NumAccessed;              // 存取次数（点击率）
	DWORD    NumReferences;            // 引用次数
	DWORD    dostFileCreationTime;   
}IE5_URL_FILEMAP_ENTRY, *LPIE5_URL_FILEMAP_ENTRY;

class IndexParser
{
public:
	explicit IndexParser();
	~IndexParser();
	const vector<CacheEntry>& getCacheEntVec() const { return m_recordsVec;}
private:
	IndexParser& operator=(const IndexParser& obj);
	IndexParser(const IndexParser& obj);
	void initializeEntriesVec();
	const string transformTimeFormat(LONGLONG dtime);
	const string getIndexPath(const char* appendice);
private:
	vector<CacheEntry>	m_recordsVec;
	HANDLE				m_hFile;
	HANDLE				m_hMapping;
	LPSTR				m_startAddr;
};

#endif