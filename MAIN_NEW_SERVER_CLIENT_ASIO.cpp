#include <iostream>
#include <chrono>
#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>


std::vector<char> vBuffer(1*1024);

void GrabSomeData(asio::ip::tcp::socket& socket)
{
	socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
		[&](std::error_code ec, std::size_t lenght)
		{
			if (!ec)
			{
				std::cout << "\n \nRead " << lenght << " bytes\n\n";

				for (int i = 0; i < lenght; i++)
				{
					std::cout << vBuffer[i];
				}
				GrabSomeData(socket);
			}
		}
	);
}

int main()
{
	setlocale(LC_ALL, "ru");
	asio::error_code ec;
	asio::io_context context;
	asio::io_context::work idleWork(context);

	std::thread thrContext = std::thread([&]() {context.run(); });
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("91.198.174.192", ec), 80);
	asio::ip::tcp::socket socket(context);

	socket.connect(endpoint, ec);

	if (!ec)
	{
		std::cout << "Connected\n";
	}
	else
	{
		std::cout << "Faild connect to addres: \n" << ec.message() << std::endl;
	}

	if (socket.is_open())
	{
		GrabSomeData(socket);
		std::string sRequest = "GET /index.html HTTP/1.1\r\n"
			"Host: example.com\r\n"
			"Connections: close\r\n\r\n";

		socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);



		using namespace std::chrono_literals;
		std::this_thread::sleep_for(2000ms);
	}

	system("pause");

	return 0;
}