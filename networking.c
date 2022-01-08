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

int NetworkRole = ROLE_HOST;
int udpSocket = -1;
socklen_t addrlen;

pthread_t sockThread;
int exitThread = 0;

void sendDatagram(NetworkDatagram *datagram, ssize_t dataLength, struct sockaddr *addr, socklen_t addrlen)
{
  sendto(udpSocket, datagram, dataLength+1, 0, addr, addrlen);
}

void killSocket()
{
  if(SOCKET_WORKING())
  {
    shutdown(udpSocket, SHUT_RDWR);
    close(udpSocket);
  }
  udpSocket = -1;

  exitThread = 0;
  pthread_exit(NULL);
}
