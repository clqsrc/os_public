#ifndef _OS_H_
#define _OS_H_


  #ifdef WIN32
    //注意头文件的顺序不能乱，否则会有 error C2011: 'fd_set' : 'struct' type redefinition 错误
    //#include <winsock.h>
    #include <winsock2.h>
    #include <windows.h>
    #include <winbase.h>
    #include <io.h>
    #include <time.h>

    //取消VC中使用STL的警告
    #pragma warning(disable : 4786)
    #pragma warning(disable:4503)


    //linux中的accept和getsockname函数都要求用socklen_t表示地址结构的长度
    typedef int socklen_t;
  #else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <db.h>
    #include <sys/select.h>
    #include <sys/fcntl.h>
    #include <sys/time.h>
    #include <signal.h>
    #include <unistd.h>
    #include <sys/ipc.h>
    #include <sys/msg.h>
    #include <sys/wait.h>
    #include <dlfcn.h>
    #include <setjmp.h>
    #include <time.h>
    #include <errno.h>
    #include <stdarg.h>
    #include <pthread.h>
    #include <semaphore.h>
    #include <arpa/inet.h>
    #include <sys/sem.h>
    #include <sys/shm.h>
    #include <ctype.h>
    #include <stdio.h>
    #include <netinet/in.h>
    
    //opendir()等需要的
    #include <dirent.h>
    
    
    ////////////////////////////////////////////////////////////////////////////
    //这里是socket的不同定义，基本来自VC的头文件
    
    /*
     * This is used instead of -1, since the
     * SOCKET type is unsigned.
     * 中文：这是用来代替-1的，因为在windows中SOCKET类型是无符号的整数。
     * 解释：因为无符号整数无法与-1相比较，所以要用一个INVALID_SOCKET来代替它。
     * 其实就目前来说INVALID_SOCKET只是在判断一个socket是否有效时不同，而在各个
     * 函数中还是可以用-1来判断操作是否成功的。不过为了安全还是用SOCKET_ERROR来
     * 判断比较好。
     *
     * 可参见http://www.gbunix.com/htmldata/2005_01/14/20/article_1023_1.html
     *
    **/
    //typedef u_int           SOCKET;//注意windows下socket是无符号的
    typedef int             SOCKET;//注意windows下socket是无符号的，但linux下却是有符号的
    #define INVALID_SOCKET  (SOCKET)(~0)
    #define SOCKET_ERROR            (-1)
    typedef struct sockaddr_in SOCKADDR_IN;
    typedef struct sockaddr SOCKADDR;
    
    //下面的来自公司的"UnixSock.h"，应当是为设置异步socket而写的
    #define ioctlsocket(a,b,c) fcntl(a,b,c)
    #define FIONBIO F_SETFL
    #define closesocket( s ) close(s)
	#define WSAGetLastError() errno
	//当异步socket的操作结果为-1时如果WSAGetLastError()的结果为WSAEWOULDBLOCK是不代表出错的
	//不过从apache的源码和ruby的参考手册中可看到,在某些unix下是不存在EWOULDBLOCK的,好象叫EAGAIN
	//不管怎么样,这样使用全局变量的东东是很危险的,应当尽可能的不依赖它
	#define WSAEWOULDBLOCK EWOULDBLOCK
	typedef struct timeval TIMEVAL;
	typedef struct sockaddr_in SOCKADDR_IN;
	typedef struct sockaddr_in *PSOCKADDR_IN;
    
    ////////////////////////////////////////////////////////////////////////////
    
  #endif
    
#endif
