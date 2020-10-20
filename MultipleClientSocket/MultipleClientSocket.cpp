#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <sstream>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

void main()
{
	while (true)
	{
		//Initialize winsock
		WSADATA wsData;
		WORD ver = MAKEWORD(2, 2);
		int wsOk = WSAStartup(ver, &wsData);

		if (wsOk != 0) {
			cerr << "Can't Initialize winsock!" << endl;
			WSACleanup();
			return;
		}

		//Create a socket
		SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
		if (listening == INVALID_SOCKET) {
			cerr << "Can't create a socket! " << endl;
			WSACleanup();
			return;
		}

		//Bind 
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(9900);
		hint.sin_addr.S_un.S_addr = INADDR_ANY;

		int iResult = bind(listening, (sockaddr*)&hint, sizeof(hint));
		if (iResult == SOCKET_ERROR) {
			cout << "Bind error!" << endl;
			WSACleanup();
			return;
		}

		//Listen
		iResult = listen(listening, SOMAXCONN);
		if (iResult == SOCKET_ERROR) {
			cout << "Bind error!" << endl;
			WSACleanup();
			return;
		}

		//Wait connection
		fd_set master;
		FD_ZERO(&master);
		FD_SET(listening, &master);

		while (true) {
			
			fd_set copy = master;
			int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

			for (int i = 0;i < socketCount;i++) {
				SOCKET socket = copy.fd_array[i];
				
				if (socket == listening) {
					//Accept new connection
					SOCKET client = accept(listening, nullptr, nullptr);
					

					//Add connection to the list
					FD_SET(client, &master);
					string welcomeMessage = "Welcome to the Chat Server! \r\n" ;
					send(client, welcomeMessage.c_str(), welcomeMessage.size() + 1, 0);
					
				}else{
					//Accept a new message
					vector<char> buf(4096);
					int bytesIn = recv(socket, buf.data(), 4096, 0);
					cout << buf.data();
					if (bytesIn <= 0) {
						//Drop the initial client
						closesocket(socket);
						FD_CLR(socket, &master);
					}
					else {
						//Send message to other clients
						for (int i = 0;i < master.fd_count;i++) {
							SOCKET outSocket = master.fd_array[i];
							if (outSocket != listening && outSocket != socket) {
								
								ostringstream ss;
								ss << buf.data() << "\r\n";
								string strOut = ss.str();
								send(outSocket, strOut.c_str(), strOut.size() + 1, 0);
							}
						}


					}

				}
			}
		}

		WSACleanup();
	}
}


// Get connected device id
/*
sockaddr_in clientAddr;
int clientSize = sizeof(clientAddr);
char host[NI_MAXHOST];
char service[NI_MAXHOST];

ZeroMemory(host, NI_MAXHOST);
ZeroMemory(service, NI_MAXHOST);

if (getnameinfo((sockaddr*)&clientAddr, sizeof(clientAddr), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
	cout << host << " connected on port " << service << endl;

}
else {
	inet_ntop(AF_INET, &clientAddr.sin_addr, host, NI_MAXHOST);
	cout << host << " connected on port " << ntohs(clientAddr.sin_port) << endl;
}*/