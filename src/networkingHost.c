#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "networking.h"
#include "KOE.h"
#include "types.h"
#include "log.h"

#define HOST_SETTINGS ((struct NetworkHostSettings*)networkSettingsPtr)

extern void sendDatagram(unsigned char *datagram, ssize_t dataLength, struct sockaddr *addr, socklen_t addrlen);
extern void initSocket();
extern void killSocket();

extern int            udpSocket;
extern void          *networkSettingsPtr;
extern SDL_Thread    *sockThread;
extern int            exitThread;
extern socklen_t      addrlen;

NetworkClient *ClientList = NULL;

void SendToClient(NetworkClient *c, unsigned char *datagram, ssize_t dataLength)
{
  sendDatagram(datagram, dataLength, (struct sockaddr*)&(c->address), addrlen);
}

NetworkClient *addClient(struct sockaddr *addr)
{
  NetworkClient *c = calloc(1, sizeof(NetworkClient));
  memcpy(&(c->address), addr, addrlen);

  c->next = ClientList;

  if(ClientList != NULL)
    ClientList->prev = c;

  ClientList = c;

  return c;
}

void removeClient(NetworkClient *c)
{
  if(c->prev != NULL) c->prev->next = c->next; 
  else ClientList= c->next;
  if(c->next != NULL) c->next->prev = c->prev;

  free(c);
}

static int hostTask(void *threadid)
{
  Log("Starting server");

  ssize_t bytes;

  while(1)
  {
    unsigned char buff[NETWORK_DATAGRAM_MAX_BYTES];
    struct sockaddr_storage addr;

    bytes = recvfrom(udpSocket, buff, NETWORK_DATAGRAM_MAX_BYTES, 0, (struct sockaddr *)&addr, &addrlen);
    if(exitThread) {killSocket(); return 0;}

    time_t now = time(NULL);

    NetworkClient *c = ClientList;
    NetworkClient *next = c;

    if(bytes > 0)
    {
      LOCK();
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
        if(*DATAGRAM_FLAGS(buff) == NETWORK_FLAG_CONNECT)
        {
          int accept = 1;

          if(HOST_SETTINGS->onConnectionAttempt)
            (*HOST_SETTINGS->onConnectionAttempt)(&accept, (struct sockaddr *)&addr);

          if(accept)
          {
            NetworkClient *newC = addClient((struct sockaddr *)&addr); 
            newC->lastDatagram = now;

            *DATAGRAM_FLAGS(buff) = NETWORK_FLAG_ACCEPTED;
            SendToClient(newC, buff, 0);

            if(HOST_SETTINGS->onConnection)
              (*HOST_SETTINGS->onConnection)(newC);

            Log("Client connected");
          }
          else
          {
            *DATAGRAM_FLAGS(buff) = NETWORK_FLAG_DECLINED;
            sendDatagram(buff, 0, (struct sockaddr *)&addr, addrlen);
          }
        }
      }
      else
      {
        matchedClient->lastDatagram = time(NULL); 

        if(HOST_SETTINGS->onData)
          (*HOST_SETTINGS->onData)(matchedClient, DATAGRAM_DATA(buff), bytes-1);

        if(*DATAGRAM_FLAGS(buff) == NETWORK_FLAG_DISCONNECT)
        {
          if(HOST_SETTINGS->onDisconnection)
              (*HOST_SETTINGS->onDisconnection)(matchedClient);
          removeClient(matchedClient);
        }
      }
      UNLOCK();
    }
    LOCK();
    c = ClientList;
    next = c;

    while(next)
    {
      next = c->next;
      if(HOST_SETTINGS->clientTimeoutTime != TIMEOUT_DISABLE && now - c->lastDatagram > HOST_SETTINGS->clientTimeoutTime)
      {
        Log("Client timed out");

        if(HOST_SETTINGS->onDisconnection)
          (*HOST_SETTINGS->onDisconnection)(c);

        *DATAGRAM_FLAGS(buff) = NETWORK_FLAG_DISCONNECT;
        SendToClient(c, buff, 0);
        removeClient(c);
      }
      c = next;
    }
    UNLOCK();
  }
}

void HostServer(struct NetworkHostSettings *settings)
{
  initSocket();
  networkSettingsPtr = settings;

  NetworkRole = ROLE_HOST;


	udpSocket = socket(PF_INET, SOCK_DGRAM, 0);
  if(udpSocket == -1)
  {
    Log("ERROR, can't create socket");
    return;
  }

  struct sockaddr_in hostAddr;

  hostAddr.sin_family = AF_INET;
  hostAddr.sin_port = htons(HOST_SETTINGS->port);
  hostAddr.sin_addr.s_addr = inet_addr(HOST_SETTINGS->address);
  memset(hostAddr.sin_zero, '\0', sizeof(hostAddr.sin_zero));

  addrlen = sizeof(hostAddr);

  int time= 1;
  #ifdef _WIN32
    const int i = 1000;
	setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&i, sizeof(const int));
	setsockopt(udpSocket,SOL_SOCKET,SO_REUSEADDR,(const char *)&time,sizeof(int));
  #else
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	setsockopt(udpSocket,SOL_SOCKET,SO_REUSEADDR,&time,sizeof(int));
  #endif
  
  

 	if(bind(udpSocket, (struct sockaddr *)&hostAddr, sizeof(hostAddr)) == -1)
  {
    Log("ERROR, can't host a server on port %d", HOST_SETTINGS->port);
    return;
  }

  int t = 1;
  sockThread = SDL_CreateThread(&hostTask, NULL, (void*)&t);
}

void updateClients()
{
  if(!HOST_SETTINGS->clientLoopCall) return;
  
  NetworkClient *c = ClientList;
  while(c)
  {
    (*HOST_SETTINGS->clientLoopCall)(c);
    c = c->next;
  }
}

void CloseServer()
{
  exitThread = 1;
  SDL_WaitThread(sockThread, NULL);
  while(ClientList)
    removeClient(ClientList);
}
