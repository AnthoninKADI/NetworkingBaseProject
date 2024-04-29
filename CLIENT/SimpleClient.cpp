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

	string typing;
	const int width = 500, height = 750;
	InitWindow(width, height, "My first chat window");
	SetTargetFPS(60);

	vector<Message> log{Message{false, "Waiting for someone to talk..."}};
	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(GRAY);
		DrawText("Welcome to ChArtFX", 220, 15, 25, WHITE);
		DrawRectangle(20, 50, width - 40, height - 150, DARKGRAY);
		DrawRectangle(20, height - 90, width - 40, 50, LIGHTGRAY);

		int inputChar = GetCharPressed();
		if (inputChar != 0)
		{
			typing += static_cast<char>(inputChar);
		}
		if (typing.size() > 0)
		{
			DrawText(typing.c_str(), 30, height - 75, 25, DARKBLUE);
		}

		for (int msg = 0; msg < log.size(); msg++)
		{
			DrawText(log[msg].content.c_str(), 30, 75 + (msg * 30), 15, log[msg].fromMe ? SKYBLUE : PURPLE);
		}

		if (typing.size() > 0)
		{
			if (IsKeyPressed(KEY_BACKSPACE)) typing.pop_back();
			else if (IsKeyPressed(KEY_ENTER))
			{
				//Send the message typing to the server here!
				log.push_back(Message{ true, typing });
				typing.clear();
			}

			DrawText(typing.c_str(), 30, height - 75, 25, DARKBLUE);
		}
		EndDrawing();
	}
	CloseWindow();

	if (SDLNet_Init() == -1) {
		cerr << "SDLNet_Init error: " << SDLNet_GetError() << endl;
		return 1;
	}

	IPaddress ip;
	string host;

	int port;
	cout << "Enter the adreses to conenct: ";
	cin >> host;
	cout << "Enter the port number: ";
	cin >> port;

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
	cout << "Thank you for using ChArtFX !\n";
	return 0;
}
