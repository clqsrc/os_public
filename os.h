#ifndef _OS_H_
#define _OS_H_


  #ifdef WIN32
    //ע��ͷ�ļ���˳�����ң�������� error C2011: 'fd_set' : 'struct' type redefinition ����
    //#include <winsock.h>
    #include <winsock2.h>
    #include <windows.h>
    #include <winbase.h>
    #include <io.h>
    #include <time.h>

    //ȡ��VC��ʹ��STL�ľ���
    #pragma warning(disable : 4786)
    #pragma warning(disable:4503)


    //linux�е�accept��getsockname������Ҫ����socklen_t��ʾ��ַ�ṹ�ĳ���
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
    
    //opendir()����Ҫ��
    #include <dirent.h>
    
    
    ////////////////////////////////////////////////////////////////////////////
    //������socket�Ĳ�ͬ���壬��������VC��ͷ�ļ�
    
    /*
     * This is used instead of -1, since the
     * SOCKET type is unsigned.
     * ���ģ�������������-1�ģ���Ϊ��windows��SOCKET�������޷��ŵ�������
     * ���ͣ���Ϊ�޷��������޷���-1��Ƚϣ�����Ҫ��һ��INVALID_SOCKET����������
     * ��ʵ��Ŀǰ��˵INVALID_SOCKETֻ�����ж�һ��socket�Ƿ���Чʱ��ͬ�����ڸ���
     * �����л��ǿ�����-1���жϲ����Ƿ�ɹ��ġ�����Ϊ�˰�ȫ������SOCKET_ERROR��
     * �жϱȽϺá�
     *
     * �ɲμ�http://www.gbunix.com/htmldata/2005_01/14/20/article_1023_1.html
     *
    **/
    //typedef u_int           SOCKET;//ע��windows��socket���޷��ŵ�
    typedef int             SOCKET;//ע��windows��socket���޷��ŵģ���linux��ȴ���з��ŵ�
    #define INVALID_SOCKET  (SOCKET)(~0)
    #define SOCKET_ERROR            (-1)
    typedef struct sockaddr_in SOCKADDR_IN;
    typedef struct sockaddr SOCKADDR;
    
    //��������Թ�˾��"UnixSock.h"��Ӧ����Ϊ�����첽socket��д��
    #define ioctlsocket(a,b,c) fcntl(a,b,c)
    #define FIONBIO F_SETFL
    #define closesocket( s ) close(s)
	#define WSAGetLastError() errno
	//���첽socket�Ĳ������Ϊ-1ʱ���WSAGetLastError()�Ľ��ΪWSAEWOULDBLOCK�ǲ���������
	//������apache��Դ���ruby�Ĳο��ֲ��пɿ���,��ĳЩunix���ǲ�����EWOULDBLOCK��,�����EAGAIN
	//������ô��,����ʹ��ȫ�ֱ����Ķ����Ǻ�Σ�յ�,Ӧ�������ܵĲ�������
	#define WSAEWOULDBLOCK EWOULDBLOCK
	typedef struct timeval TIMEVAL;
	typedef struct sockaddr_in SOCKADDR_IN;
	typedef struct sockaddr_in *PSOCKADDR_IN;
    
    ////////////////////////////////////////////////////////////////////////////
    
  #endif
    
#endif
