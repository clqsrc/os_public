#ifndef _FILE_SYSTEM_H_
#define _FILE_SYSTEM_H_

#include "os.h"
#include "thread_api.h"
#include "thread_lock.h"
#include "public_function.h"
#include <iostream>
#include <vector>
#include <sys/stat.h>//vc6 下也能用 stat() 函数
#include "direct.h"

/*
[UNIX]有关的系统调用介绍  
lstat系统调用：这是一个非常有用的系统调用，用来获取存储在一个I节点上的信息。
  它接受两个参数：一个字符串指针，指向说明一个文件的路径名；
  另一个是指向stat结构的指针，在这个结构中，存放着有关这个文件的信息。
Stat结构包括以下成员：  
st_mode：这个字段包含文件类型和它所具有的访问权限  
st_ino：这个字段在一个给定的文件系统中唯一的标识了这个文件 
st_dev：这个字段唯一地标识了包括这个文件的文件系统 
st_rdev：如果I节点是一个特殊设备文件，则这个字段标识设备的类型 
st_nlink：文件链接的个数 
st_uid：文件属主的用户ID 
st_gid：文件的用户组的组ID 
st_size：文件的字节数 
st_atime：文件数据最近一次被访问的时间 
st_mtime：文件数据最近一次被更改的时间 
st_ctime：文件状态最近一次被修改的时间
*/

//来自 ethereal filesystem.c
/*
 * Visual C++ on Win32 systems doesn't define these.  (Old UNIX systems don't
 * define them either.)
 *
 * Visual C++ on Win32 systems doesn't define S_IFIFO, it defines _S_IFIFO.
 */
#ifndef S_ISREG
#define S_ISREG(mode)   (((mode) & S_IFMT) == S_IFREG)
#endif
#ifndef S_IFIFO
#define S_IFIFO	_S_IFIFO
#endif
#ifndef S_ISFIFO
#define S_ISFIFO(mode)  (((mode) & S_IFMT) == S_IFIFO)
#endif
#ifndef S_ISDIR
#define S_ISDIR(mode)   (((mode) & S_IFMT) == S_IFDIR)
#endif


/*改名和删除 win32: MoveFile DeleteFile*/

/* 跨平台的文件操作[例如：得到目录列表等] */
/* 由于有可能要用到各个平台下的不同信息,所以保留各个平台下的信息 */

class file_info
{
public:  
	std::string file_name;//文件名
	long file_size;//文件长度
};



class file_system
{
public://变量
	//为了方便用vector保存目录列表
	std::vector<std::string *> file_names;//文件名
	std::vector<time_t *> file_dates;//文件最后的修改时间

protected:
private:
	//清理文件名列表
	void file_system::clear()
	{
		for (int i=0; i<file_names.size(); i++)
		{
			delete file_names[i];
			delete file_dates[i];
		}
		file_names.clear();
		file_dates.clear();
	}
  
public://函数

	file_system::~file_system()
	{
		clear();
	}


	//取得目录列表//dir_name是不带最后的那个[\]或[/]的
	void file_system::get_dir_filenames(std::string dir_name)
	{
		clear();//先清理
		std::string path = dir_name;

	#ifdef WIN32
		//////////////////////////////////////////////////////
		WIN32_FIND_DATA fd;
		std::string * s = new std::string;

		path += "/*.*"; //最后的字符串是类似于[c:/*.*]
		HANDLE hd=::FindFirstFile((LPCTSTR)(path.c_str()), &fd);//开始查找
		if(hd==INVALID_HANDLE_VALUE) return;

		*s = fd.cFileName;
		if ((strcmp(s->c_str(),".")!=0)&&(strcmp(s->c_str(),"..")!=0))
		{
			file_names.push_back( s ); 
		}

		while(FindNextFile(hd,&fd)) //继续查找
		{
			//push_back()是在后面插入一个字符串
			s = new std::string;
			*s = fd.cFileName;
			time_t * t = new time_t;
			*t = get_time_t(fd.ftLastWriteTime);

			//if (((*s)==".")||((*s)=="..") ) continue;
			if ((strcmp(s->c_str(),".")==0)||(strcmp(s->c_str(),"..")==0)) continue;

			file_names.push_back( s ); 
			file_dates.push_back( t ); 
		}

		FindClose(hd);//关闭查找
		//////////////////////////////////////////////////////
	#else
		//////////////////////////////////////////////////////
		DIR * dir; 
		struct dirent * ent; 
		int n = 0;

		dir = opendir(path.c_str()); 
		if (!dir) return; 

		while ((ent = readdir(dir))) 
		{ 
			//push_back()是在后面插入一个字符串
			std::string * s = new std::string;
			*s = strdup(ent->d_name);
			if ((strcmp(s->c_str(),".")==0)||(strcmp(s->c_str(),"..")==0)) continue;
			file_names.push_back( s ); 
		} 

		closedir(dir);
		//////////////////////////////////////////////////////
	#endif

	}


	//某个文件名
	std::string file_system::get_file_name(int index)
	{
		return (* file_names[index]);
	}


	//某个文件长度
	long file_system::get_file_length(int index)
	{
		return ( get_file_length(get_file_name(index).c_str()));
	}

	//某个文件日期
	time_t file_system::get_file_date(int index)
	{
		return (* file_dates[index]);
	}

public://类函数

	//文件是否存在
	//if(-1==GetFileAttributes("\\\\UNC\\computer\\path\\filename")) ...;
	//if(-1==GetFileAttributes("path\\filename")) ...;
	static bool file_exist_old(const char * fn)
	{
		bool r = true;
	#ifdef WIN32
		//如果返回值为 FILE_ATTRIBUTE_DIRECTORY 就是目录
		//if ( -1 = GetFileAttributes(fn) )
		//	r = false;
		//return r;
	#else
	#endif
		FILE * f = fopen(fn, "r");
		if (f == NULL)
			r = false;

		if (f != NULL)
			fclose(f);

		return r;
	}

	static bool file_exist(const char * fn)
	{//来自 ethereal filesystem.c 的 gboolean file_exists(const char *fname)
		//不过根据 linux 编程 书籍所说 stat 的参数未必有用，所以旧的 file_exist 虽然笨拙，但更有效
		//另外 php 里面也有 stat 函数,也可看 php 是如何实现的

		bool r = false;

		struct stat file_stat;

		/*
		* This is a bit tricky on win32. The st_ino field is documented as:
		* "The inode, and therefore st_ino, has no meaning in the FAT, ..."
		* but it *is* set to zero if stat() returns without an error,
		* so this is working, but maybe not quite the way expected. ULFL

		其中
		struct stat { 
		dev_t st_dev; // 设备  
		ino_t st_ino; // 节点  
		mode_t st_mode; // 模式  
		nlink_t st_nlink; // 硬连接
		uid_t st_uid; // 用户ID 
		gid_t st_gid; // 组ID  
		dev_t st_rdev; // 设备类型
		off_t st_off; // 文件字节数 
		unsigned long  st_blksize; // 块大小
		unsigned long st_blocks; // 块数   
		time_t st_atime; // 最后一次访问时间 
		time_t st_mtime; // 最后一次修改时间 
		time_t st_ctime; // 最后一次改变时间(指属性) 
		};

		*/
		file_stat.st_ino = 1;   /* this will make things work if an error occured *///不知道是什么意思
		stat(fn, &file_stat);
		if (file_stat.st_ino == 0) 
		{
			r = true;

			//与 ethereal 不同的是,我们还要判断是否是目录
			//if (((file_stat.st_mode) & S_IFMT) == S_IFDIR)
			if (S_ISDIR(file_stat.st_mode))
			{
				r = false;
			}
		} 
		else 
		{
			r = false;
		}

		return r;
	}

	//创建目录
	//static bool mkdir(const char * dir)
	static bool mkdir(const std::string dir)
	{
		/*
		function CreateDir(const Dir: string): Boolean;
		begin
		{$IFDEF MSWINDOWS}
		  Result := CreateDirectory(PChar(Dir), nil);
		{$ENDIF}
		{$IFDEF LINUX}
		  Result := __mkdir(PChar(Dir), mode_t(-1)) = 0;
		{$ENDIF}
		end;
		*/

		//return ::mkdir(dir.c_str());

		if (::mkdir(dir.c_str()) == 0)
		{
			return true;
		}
		else
		{
			return false;
		}

	}

	//创建目录//mkdir 只能生成单级目录,这个是生成所有需要的
	static bool mkdirs(const std::string dir)
	{
		std::string path = "";//每次要生成目录的文件名
		for (int i = 0; i < dir.size(); i++)
		{
			if ((dir[i] == '\\')||(dir[i] == '/'))
			{
				::mkdir(path.c_str());
				//continue;
			}
			path += dir[i];
		}

		return true;
	}


	int	test_for_directory(const char *path)
	{
		struct stat statb;

		if (stat(path, &statb) < 0)
			return errno;

		if (S_ISDIR(statb.st_mode))
			return EISDIR;
		else
			return 0;
	}

	int	test_for_fifo(const char *path)
	{
		struct stat statb;

		if (stat(path, &statb) < 0)
			return errno;

		if (S_ISFIFO(statb.st_mode))
			return ESPIPE;
		else
			return 0;
	}


	//文件长度
	static long get_file_length(const char * fn)
	{
		if ((fn == NULL)||(strlen(fn) == 0))
		{
			return 0;
		}

		unsigned long len = 0;
		//long len = 0;//奇怪如果用 long 接收返回值,在 release 版本下有问题,有时会返回 -1
		FILE * pFile = NULL;
		pFile = fopen(fn, "rb");

		if (pFile == NULL) return 0;

		len = get_file_length(pFile);

		fclose(pFile);
		return len;
	}


	//文件长度//可以和 lockfile 一起用
	static long get_file_length(FILE * f)
	{//linux 下未测试//这样的实现不太好，最好还是用 ftell
		if (f == NULL) return 0;
		//long len = filelength(fileno(f));
		//unsigned long len = filelength(fileno(f));//vc6 的 release 下用 long 时会返回 -1

		//if (–1L == len) return 0;

	//#ifdef WIN32
		//int no = fileno(pFile);
		//len = filelength(no);
		//奇怪, release 版本下有问题
		//len = filelength(fileno(f));
		//GetFileSize((HANDLE)_get_osfhandle(_fileno(pFile)), &len);
	//#else
		long lFileLen;//文件长度
		long lFilePos;//当前位置
		lFilePos = ftell(f);//得到当前位置
		fseek(f, 0, SEEK_END);
		lFileLen = ftell(f);
		fseek(f, lFilePos, SEEK_SET);//回到原位置
		long len = lFileLen;
	//#endif

		if (0 > len) return 0;

		return len;
	}

	//文件最后的修改时间
	static time_t get_file_time(const char * fn)
	{//linux 下未测试

		time_t r = 0;

		struct stat file_stat;

		file_stat.st_ino = 1;   /* this will make things work if an error occured *///不知道是什么意思
		stat(fn, &file_stat);
		if (file_stat.st_ino == 0) 
		{
			r = file_stat.st_mtime;
		}

		return r;
	}

	//读取全部内容//用在 dbf 文件的锁定读取之类//调用者一定要记得 free() 掉它的返回值，因为它是在函数内分配的内存
	static char * load_file(std::string fn, long * len)
	{
		char * file_buf = NULL;
		(* len) = 0;

		FILE * handle = fopen(fn.c_str(), "rb");
		if (handle == NULL)
		{
			printf("文件[%s]打开失败!\r\n", fn.c_str());
			return file_buf;
		}

		//锁定文件
		file_system::lock_file(handle);

		//文件长度
		long file_len = file_system::get_file_length(handle);

		if (file_len < 1)
		{
			printf("dbf文件[%s]打开失败!未取得文件长度。\r\n", fn.c_str());

			file_system::unlock_file(handle);
			return NULL;
		}

		file_buf = (char *)malloc(file_len);

		if (file_buf == NULL)
		{
			while (true)
				printf("内存不足,dbf::load_from_mem()\r\n");
		}


		int r = fread(file_buf, 1, file_len, handle);

		//解锁文件//好象在 windows 下不用这个也是可以的，文件会在时自动解锁?
		file_system::unlock_file(handle);

		if (r != file_len)
		{
			free(file_buf);
			fclose(handle);
			return NULL;
		}
		fclose(handle);

		(* len) = file_len;
		return file_buf;
	}


	//保存文件全部内容//用在 dbf 文件的锁定读取之类
	static bool save_file(std::string fn, const char * file_buf, const long len)
	{
		FILE * handle = fopen(fn.c_str(), "wb");//这个标志好象文件存在的话不会覆盖
		//FILE * handle = fopen(fn.c_str(), "w+b");
		if (handle == NULL)
		{
			printf("文件[%s]打开失败!\r\n", fn.c_str());
			return false;
		}

		//锁定文件
		file_system::lock_file(handle);

		int r = fwrite(file_buf, 1, len, handle);

		//解锁文件//好象在 windows 下不用这个也是可以的，文件会在时自动解锁?
		file_system::unlock_file(handle);

		if (r != len)
		{
			fclose(handle);
			return false;
		}
		fclose(handle);

		return true;
	}



	static std::string extract_filename(const std::string s)
	{
		std::string fn = "";
		char c;
		for (int i = s.size()-1; i>=0; i--)
		{
			c = s[i];
			if( (c=='/')||(c=='\\') ) break;
			fn = c + fn;
		}
		return fn;
	}

	static std::string extract_file_path(const std::string s)
	{
		std::string fn = "";
		char c;
		for (int i = s.size()-1; i>=0; i--)
		{
			c = s[i];
			if( (c=='/')||(c=='\\') )
			{
				fn = s;
				fn[i] = '\0';

				break;
			}
			//fn = c + fn;
		}
		return fn;
	
	}

	//删除文件
	static bool delete_file(std::string s)
	{
	#ifdef WIN32
		if (DeleteFile(s.c_str()) == TRUE)
			return true;
	#else
	#endif
		return false;

	}

	//复制文件
	static bool copy_file(std::string src, std::string des)
	{
		//最后一个参数表示文件存在就覆盖
		BOOL r = CopyFile(src.c_str(), des.c_str(), false);


		if (r == TRUE)
		{
			return true;
		}
		else 
		{
			return false;
		}

	}

	//查找文件,包括子目录
	#ifdef WIN32
	void FindFileInDir(char* rootDir, char* strRet)
	{
		const int MAC_FILENAMELENOPATH = 4096;
		char fname[MAC_FILENAMELENOPATH];
		ZeroMemory(fname, MAC_FILENAMELENOPATH);

		WIN32_FIND_DATA fd;
		ZeroMemory(&fd, sizeof(WIN32_FIND_DATA));

		HANDLE hSearch;
		char filePathName[256];
		char tmpPath[256];
		ZeroMemory(filePathName, 256);
		ZeroMemory(tmpPath, 256);
		strcpy(filePathName, rootDir);
		bool bSearchFinished = false;
		if( filePathName[strlen(filePathName) -1] != '\\' )
		{
		   strcat(filePathName, "\\");
		}
		strcat(filePathName, "*");
		hSearch = FindFirstFile(filePathName, &fd);
		//Is directory
		if( (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		   && strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, "..") )       
		{
		   strcpy(tmpPath, rootDir);
		   strcat(tmpPath, fd.cFileName);
		   FindFileInDir(tmpPath, strRet);
		}

		else   if( strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, "..") )
				  {
					  sprintf(fname, "%-50.50s", fd.cFileName);
					  strcat(strRet + strRet[strlen(strRet)] , fname);
				  }

		while( !bSearchFinished )
		{
		   if( FindNextFile(hSearch, &fd) )
		   {
			   if( (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				  && strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, "..") )       
			   {
				  strcpy(tmpPath, rootDir);
				  strcat(tmpPath, fd.cFileName);
				  FindFileInDir(tmpPath, strRet);
			   }
			   else   if( strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, "..") )
						 {
							 sprintf(fname, "%-50.50s", fd.cFileName);
							 strcat(strRet + strRet[strlen(strRet)] , fname);
						 }
		   }
		   else
		   {
			   if( GetLastError() == ERROR_NO_MORE_FILES )          //Normal Finished
			   {
				  bSearchFinished = true;
			   }
			   else
				  bSearchFinished = true;     //Terminate Search
		   }
		}
		FindClose(hSearch);
	}
	#else
	#endif

  //取得目录列表//dir_name是不带最后的那个[\]或[/]的//include_sub控制是否含子目录
  void file_system::get_dir_filenames(std::string dir_name, bool include_sub)
  {
	//std::vector<std::string *> file_names;//文件名
    //clear();//先清理//因为有递归,不能用这个,所以要手工清理
    std::string path = dir_name;

    #ifdef WIN32
    //////////////////////////////////////////////////////
    WIN32_FIND_DATA fd;
    std::string * s = new std::string;

    path += "/*.*"; //最后的字符串是类似于[c:/*.*]
    HANDLE hd=::FindFirstFile((LPCTSTR)(path.c_str()), &fd);//开始查找
    if(hd==INVALID_HANDLE_VALUE) return;


    *s = fd.cFileName;
	time_t * t = new time_t;
	*t = get_time_t(fd.ftLastWriteTime);
    if ((strcmp(s->c_str(),".")!=0)&&(strcmp(s->c_str(),"..")!=0))
    {
		*s = dir_name + "\\" + *s;//使用全路径名//因为是全路径所以还是用[\\]这样系统相关的好点
		//如果是目录
		if( (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY )
		{
			//递归调用
			get_dir_filenames((*s) , include_sub);
		}
		else
		{
			file_names.push_back( s ); 
			file_dates.push_back( t ); 
		}
		//如果是目录_end;
    }
    
    while(FindNextFile(hd,&fd)) //继续查找
    {
		//push_back()是在后面插入一个字符串
		s = new std::string;
		*s = fd.cFileName;
		time_t * t = new time_t;
		*t = get_time_t(fd.ftLastWriteTime);

		//if (((*s)==".")||((*s)=="..") ) continue;
		if ((strcmp(s->c_str(),".")==0)||(strcmp(s->c_str(),"..")==0)) continue;

		*s = dir_name + "\\" + *s;//使用全路径名//因为是全路径所以还是用[\\]这样系统相关的好点

		//如果是目录
		if( (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY )
		{
			//递归调用
			get_dir_filenames((*s), include_sub);
		}
		else
		{
			file_names.push_back( s ); 
			file_dates.push_back( t ); 	
		}
		//如果是目录_end;
    }

    FindClose(hd);//关闭查找
    //////////////////////////////////////////////////////
    #else
    //////////////////////////////////////////////////////
    DIR * dir; 
    struct dirent * ent; 
    int n = 0;
    
    dir = opendir(path.c_str()); 
    if (!dir) return; 
    
    while ((ent = readdir(dir))) 
    { 
      //push_back()是在后面插入一个字符串
      std::string * s = new std::string;
      *s = strdup(ent->d_name);
      if ((strcmp(s->c_str(),".")==0)||(strcmp(s->c_str(),"..")==0)) continue;
      file_names.push_back( s ); 
    } 
    
    closedir(dir);
    //////////////////////////////////////////////////////
    #endif
    
  }

	/* 跨平台锁定的 python 源码
   1 import os
   2 
   3 # needs win32all to work on Windows
   4 if os.name == 'nt':
   5     import win32con, win32file, pywintypes
   6     LOCK_EX = win32con.LOCKFILE_EXCLUSIVE_LOCK
   7     LOCK_SH = 0 # the default
   8     LOCK_NB = win32con.LOCKFILE_FAIL_IMMEDIATELY
   9     _ _overlapped = pywintypes.OVERLAPPED(  )
  10 
  11     def lock(file, flags):
  12         hfile = win32file._get_osfhandle(file.fileno(  ))
  13         win32file.LockFileEx(hfile, flags, 0, 0xffff0000, _ _overlapped)
  14 
  15     def unlock(file):
  16         hfile = win32file._get_osfhandle(file.fileno(  ))
  17         win32file.UnlockFileEx(hfile, 0, 0xffff0000, _ _overlapped)
  18 elif os.name == 'posix':
  19     from fcntl import LOCK_EX, LOCK_SH, LOCK_NB
  20 
  21     def lock(file, flags):
  22         fcntl.flock(file.fileno(  ), flags)
  23 
  24     def unlock(file):
  25         fcntl.flock(file.fileno(  ), fcntl.LOCK_UN)
  26 else:
  27     raise RuntimeError("PortaLocker only defined for nt and posix platforms")
  */
	//锁定一个已经打开的文件不让别人读写//阻塞模式
	static bool file_system::lock_file(FILE * f)
	{
		bool r = false;

	#ifdef WIN32

		OVERLAPPED o;
		DWORD flag = LOCKFILE_EXCLUSIVE_LOCK;
		HANDLE h = (HANDLE)_get_osfhandle(fileno(f));

		//注意 win95 下没有这个函数
		//if (0 != LockFileEx(h, flag, 0, 0xffff0000, &o))
		if (0 != LockFileEx(h, flag, 0, 0xffff, 0, &o))
		{
			r = true;
		}

	#endif

		return r;
	}

	//锁定一个已经打开的文件不让别人读写//阻塞模式
	static bool file_system::unlock_file(FILE * f)
	{
		bool r = false;

	#ifdef WIN32

		OVERLAPPED o;
		DWORD flag = LOCKFILE_EXCLUSIVE_LOCK;
		HANDLE h = (HANDLE)_get_osfhandle(fileno(f));

		//注意 win95 下没有这个函数
		if (0 != UnlockFileEx(h, 0, 0xffff, 0, &o))
		{
			r = true;
		}

	#endif

		return r;
	}


};


#endif
