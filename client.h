#ifndef CLIENT_H_
#define CLEINT_H_

#include <string>
#include <exception>
#include <typeinfo>

#ifdef _WIN32

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

namespace server
{
using namespace std;

class BaseClient
{
public:
	virtual string generateErr() const;
	virtual void connectTo() = 0;
	virtual ~BaseClient() = 0 {}

};

namespace tcp
{

class TCPClient : public BaseClient
{
public:
	TCPClient(const PCSTR& ip, const PCSTR& port = "9100");
	void connectTo();
	~TCPClient();

private:
	WSADATA m_wsaData;
	SOCKET m_CSocket;
	addrinfo m_addrInfoData;

};

}

}

#endif // _WIN32


#endif // !CLIENT_H_
