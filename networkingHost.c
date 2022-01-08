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

NetworkClient *clients = NULL;

void (*hostOnConnectionAttempt)(int *accept, struct sockaddr *addr) = NULL;
void (*hostOnConnection)(NetworkClient *c) = NULL;
void (*hostOnDisconnection)(NetworkClient *c) = NULL;
void (*hostOnData)(unsigned char *data, ssize_t size) = NULL;

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
      NetworkClient *matchedClient = NULL;
      while(next && !matchedClient)
      {
        next = c->next;

        if(memcmp(&(c->address), (struct sockaddr*)&addr, addrlen))
        {
          Log("Match");
          matchedClient = c;
          break;
        }
      }

      if(!matchedClient)
      {
        if(*DATAGRAM_FLAGS(&buff) == NETWORK_FLAG_CONNECT)
        {
          int accept = 1;

          if(hostOnConnectionAttempt)
            (*hostOnConnectionAttempt)(&accept, (struct sockaddr *)&addr);

          if(accept)
          {
            NetworkClient *newC = addClient((struct sockaddr *)&addr); 
            newC->lastDatagram = now;

            *DATAGRAM_FLAGS(&buff) = NETWORK_FLAG_ACCEPTED;
            SendToClient(newC, &buff, 0);

            if(hostOnConnection)
              (*hostOnConnection)(newC);

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
      }
    }
    c = clients;
    next = c;

    while(next)
    {
      next = c->next;
      if(now - c->lastDatagram > 5)
      {
        Log("Client timeouted");
        *DATAGRAM_FLAGS(&buff) = NETWORK_FLAG_DISCONNECT;
        SendToClient(c, &buff, 0);
        removeClient(c);
      }
    }
  }
}

void HostServer(
  char *address, int port, 
  void (*onConnectionAttemptPtr)(int *accept, struct sockaddr *addr),
  void (*onConnectionPtr)(NetworkClient *c),
  void (*onDisconnectionPtr)(NetworkClient *c),
  void (*onDataPtr)(unsigned char *data, ssize_t size)
)
{
  hostOnConnectionAttempt = onConnectionAttemptPtr;
  hostOnConnection = onConnectionPtr;
  hostOnDisconnection= onDisconnectionPtr;
  hostOnData = onDataPtr;

  NetworkRole = ROLE_HOST;

	udpSocket = socket(PF_INET, SOCK_DGRAM, 0);
  if(udpSocket == -1)
  {
    Log("ERROR, can't create socket");
    return;
  }

	struct sockaddr_in hostAddr;

	hostAddr.sin_family = AF_INET;
  hostAddr.sin_port = htons(port);
  hostAddr.sin_addr.s_addr = inet_addr(address);
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
    Log("ERROR, can't host a server on port %d", port);
    return;
  }

  int i = 1;
  pthread_create(&sockThread, NULL, hostTask, (void*)&i);
}

void CloseServer()
{
  exitThread = 1;
  pthread_join(sockThread, NULL);
  while(clients)
    removeClient(clients);
}
