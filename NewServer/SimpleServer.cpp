#include <iostream>
#include <olc_net.h>


enum class CustomMsgTypes : uint32_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage,
	MessageOne,
	ReadFile,
	SendFiel,
};

std::ifstream fin;
std::ofstream fout;




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

	virtual void OnClientDisconnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client)
	{
		std::cout << "Removing client [" << client->GetID() << "]\n";
	}




	virtual void OnMessage(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client, olc::net::message<CustomMsgTypes>& msg)
	{
		switch (msg.header.id)
		{
		case CustomMsgTypes::ServerPing:
		{
			std::cout << "[" << client->GetID() << "]: Server Ping\n";
			client->Send(msg);
		}break;

		case CustomMsgTypes::MessageAll:
		{
			setlocale(LC_ALL, "ru");
			std::cout << "[" << client->GetID() << "]: Message All\n";
			olc::net::message<CustomMsgTypes> msg2;
			msg2.header.id = CustomMsgTypes::ServerMessage;
			char M[256] = {};
			msg >> M;
			int Id = client->GetID();
			std::string s = std::to_string(Id) + ": " + M;
			for (int i = 0; i < 255; i++)
			{
				M[i] = s[i];
			}
			fout.open("C:/Users/caxax/OneDrive/Рабочий стол/c++/SERVER_CLIENT_ASIO_MSGHISTORY/MsgHistory.txt",std::fstream::app);
			fout << "Message from client #" << M << std::endl;
			msg2 << M;
			MessageAllClients(msg2, client);
			fout.close();

		}break;

		case CustomMsgTypes::MessageOne:
		{
			setlocale(LC_ALL, "ru");
			olc::net::message<CustomMsgTypes> msg2;
			msg2.header.id = CustomMsgTypes::ServerMessage;
			char M[256] = {};
			msg >> M;
			int ID = BildID(M);
			std::cout << "[" << client->GetID() << "]: Message One to Client #" << ID;
			shift(M, 4);
			int Id = client->GetID();
			std::string s = std::to_string(Id) + ": " + M;
			for (int i = 0; i < 255; i++)
			{
				M[i] = s[i];
			}
			fout.open("C:/Users/caxax/OneDrive/Рабочий стол/c++/SERVER_CLIENT_ASIO_MSGHISTORY/MsgHistory.txt", std::fstream::app);
			fout<< "Message from client #" << ID << " from client #" << M << std::endl;
			msg2 << M;
			MessageOneClient(msg2, ID);
			fout.close();
		}break; 

		case CustomMsgTypes::SendFiel:
		{
			std::string FielName = msg.header.FielName;			
			fout.open(FielName, std::ios_base::binary | std::ios_base::app);
			char buf[255] = {};
			msg >> buf;
			for (int i = 0; i<sizeof(buf) ; i++)
			{
				fout << buf[i];
			}
			fout.close();
		}break;
		
		}
	}
private:
	void shift(char msg[256], int n)
	{
		for (int i = 0; i < n; i++)
		{
			for (int j = 1; j < 254; j++)
			{
				msg[j - 1] = msg[j];
			}
		}
	}
	int BildID(char M[])
	{
		int ID = int(M[0]);
		ID *= 10;
		ID += int(M[1]);
		ID *= 10;
		ID += int(M[2]);
		ID *= 10;
		ID += int(M[3]);
		return ID;
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