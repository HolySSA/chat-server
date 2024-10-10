#pragma once

#include "TCPListener.h"

class MultiClientChat : public TCPListener {
public:
	MultiClientChat(const char* ipAddress, int port) :
		TCPListener(ipAddress, port) { }

protected:
	// client 연결 Handler
	virtual void onClientConnected(int clientSocket);

	// client 해제 Handler
	virtual void onClientDisconnected(int clientSocket);

	// client 메시지 받기 Handler
	virtual void onMessageReceived(int clientSocket, const char* message, int length);
};