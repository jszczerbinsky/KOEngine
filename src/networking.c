#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "networking.h"
#include "types.h"
#include "log.h"

int NetworkRole = ROLE_HOST;
int udpSocket = -1;
socklen_t addrlen;

void *networkSettingsPtr = NULL;

SDL_Thread *sockThread;
int exitThread = 0;

bool SocketWorking()
{
  #ifdef _WIN32
	return (udpSocket == INVALID_SOCKET);
  #else
	return (udpSocket != -1);
  #endif
}

void sendDatagram(unsigned char *datagram, ssize_t dataLength, struct sockaddr *addr, socklen_t addrlen)
{
  sendto(udpSocket, datagram, dataLength+1, 0, addr, addrlen);
}

void initSocket()
{
  #ifdef _WIN32
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(1,1), &wsa_data);
  #endif
}

void killSocket()
{
  if(SocketWorking())
  {
	#ifdef _WIN32
	  shutdown(udpSocket, SD_BOTH);
	#else
      shutdown(udpSocket, SHUT_RDWR);
	  close(udpSocket);
    #endif
  }
  #ifdef _WiN32
	udpSocket = INVALID_SOCKET;
  #else
    udpSocket = -1;
  #endif

  exitThread = 0;
  
  #ifdef _WIN32
    WSACleanup();
  #endif
}
