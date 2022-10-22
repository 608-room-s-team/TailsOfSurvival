#ifndef TCPCLIENT_H_
#define TCPCLIENT_H_

#include <string>
#include <typeinfo>
#include <memory>
#include <thread>
#include <mutex>
#include <functional>
#include <list>

#ifdef _WIN32

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

namespace server
{
	using namespace std;

	struct DataBuffer
	{
		int size;
		void* data_ptr;
		DataBuffer() noexcept : size(0), data_ptr(nullptr) {}
		DataBuffer(int sz, void* ptr) noexcept : size(sz), data_ptr(ptr) {}
		DataBuffer(const DataBuffer& rhs) : size(rhs.size), data_ptr(::operator new(rhs.size))
		{ memcpy(data_ptr, rhs.data_ptr, rhs.size); }
		DataBuffer(DataBuffer&& tmp) noexcept : size(tmp.size), data_ptr(tmp.data_ptr)
		{ tmp.data_ptr = nullptr; }
		~DataBuffer() { if (data_ptr) ::operator delete(data_ptr, size); }

		bool isEmpty() const { return !size || !data_ptr; }
		
		operator bool() const { return size && data_ptr; }
	};

class BaseServer
{
public:
	virtual string generateErr() const;
	virtual void run(int maxConn) = 0;
	virtual ~BaseServer() = 0 {}
};

namespace tcp
{

	class TCPServer : public BaseServer
	{
	public:
		class Client
		{
		public:
			SOCKET socket;
			bool state;

			Client(SOCKET t_socket) : socket(t_socket), state(true) {}
		};

		typedef function<void(Client&, DataBuffer)> client_data_handler;
		typedef function<void(Client&)> client_conn_handler;

		TCPServer(client_data_handler cdh = [](Client&, DataBuffer){},
			client_conn_handler cch = [](Client&){}, const PCSTR& port = "9100");
		void run(int maxConn = SOMAXCONN);
		const WSADATA& getWsaData() const { return m_wsaData; }
		~TCPServer();
	
	private:
		client_conn_handler conn_handler;
		client_data_handler data_handler;

		thread data_handler_thread;
		thread connection_handler_thread;

		WSADATA m_wsaData;
		SOCKET m_LSocket;
		bool m_state;
		list<unique_ptr<Client>> clients;

		void handlingAcceptLoop(int maxConn);
	};

}

}

#endif // _WIN32

#endif // !TCPCLIENT_H_
