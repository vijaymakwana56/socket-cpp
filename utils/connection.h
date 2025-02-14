#include "../headers/common.h"
#include "../headers/networking.h"
#include "../headers/types.h"
#include "../constants/db.h" // current db
#include "../headers/namespace.h"

#ifndef CONNECTION
#define CONNECTION

#define BUFFER_SIZE 1024

// socket connection class
// containing socket and username information
class Connection
{
public:
    SOCKET s; // connection socket
    string username;
    Connection(SOCKET socket) : s(socket) {}
    Connection(SOCKET socket, string username) : s(socket), username(username) {}

    // sends the message to connected client
    void send_(string message)
    {
        send(s, message.c_str(), message.size(), 0);
    }

    // sends a message from the connected client to target client
    void send_to(string message, SOCKET target_client)
    {
        message = "[" + username + "] : " + message;
        send(target_client, message.c_str(), message.size(), 0);
    }

    // recieves from connected client
    psi receive()
    {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer)); // reset buffer

        int bytes_recieved = recv(s, buffer, sizeof(buffer) - 1, 0);
        string rec_message = string(buffer);

        psi result(rec_message, bytes_recieved);

        return result;
    }

    // broadcast to other connected clients
    void broadcast(string message, sS sockets = db.get_clients())
    {
        for (SOCKET client : sockets)
        {
            if (client != s)
                send(client, message.c_str(), message.size(), 0);
        }
    }

    // broadcast from connected client to other connected clients
    void broadcast_by(string message, sS sockets = db.get_clients())
    {
        message = "[" + username + "] : " + message;
        broadcast(message, sockets);
    }

    // broadcast to other connected clients in a group
    void broadcast_group(string message, string group_name)
    {
        message = "[Group " + group_name + "] : " + message;
        broadcast(message, db.get_group_members(group_name));
    }

    // broadcast from connected client to other connected clients in group
    void broadcast_group_by(string message, string group_name)
    {
        message = "[Group " + group_name + " (" + username + ")] : " + message;
        broadcast(message, db.get_group_members(group_name));
    }

    // close the connection
    void close_()
    {
        broadcast(username + " has left the chat.");
        db.remove_user(s);
        db.remove_user(username);

// close socket
#ifdef _WIN32
        closesocket(s);
#else
        close(s);
#endif
    }

    // close the connection after sending a message to connected client
    void close_(string message)
    {
        send_(message);
        broadcast(username + " has left the chat.");
        db.remove_user(s);
        db.remove_user(username);

// close socket
#ifdef _WIN32
        closesocket(s);
#else
        close(s);
#endif
    }
};

#endif