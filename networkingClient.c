#include <string.h>
#include <arpa/inet.h>
#include "networking.h"
#include "log.h"
#include "KOEngine.h"

#define CLIENT_SETTINGS ((struct NetworkClientSettings*)networkSettingsPtr)

extern void sendDatagram(unsigned char *datagram, ssize_t dataLength, struct sockaddr *addr, socklen_t addrlen);
extern void killSocket();

extern int            udpSocket;
extern void          *networkSettingsPtr;
extern pthread_t      sockThread;
extern int            exitThread;
extern socklen_t      addrlen;

struct sockaddr_in serverAddr;

void SendToServer(unsigned char *datagram, ssize_t dataLength)
{
  sendDatagram(datagram, dataLength, (struct sockaddr *)&serverAddr, addrlen);
}

void *clientTask(void *threadid)
{
  Log("Connecting to server");

  ssize_t bytes;

  unsigned char buff[NETWORK_DATAGRAM_MAX_BYTES];
  (*DATAGRAM_FLAGS(&buff)) = NETWORK_FLAG_CONNECT;
  SendToServer(buff, 0);

  int status = NETWORK_STATUS_SERVER_NOT_RESPONDING;

  for(int tries = 0; tries < 10; tries++)
  {
    (*DATAGRAM_FLAGS(buff)) = 0;

    bytes = recvfrom(udpSocket, &buff, 1, 0, NULL, NULL);
    if(exitThread) killSocket();

    if(bytes > 0)
    {
      if((*DATAGRAM_FLAGS(buff)) == NETWORK_FLAG_ACCEPTED) status = NETWORK_STATUS_SUCCESS;
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

  if(CLIENT_SETTINGS->onConnection)
    (*CLIENT_SETTINGS->onConnection)(status);

  if(status != NETWORK_STATUS_SUCCESS) killSocket();
  (*DATAGRAM_FLAGS(buff)) = 0;

  int timeoutSeconds = 0;
  while(1)
  {
    bytes = recvfrom(udpSocket, buff, NETWORK_DATAGRAM_MAX_BYTES, 0, NULL, NULL);
    if(exitThread)
    {
      if(CLIENT_SETTINGS->onDisconnection)
        (*CLIENT_SETTINGS->onDisconnection)(NETWORK_STATUS_SUCCESS);
      killSocket();
    }

    if(bytes > 0)
    {
      timeoutSeconds = 0;

      LOCK();

      if((*DATAGRAM_FLAGS(buff)) == NETWORK_FLAG_DISCONNECT)
      {
        Log("Kicked by server");

        if(CLIENT_SETTINGS->onDisconnection)
          (*CLIENT_SETTINGS->onDisconnection)(NETWORK_STATUS_KICKED);
        killSocket();
      }

      ssize_t dataSize = bytes-1;
      if(dataSize > 0)
      {
        if(CLIENT_SETTINGS->onData)
          (*CLIENT_SETTINGS->onData)(DATAGRAM_DATA(buff), dataSize);
      }

      UNLOCK();
    }
    else
    {
      timeoutSeconds++;
      if(CLIENT_SETTINGS->timeoutTime != TIMEOUT_DISABLE && timeoutSeconds > CLIENT_SETTINGS->timeoutTime)
      {
        Log("Server connection timeout");

        LOCK();

        if(CLIENT_SETTINGS->onDisconnection)
          (*CLIENT_SETTINGS->onDisconnection)(NETWORK_STATUS_SERVER_NOT_RESPONDING);

        UNLOCK();

        killSocket();
      }
    }
  }
}
void Connect(struct NetworkClientSettings *settings)
{
  networkSettingsPtr = settings;

  NetworkRole = ROLE_CLIENT;

	udpSocket = socket(PF_INET, SOCK_DGRAM, 0);
  if(udpSocket == -1)
  {
    Log("ERROR, can't create socket");
    return;
  }

	serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(CLIENT_SETTINGS->serverPort);
  serverAddr.sin_addr.s_addr = inet_addr(CLIENT_SETTINGS->serverAddress);
  memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

  addrlen = sizeof(serverAddr);

  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

  int i = 1;
  pthread_create(&sockThread, NULL, clientTask, (void*)&i);
}

void Disconnect()
{
  unsigned char buff[NETWORK_DATAGRAM_MAX_BYTES];
  (*DATAGRAM_FLAGS(buff)) = NETWORK_FLAG_DISCONNECT;
  SendToServer(buff, 0);

  exitThread = 1;
  pthread_join(sockThread, NULL);
}
