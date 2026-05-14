#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <cstdlib>

#include <ws2tcpip.h>

#include <iostream>

#include <Winsock2.h>

#include <conio.h>

#include "Packet.h"

#pragma comment(lib, "ws2_32.lib")

int main()
{
	WSADATA wsaData;

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (result != 0)
	{
		std::cerr << "WSAStartup failed: " << result << std::endl;
		exit(-1);
	}

	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (ServerSocket == INVALID_SOCKET)
	{
		std::cerr << "socket failed: " << WSAGetLastError() << std::endl;
		WSACleanup();
		exit(-1);
	}

	SOCKADDR_IN ServerSockADDR;
	ServerSockADDR.sin_family = AF_INET;
	ServerSockADDR.sin_port = htons(31000);
	ServerSockADDR.sin_addr.s_addr = inet_addr("127.0.0.1");

	result = connect(ServerSocket, (SOCKADDR*)&ServerSockADDR, sizeof(ServerSockADDR));
	if(result == SOCKET_ERROR)
	{
		std::cout << "connect failed: " << WSAGetLastError() << std::endl;
		closesocket(ServerSocket);
		WSACleanup();
		exit(-1);
	}

	while (true)
	{
		char Dir;
		Dir = _getch();

		printf("SendData: %c ReturnData : ", Dir);

		PacketHeader Header;
		Header.Size = sizeof(char);
		Header.Code = (unsigned short)PacketType::Move;


		Header.Size = htons(Header.Size);
		Header.Code = htons(Header.Code);


		int WantedBytes = sizeof(Header);
		int SendBytes = 0;
		int TotalSentBytes = 0;

		do
		{
			SendBytes = send(ServerSocket, (char*)&Header, sizeof(Header), 0);
			if (SendBytes == 0)
			{
				std::cout << "Connection closed by server." << std::endl;
				break;
			}
			else if (SendBytes < 0)
			{
				std::cout << "send failed: " << WSAGetLastError() << std::endl;
				break;
			}
			TotalSentBytes += SendBytes;
		} while (WantedBytes > TotalSentBytes);
		
		/*char Data[1024] = { 0, };
		int Temp = htonl(Dir);
		memcpy(&Data[0], &Temp, sizeof(char));*/

		WantedBytes = sizeof(char);
		SendBytes = 0;
		TotalSentBytes = 0;

		do
		{
			SendBytes = send(ServerSocket, &Dir, WantedBytes - TotalSentBytes, 0);
			if (SendBytes == 0)
			{
				std::cout << "Connection closed by server." << std::endl;
				break;
			}
			else if (SendBytes < 0)
			{
				std::cout << "send failed: " << WSAGetLastError() << std::endl;
				break;
			}
			TotalSentBytes += SendBytes;
		} while (WantedBytes > TotalSentBytes);
		
		PacketHeader ReceivedHeader;
		int WantRecvBytes = sizeof(ReceivedHeader);
		int RecvBytes = 0;
		int TotalRecvBytes = 0;

		do
		{
			RecvBytes = recv(ServerSocket, (char*)&ReceivedHeader + TotalRecvBytes, WantRecvBytes - TotalRecvBytes, 0);
			if (RecvBytes == 0)
			{
				std::cout << "Connection closed by server." << std::endl;
				break;
			}
			else if (RecvBytes < 0)
			{
				std::cout << "recv failed: " << WSAGetLastError() << std::endl;
				break;
			}
			TotalRecvBytes += RecvBytes;
		} while (WantRecvBytes > TotalRecvBytes);

		if(static_cast<PacketType>(ntohs(ReceivedHeader.Code)) == PacketType::Position)
		{
			PositionData PosData;
			WantRecvBytes = ntohs(ReceivedHeader.Size);
			RecvBytes = 0;
			TotalRecvBytes = 0;
			do
			{
				RecvBytes = recv(ServerSocket, (char*)&PosData + TotalRecvBytes, WantRecvBytes - TotalRecvBytes, 0);
				if (RecvBytes == 0)
				{
					std::cout << "Connection closed by server." << std::endl;
					break;
				}
				else if (RecvBytes < 0)
				{
					std::cout << "recv failed: " << WSAGetLastError() << std::endl;
					break;
				}
				TotalRecvBytes += RecvBytes;
			} while (WantRecvBytes > TotalRecvBytes);

			printf("X: %d Y: %d\n", ntohl(PosData.X), ntohl(PosData.Y));
		}

	}

	shutdown(ServerSocket, SD_BOTH);
	closesocket(ServerSocket);
	WSACleanup();
	return 0;
}