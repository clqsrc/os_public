#ifndef _BASE_SOCKET_H_
#define _BASE_SOCKET_H_

#include "os.h"
#include "thread_api.h"
#include "thread_lock.h"
#include "public_function.h"

#include <iostream>

/* socket API的封装函数*/
class base_socket
{
public://变量
	SOCKET socket_handle;//socket的句柄
	SOCKADDR_IN remote_addr;//对方的地址[accept中得到]
	bool is_connect;//处于连接状态
	int timeout;//socket超时值//秒
	int idle_timeout;//空闲超时//这是最后一道防线//秒
	int max_error_count;//连续出错次数
	time_t last_time;//上次成功的时间,用于判断超时//目前只是给外部程序判断 have_data_recv() 时用

public://非阻塞模式下需要的变量
	bool is_non_mode;//是否非阻塞模式
	int non_send_data_len;//要发送的长度//只是当前要发送的包而言
	int non_recv_data_len;//要接收的长度//只是当前要接收的包而言
	int non_send_len;//已经发送的长度//只是当前要发送的包而言
	int non_recv_len;//已经接收的长度//只是当前要接收的包而言
	int non_send_session_len;//已经发送的长度//一次对话中的，由外部程序根据需要修改及重新置0等
	int non_recv_session_len;//已经接收的长度//一次对话中的，由外部程序根据需要修改及重新置0等

	char * non_send_buf;//发送缓冲区
	char * non_recv_buf;//接收缓冲区

public://函数

	base_socket::base_socket()
	{
		socket_handle = INVALID_SOCKET;
		is_connect = false;
		timeout = 120;//60;//默认 2 分钟超时
		idle_timeout = 10 * 60;
		max_error_count = 2;
		last_time = 0;//为 0 时外部不应当认为断开了

		//非阻塞模式下需要的变量
		is_non_mode = false;//是否非阻塞模式
		non_send_len = 0;//已经发送的长度//只是当前要发送的包而言
		non_recv_len = 0;//已经接收的长度//只是当前要接收的包而言
		non_send_data_len = 0;
		non_recv_data_len = 0;
		non_send_session_len = 0;//已经发送的长度//一次对话中的，由外部程序根据需要修改及重新置0等
		non_recv_session_len = 0;//已经接收的长度//一次对话中的，由外部程序根据需要修改及重新置0等

		non_send_buf = NULL;//发送缓冲区
		non_recv_buf = NULL;//接收缓冲区

	}


	base_socket::~base_socket()
	{
		printfd("free: base_socket\r\n");
		close();

		//debug_new_delete(this, 0);

	}

	void base_socket::create_tcp_socket()
	{
		socket_handle = socket(AF_INET,SOCK_STREAM,0);
		//INVALID_SOCKET
	}

  void base_socket::create_udp_socket()
  {
    socket_handle = socket(AF_INET,SOCK_DGRAM,0);
    //INVALID_SOCKET
  }

  bool base_socket::listen(int port)
  {
    SOCKADDR_IN addr;
    memset(&addr, 0 , sizeof(SOCKADDR_IN));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 设置监听端口号
    if (::bind(socket_handle, (struct sockaddr*)&addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) 
    {
      ::closesocket(socket_handle);
      socket_handle = INVALID_SOCKET;
      return false;
    }

    //关于SOMAXCONN是有一些讨论的，也许为128或者256比较好
    if (::listen(socket_handle, SOMAXCONN) == SOCKET_ERROR)
    {
      ::closesocket(socket_handle);
      socket_handle = INVALID_SOCKET;
      return false;
    }
    return true;
  }

  bool base_socket::bind(int port)
  {
    SOCKADDR_IN addr;
    memset(&addr, 0 , sizeof(SOCKADDR_IN));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 设置监听端口号
    if (::bind(socket_handle, (struct sockaddr*)&addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) 
    {
      ::closesocket(socket_handle);
      socket_handle = INVALID_SOCKET;
      return false;
    }

    return true;
  }

  void base_socket::close()
  {
    if (socket_handle != INVALID_SOCKET)
    {
      //shutdown(socket_handle, 0x00);//因为Windows NT/2000: Unsupported.
      ::closesocket(socket_handle);
      socket_handle = INVALID_SOCKET;
    }
  }

  base_socket * base_socket::accept()
  {
    base_socket * new_socket = new base_socket;
    int addr_size = sizeof(SOCKADDR_IN);
    
    /* 后两个参数也可以为NULL，另外最后一个参数在linux下一定要转换为socklen_t，
     * 这在bnbt的代码中也是一样的，而公司代码中全部为NULL就不用转换也会编译成功。
     */
#ifdef WIN32
    new_socket->socket_handle = ::accept(socket_handle, (SOCKADDR *)&new_socket->remote_addr, &addr_size);
#else    
    new_socket->socket_handle = ::accept(socket_handle, (SOCKADDR *)&new_socket->remote_addr, (socklen_t*)&addr_size);
#endif
    new_socket->is_connect = true;

    if (new_socket->socket_handle == INVALID_SOCKET)
    {
      ::closesocket(new_socket->socket_handle);
      delete new_socket;
      new_socket = NULL;
    }

    return new_socket;
  }

	int base_socket::send(const char * buf, int len)
	{
		return ::send(socket_handle, buf, len, 0);
	}

	int base_socket::recv(char * buf, int len)
	{
		return ::recv(socket_handle, buf, len, 0);
	}

	//非阻塞模式下专用的读取//设置要读取的东西//自己分配内存，因为阻塞模式下用来接收一般都是临时的结构，
	//所以一定要注意将接收的缓冲区分配内存，不能直接使用一个结构的临时变量
	void base_socket::set_non_recv_len(int len)
	{
		non_recv_len = 0;//已经接收的长度//只是当前要接收的包而言
		//non_recv_data_len = 0;//要接收的长度//只是当前要接收的包而言

		if (non_recv_buf == NULL)
		{
			free(non_recv_buf);
		}

		non_recv_buf = (char *)malloc(len);
		//memcpy(non_recv_buf, buf, len);

		//non_recv_buf = buf;
		non_recv_data_len = len;
		non_recv_len = 0;

		//return ::recv(socket_handle, buf, len, 0);
	}

	//非阻塞模式下专用的读取//读完所要的东东后才返回为真值
	bool base_socket::non_recv()
	{
		//non_recv_len = 0;//已经接收的长度//只是当前要接收的包而言


		/*
		// 还有数据需要收
		struct timeval timeout;
		fd_set fr;
		FD_ZERO(&fr);
		FD_SET(socket_handle, &fr);
		timeout.tv_sec = 0; 
		timeout.tv_usec = 0;
		
		int iRet = select(socket_handle+1, &fr, 0, 0, &timeout);
		if (iRet < 0)	// 可读
		{
			is_connect = false;//断开了
			printf("select() == -1\r\n");
			
		}
		*/

		if (can_recv_non_block() == false)
		{//如果没有数据可读取
			return false;
		}


		if (non_recv_len == non_recv_data_len)
		{//收完了
			return true;
		}


		int r = ::recv(socket_handle, non_recv_buf + non_recv_len, non_recv_data_len - non_recv_len, 0);

		if (r > 0)
		{
			non_recv_len += r;
			non_recv_session_len += r;

			if (non_recv_len == non_recv_data_len)
			{//收完了
				
				//要复制内容给接收的缓冲区
				//memcpy(buf, non_recv_buf, non_recv_data_len);

				//要重置接收长度//non_get_recv_data() 中还要用到，所不能在这里清零
				//non_recv_len = 0;
				//non_recv_data_len = 0;
				return true;
			}
		}
		else if (r == 0)
		{
			//非阻塞模式下不能因为返回值为 0 就判断为断开//前面有 can_recv 等 select 调用时才能认为是断开连接了
			is_connect = false;//断开了
			printf("recv() == 0\r\n");

			//linux 第一个参数一定要赋值
			//int r = ::select(socket_handle+1, NULL, &fd_write, NULL, &l_timeout);

		}

		//if (r <= 0)
		if (r < 0)
		{
			//到了连接被断开的时候WSAGetLastError返回既不是WSAECONNRESET也不是WSAECONNABORT。。。		
			int eno = WSAGetLastError();

			//if (eno == WSAECONNRESET && eno == WSAECONNABORTED)
			if (eno != WSAEWOULDBLOCK)//按 delphi ScktComp.pas 的源码,返回值为 0 时是不处理的,当返回值为 -1 时只要判断是否为 WSAEWOULDBLOCK 就可以了//发送时也是一样的
			{
				is_connect = false;//断开了
			}

		}

		return false;
	}


	//非阻塞模式下专用的读取//得到要读取的内容，长度由读取时设定，这里只要给出接收指针就行了
	void base_socket::non_get_recv_data(char * buf)
	{
		//要复制内容给接收的缓冲区
		memcpy(buf, non_recv_buf, non_recv_data_len);
		non_recv_len = 0;
		non_recv_data_len = 0;
	}



	//非阻塞模式下的发送//设置要发送的数据
	void base_socket::set_non_send_data(const char * buf, int len)
	{
		non_send_len = 0;//已经发送的长度
		non_send_data_len = 0;//要发送的长度

		if (non_send_buf == NULL)
		{
			free(non_send_buf);
		}

		non_send_buf = (char *)malloc(len);
		memcpy(non_send_buf, buf, len);

		non_send_data_len = len;
		non_send_len = 0;

	}

	//非阻塞模式下专用的发送//发完所要的东东后才返回为真值
	bool base_socket::non_send()
	{

		int r = ::send(socket_handle, non_send_buf + non_send_len, non_send_data_len - non_send_len, 0);

		if (r > 0)
		{
			printf("base_socket::non_send()\r\n");

			non_send_len += r;
			non_send_session_len += r;

			if (non_send_len == non_send_data_len)
			{//发完了
				
				//要复制内容给接收的缓冲区
				//memcpy(buf, non_recv_buf, non_recv_data_len);

				//要重置接收长度
				non_send_len = 0;
				non_send_data_len = 0;
				printf("base_socket::non_send() ok.\r\n");
				return true;
			}
		}

		return false;
	}




  //得到对方的IP地址
  char * get_remote_addr()
  {
    return inet_ntoa(remote_addr.sin_addr);
  }
  
  //得到对方的IP地址
  static char * get_remote_addr(SOCKADDR_IN & addr)
  {
    return inet_ntoa(addr.sin_addr);
  }

  //得到对方的端口
  int get_remote_port()
  {
    return ntohs(remote_addr.sin_port);
  }

  //发送一个0结尾的字符串
  bool base_socket::send_string(const char * buf)
  {
    int len = strlen(buf);

	//return send(buf, len);
	//printfd2("%s\r\n", buf);
	return send_buf(buf, len);
  }

  //接收一个网络行结尾的命令//max_len为安全起见不能超过的最大值//sp 为行结束符号
  std::string base_socket::recv_line(std::string sp = "\r\n", int max_len = 4096)
  {
    std::string s1 = "";
    char c1;
    int r1 = 0;
	int r_count1 = 0;//已经接收的长度
    int send_count1 = 0;//连继多少次-1后就算错误//以后改为时间更准确些，特别是非阻塞的情况下
    
    while (true)
    {
	  if(!can_recv())//超时了
	  {
        is_connect = false;//断开了
        return "";
	  }

      r1 = recv(&c1 , 1);
      //if ((r1 == 0)||(r1 == -1)) //严格来说-1并不表示断开,不过对阻塞socket可近似这样认为
	  if (r1 == 0) //严格来说-1并不表示断开,不过对阻塞socket可近似这样认为
      {
        is_connect = false;//断开了
        return "";
      }

      if (r1 == -1) //严格来说-1并不表示断开,不过对阻塞socket可近似这样认为
      {
		send_count1++;
		if (send_count1 >= max_error_count)//太多次了就算出错
		{
			is_connect = false;//断开了
			printf("%s\r\n", "recv_line timeout!");
			return "";
		}

		thread_sleep(1);
		continue;
      }

      s1 += c1;
	  r_count1++;
	  send_count1 = 0;//恢复连续出错次数
	  if (r_count1>max_len)//超过了最长限制,可能是攻击
	  {
        is_connect = false;//断开了
		printfd2("error! too length at base_socket::recv_line:%s", s1.c_str());
        return "";
		
	  }

      //得到了CRLF
      //if (s1.find("\r\n") != -1) 
      if (s1.find(sp) != -1) 
	  {
		  //s1 = s1.substr(0, s1.length()-2);//去掉\r\n
		  s1 = s1.substr(0, s1.length()-sp.size());//去掉\r\n
		  break;
	  }
    }
    
	  return s1;
  }

  //接收一个网络行结尾的命令//max_len为安全起见不能超过的最大值
  std::string base_socket::recv_string(std::string end_string, int max_len = 4096)
  {
    std::string s1 = "";
    char c1;
    int r1 = 0;
	int r_count1 = 0;//已经接收的长度
    int send_count1 = 0;//连继多少次-1后就算错误//以后改为时间更准确些，特别是非阻塞的情况下
    
    while (true)
    {
	  if(!can_recv())//超时了
	  {
        is_connect = false;//断开了
        return "";
	  }

      r1 = recv(&c1 , 1);
      //if ((r1 == 0)||(r1 == -1)) //严格来说-1并不表示断开,不过对阻塞socket可近似这样认为
	  if (r1 == 0) //严格来说-1并不表示断开,不过对阻塞socket可近似这样认为
      {
        is_connect = false;//断开了
        return "";
      }

      if (r1 == -1) //严格来说-1并不表示断开,不过对阻塞socket可近似这样认为
      {
		send_count1++;
		if (send_count1 >= max_error_count)//太多次了就算出错
		{
			is_connect = false;//断开了
			printf("%s\r\n", "recv_line timeout!");
			return "";
		}

		thread_sleep(1);
		continue;
      }

      s1 += c1;
	  r_count1++;
	  send_count1 = 0;//恢复连续出错次数
	  if (r_count1>max_len)//超过了最长限制,可能是攻击
	  {
        is_connect = false;//断开了
		printfd2("error! too length at base_socket::recv_line:%s", s1.c_str());
        return "";
		
	  }

      //得到了CRLF[结束字符串]
      if (s1.find(end_string) != -1) 
	  {
		  s1 = s1.substr(0, s1.length()-2);//去掉\r\n
		  break;
	  }
    }
    
	  return s1;
  }  
  
  //连接到一个地址上
  bool base_socket::connect(struct sockaddr_in * addr)
  {
	//socket_handle =  ::socket(AF_INET,SOCK_STREAM,0);

    if(::connect(socket_handle,(struct sockaddr*)addr,sizeof(SOCKADDR_IN)) == 0 )    
    {//成功
      is_connect = true;
      return true;
    }
    else
    {//失败
#ifdef WIN32
      int r = WSAGetLastError();
      //WSABASEERR//WSAEACCES
      printf("r:%d WSABASEERR:%d\r\n",r,WSABASEERR);
#endif
      is_connect = false;
      return false;
    }    
  }

  //clq 这是我新加的函数，目的是可以根据域名来访问，并且原来的代码只能访问局域网
  //返回：-1 连接失败；0 连接成功
  bool connect(const char * host, int port)
  {
	const char * address = host;
	
	// Create an address structure and clear it
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	
	// Fill in the address if possible//先尝试当做IP来解析
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(address);
	
	// Was the string a valid IP address?//如果不是IP就当做域名来解析
	if (addr.sin_addr.s_addr == -1)
	{
	  // No, so get the actual IP address of the host name specified
	  struct hostent *pHost;
	  pHost = gethostbyname(address);
	  if (pHost != NULL)
	  {
		  if (pHost->h_addr == NULL)
			  return false;
		  addr.sin_addr.s_addr = ((struct in_addr *)pHost->h_addr)->s_addr;
	  }
	  else
	    return false;
	}

	addr.sin_port = htons(port);

	if (::connect(socket_handle, (struct sockaddr *)&addr, sizeof(addr)) == 0)
	{
		is_connect = true;
	}
	else
	{
		is_connect = false;
		//连接失败
		printfd3("WSAGetLastError:%d, WSAEWOULDBLOCK:%d\r\n", WSAGetLastError()-WSABASEERR, WSAEWOULDBLOCK-WSABASEERR);
	}
	return is_connect;
  }

  static bool make_addr(struct sockaddr_in & addr, const char * host, int port)
  {
	const char * address = host;
	
	memset(&addr, 0, sizeof(addr));
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(address);
	
	// Was the string a valid IP address?//如果不是IP就当做域名来解析
	if (addr.sin_addr.s_addr == -1)
	{
	  // No, so get the actual IP address of the host name specified
	  struct hostent *pHost;
	  pHost = gethostbyname(address);
	  if (pHost != NULL)
	  {
		  if (pHost->h_addr == NULL)
			  return false;
		  addr.sin_addr.s_addr = ((struct in_addr *)pHost->h_addr)->s_addr;
	  }
	  else
	    return false;
	}

	addr.sin_port = htons(port);

	return true;
  }

  //2008.1.3 add//可以生成 sock4 需要的 ip 地址
  static unsigned long make_ip(const char * host)
  {
	const char * address = host;
	
	// Create an address structure and clear it
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	
	// Fill in the address if possible//先尝试当做IP来解析
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(address);
	
	// Was the string a valid IP address?//如果不是IP就当做域名来解析
	//if (addr.sin_addr.s_addr == -1)
	if (addr.sin_addr.s_addr == INADDR_NONE)
	{
	  // No, so get the actual IP address of the host name specified
	  struct hostent *pHost;
	  pHost = gethostbyname(address);
	  if (pHost != NULL)
	  {
		  if (pHost->h_addr == NULL)
			  return INADDR_NONE;
		  addr.sin_addr.s_addr = ((struct in_addr *)pHost->h_addr)->s_addr;
	  }
	  else
	    return INADDR_NONE;
	}

	//addr.sin_port = htons(port);
	
	return addr.sin_addr.s_addr;
	
  }


  //设置为无阻塞模式//可参考delphi的代码
  bool set_non_block()
  {
#ifdef WIN32
		unsigned long argp = 1;
		if (ioctlsocket(socket_handle, FIONBIO, &argp) != 0)
		{
			printf("CBaseSocket::SetNonBlock() error !\r\n");
			return false;
		}
		is_non_mode = true;
		return true;
#else
		//fcntl(fd_listen, F_SETFL, O_NONBLOCK);//也可用这个
		unsigned long argp = 1;
		argp = fcntl(socket_handle, F_SETFL, 0);
		if (fcntl(socket_handle, F_SETFL, argp|O_NONBLOCK) != 0)
		{
			printf("CBaseSocket::SetNonBlock() error !\r\n");
			return false;
		}
		is_non_mode = true;
		return true;
#endif
  }

  //设置为阻塞模式//不知是否有效
  bool set_block()
  {
#ifdef WIN32
		unsigned long argp = 0;
		if (ioctlsocket(socket_handle, FIONBIO, &argp) != 0)
		{
			printf("CBaseSocket::SetNonBlock() error !\r\n");
			return false;
		}
		is_non_mode = false;
		return true;
#else
		unsigned long argp = 0;
		argp = fcntl(socket_handle, F_SETFL, 0);
		if (fcntl(socket_handle, F_SETFL, argp|O_NONBLOCK) != 0)
		{
			printf("CBaseSocket::SetNonBlock() error !\r\n");
			return false;
		}
		is_non_mode = false;
		return true;
#endif
  }


  /* 得到一个socket的本机地址，注意：标准做法是一个socket连接对方后用它取得自己
   * 在对方“眼中”的地址，而用在listen这样的socket上则没见过相应的代码。
   * 代码来自VC的CAsyncSocket::GetSockName[文件SOCKCORE.CPP]
   */
  bool base_socket::get_sock_name(std::string & rSocketAddress, int & rSocketPort)
  {
    SOCKADDR_IN sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    
    int nSockAddrLen = sizeof(sockAddr);
    
    if (!get_sock_name(sockAddr))
    {
      rSocketPort = ntohs(sockAddr.sin_port);
      rSocketAddress = inet_ntoa(sockAddr.sin_addr);
      return true;
    }
    return false;
  } 
  
  bool base_socket::get_sock_name(SOCKADDR_IN & sockAddr)
  {
    memset(&sockAddr, 0, sizeof(sockAddr));
    
    int nSockAddrLen = sizeof(sockAddr);

#ifdef WIN32    
    if (::getsockname(socket_handle, (SOCKADDR*)&sockAddr, (socklen_t*)&nSockAddrLen) != SOCKET_ERROR)
#else    
    if (::getsockname(socket_handle, (SOCKADDR*)&sockAddr, (socklen_t*)&nSockAddrLen) != SOCKET_ERROR)
#endif
    {
      return true;
    }
    return false;
  } 
 
bool base_socket::can_send()
{
	struct timeval l_timeout ;
	fd_set fd_write ; 

	FD_ZERO(&fd_write);
	FD_SET(this->socket_handle, &fd_write);

	l_timeout.tv_sec = timeout;
	l_timeout.tv_usec = 0;

	//linux 第一个参数一定要赋值//int r = ::select(0, NULL, &fd_write, NULL, &l_timeout);
	int r = ::select(this->socket_handle+1, NULL, &fd_write, NULL, &l_timeout);
	if(r <= 0)//超时了
	{
		return false;
	}

	return true;
}

int base_socket::select_send()
{
	struct timeval l_timeout ;
	fd_set fd_write ; 

	FD_ZERO(&fd_write);
	FD_SET(this->socket_handle, &fd_write);

	l_timeout.tv_sec = timeout;
	l_timeout.tv_usec = 0;

	//linux 第一个参数一定要赋值
	int r = ::select(socket_handle+1, NULL, &fd_write, NULL, &l_timeout);
	if(r < 0)//出错了
	{
		int r = WSAGetLastError();
		//WSABASEERR//WSAEACCES
		//失败
		printfd3("select:WSAGetLastError:%d, WSAEFAULT:%d\r\n", WSAGetLastError()-WSABASEERR, WSAEFAULT-WSABASEERR);
	}

	return r;
}

bool base_socket::can_recv()
{
	struct timeval l_timeout ;
	fd_set fd_read ; 

	FD_ZERO(&fd_read);
	FD_SET(this->socket_handle, &fd_read);

	l_timeout.tv_sec = timeout;
	l_timeout.tv_usec = 0;

	//linux 第一个参数一定要赋值
	int r = ::select(socket_handle+1, &fd_read, NULL, NULL, &l_timeout);
	if(r <= 0)//超时了
	{
		return false;
	}

	return true;
}

//非阻塞的
bool base_socket::can_recv_non_block()
{
	struct timeval l_timeout ;
	fd_set fd_read ; 

	FD_ZERO(&fd_read);
	FD_SET(this->socket_handle, &fd_read);

	l_timeout.tv_sec = 0;
	l_timeout.tv_usec = 0;

	//linux 第一个参数一定要赋值
	int r = ::select(socket_handle+1, &fd_read, NULL, NULL, &l_timeout);
	if(r <= 0)//超时了//没有数据或者事件
	{
		return false;
	}

	return true;
}



//是否有数据要读取
bool base_socket::have_data_recv()
{
	struct timeval l_timeout ;
	fd_set fd_read ; 

	FD_ZERO(&fd_read);
	FD_SET(this->socket_handle, &fd_read);

	l_timeout.tv_sec = 0;
	l_timeout.tv_usec = 0;

	//如果参数timeout设为NULL则表示select（）没有timeout。就是说永远不超时直阻塞。
	//如果 tv_sec 和 tv_usec 都是 0 则立即返回

	//linux 第一个参数一定要赋值
	int r = ::select(socket_handle+1, &fd_read, NULL, NULL, &l_timeout);
	if(r <= 0)//超时了
	{
		return false;
	}

	return true;
}



int base_socket::select_read()
{
	struct timeval l_timeout ;
	fd_set fd_read ; 

	FD_ZERO(&fd_read);
	FD_SET(this->socket_handle, &fd_read);

	l_timeout.tv_sec = timeout;
	l_timeout.tv_usec = 0;

	//linux 第一个参数一定要赋值
	int r = ::select(socket_handle+1, &fd_read, NULL, NULL, &l_timeout);

	if(r < 0)//出错了
	{
		int r = WSAGetLastError();
		//WSABASEERR//WSAEACCES
		//失败
		printfd3("select:WSAGetLastError:%d, WSAEFAULT:%d\r\n", WSAGetLastError()-WSABASEERR, WSAEFAULT-WSABASEERR);
	}

	return r;
}



//发送一个缓冲区，与单纯的send不同完全发送后才返回，如果失败就是超时了--可认为是socket断开了
bool base_socket::send_buf(const char * p_buf, long p_size)
{
  int needsendbytes;
  int sendbytes = 0;
  const char * currptr = NULL;
  int send_count1 = 0;//连继多少次-1后就算错误//以后改为时间更准确些，特别是非阻塞的情况下

  if(!is_connect) 
  {
    return false;
  }
    
  currptr = p_buf;//当前写入地址
  needsendbytes = p_size;
  	  
  while(needsendbytes > 0)
  {

	if(!can_send())//超时了
	{
		is_connect = false;//断开了
		return false;
	}

	//这里有个奇怪的现象，如果currptr中要发送的内容越界了，则send的结果会一直是-1，导致死循环，而不产生内存访问异常
	sendbytes = ::send(this->socket_handle,currptr,needsendbytes,0);
	//sendbytes = ::send(this->socket_handle, currptr, 256, 0);

	if (sendbytes < 0)//出错//小于0不算出错
	{
		send_count1++;
		if (send_count1 >= max_error_count)//太多次了就算出错
		{
			printf("%s\r\n", "send_buf timeout!");
			is_connect = false;//断开了
			return false;
		}

		thread_sleep(1);
		continue;
	}

	if (sendbytes == 0)//出错
	{
		is_connect = false;//断开了
		return false;
	}
	send_count1 = 0;
	needsendbytes -= sendbytes;
	currptr += sendbytes;

  }
  return true;
}

	//接收一个缓冲区，与单纯的recv不同,完全接收后才返回，如果失败就是超时了--可认为是socket断开了//不管是否出错都有返回当前的接收长度,只有当返回长度与要求接收的长度相同时,才能认为是接收中无任何错误
	int recv_buf(char * p_buf, int p_size)//没收完就是false
	{

		//不行自己填0太危险了//p_buf[p_size] = '\0';//先放一个在结尾,以免误用时崩溃//所以调用者在分配缓冲区时的大小至少应当是 [ p_size+1 ]!!!
		int len = p_size;//长度
		int r_len = 0;//已经接收的长度
		int error_count1 = 0;//连继多少次-1后就算错误//以后改为时间更准确些，特别是非阻塞的情况下

		if(!is_connect) 
		{
			return r_len;
		}
		if(p_size < 1) 
		{
			return r_len;
		}

		//char buf[409600];
		if ( (len < 1) || (len > 10*1024*1024) )//校验长度//不能大于10M
		{
			return r_len;
		}
		char * buf = p_buf;//要接收的缓冲
		int r = 0;

		while(true)
		{
			if(!can_recv())//超时了
			{
				return r_len;
			}

			r = ::recv(this->socket_handle, buf + r_len, len - r_len, 0);

			if (r == -1) 
			{
				error_count1++;
				if (error_count1 >= 2)//太多次了就算出错
				{
					printf("%s\r\n", "recv_buf timeout!");
					return r_len;
				}

				continue;
			}
			if (r == 0) 
			{
				return r_len;
			}

			r_len += r;

			if (r_len >= len) break;
			thread_sleep(1);

			error_count1 = 0;

		}

		//不行自己填0太危险了//buf[r_len] = '\0';//要有这个,要不转化为字符串时会出错

		return r_len;
	}

public://类函数
  //初始化 socket 环境，这是windows规定的
  static bool base_socket::init_socket_env()
  {
#ifdef WIN32
    WSADATA WSAData;
    if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
    {
      // socket初始化失败
      return false;
    }
#endif
    return true;
  }

  //清理 socket 环境
  static void base_socket::exit_socket_env()
  {
#ifdef WIN32
    WSACleanup();
#endif
  }

};


#endif
