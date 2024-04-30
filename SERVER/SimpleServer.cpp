#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
using namespace std;

struct ClientInfo {
    TCPsocket socket;
    string name;
};

vector<ClientInfo> clients;
mutex mtx;

void HandleClient(TCPsocket clientSocket) {

    char nameBuffer[1024];
    int bytesReadName = SDLNet_TCP_Recv(clientSocket, nameBuffer, sizeof(nameBuffer));
    string clientName(nameBuffer);
    if (bytesReadName > 0)
    {
        cout << "Client " << clientName << " joined the server\n";
    }

    while (true) {
        char buffer[1024];
        int bytesRead = SDLNet_TCP_Recv(clientSocket, buffer, sizeof(buffer));
        if (bytesRead > 0) {
            cout << clientName << " : " << buffer << '\n';

            // Broadcast the message to all clients
            string message = string(buffer);
            {
                lock_guard<mutex> lock(mtx);
                for (auto& client : clients) {
                    if (client.socket != clientSocket) {
                        int bytesSent = SDLNet_TCP_Send(client.socket, message.c_str(), message.length() + 1);
                        if (bytesSent < message.length() + 1) {
                            cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << endl;
                        }
                    }
                }
            }
        }
        else {

            SDLNet_TCP_Close(clientSocket);
            {
                lock_guard<mutex> lock(mtx);
                clients.erase(remove_if(clients.begin(), clients.end(), [clientSocket](const ClientInfo& client) {
                    return client.socket == clientSocket;
                    }), clients.end());
            }
            cout << "Client disconnected." << endl;
            break;
        }
    }
}

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

    while (true) {
        TCPsocket clientSocket = SDLNet_TCP_Accept(serverSocket);
        if (clientSocket) {
            thread(HandleClient, clientSocket).detach();

            ClientInfo clientInfo;
            clientInfo.socket = clientSocket;
            {
                lock_guard<mutex> lock(mtx);
                clients.push_back(clientInfo);
            }

            cout << "Client connected." << endl;
        }
    }

    SDLNet_TCP_Close(serverSocket);
    SDLNet_Quit();
    return 0;
}