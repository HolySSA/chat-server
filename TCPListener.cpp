#include "TCPListener.h"

int TCPListener::init() {
	// winsock 초기화
	WSADATA wsData; // Winsock 데이터 구조체
	WORD ver = MAKEWORD(2, 2); // Winsock 버전 설정

	int wsOK = WSAStartup(ver, &wsData); // Winsock 초기화
	if (wsOK != 0) {
		return wsOK;
	}

	// socket 생성
	m_socket = socket(AF_INET, SOCK_STREAM, 0); // TCP 소켓 <-> UDP: SOCK_DGRAM
	if (m_socket == INVALID_SOCKET) {
		return WSAGetLastError();
	}

	// socket에 ip 주소와 포트 바인딩
	sockaddr_in hint; // 소켓 주소 구조체
	hint.sin_family = AF_INET; // 주소 체계 설정 (IPv4)
	hint.sin_port = htons(m_port); // host to network socket - 포트 번호 설정 (네트워크 바이트 오더로 변환)
	inet_pton(AF_INET, m_ipAddress, &hint.sin_addr);

	if (bind(m_socket, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
		return WSAGetLastError();
	}


	// socket을 Listening 상태로 설정
	if (listen(m_socket, SOMAXCONN) == SOCKET_ERROR) {
		return WSAGetLastError();
	}

	FD_ZERO(&m_master); // 소켓 집합 초기화

	FD_SET(m_socket, &m_master); // Listening 소켓 set

	return 0;
}

int TCPListener::run() {
	// \quit 입력 시 종료
	bool running = true;

	while (running) {
		fd_set copy = m_master;

		// socket에서 이벤트 발생 대기
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		// 현재 연결 - 새로운 연결
		for (int i = 0; i < socketCount; i++) {
			SOCKET sock = copy.fd_array[i]; // 활성화된 socket 가져오기
			if (sock == m_socket) {
				// 새로운 연결 시도
				SOCKET client = accept(m_socket, nullptr, nullptr); // client 연결 수락

				// client 연결 리스트에 새로운 연결 추가
				FD_SET(client, &m_master);

				// 새로운 client 연결
				onClientConnected(client);
			}
			else {
				// 수신 버퍼 선언 및 초기화
				char buf[4096];
				ZeroMemory(buf, 4096);

				// 메시지 받기
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0) {
					onClientDisconnected(sock);
					// 클라이언트 연결 종료 시 처리 (소켓 닫기, 소켓 집합에서 제거)
					closesocket(sock);
					FD_CLR(sock, &m_master);
				}
				else {
					onMessageReceived(sock, buf, bytesIn);

					/*
					if (buf[0] == '\\') {
						string cmd = string(buf, bytesIn);
						// \quit: 서버 종료
						if (cmd == "\\quit") {
							running = false;
							break;
						}

						// quit 이외는 계속
						continue;
					}
					*/

					// 다른 client에게 메세지. listening socket 제외
					//for (int i = 0; i < m_master.fd_count; i++) {
					//	SOCKET outSock = m_master.fd_array[i];
					//	if (outSock != m_socket && outSock != sock) {
					//		/*
					//		ostringstream ss;
					//		ss << "SOCKET #" << sock << ": " << buf << "\r\n";
					//		string strOut = ss.str();

					//		send(outSock, strOut.c_str(), strOut.size() + 1, 0);
					//		*/
					//	}
					//}
				}
			}
		}
	}

	// listening socket 정리 - 다른 연결 방지
	FD_CLR(m_socket, &m_master);
	closesocket(m_socket);

	/*
	// 종료 메세지
	string msg = "Server is shutting down. Goodbye\r\n";
	*/

	while (m_master.fd_count > 0) {
		// socket 번호
		SOCKET sock = m_master.fd_array[0];

		/*
		// 종료 메세지 전송
		send(sock, msg.c_str(), msg.size() + 1, 0);
		*/

		// socket 닫기
		FD_CLR(sock, &m_master);
		closesocket(sock);
	}

	// winsock 정리
	WSACleanup();

	return 0;
}

// client에게 메세지 보내기
void TCPListener::sendToClient(int clientSocket, const char* message, int length) {
	send(clientSocket, message, length, 0);
}

// 모든 client에게 메세지 보내기
void TCPListener::broadcastToClients(int sendingClient, const char* message, int length) {
	// 보내는 client 제외한, 다른 client에게 메세지
	for (int i = 0; i < m_master.fd_count; i++) {
		SOCKET outSock = m_master.fd_array[i];
		if (outSock != m_socket && outSock != sendingClient) {
			sendToClient(outSock, message, length);
		}
	}
}

void TCPListener::onClientConnected(int clientSocket) {

}

void TCPListener::onClientDisconnected(int clientSocket) {

}

void TCPListener::onMessageReceived(int clientSocket, const char* message, int length) {

}

