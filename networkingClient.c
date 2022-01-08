#include <string.h>
#include <arpa/inet.h>
#include "networking.h"
#include "log.h"

struct sockaddr_in serverAddr;

void (*clientOnConnection)(int status) = NULL;
void (*clientOnDisconnection)(int status) = NULL;
void (*clientOnData)(unsigned char * data, ssize_t size) = NULL;

void SendToServer(NetworkDatagram *datagram, ssize_t dataLength)
{
  sendDatagram(datagram, dataLength, (struct sockaddr *)&serverAddr, addrlen);
}

void *clientTask(void *threadid)
{
  Log("Connecting to server");

  ssize_t bytes;

  NetworkDatagram buff;
  (*DATAGRAM_FLAGS(&buff)) = NETWORK_FLAG_CONNECT;
  SendToServer(&buff, 0);

  int status = NETWORK_STATUS_SERVER_NOT_RESPONDING;

  for(int tries = 0; tries < 10; tries++)
  {
    (*DATAGRAM_FLAGS(&buff)) = 0;

    bytes = recvfrom(udpSocket, &buff, 1, 0, NULL, NULL);
    if(exitThread) killSocket();

    if(bytes > 0)
    {
      if((*DATAGRAM_FLAGS(&buff)) == NETWORK_FLAG_ACCEPTED) status = NETWORK_STATUS_SUCCESS;
      else status = NETWORK_STATUS_CONNECTION_DECLINED;
      break;
    }
  }

  switch(status)
  {
    case NETWORK_STATUS_SUCCESS:
      Log("Successfully connected");
      break;
    case NETWORK_STATUS_SERVER_NOT_RESPONDING:
      Log("Can't connect, server not responding");
      break;
    case NETWORK_STATUS_CONNECTION_DECLINED:
      Log("Can't connect, server declined connection");
      break;
  }

  if(clientOnConnection)
    (*clientOnConnection)(status);

  if(status != NETWORK_STATUS_SUCCESS) killSocket();
  (*DATAGRAM_FLAGS(&buff)) = 0;

  int timeoutSeconds = 0;
  while(1)
  {
    bytes = recvfrom(udpSocket, &buff, NETWORK_MAX_DATAGRAM, 0, NULL, NULL);
    if(exitThread) killSocket();

    if(bytes > 0)
    {
      timeoutSeconds = 0;

      if((*DATAGRAM_FLAGS(&buff)) == NETWORK_FLAG_DISCONNECT)
      {
        Log("Kicked by server");

        if(clientOnDisconnection)
          (*clientOnDisconnection)(NETWORK_STATUS_KICKED);
        killSocket();
      }

      ssize_t dataSize = bytes-1;
      if(dataSize > 0)
      {
        if(clientOnData)
          (*clientOnData)(DATAGRAM_DATA(buff), dataSize);
        Log("%s", DATAGRAM_DATA(buff));
      }
    }
    else
    {
      timeoutSeconds++;
      if(timeoutSeconds > 20)
      {
        Log("Server connection timeout");

        if(clientOnDisconnection)
          (*clientOnDisconnection)(NETWORK_STATUS_SERVER_NOT_RESPONDING);
        killSocket();
      }
    }
  }
}
void Connect(
  char *address, int port,
  void (*onConnectionPtr)(int status),
  void (*onDisconnectionPtr)(int status),
  void (*onDataPtr)(unsigned char * data, ssize_t size)
)
{
  clientOnConnection = onConnectionPtr;
  clientOnDisconnection = onDisconnectionPtr;
  clientOnData = onDataPtr;

  NetworkRole = ROLE_CLIENT;

	udpSocket = socket(PF_INET, SOCK_DGRAM, 0);
  if(udpSocket == -1)
  {
    Log("ERROR, can't create socket");
    return;
  }

	serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  serverAddr.sin_addr.s_addr = inet_addr(address);
  memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

  addrlen = sizeof(serverAddr);

  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

  int i = 1;
  pthread_create(&sockThread, NULL, clientTask, (void*)&i);
}
