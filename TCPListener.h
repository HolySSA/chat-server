#pragma once

#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <sstream>

#pragma comment (lib, "ws2_32.lib")

class TCPListener {
public:
	TCPListener(const char* ipAddress, int port) :
		m_ipAddress(ipAddress), m_port(port) { }

	// listener 초기화
	int init();
	// listener 실행
	int run();

protected:
	// client 연결 Handler
	virtual void onClientConnected(int clientSocket);

	// client 해제 Handler
	virtual void onClientDisconnected(int clientSocket);

	// client 메시지 받기 Handler
	virtual void onMessageReceived(int clientSocket, const char* message, int length);

	// client에게 메세지 보내기
	void sendToClient(int clientSocket, const char* message, int length);

	// 모든 client에게 메세지 보내기
	void broadcastToClients(int sendingClient, const char* message, int length);

private:
	const char* m_ipAddress; // IP 주소 서버
	int			m_port;		 // 포트
	int			m_socket;	 // listening socket 내부 FD
	fd_set		m_master;	 // 소켓 집합 구조체
};