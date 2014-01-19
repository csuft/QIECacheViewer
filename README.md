QIECacheViewer
==============

A cache viewer designed for Internet Explorers. QIECacheViewer is a simple tool ran under windows platform, which I developed with Qt 5.0 application framework. With the comparison of most commercial tools, this program is totally free for all users from all around the world.
Introduction
------------
```Cache``` is a brilliant technology presents in IT world, and it is widely used by hardware engineers and software developers since this trick can highly improve the running performance of hardware and software. So do ```Internet Explorer``` browsers(Abbr. IE).  
On Windows platform, we have two alternatives method to retrive the cache file that ```IE``` save to improve the speed of page rendering.   
Method one:   
> Using ```url cache related API``` provided by Microsoft. Microsoft has provided almost all application interfaces we need to retrieve the cache entries. For instance: 
```c
FindFirstUrlCacheEntry
```
```c
FindNextUrlCacheEntry
```
```c
getNextCacheEntry
```
```c
FindCloseUrlCache
```
With these APIs in hand, all we need to do is call them in sequence and save the URL cache entry in containers.   

Method two:
> Parse the index.dat file manually. Technically speaking, this is hardest method to accomplish the task. To achieve the goal, we have to get a deep understanding of the structure of ```index.dat``` file.      
On ```Windows 2000/XP```, this file was saved in:
```bash
C:/Documents and Settings/Administrator/Local Settings/Temporary Internet Files/Content.IE5/
``` 
On ```Windows vista/7```, things become more complicated. As the stricter access control policy in Windows vista/7, the index.dat file was separated in two different locations:
```bash
C:/Users/[user name]/AppData/Local/Microsoft/Windows/Temporary Internet Files/
```
And:
```bash
C:/Users/[user name]/AppData/Local/Microsoft/Windows/Temporary Internet Files/Low/
```
To access the file, you had better do some extra operations to show all hidden file in the system. Usually, you can change the settings in ```Windows File Browser```.   

The following structures are useful for those who want to parse the index.dat file structure:
```c
typedef struct _FILEMAP_ENTRY
{
	DWORD dwSig;
	DWORD nBlocks;
}FILEMAP_ENTRY, *LPFILEMAP_ENTRY;
```
```c
typedef struct _HASH_FILEMAP_ENTRY : _FILEMAP_ENTRY
{
	DWORD dwNext;  
	DWORD nOrder;  
}HASH_FILEMAP_ENTRY, *LPHASH_FILEMAP_ENTRY;
```
```c
typedef struct _HASH_ITEM
{
	DWORD dwHash;
	DWORD dwOffset;
}HASH_ITEM, *LPHASH_ITEM;
```
```c
typedef struct _IE5_URL_FILEMAP_ENTRY : _FILEMAP_ENTRY
{
	LONGLONG LastModifiedTime;
	LONGLONG LastAccessedTime; 
	DWORD    dostExpireTime;  
	DWORD    dostPostCheckTime;

	DWORD    dwFileSize;              
	DWORD    dwRedirHashItemOffset;   
	DWORD    dwGroupOffset;
	union
	{
		DWORD  dwExemptDelta;         
		DWORD  dwNextLeak;         
	};

	DWORD    CopySize;                
	DWORD    UrlNameOffset;   
	BYTE     DirIndex;  
	BYTE     bSyncState;        
	BYTE     bVerCreate;           
	BYTE     bVerUpdate;            

	DWORD    InternalFileNameOffset; 
	DWORD    CacheEntryType;           
	DWORD    HeaderInfoOffset;        
	DWORD    HeaderInfoSize;          
	DWORD    FileExtensionOffset;    
	DWORD    dostLastSyncTime;      
	DWORD    NumAccessed;             
	DWORD    NumReferences;         
	DWORD    dostFileCreationTime;   
}IE5_URL_FILEMAP_ENTRY, *LPIE5_URL_FILEMAP_ENTRY;
```
More information can be found:[How to implement a IE cache viewer](http://www.cnblogs.com/csuftzzk "How to implement a IE cache viewer")

System Requirements
-------------------
This program is developed under ```Windows XP SP3``` using ```Qt 5.0``` application framework. So, some requirements must be meet to run the program:   
For ```NORMAL USER```:   
* Windows XP or later(Currently only for 32bits)
* ```Windows SDK```. Usually, windows SDK was shipped with Window Operating System.

For ```DEVELOPER```:
* Microsoft Visual Studio 2010 ultimate edition
* Qt 5.0 framework library or higher, earlier versions may also work.


Screen Shot
-----------
Method One:  
![MethodOne.jpg](E:/codes/qt/IECacheViewer/captures/Schirmfoto_2014-01-19_104707.jpg "")
Method Two:
![MethodTwo.jpg](E:/codes/qt/IECacheViewer/captures/Schirmfoto_2014-01-19_010411.jpg "Parse index.dat file")
To Do
-----
* ```Windows 7``` support
* Add context menu for ```QTableView```
* Implement ```refresh``` and ```export``` functionality
* Add item delegates for ```QTableView```
* Add custom selection model for ```QSortFilterProxyModel```
* Delay loading support for ```QTableView```

Copyright
---------
This software is licensed under LGPL, you can use or redistribute it freely.
