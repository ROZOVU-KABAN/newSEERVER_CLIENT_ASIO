#include <iostream>
#include <olc_net.h>

#define NFUNC 5


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



class CustomClient : public olc::net::client_interface<CustomMsgTypes>
{
public:
	void PingServer()
	{
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerPing;

		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();

		msg << timeNow;
		Send(msg);
	}

	void MessageAll()
	{
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::MessageAll;
		char m[256] = {};
		std::cout << "Enter your msg: ";
		gets_s(m, 256);
		msg << m;
		Send(msg);
	}

	void MessageOne()
	{
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::MessageOne;
		char m[256];
		int MPID;
		std::cout << "Enter ID of your message-partner: ";
		std::cin >> MPID;
		char t;
		std::cout << "Enter your msg: ";
		scanf_s("%c", &t);
		gets_s(m, 256);
		shift(m, 4,MPID);
		msg << m;
		Send(msg);
	}
	
	void SendFile()
	{
		std::string path;
		std::string FielName;
		std::ifstream fin;
		do
		{
			std::cout << "Enter path to File and File name: ";
			getline(std::cin, path);
			fin.open(path, std::ios_base::binary);
			if (!fin.is_open()) std::cout << "\nError. File cant be open. Please try one more time.\n";
		} while (!fin.is_open());

		std::cout << "Enter File name with extension: ";
		getline(std::cin, FielName);

		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::SendFiel;
		msg.header.FielName = FielName;
		char buf[255];
		char ch;
		int i = 0;
	    while (fin.get(ch))
		{
			buf[i] = ch;
			i++;
			if (i == 255)
			{
				msg << buf;
				Send(msg);
				ZeroMemory(buf, 255);
				i = 0;
			}
		}
		fin.close();
		if (i > 0)
		{
			msg << buf;
			Send(msg);
		}
		
		std::cout << "*File was sended*\n";
	}
private:
	void shift(char msg[256],int n,int ID)
	{
		for (int i = 0; i <n ; i++)
		{
			for (int j = 254; j >= 0; j--)
			{
				msg[j + 1] = msg[j];
			}
			msg[0] = (char)(ID % 10);
			ID /= 10;
		}
	}
};


int main()
{
	CustomClient c;
	c.Connect("127.0.0.1", 60000);
	//std::cout << "Enter your Nicname: ";
	//std::string Nickname;
	//std::cin >> Nickname;
	bool key[5];
	bool old_key[5];
	for (int i = 0; i < NFUNC; i++)
	{
		key[i] = false;
		old_key[i] = false;
	}
	std::string path;
	bool bQuit = false;
	while (!bQuit)
	{
		if (GetForegroundWindow() == GetConsoleWindow())
		{
			key[0] = GetAsyncKeyState('1') & 0x8000;
			key[1] = GetAsyncKeyState('2') & 0x8000;
			key[2] = GetAsyncKeyState('3') & 0x8000;
			key[3] = GetAsyncKeyState('4') & 0x8000;
			key[4] = GetAsyncKeyState('0') & 0x8000;
		}

		if (key[0] && !old_key[0]) c.PingServer();
		if (key[1] && !old_key[1]) c.MessageAll();
		if (key[2] && !old_key[2]) c.MessageOne();
		if (key[3] && !old_key[3])  c.SendFile();
		if (key[4] && !old_key[4]) bQuit = true;

		for (int i = 0; i < NFUNC; i++) old_key[i] = key[i];

		if (c.IsConnected())
		{
			if (!c.Incoming().empty())
			{


				auto msg = c.Incoming().pop_front().msg;

				switch (msg.header.id)
				{
				case CustomMsgTypes::ServerAccept:
				{
					std::cout << "Server Accepted Connection\n";
				}
				break;


				case CustomMsgTypes::ServerPing:
				{
					std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
					std::chrono::system_clock::time_point timeThen;
					msg >> timeThen;
					std::cout << "Ping: " << std::chrono::duration<double>(timeNow - timeThen).count() << "\n";
				}
				break;

				case CustomMsgTypes::ServerMessage:
				{
					char M[256];
					msg >> M;
					std::cout << "Message from client #" << M << "\n";
				}
				break;
				}
			}
		}
		else
		{
			std::cout << "Server Down\n";
			bQuit = true;
		}

	}

	return 0;
}