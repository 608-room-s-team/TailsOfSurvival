#include <stdexcept>
#include <iostream>
#include "client.h"

namespace server
{
	string BaseClient::generateErr() const
	{
		throw runtime_error("ERROR - "s + typeid(*this).name() + ": code "s + to_string(WSAGetLastError()));
	}

namespace tcp
{

	TCPClient::TCPClient(const PCSTR& ip, const PCSTR& port) : m_wsaData(WSADATA())
	{
		int err = WSAStartup(MAKEWORD(2, 2), &m_wsaData);

		if (err != 0)
		{
			WSACleanup();
			generateErr();
		}

		addrinfo* result = nullptr, hints{};

		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		err = getaddrinfo(ip, port, &hints, &result);

		if (err != 0)
		{
			WSACleanup();
			generateErr();
		}

		m_addrInfoData = *result;

		m_CSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

		if (m_CSocket == INVALID_SOCKET)
		{
			closesocket(m_CSocket);
			WSACleanup();
			generateErr();
		}

		freeaddrinfo(result);

	}

	TCPClient::~TCPClient()
	{
		closesocket(m_CSocket);
		WSACleanup();
	}

	void TCPClient::connectTo()
	{
		if (connect(m_CSocket, m_addrInfoData.ai_addr, m_addrInfoData.ai_addrlen) != 0)
		{
			closesocket(m_CSocket);
			WSACleanup();
			generateErr();
		}
		cout << "Success!" << endl;

	}

}

}