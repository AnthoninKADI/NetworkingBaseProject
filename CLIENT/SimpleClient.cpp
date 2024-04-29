#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
#include <string>
#include <vector>

#include "raylib.h"
//#include <string>
using namespace std;

struct Message
{
	bool fromMe = false;
	string content;

};

int main(int argc, char* argv[])
{

	const int width = 400, height = 300;
	InitWindow(width, height, "Login Window");
	SetTargetFPS(60);

	string host;
	string s_port;
	string name;
	int port = 4242;
	bool ipSelected = false;
	bool hostComplete = false;
	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(GRAY);
		DrawText("Login", 155, 10, 40, WHITE); // Login Text
		DrawText("ip", 25, 90, 25, WHITE); // Ip Adress
		DrawText("port", 10, 160, 25, WHITE); // Port
		DrawText("name", 10, 230, 25, WHITE); // Name

		DrawRectangle(70, 90, width - 100, 25, DARKGRAY);
		DrawRectangle(70, 160, width - 100, 25, DARKGRAY);
		DrawRectangle(70, 230, width - 100, 25, DARKGRAY);

		int inputChar = GetCharPressed();

		if (inputChar != 0)
		{
			if (!hostComplete) host += static_cast<char>(inputChar);
			else s_port += static_cast<char>(inputChar);

		}
		if (IsKeyPressed(KEY_ENTER))
		{
			if (!hostComplete)
			{
				hostComplete = true;
				cout << hostComplete << endl;
			}
			else
			{
				port = stoi(s_port);
				break;
			}
		}
		else if (IsKeyPressed(KEY_BACKSPACE))
		{
			if (hostComplete && !s_port.empty()) s_port.pop_back();
			else if (!host.empty())  host.pop_back();
		}
		if (!host.empty())
		{
			DrawText(host.c_str(), 75, 90, 25, WHITE);
		}
		if (!s_port.empty())
		{
			DrawText(s_port.c_str(), 75, 160, 25, WHITE);
		}

		EndDrawing();

	}
	CloseWindow();

	if (SDLNet_Init() == -1) {
		cerr << "SDLNet_Init error: " << SDLNet_GetError() << endl;
		return 1;
	}

	IPaddress ip;


	if (SDLNet_ResolveHost(&ip, host.c_str(), port) == -1)
	{
		cerr << "Resolver Host error: " << SDLNet_GetError() << endl;
		return 1;
	}

	TCPsocket clientSocket = SDLNet_TCP_Open(&ip);
	if (!clientSocket)
	{
		cerr << "TCP Open error: " << SDLNet_GetError() << endl;
		SDLNet_Quit();
		return 1;
	}

	string typing;
	const int Mwidth = 500, Mheight = 750;
	InitWindow(Mwidth, Mheight, "ChatBox Window");
	SetTargetFPS(60);

	vector<Message> log{Message{false, "Welcome to the ChatBox"}};
	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(GRAY);
		DrawText("Welcome to ChatBox", 150, 15, 25, WHITE);
		DrawRectangle(20, 50, Mwidth - 40, Mheight - 150, DARKGRAY);
		DrawRectangle(20, Mheight - 90, Mwidth - 40, 50, LIGHTGRAY);

		int inputChar = GetCharPressed();
		if (inputChar != 0)
		{
			typing += static_cast<char>(inputChar);
		}
		if (!typing.empty())
		{
			if (IsKeyPressed(KEY_BACKSPACE)) typing.pop_back();
			else if (IsKeyPressed(KEY_ENTER))
			{
				log.push_back(Message{ true,typing });
				typing.clear();
			}
			DrawText(typing.c_str(), 30, Mheight - 75, 25, DARKBLUE);
		}

		for (int msg = 0; msg < log.size(); msg++)
		{
			DrawText(log[msg].content.c_str(), 30, 75 + (msg * 30), 15, log[msg].fromMe ? SKYBLUE : PURPLE);
		}

		EndDrawing();

	}
	CloseWindow();



	string message;

	cout << "Please enter a message : " << endl;
	cin.clear();
	getline(cin, message);

	int bytesSent = SDLNet_TCP_Send(clientSocket, message.c_str(), message.length() + 1);
	if (bytesSent < message.length() + 1)
	{
		cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << endl;
		SDLNet_TCP_Close(clientSocket);
		SDLNet_Quit();
		return 1;
	}
	char buffer[1024];
	int bytesRead = SDLNet_TCP_Recv(clientSocket, buffer, sizeof(buffer));
	if (bytesRead <= 0)
	{
		cerr << "SDLNet TCP Recv error: " << SDLNet_GetError() << endl;
		SDLNet_Quit();
		return 1;
	}
	cout << "Incoming response: " << buffer << endl;
	cout << "Sent " << bytesSent << " bytes to the server !" << endl;
	SDLNet_TCP_Close(clientSocket);
	SDLNet_Quit();
	cout << "Thank you for using ChatBox !\n";
	return 0;
}