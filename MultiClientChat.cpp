#include "MultiClientChat.h"
#include <string>

void MultiClientChat::onClientConnected(int clientSocket) {
	// 새로운 client 연결 시 메세지
	std::string welcomeMsg = "Welcome to the HOLY Chat Server!\r\n";
	sendToClient(clientSocket, welcomeMsg.c_str(), welcomeMsg.size() + 1);
}

void MultiClientChat::onClientDisconnected(int clientSocket) {

}

void MultiClientChat::onMessageReceived(int clientSocket, const char* message, int length) {
	broadcastToClients(clientSocket, message, length);
}
