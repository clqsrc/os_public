#ifndef _FILE_SYSTEM_H_
#define _FILE_SYSTEM_H_

#include "os.h"
#include "thread_api.h"
#include "thread_lock.h"
#include "public_function.h"
#include <iostream>
#include <vector>
#include <sys/stat.h>//vc6 ��Ҳ���� stat() ����
#include "direct.h"

/*
[UNIX]�йص�ϵͳ���ý���  
lstatϵͳ���ã�����һ���ǳ����õ�ϵͳ���ã�������ȡ�洢��һ��I�ڵ��ϵ���Ϣ��
  ����������������һ���ַ���ָ�룬ָ��˵��һ���ļ���·������
  ��һ����ָ��stat�ṹ��ָ�룬������ṹ�У�������й�����ļ�����Ϣ��
Stat�ṹ�������³�Ա��  
st_mode������ֶΰ����ļ����ͺ��������еķ���Ȩ��  
st_ino������ֶ���һ���������ļ�ϵͳ��Ψһ�ı�ʶ������ļ� 
st_dev������ֶ�Ψһ�ر�ʶ�˰�������ļ����ļ�ϵͳ 
st_rdev�����I�ڵ���һ�������豸�ļ���������ֶα�ʶ�豸������ 
st_nlink���ļ����ӵĸ��� 
st_uid���ļ��������û�ID 
st_gid���ļ����û������ID 
st_size���ļ����ֽ��� 
st_atime���ļ��������һ�α����ʵ�ʱ�� 
st_mtime���ļ��������һ�α����ĵ�ʱ�� 
st_ctime���ļ�״̬���һ�α��޸ĵ�ʱ��
*/

//���� ethereal filesystem.c
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


/*������ɾ�� win32: MoveFile DeleteFile*/

/* ��ƽ̨���ļ�����[���磺�õ�Ŀ¼�б��] */
/* �����п���Ҫ�õ�����ƽ̨�µĲ�ͬ��Ϣ,���Ա�������ƽ̨�µ���Ϣ */

class file_info
{
public:  
	std::string file_name;//�ļ���
	long file_size;//�ļ�����
};



class file_system
{
public://����
	//Ϊ�˷�����vector����Ŀ¼�б�
	std::vector<std::string *> file_names;//�ļ���
	std::vector<time_t *> file_dates;//�ļ������޸�ʱ��

protected:
private:
	//�����ļ����б�
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
  
public://����

	file_system::~file_system()
	{
		clear();
	}


	//ȡ��Ŀ¼�б�//dir_name�ǲ��������Ǹ�[\]��[/]��
	void file_system::get_dir_filenames(std::string dir_name)
	{
		clear();//������
		std::string path = dir_name;

	#ifdef WIN32
		//////////////////////////////////////////////////////
		WIN32_FIND_DATA fd;
		std::string * s = new std::string;

		path += "/*.*"; //�����ַ�����������[c:/*.*]
		HANDLE hd=::FindFirstFile((LPCTSTR)(path.c_str()), &fd);//��ʼ����
		if(hd==INVALID_HANDLE_VALUE) return;

		*s = fd.cFileName;
		if ((strcmp(s->c_str(),".")!=0)&&(strcmp(s->c_str(),"..")!=0))
		{
			file_names.push_back( s ); 
		}

		while(FindNextFile(hd,&fd)) //��������
		{
			//push_back()���ں������һ���ַ���
			s = new std::string;
			*s = fd.cFileName;
			time_t * t = new time_t;
			*t = get_time_t(fd.ftLastWriteTime);

			//if (((*s)==".")||((*s)=="..") ) continue;
			if ((strcmp(s->c_str(),".")==0)||(strcmp(s->c_str(),"..")==0)) continue;

			file_names.push_back( s ); 
			file_dates.push_back( t ); 
		}

		FindClose(hd);//�رղ���
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
			//push_back()���ں������һ���ַ���
			std::string * s = new std::string;
			*s = strdup(ent->d_name);
			if ((strcmp(s->c_str(),".")==0)||(strcmp(s->c_str(),"..")==0)) continue;
			file_names.push_back( s ); 
		} 

		closedir(dir);
		//////////////////////////////////////////////////////
	#endif

	}


	//ĳ���ļ���
	std::string file_system::get_file_name(int index)
	{
		return (* file_names[index]);
	}


	//ĳ���ļ�����
	long file_system::get_file_length(int index)
	{
		return ( get_file_length(get_file_name(index).c_str()));
	}

	//ĳ���ļ�����
	time_t file_system::get_file_date(int index)
	{
		return (* file_dates[index]);
	}

public://�ຯ��

	//�ļ��Ƿ����
	//if(-1==GetFileAttributes("\\\\UNC\\computer\\path\\filename")) ...;
	//if(-1==GetFileAttributes("path\\filename")) ...;
	static bool file_exist_old(const char * fn)
	{
		bool r = true;
	#ifdef WIN32
		//�������ֵΪ FILE_ATTRIBUTE_DIRECTORY ����Ŀ¼
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
	{//���� ethereal filesystem.c �� gboolean file_exists(const char *fname)
		//�������� linux ��� �鼮��˵ stat �Ĳ���δ�����ã����Ծɵ� file_exist ��Ȼ��׾��������Ч
		//���� php ����Ҳ�� stat ����,Ҳ�ɿ� php �����ʵ�ֵ�

		bool r = false;

		struct stat file_stat;

		/*
		* This is a bit tricky on win32. The st_ino field is documented as:
		* "The inode, and therefore st_ino, has no meaning in the FAT, ..."
		* but it *is* set to zero if stat() returns without an error,
		* so this is working, but maybe not quite the way expected. ULFL

		����
		struct stat { 
		dev_t st_dev; // �豸  
		ino_t st_ino; // �ڵ�  
		mode_t st_mode; // ģʽ  
		nlink_t st_nlink; // Ӳ����
		uid_t st_uid; // �û�ID 
		gid_t st_gid; // ��ID  
		dev_t st_rdev; // �豸����
		off_t st_off; // �ļ��ֽ��� 
		unsigned long  st_blksize; // ���С
		unsigned long st_blocks; // ����   
		time_t st_atime; // ���һ�η���ʱ�� 
		time_t st_mtime; // ���һ���޸�ʱ�� 
		time_t st_ctime; // ���һ�θı�ʱ��(ָ����) 
		};

		*/
		file_stat.st_ino = 1;   /* this will make things work if an error occured *///��֪����ʲô��˼
		stat(fn, &file_stat);
		if (file_stat.st_ino == 0) 
		{
			r = true;

			//�� ethereal ��ͬ����,���ǻ�Ҫ�ж��Ƿ���Ŀ¼
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

	//����Ŀ¼
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

	//����Ŀ¼//mkdir ֻ�����ɵ���Ŀ¼,���������������Ҫ��
	static bool mkdirs(const std::string dir)
	{
		std::string path = "";//ÿ��Ҫ����Ŀ¼���ļ���
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


	//�ļ�����
	static long get_file_length(const char * fn)
	{
		if ((fn == NULL)||(strlen(fn) == 0))
		{
			return 0;
		}

		unsigned long len = 0;
		//long len = 0;//�������� long ���շ���ֵ,�� release �汾��������,��ʱ�᷵�� -1
		FILE * pFile = NULL;
		pFile = fopen(fn, "rb");

		if (pFile == NULL) return 0;

		len = get_file_length(pFile);

		fclose(pFile);
		return len;
	}


	//�ļ�����//���Ժ� lockfile һ����
	static long get_file_length(FILE * f)
	{//linux ��δ����//������ʵ�ֲ�̫�ã���û����� ftell
		if (f == NULL) return 0;
		//long len = filelength(fileno(f));
		//unsigned long len = filelength(fileno(f));//vc6 �� release ���� long ʱ�᷵�� -1

		//if (�C1L == len) return 0;

	//#ifdef WIN32
		//int no = fileno(pFile);
		//len = filelength(no);
		//���, release �汾��������
		//len = filelength(fileno(f));
		//GetFileSize((HANDLE)_get_osfhandle(_fileno(pFile)), &len);
	//#else
		long lFileLen;//�ļ�����
		long lFilePos;//��ǰλ��
		lFilePos = ftell(f);//�õ���ǰλ��
		fseek(f, 0, SEEK_END);
		lFileLen = ftell(f);
		fseek(f, lFilePos, SEEK_SET);//�ص�ԭλ��
		long len = lFileLen;
	//#endif

		if (0 > len) return 0;

		return len;
	}

	//�ļ������޸�ʱ��
	static time_t get_file_time(const char * fn)
	{//linux ��δ����

		time_t r = 0;

		struct stat file_stat;

		file_stat.st_ino = 1;   /* this will make things work if an error occured *///��֪����ʲô��˼
		stat(fn, &file_stat);
		if (file_stat.st_ino == 0) 
		{
			r = file_stat.st_mtime;
		}

		return r;
	}

	//��ȡȫ������//���� dbf �ļ���������ȡ֮��//������һ��Ҫ�ǵ� free() �����ķ���ֵ����Ϊ�����ں����ڷ�����ڴ�
	static char * load_file(std::string fn, long * len)
	{
		char * file_buf = NULL;
		(* len) = 0;

		FILE * handle = fopen(fn.c_str(), "rb");
		if (handle == NULL)
		{
			printf("�ļ�[%s]��ʧ��!\r\n", fn.c_str());
			return file_buf;
		}

		//�����ļ�
		file_system::lock_file(handle);

		//�ļ�����
		long file_len = file_system::get_file_length(handle);

		if (file_len < 1)
		{
			printf("dbf�ļ�[%s]��ʧ��!δȡ���ļ����ȡ�\r\n", fn.c_str());

			file_system::unlock_file(handle);
			return NULL;
		}

		file_buf = (char *)malloc(file_len);

		if (file_buf == NULL)
		{
			while (true)
				printf("�ڴ治��,dbf::load_from_mem()\r\n");
		}


		int r = fread(file_buf, 1, file_len, handle);

		//�����ļ�//������ windows �²������Ҳ�ǿ��Եģ��ļ�����ʱ�Զ�����?
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


	//�����ļ�ȫ������//���� dbf �ļ���������ȡ֮��
	static bool save_file(std::string fn, const char * file_buf, const long len)
	{
		FILE * handle = fopen(fn.c_str(), "wb");//�����־�����ļ����ڵĻ����Ḳ��
		//FILE * handle = fopen(fn.c_str(), "w+b");
		if (handle == NULL)
		{
			printf("�ļ�[%s]��ʧ��!\r\n", fn.c_str());
			return false;
		}

		//�����ļ�
		file_system::lock_file(handle);

		int r = fwrite(file_buf, 1, len, handle);

		//�����ļ�//������ windows �²������Ҳ�ǿ��Եģ��ļ�����ʱ�Զ�����?
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

	//ɾ���ļ�
	static bool delete_file(std::string s)
	{
	#ifdef WIN32
		if (DeleteFile(s.c_str()) == TRUE)
			return true;
	#else
	#endif
		return false;

	}

	//�����ļ�
	static bool copy_file(std::string src, std::string des)
	{
		//���һ��������ʾ�ļ����ھ͸���
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

	//�����ļ�,������Ŀ¼
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

  //ȡ��Ŀ¼�б�//dir_name�ǲ��������Ǹ�[\]��[/]��//include_sub�����Ƿ���Ŀ¼
  void file_system::get_dir_filenames(std::string dir_name, bool include_sub)
  {
	//std::vector<std::string *> file_names;//�ļ���
    //clear();//������//��Ϊ�еݹ�,���������,����Ҫ�ֹ�����
    std::string path = dir_name;

    #ifdef WIN32
    //////////////////////////////////////////////////////
    WIN32_FIND_DATA fd;
    std::string * s = new std::string;

    path += "/*.*"; //�����ַ�����������[c:/*.*]
    HANDLE hd=::FindFirstFile((LPCTSTR)(path.c_str()), &fd);//��ʼ����
    if(hd==INVALID_HANDLE_VALUE) return;


    *s = fd.cFileName;
	time_t * t = new time_t;
	*t = get_time_t(fd.ftLastWriteTime);
    if ((strcmp(s->c_str(),".")!=0)&&(strcmp(s->c_str(),"..")!=0))
    {
		*s = dir_name + "\\" + *s;//ʹ��ȫ·����//��Ϊ��ȫ·�����Ի�����[\\]����ϵͳ��صĺõ�
		//�����Ŀ¼
		if( (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY )
		{
			//�ݹ����
			get_dir_filenames((*s) , include_sub);
		}
		else
		{
			file_names.push_back( s ); 
			file_dates.push_back( t ); 
		}
		//�����Ŀ¼_end;
    }
    
    while(FindNextFile(hd,&fd)) //��������
    {
		//push_back()���ں������һ���ַ���
		s = new std::string;
		*s = fd.cFileName;
		time_t * t = new time_t;
		*t = get_time_t(fd.ftLastWriteTime);

		//if (((*s)==".")||((*s)=="..") ) continue;
		if ((strcmp(s->c_str(),".")==0)||(strcmp(s->c_str(),"..")==0)) continue;

		*s = dir_name + "\\" + *s;//ʹ��ȫ·����//��Ϊ��ȫ·�����Ի�����[\\]����ϵͳ��صĺõ�

		//�����Ŀ¼
		if( (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY )
		{
			//�ݹ����
			get_dir_filenames((*s), include_sub);
		}
		else
		{
			file_names.push_back( s ); 
			file_dates.push_back( t ); 	
		}
		//�����Ŀ¼_end;
    }

    FindClose(hd);//�رղ���
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
      //push_back()���ں������һ���ַ���
      std::string * s = new std::string;
      *s = strdup(ent->d_name);
      if ((strcmp(s->c_str(),".")==0)||(strcmp(s->c_str(),"..")==0)) continue;
      file_names.push_back( s ); 
    } 
    
    closedir(dir);
    //////////////////////////////////////////////////////
    #endif
    
  }

	/* ��ƽ̨������ python Դ��
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
	//����һ���Ѿ��򿪵��ļ����ñ��˶�д//����ģʽ
	static bool file_system::lock_file(FILE * f)
	{
		bool r = false;

	#ifdef WIN32

		OVERLAPPED o;
		DWORD flag = LOCKFILE_EXCLUSIVE_LOCK;
		HANDLE h = (HANDLE)_get_osfhandle(fileno(f));

		//ע�� win95 ��û���������
		//if (0 != LockFileEx(h, flag, 0, 0xffff0000, &o))
		if (0 != LockFileEx(h, flag, 0, 0xffff, 0, &o))
		{
			r = true;
		}

	#endif

		return r;
	}

	//����һ���Ѿ��򿪵��ļ����ñ��˶�д//����ģʽ
	static bool file_system::unlock_file(FILE * f)
	{
		bool r = false;

	#ifdef WIN32

		OVERLAPPED o;
		DWORD flag = LOCKFILE_EXCLUSIVE_LOCK;
		HANDLE h = (HANDLE)_get_osfhandle(fileno(f));

		//ע�� win95 ��û���������
		if (0 != UnlockFileEx(h, 0, 0xffff, 0, &o))
		{
			r = true;
		}

	#endif

		return r;
	}


};


#endif
