#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {

    if (SDLNet_Init() == -1) {
        cerr << "SDLNet Initialization error: " << SDLNet_GetError() << endl;
        return 1;
    }

    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, nullptr, 4242) == -1) {
        cerr << "Resolve Host error: " << SDLNet_GetError() << endl;
        SDLNet_Quit();
        return 1;
    }

    TCPsocket serverSocket = SDLNet_TCP_Open(&ip);
    if (!serverSocket) {
        cerr << "TCP Open error: " << SDLNet_GetError() << endl;
        SDLNet_Quit();
        return 1;
    }

    TCPsocket clientSocket;
    while (true) {
        clientSocket = SDLNet_TCP_Accept(serverSocket);
        if (clientSocket) {
            cout << "A client joined the server!" << endl;
            while (true) {
                char buffer[1024];
                int bytesRead = SDLNet_TCP_Recv(clientSocket, buffer, sizeof(buffer));
                if (bytesRead > 0) {
                    cout << "Incoming message: " << buffer << endl;
                    string answer = "Message received 5/5, client!";
                    int bytesSent = SDLNet_TCP_Send(clientSocket, answer.c_str(), answer.length() + 1);
                    if (bytesSent < answer.length() + 1) {
                        cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << endl;
                        break;
                    }
                }
                else {
                    cout << "Client left the server." << endl;
                    break;
                }
            }
            SDLNet_TCP_Close(clientSocket);
        }
    }

    SDLNet_TCP_Close(serverSocket);
    SDLNet_Quit();
    return 0;
}
