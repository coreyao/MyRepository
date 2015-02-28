#include "LogSystem.h"
#include "Network/tcp/TcpSocket.h"
#include "Network/Packet/Msg.h"

#define GATE_PORT 21000
#define GATE_IP "127.0.0.1"

using Core::Utility::CTcpSocket;
using Core::Utility::SMsg;
using Core::Utility::MsgTest;

int main()
{
	CTcpSocket tcpSocket;
	tcpSocket.Connect(GATE_IP, GATE_PORT);

	MsgTest msg;
	msg.x = 10;

	send(tcpSocket.GetSocket(), (char*)&msg, sizeof(MsgTest), 0);

    system("pause");
    return 0;
}