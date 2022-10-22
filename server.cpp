#include <stdexcept>
#include <iostream>
#include <algorithm>
#include "server.h"

namespace server
{

	string BaseServer::generateErr() const
	{
		throw runtime_error("ERROR - "s + typeid(*this).name() + ": code "s + to_string(WSAGetLastError()));
	}

namespace tcp
{

	TCPServer::TCPServer(client_data_handler cdh, client_conn_handler cch, const PCSTR& port)
		: m_wsaData(WSADATA()), data_handler(cdh), conn_handler(cch),
		m_state(false)
	{
		int err = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
		if (err != 0)
		{
			WSACleanup();
			generateErr();
		}

		

		addrinfo *result = nullptr, hints{};
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		err = getaddrinfo(NULL, port, &hints, &result);

		if (err != 0)
		{
			WSACleanup();
			generateErr();
		}

		m_LSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

		if (m_LSocket == INVALID_SOCKET)
		{
			freeaddrinfo(result);
			WSACleanup();
			generateErr();
		}

		err = ::bind(m_LSocket, result->ai_addr, (int)result->ai_addrlen);
		
		if (err != 0)
		{
			closesocket(m_LSocket);
			WSACleanup();
			generateErr();
		}
		
		m_state = true;
		freeaddrinfo(result);

	}

	TCPServer::~TCPServer()
	{
		m_state = false;
		clients.clear();
		closesocket(m_LSocket);
		WSACleanup();
	}

	void TCPServer::handlingAcceptLoop(int maxConn)
	{
		while (m_state)
		{
			if (listen(m_LSocket, maxConn) == SOCKET_ERROR)
			{
				m_state = false;
				closesocket(m_LSocket);
				WSACleanup();
				generateErr();
			}

			SOCKET CSocket = accept(m_LSocket, NULL, NULL);

			if (CSocket == INVALID_SOCKET)
			{
				m_state = false;
				closesocket(m_LSocket);
				WSACleanup();
				generateErr();
			}

			unique_ptr<Client> client (new Client(CSocket));
			conn_handler(*client);
			clients.emplace_back(move(client));

			for (int i = 1; i <= clients.size(); ++i)
			{
				cout << i << " ";
			}
			cout << endl;
			
		}
	}

	void TCPServer::run(int maxConn)
	{
		handlingAcceptLoop(maxConn);
		
	}

}

}