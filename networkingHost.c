#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include "networking.h"
#include "types.h"
#include "log.h"

#define HOST_SETTINGS ((NetworkHostSettings*)networkSettingsPtr)

NetworkClient *clients = NULL;

pthread_mutex_t clientsLock;

void SendToClient(NetworkClient *c, NetworkDatagram *datagram, ssize_t dataLength)
{
  sendDatagram(datagram, dataLength, (struct sockaddr*)&(c->address), addrlen);
}

NetworkClient *addClient(struct sockaddr *addr)
{
  NetworkClient *c = calloc(1, sizeof(NetworkClient));
  memcpy(&(c->address), addr, addrlen);

  c->next = clients;

  if(clients != NULL)
    clients->prev = c;

  clients = c;

  return c;
}

void removeClient(NetworkClient *c)
{
  if(c->prev != NULL) c->prev->next = c->next; 
  else clients = c->next;
  if(c->next != NULL) c->next->prev = c->prev;

  free(c);
}

void *hostTask(void *threadid)
{
  Log("Starting server");

  ssize_t bytes;

  while(1)
  {
    NetworkDatagram buff;
    struct sockaddr_storage addr;

    bytes = recvfrom(udpSocket, &buff, NETWORK_MAX_DATAGRAM, 0, (struct sockaddr *)&addr, &addrlen);
    if(exitThread) killSocket();

    time_t now = time(NULL);

    NetworkClient *c = clients;
    NetworkClient *next = c;

    if(bytes > 0)
    {
      pthread_mutex_lock(&clientsLock);
      NetworkClient *matchedClient = NULL;
      while(next && !matchedClient)
      {
        next = c->next;

        if(memcmp(&(c->address), (struct sockaddr*)&addr, addrlen) == 0)
        {
          matchedClient = c;
          break;
        }

        c = next;
      }

      if(!matchedClient)
      {
        if(*DATAGRAM_FLAGS(&buff) == NETWORK_FLAG_CONNECT)
        {
          int accept = 1;

          if(HOST_SETTINGS->onConnectionAttempt)
            (*HOST_SETTINGS->onConnectionAttempt)(&accept, (struct sockaddr *)&addr);

          if(accept)
          {
            NetworkClient *newC = addClient((struct sockaddr *)&addr); 
            newC->lastDatagram = now;

            *DATAGRAM_FLAGS(&buff) = NETWORK_FLAG_ACCEPTED;
            SendToClient(newC, &buff, 0);

            if(HOST_SETTINGS->onConnection)
              (*HOST_SETTINGS->onConnection)(newC);

            Log("Client connected");
          }
          else
          {
            *DATAGRAM_FLAGS(&buff) = NETWORK_FLAG_DECLINED;
            sendDatagram(&buff, 0, (struct sockaddr *)&addr, addrlen);
          }
        }
              }
      else
      {
        matchedClient->lastDatagram = time(NULL); 

        if(HOST_SETTINGS->onData)
          (*HOST_SETTINGS->onData)(matchedClient, DATAGRAM_DATA(&buff), bytes-1);

        if(*DATAGRAM_FLAGS(&buff) == NETWORK_FLAG_DISCONNECT)
        {
          if(HOST_SETTINGS->onDisconnection)
              (*HOST_SETTINGS->onDisconnection)(matchedClient);
          removeClient(matchedClient);
        }
      }
      pthread_mutex_unlock(&clientsLock);
    }
    c = clients;
    next = c;

    while(next)
    {
      next = c->next;
      if(HOST_SETTINGS->clientTimeoutTime != TIMEOUT_DISABLE && now - c->lastDatagram > HOST_SETTINGS->clientTimeoutTime)
      {
        Log("Client timed out");

        if(HOST_SETTINGS->onDisconnection)
          (*HOST_SETTINGS->onDisconnection)(c);

        *DATAGRAM_FLAGS(&buff) = NETWORK_FLAG_DISCONNECT;
        SendToClient(c, &buff, 0);
        removeClient(c);
      }
      c = next;
    }
  }
}

void HostServer(NetworkHostSettings *settings)
{
  networkSettingsPtr = settings;

  NetworkRole = ROLE_HOST;

  pthread_mutex_init(&clientsLock, NULL);

	udpSocket = socket(PF_INET, SOCK_DGRAM, 0);
  if(udpSocket == -1)
  {
    pthread_mutex_destroy(&clientsLock);
    Log("ERROR, can't create socket");
    return;
  }

	struct sockaddr_in hostAddr;

	hostAddr.sin_family = AF_INET;
  hostAddr.sin_port = htons(HOST_SETTINGS->port);
  hostAddr.sin_addr.s_addr = inet_addr(HOST_SETTINGS->address);
  memset(hostAddr.sin_zero, '\0', sizeof(hostAddr.sin_zero));

  addrlen = sizeof(hostAddr);

  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  int t= 1;
  setsockopt(udpSocket,SOL_SOCKET,SO_REUSEADDR,&t,sizeof(int));

 	if(bind(udpSocket, (struct sockaddr *)&hostAddr, sizeof(hostAddr)) == -1)
  {
    Log("ERROR, can't host a server on port %d", HOST_SETTINGS->port);
    pthread_mutex_destroy(&clientsLock);
    return;
  }

  int i = 1;
  pthread_create(&sockThread, NULL, hostTask, (void*)&i);
}

void updateClients()
{
  if(!HOST_SETTINGS->clientLoopCall) return;
  
  pthread_mutex_lock(&clientsLock);

  NetworkClient *c = clients;
  while(c)
  {
    (*HOST_SETTINGS->clientLoopCall)(c);
    c = c->next;
  }

  pthread_mutex_unlock(&clientsLock);
}

void CloseServer()
{
  exitThread = 1;
  pthread_join(sockThread, NULL);
  while(clients)
    removeClient(clients);
  pthread_mutex_destroy(&clientsLock);
}
