#ifndef _BASE_SOCKET_H_
#define _BASE_SOCKET_H_

#include "os.h"
#include "thread_api.h"
#include "thread_lock.h"
#include "public_function.h"

#include <iostream>

/* socket API�ķ�װ����*/
class base_socket
{
public://����
	SOCKET socket_handle;//socket�ľ��
	SOCKADDR_IN remote_addr;//�Է��ĵ�ַ[accept�еõ�]
	bool is_connect;//��������״̬
	int timeout;//socket��ʱֵ//��
	int idle_timeout;//���г�ʱ//�������һ������//��
	int max_error_count;//�����������
	time_t last_time;//�ϴγɹ���ʱ��,�����жϳ�ʱ//Ŀǰֻ�Ǹ��ⲿ�����ж� have_data_recv() ʱ��

public://������ģʽ����Ҫ�ı���
	bool is_non_mode;//�Ƿ������ģʽ
	int non_send_data_len;//Ҫ���͵ĳ���//ֻ�ǵ�ǰҪ���͵İ�����
	int non_recv_data_len;//Ҫ���յĳ���//ֻ�ǵ�ǰҪ���յİ�����
	int non_send_len;//�Ѿ����͵ĳ���//ֻ�ǵ�ǰҪ���͵İ�����
	int non_recv_len;//�Ѿ����յĳ���//ֻ�ǵ�ǰҪ���յİ�����
	int non_send_session_len;//�Ѿ����͵ĳ���//һ�ζԻ��еģ����ⲿ���������Ҫ�޸ļ�������0��
	int non_recv_session_len;//�Ѿ����յĳ���//һ�ζԻ��еģ����ⲿ���������Ҫ�޸ļ�������0��

	char * non_send_buf;//���ͻ�����
	char * non_recv_buf;//���ջ�����

public://����

	base_socket::base_socket()
	{
		socket_handle = INVALID_SOCKET;
		is_connect = false;
		timeout = 120;//60;//Ĭ�� 2 ���ӳ�ʱ
		idle_timeout = 10 * 60;
		max_error_count = 2;
		last_time = 0;//Ϊ 0 ʱ�ⲿ��Ӧ����Ϊ�Ͽ���

		//������ģʽ����Ҫ�ı���
		is_non_mode = false;//�Ƿ������ģʽ
		non_send_len = 0;//�Ѿ����͵ĳ���//ֻ�ǵ�ǰҪ���͵İ�����
		non_recv_len = 0;//�Ѿ����յĳ���//ֻ�ǵ�ǰҪ���յİ�����
		non_send_data_len = 0;
		non_recv_data_len = 0;
		non_send_session_len = 0;//�Ѿ����͵ĳ���//һ�ζԻ��еģ����ⲿ���������Ҫ�޸ļ�������0��
		non_recv_session_len = 0;//�Ѿ����յĳ���//һ�ζԻ��еģ����ⲿ���������Ҫ�޸ļ�������0��

		non_send_buf = NULL;//���ͻ�����
		non_recv_buf = NULL;//���ջ�����

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

    // ���ü����˿ں�
    if (::bind(socket_handle, (struct sockaddr*)&addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) 
    {
      ::closesocket(socket_handle);
      socket_handle = INVALID_SOCKET;
      return false;
    }

    //����SOMAXCONN����һЩ���۵ģ�Ҳ��Ϊ128����256�ȽϺ�
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

    // ���ü����˿ں�
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
      //shutdown(socket_handle, 0x00);//��ΪWindows NT/2000: Unsupported.
      ::closesocket(socket_handle);
      socket_handle = INVALID_SOCKET;
    }
  }

  base_socket * base_socket::accept()
  {
    base_socket * new_socket = new base_socket;
    int addr_size = sizeof(SOCKADDR_IN);
    
    /* ����������Ҳ����ΪNULL���������һ��������linux��һ��Ҫת��Ϊsocklen_t��
     * ����bnbt�Ĵ�����Ҳ��һ���ģ�����˾������ȫ��ΪNULL�Ͳ���ת��Ҳ�����ɹ���
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

	//������ģʽ��ר�õĶ�ȡ//����Ҫ��ȡ�Ķ���//�Լ������ڴ棬��Ϊ����ģʽ����������һ�㶼����ʱ�Ľṹ��
	//����һ��Ҫע�⽫���յĻ����������ڴ棬����ֱ��ʹ��һ���ṹ����ʱ����
	void base_socket::set_non_recv_len(int len)
	{
		non_recv_len = 0;//�Ѿ����յĳ���//ֻ�ǵ�ǰҪ���յİ�����
		//non_recv_data_len = 0;//Ҫ���յĳ���//ֻ�ǵ�ǰҪ���յİ�����

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

	//������ģʽ��ר�õĶ�ȡ//������Ҫ�Ķ�����ŷ���Ϊ��ֵ
	bool base_socket::non_recv()
	{
		//non_recv_len = 0;//�Ѿ����յĳ���//ֻ�ǵ�ǰҪ���յİ�����


		/*
		// ����������Ҫ��
		struct timeval timeout;
		fd_set fr;
		FD_ZERO(&fr);
		FD_SET(socket_handle, &fr);
		timeout.tv_sec = 0; 
		timeout.tv_usec = 0;
		
		int iRet = select(socket_handle+1, &fr, 0, 0, &timeout);
		if (iRet < 0)	// �ɶ�
		{
			is_connect = false;//�Ͽ���
			printf("select() == -1\r\n");
			
		}
		*/

		if (can_recv_non_block() == false)
		{//���û�����ݿɶ�ȡ
			return false;
		}


		if (non_recv_len == non_recv_data_len)
		{//������
			return true;
		}


		int r = ::recv(socket_handle, non_recv_buf + non_recv_len, non_recv_data_len - non_recv_len, 0);

		if (r > 0)
		{
			non_recv_len += r;
			non_recv_session_len += r;

			if (non_recv_len == non_recv_data_len)
			{//������
				
				//Ҫ�������ݸ����յĻ�����
				//memcpy(buf, non_recv_buf, non_recv_data_len);

				//Ҫ���ý��ճ���//non_get_recv_data() �л�Ҫ�õ�������������������
				//non_recv_len = 0;
				//non_recv_data_len = 0;
				return true;
			}
		}
		else if (r == 0)
		{
			//������ģʽ�²�����Ϊ����ֵΪ 0 ���ж�Ϊ�Ͽ�//ǰ���� can_recv �� select ����ʱ������Ϊ�ǶϿ�������
			is_connect = false;//�Ͽ���
			printf("recv() == 0\r\n");

			//linux ��һ������һ��Ҫ��ֵ
			//int r = ::select(socket_handle+1, NULL, &fd_write, NULL, &l_timeout);

		}

		//if (r <= 0)
		if (r < 0)
		{
			//�������ӱ��Ͽ���ʱ��WSAGetLastError���ؼȲ���WSAECONNRESETҲ����WSAECONNABORT������		
			int eno = WSAGetLastError();

			//if (eno == WSAECONNRESET && eno == WSAECONNABORTED)
			if (eno != WSAEWOULDBLOCK)//�� delphi ScktComp.pas ��Դ��,����ֵΪ 0 ʱ�ǲ������,������ֵΪ -1 ʱֻҪ�ж��Ƿ�Ϊ WSAEWOULDBLOCK �Ϳ�����//����ʱҲ��һ����
			{
				is_connect = false;//�Ͽ���
			}

		}

		return false;
	}


	//������ģʽ��ר�õĶ�ȡ//�õ�Ҫ��ȡ�����ݣ������ɶ�ȡʱ�趨������ֻҪ��������ָ�������
	void base_socket::non_get_recv_data(char * buf)
	{
		//Ҫ�������ݸ����յĻ�����
		memcpy(buf, non_recv_buf, non_recv_data_len);
		non_recv_len = 0;
		non_recv_data_len = 0;
	}



	//������ģʽ�µķ���//����Ҫ���͵�����
	void base_socket::set_non_send_data(const char * buf, int len)
	{
		non_send_len = 0;//�Ѿ����͵ĳ���
		non_send_data_len = 0;//Ҫ���͵ĳ���

		if (non_send_buf == NULL)
		{
			free(non_send_buf);
		}

		non_send_buf = (char *)malloc(len);
		memcpy(non_send_buf, buf, len);

		non_send_data_len = len;
		non_send_len = 0;

	}

	//������ģʽ��ר�õķ���//������Ҫ�Ķ�����ŷ���Ϊ��ֵ
	bool base_socket::non_send()
	{

		int r = ::send(socket_handle, non_send_buf + non_send_len, non_send_data_len - non_send_len, 0);

		if (r > 0)
		{
			printf("base_socket::non_send()\r\n");

			non_send_len += r;
			non_send_session_len += r;

			if (non_send_len == non_send_data_len)
			{//������
				
				//Ҫ�������ݸ����յĻ�����
				//memcpy(buf, non_recv_buf, non_recv_data_len);

				//Ҫ���ý��ճ���
				non_send_len = 0;
				non_send_data_len = 0;
				printf("base_socket::non_send() ok.\r\n");
				return true;
			}
		}

		return false;
	}




  //�õ��Է���IP��ַ
  char * get_remote_addr()
  {
    return inet_ntoa(remote_addr.sin_addr);
  }
  
  //�õ��Է���IP��ַ
  static char * get_remote_addr(SOCKADDR_IN & addr)
  {
    return inet_ntoa(addr.sin_addr);
  }

  //�õ��Է��Ķ˿�
  int get_remote_port()
  {
    return ntohs(remote_addr.sin_port);
  }

  //����һ��0��β���ַ���
  bool base_socket::send_string(const char * buf)
  {
    int len = strlen(buf);

	//return send(buf, len);
	//printfd2("%s\r\n", buf);
	return send_buf(buf, len);
  }

  //����һ�������н�β������//max_lenΪ��ȫ������ܳ��������ֵ//sp Ϊ�н�������
  std::string base_socket::recv_line(std::string sp = "\r\n", int max_len = 4096)
  {
    std::string s1 = "";
    char c1;
    int r1 = 0;
	int r_count1 = 0;//�Ѿ����յĳ���
    int send_count1 = 0;//���̶��ٴ�-1��������//�Ժ��Ϊʱ���׼ȷЩ���ر��Ƿ������������
    
    while (true)
    {
	  if(!can_recv())//��ʱ��
	  {
        is_connect = false;//�Ͽ���
        return "";
	  }

      r1 = recv(&c1 , 1);
      //if ((r1 == 0)||(r1 == -1)) //�ϸ���˵-1������ʾ�Ͽ�,����������socket�ɽ���������Ϊ
	  if (r1 == 0) //�ϸ���˵-1������ʾ�Ͽ�,����������socket�ɽ���������Ϊ
      {
        is_connect = false;//�Ͽ���
        return "";
      }

      if (r1 == -1) //�ϸ���˵-1������ʾ�Ͽ�,����������socket�ɽ���������Ϊ
      {
		send_count1++;
		if (send_count1 >= max_error_count)//̫����˾������
		{
			is_connect = false;//�Ͽ���
			printf("%s\r\n", "recv_line timeout!");
			return "";
		}

		thread_sleep(1);
		continue;
      }

      s1 += c1;
	  r_count1++;
	  send_count1 = 0;//�ָ������������
	  if (r_count1>max_len)//�����������,�����ǹ���
	  {
        is_connect = false;//�Ͽ���
		printfd2("error! too length at base_socket::recv_line:%s", s1.c_str());
        return "";
		
	  }

      //�õ���CRLF
      //if (s1.find("\r\n") != -1) 
      if (s1.find(sp) != -1) 
	  {
		  //s1 = s1.substr(0, s1.length()-2);//ȥ��\r\n
		  s1 = s1.substr(0, s1.length()-sp.size());//ȥ��\r\n
		  break;
	  }
    }
    
	  return s1;
  }

  //����һ�������н�β������//max_lenΪ��ȫ������ܳ��������ֵ
  std::string base_socket::recv_string(std::string end_string, int max_len = 4096)
  {
    std::string s1 = "";
    char c1;
    int r1 = 0;
	int r_count1 = 0;//�Ѿ����յĳ���
    int send_count1 = 0;//���̶��ٴ�-1��������//�Ժ��Ϊʱ���׼ȷЩ���ر��Ƿ������������
    
    while (true)
    {
	  if(!can_recv())//��ʱ��
	  {
        is_connect = false;//�Ͽ���
        return "";
	  }

      r1 = recv(&c1 , 1);
      //if ((r1 == 0)||(r1 == -1)) //�ϸ���˵-1������ʾ�Ͽ�,����������socket�ɽ���������Ϊ
	  if (r1 == 0) //�ϸ���˵-1������ʾ�Ͽ�,����������socket�ɽ���������Ϊ
      {
        is_connect = false;//�Ͽ���
        return "";
      }

      if (r1 == -1) //�ϸ���˵-1������ʾ�Ͽ�,����������socket�ɽ���������Ϊ
      {
		send_count1++;
		if (send_count1 >= max_error_count)//̫����˾������
		{
			is_connect = false;//�Ͽ���
			printf("%s\r\n", "recv_line timeout!");
			return "";
		}

		thread_sleep(1);
		continue;
      }

      s1 += c1;
	  r_count1++;
	  send_count1 = 0;//�ָ������������
	  if (r_count1>max_len)//�����������,�����ǹ���
	  {
        is_connect = false;//�Ͽ���
		printfd2("error! too length at base_socket::recv_line:%s", s1.c_str());
        return "";
		
	  }

      //�õ���CRLF[�����ַ���]
      if (s1.find(end_string) != -1) 
	  {
		  s1 = s1.substr(0, s1.length()-2);//ȥ��\r\n
		  break;
	  }
    }
    
	  return s1;
  }  
  
  //���ӵ�һ����ַ��
  bool base_socket::connect(struct sockaddr_in * addr)
  {
	//socket_handle =  ::socket(AF_INET,SOCK_STREAM,0);

    if(::connect(socket_handle,(struct sockaddr*)addr,sizeof(SOCKADDR_IN)) == 0 )    
    {//�ɹ�
      is_connect = true;
      return true;
    }
    else
    {//ʧ��
#ifdef WIN32
      int r = WSAGetLastError();
      //WSABASEERR//WSAEACCES
      printf("r:%d WSABASEERR:%d\r\n",r,WSABASEERR);
#endif
      is_connect = false;
      return false;
    }    
  }

  //clq �������¼ӵĺ�����Ŀ���ǿ��Ը������������ʣ�����ԭ���Ĵ���ֻ�ܷ��ʾ�����
  //���أ�-1 ����ʧ�ܣ�0 ���ӳɹ�
  bool connect(const char * host, int port)
  {
	const char * address = host;
	
	// Create an address structure and clear it
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	
	// Fill in the address if possible//�ȳ��Ե���IP������
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(address);
	
	// Was the string a valid IP address?//�������IP�͵�������������
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
		//����ʧ��
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
	
	// Was the string a valid IP address?//�������IP�͵�������������
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

  //2008.1.3 add//�������� sock4 ��Ҫ�� ip ��ַ
  static unsigned long make_ip(const char * host)
  {
	const char * address = host;
	
	// Create an address structure and clear it
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	
	// Fill in the address if possible//�ȳ��Ե���IP������
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(address);
	
	// Was the string a valid IP address?//�������IP�͵�������������
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


  //����Ϊ������ģʽ//�ɲο�delphi�Ĵ���
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
		//fcntl(fd_listen, F_SETFL, O_NONBLOCK);//Ҳ�������
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

  //����Ϊ����ģʽ//��֪�Ƿ���Ч
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


  /* �õ�һ��socket�ı�����ַ��ע�⣺��׼������һ��socket���ӶԷ�������ȡ���Լ�
   * �ڶԷ������С��ĵ�ַ��������listen������socket����û������Ӧ�Ĵ��롣
   * ��������VC��CAsyncSocket::GetSockName[�ļ�SOCKCORE.CPP]
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

	//linux ��һ������һ��Ҫ��ֵ//int r = ::select(0, NULL, &fd_write, NULL, &l_timeout);
	int r = ::select(this->socket_handle+1, NULL, &fd_write, NULL, &l_timeout);
	if(r <= 0)//��ʱ��
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

	//linux ��һ������һ��Ҫ��ֵ
	int r = ::select(socket_handle+1, NULL, &fd_write, NULL, &l_timeout);
	if(r < 0)//������
	{
		int r = WSAGetLastError();
		//WSABASEERR//WSAEACCES
		//ʧ��
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

	//linux ��һ������һ��Ҫ��ֵ
	int r = ::select(socket_handle+1, &fd_read, NULL, NULL, &l_timeout);
	if(r <= 0)//��ʱ��
	{
		return false;
	}

	return true;
}

//��������
bool base_socket::can_recv_non_block()
{
	struct timeval l_timeout ;
	fd_set fd_read ; 

	FD_ZERO(&fd_read);
	FD_SET(this->socket_handle, &fd_read);

	l_timeout.tv_sec = 0;
	l_timeout.tv_usec = 0;

	//linux ��һ������һ��Ҫ��ֵ
	int r = ::select(socket_handle+1, &fd_read, NULL, NULL, &l_timeout);
	if(r <= 0)//��ʱ��//û�����ݻ����¼�
	{
		return false;
	}

	return true;
}



//�Ƿ�������Ҫ��ȡ
bool base_socket::have_data_recv()
{
	struct timeval l_timeout ;
	fd_set fd_read ; 

	FD_ZERO(&fd_read);
	FD_SET(this->socket_handle, &fd_read);

	l_timeout.tv_sec = 0;
	l_timeout.tv_usec = 0;

	//�������timeout��ΪNULL���ʾselect����û��timeout������˵��Զ����ʱֱ������
	//��� tv_sec �� tv_usec ���� 0 ����������

	//linux ��һ������һ��Ҫ��ֵ
	int r = ::select(socket_handle+1, &fd_read, NULL, NULL, &l_timeout);
	if(r <= 0)//��ʱ��
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

	//linux ��һ������һ��Ҫ��ֵ
	int r = ::select(socket_handle+1, &fd_read, NULL, NULL, &l_timeout);

	if(r < 0)//������
	{
		int r = WSAGetLastError();
		//WSABASEERR//WSAEACCES
		//ʧ��
		printfd3("select:WSAGetLastError:%d, WSAEFAULT:%d\r\n", WSAGetLastError()-WSABASEERR, WSAEFAULT-WSABASEERR);
	}

	return r;
}



//����һ�����������뵥����send��ͬ��ȫ���ͺ�ŷ��أ����ʧ�ܾ��ǳ�ʱ��--����Ϊ��socket�Ͽ���
bool base_socket::send_buf(const char * p_buf, long p_size)
{
  int needsendbytes;
  int sendbytes = 0;
  const char * currptr = NULL;
  int send_count1 = 0;//���̶��ٴ�-1��������//�Ժ��Ϊʱ���׼ȷЩ���ر��Ƿ������������

  if(!is_connect) 
  {
    return false;
  }
    
  currptr = p_buf;//��ǰд���ַ
  needsendbytes = p_size;
  	  
  while(needsendbytes > 0)
  {

	if(!can_send())//��ʱ��
	{
		is_connect = false;//�Ͽ���
		return false;
	}

	//�����и���ֵ��������currptr��Ҫ���͵�����Խ���ˣ���send�Ľ����һֱ��-1��������ѭ�������������ڴ�����쳣
	sendbytes = ::send(this->socket_handle,currptr,needsendbytes,0);
	//sendbytes = ::send(this->socket_handle, currptr, 256, 0);

	if (sendbytes < 0)//����//С��0�������
	{
		send_count1++;
		if (send_count1 >= max_error_count)//̫����˾������
		{
			printf("%s\r\n", "send_buf timeout!");
			is_connect = false;//�Ͽ���
			return false;
		}

		thread_sleep(1);
		continue;
	}

	if (sendbytes == 0)//����
	{
		is_connect = false;//�Ͽ���
		return false;
	}
	send_count1 = 0;
	needsendbytes -= sendbytes;
	currptr += sendbytes;

  }
  return true;
}

	//����һ�����������뵥����recv��ͬ,��ȫ���պ�ŷ��أ����ʧ�ܾ��ǳ�ʱ��--����Ϊ��socket�Ͽ���//�����Ƿ�����з��ص�ǰ�Ľ��ճ���,ֻ�е����س�����Ҫ����յĳ�����ͬʱ,������Ϊ�ǽ��������κδ���
	int recv_buf(char * p_buf, int p_size)//û�������false
	{

		//�����Լ���0̫Σ����//p_buf[p_size] = '\0';//�ȷ�һ���ڽ�β,��������ʱ����//���Ե������ڷ��仺����ʱ�Ĵ�С����Ӧ���� [ p_size+1 ]!!!
		int len = p_size;//����
		int r_len = 0;//�Ѿ����յĳ���
		int error_count1 = 0;//���̶��ٴ�-1��������//�Ժ��Ϊʱ���׼ȷЩ���ر��Ƿ������������

		if(!is_connect) 
		{
			return r_len;
		}
		if(p_size < 1) 
		{
			return r_len;
		}

		//char buf[409600];
		if ( (len < 1) || (len > 10*1024*1024) )//У�鳤��//���ܴ���10M
		{
			return r_len;
		}
		char * buf = p_buf;//Ҫ���յĻ���
		int r = 0;

		while(true)
		{
			if(!can_recv())//��ʱ��
			{
				return r_len;
			}

			r = ::recv(this->socket_handle, buf + r_len, len - r_len, 0);

			if (r == -1) 
			{
				error_count1++;
				if (error_count1 >= 2)//̫����˾������
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

		//�����Լ���0̫Σ����//buf[r_len] = '\0';//Ҫ�����,Ҫ��ת��Ϊ�ַ���ʱ�����

		return r_len;
	}

public://�ຯ��
  //��ʼ�� socket ����������windows�涨��
  static bool base_socket::init_socket_env()
  {
#ifdef WIN32
    WSADATA WSAData;
    if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
    {
      // socket��ʼ��ʧ��
      return false;
    }
#endif
    return true;
  }

  //���� socket ����
  static void base_socket::exit_socket_env()
  {
#ifdef WIN32
    WSACleanup();
#endif
  }

};


#endif
