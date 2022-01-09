#ifndef KO_NETWORKING_H
#define KO_NETWORKING_H

#include <pthread.h>
#include <sys/socket.h>
#include "types.h"

#define ROLE_HOST   0
#define ROLE_CLIENT 1

#define NETWORK_FLAG_CONNECT    1
#define NETWORK_FLAG_DISCONNECT 2
#define NETWORK_FLAG_ACCEPTED   4
#define NETWORK_FLAG_DECLINED   8

#define NETWORK_STATUS_SUCCESS                1
#define NETWORK_STATUS_CONNECTION_DECLINED   -1
#define NETWORK_STATUS_SERVER_NOT_RESPONDING -2
#define NETWORK_STATUS_KICKED                -3

#define DATAGRAM_FLAGS(d) (((unsigned char *)d))
#define DATAGRAM_DATA(d) (((unsigned char *)d)+1)

#define SOCKET_WORKING() (udpSocket != -1)

extern int NetworkRole;
extern NetworkClient *clients;
extern int udpSocket;

extern pthread_t sockThread;
extern int exitThread;

extern socklen_t addrlen;

void sendDatagram(NetworkDatagram *datagram, ssize_t dataLength, struct sockaddr *addr, socklen_t addrlen);
void killSocket();

void HostServer(
  char *address, int port, 
  void (*onConnectionAttempt)(int *accept, struct sockaddr *addr),
  void (*onConnection)(NetworkClient *c),
  void (*onDisconnection)(NetworkClient *c),
  void (*onData)(NetworkClient *c, unsigned char *data, ssize_t size)
);
void SendToClient(NetworkClient *c, NetworkDatagram *datagram, ssize_t dataLength);
void CloseServer();

void Connect(
  char *address, int port,
  void (*onConnectionPtr)(int status),
  void (*onDisconnectionPtr)(int status),
  void (*onDataPtr)(unsigned char * data, ssize_t size)
);
void SendToServer(NetworkDatagram *datagram, ssize_t dataLength);
void Disconnect();

#endif
