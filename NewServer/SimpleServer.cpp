#include <iostream>
#include <olc_net.h>
#include <fstream>
enum class CustomMsgTypes : uint32_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage,
};

std::fstream fin;

class CustomServer : public olc::net::server_interface<CustomMsgTypes>
{
public:
	CustomServer(uint16_t nPort) : olc::net::server_interface<CustomMsgTypes>(nPort)
	{

	}

protected:
	virtual bool OnClientConnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client)
	{
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerAccept;
		client->Send(msg);
		return true;
	}

	// Called when a client appears to have disconnected
	virtual void OnClientDisconnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client)
	{
		std::cout << "Removing client [" << client->GetID() << "]\n";
	}

	// Called when a message arrives
	virtual void OnMessage(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client, olc::net::message<CustomMsgTypes>& msg)
	{
		switch (msg.header.id)
		{
		case CustomMsgTypes::ServerPing:
		{
			std::cout << "[" << client->GetID() << "]: Server Ping\n";

			// Simply bounce message back to client
			client->Send(msg);
		}
		break;

		case CustomMsgTypes::MessageAll:
		{
			fin.open("C:/Users/caxax/OneDrive/Рабочий стол/c++/SERVER_CLIENT_ASIO_MSGHISTORY/MsgHistory.txt", std::fstream::in | std::fstream::app);
			std::cout << "[" << client->GetID() << "]: Message All\n";
			olc::net::message<CustomMsgTypes> msg2;
			msg2.header.id = CustomMsgTypes::ServerMessage;
			char M[256] = {};
			msg >> M;
			int Id = client->GetID();
			std::string s = std::to_string(Id)+": "+ M;
			for (int i = 0; i < 255; i++)
			{
				M[i] = s[i];
			}
			fin << "Message from client #" << M << std::endl;
			msg2 << M;
			MessageAllClients(msg2, client);

		}
		break;
		}
	}
};

int main()
{
	CustomServer server(60000);
	server.Start();

	while (1)
	{
		server.Update(-1, true);
	}



	return 0;
}