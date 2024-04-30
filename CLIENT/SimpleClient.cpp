#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
#include <string>
#include <vector>
#include "raylib.h"
using namespace std;

struct Message
{
    string sender;
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
    bool hostComplete = false;
    bool nameComplete = false;
    bool portComplete = false;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKGRAY);
        DrawText("Login", 155, 10, 40, WHITE); // Login Text
        DrawText("ip", 25, 90, 25, WHITE);    // Ip Adress
        DrawText("port", 10, 160, 25, WHITE); // Port
        DrawText("name", 10, 230, 25, WHITE); // Name

        DrawRectangle(70, 90, width - 100, 25, WHITE);
        DrawRectangle(70, 160, width - 100, 25, WHITE);
        DrawRectangle(70, 230, width - 100, 25, WHITE);

        int inputChar = GetCharPressed();

        if (inputChar != 0)
        {
            if (!hostComplete)
            {
                host += static_cast<char>(inputChar);
            }
            else if (!portComplete)
            {
                s_port += static_cast<char>(inputChar);
            }
            else if (!nameComplete)
            {
                name += static_cast<char>(inputChar);
            }
        }
        if (IsKeyPressed(KEY_ENTER))
        {
            if (!hostComplete)
            {
                hostComplete = true;
            }
            else if (!portComplete)
            {
                port = stoi(s_port);
                portComplete = true;
            }
            else if (!name.empty())
            {
                nameComplete = true;
                break;
            }
        }
        else if (IsKeyPressed(KEY_BACKSPACE))
        {
            if (hostComplete && !name.empty())
                name.pop_back();
            else if (portComplete && !s_port.empty())
                s_port.pop_back();
            else if (!host.empty())
                host.pop_back();
        }
        if (!host.empty())
        {
            DrawText(host.c_str(), 75, 90, 25, BLACK);
        }
        if (!s_port.empty())
        {
            DrawText(s_port.c_str(), 75, 160, 25, BLACK);
        }
        if (!name.empty())
        {
            DrawText(name.c_str(), 75, 230, 25, BLACK);
        }
        EndDrawing();
    }
    CloseWindow();

    if (SDLNet_Init() == -1)
    {
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

    // user's name --> server
    int bytesSent = SDLNet_TCP_Send(clientSocket, name.c_str(), name.length() + 1);
    if (bytesSent < name.length() + 1)
    {
        cerr << "SDLNet TCP Send error : " << SDLNet_GetError() << '\n';
        SDLNet_TCP_Close(clientSocket);
        SDLNet_Quit();
        return 1;
    }

    const int Mwidth = 500, Mheight = 750;
    InitWindow(Mwidth, Mheight, "ChatBox Window");
    SetTargetFPS(60);

    vector<Message> log{Message{"Welcome to the ChatBox"}};

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(GRAY);
        DrawText("Welcome to ChatBox", 150, 15, 25, WHITE);
        DrawText("by Anthonin", 10, 730, 10, WHITE); // Copyright
        DrawRectangle(20, 50, Mwidth - 40, Mheight - 150, DARKGRAY);
        DrawRectangle(20, Mheight - 90, Mwidth - 40, 50, WHITE);

        int inputChar = GetCharPressed();
        if (inputChar != 0)
        {
            typing += static_cast<char>(inputChar);
        }
        if (!typing.empty())
        {
            if (IsKeyPressed(KEY_BACKSPACE))
                typing.pop_back();
            else if (IsKeyPressed(KEY_ENTER))
            {
                // Send message to the server
                int bytesSent = SDLNet_TCP_Send(clientSocket, typing.c_str(), typing.length() + 1);
                if (bytesSent < typing.length() + 1)
                {
                    cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << endl;
                    SDLNet_TCP_Close(clientSocket);
                    SDLNet_Quit();
                    return 1;
                }
                // Add the message to the chat log
                log.push_back(Message{ name, typing });
                typing.clear();
            }
            DrawText(typing.c_str(), 30, Mheight - 75, 25, BLACK);
        }

        for (size_t i = 0; i < log.size(); i++)
        {
            DrawText(TextFormat("[%s] %s", log[i].sender.c_str(), log[i].content.c_str()), 30, 75 + (i * 30), 15, SKYBLUE);
        }

        EndDrawing();
    }
    CloseWindow();

    SDLNet_TCP_Close(clientSocket);
    SDLNet_Quit();
    return 0;
}