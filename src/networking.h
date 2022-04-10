#ifndef KO_NETWORKING_H
#define KO_NETWORKING_H

#ifdef _WIN32
  #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501  /* Windows XP. */
  #endif
  #include <winsock2.h>
  #include <Ws2tcpip.h>
#else
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  #include <netinet/in.h>
#endif
#include "types.h"

#define NETWORK_DATAGRAM_MAX_BYTES 1024

#define ROLE_HOST   0
#define ROLE_CLIENT 1

#define NETWORK_FLAG_CONNECT    1
#define NETWORK_FLAG_DISCONNECT 2
#define NETWORK_FLAG_ACCEPTED   4
#define NETWORK_FLAG_DECLINED   8

#define NETWORK_STATUS_SUCCESS                1
#define NETWORK_STATUS_CONNECTION_DECLINED   -1
#define NETWORK_STATUS_SERVER_NOT_RESPONDING -2
#define NETWORK_STATUS_CLIENT_NOT_RESPONDING -3
#define NETWORK_STATUS_KICKED                -4

#define TIMEOUT_DISABLE -1

#define DATAGRAM_FLAGS(d) (((unsigned char *)d))
#define DATAGRAM_DATA(d) (d+1)

struct NetworkHostSettings
{
  char   *address;
  int     port;
  float   clientTimeoutTime; 

  void (* onConnectionAttempt )( int *accept, struct sockaddr *addr                   );
  void (* onConnection        )( NetworkClient *c                                     );
  void (* onDisconnection     )( NetworkClient *c                                     );
  void (* onData              )( NetworkClient *c, unsigned char *data, ssize_t size  );
  void (* clientLoopCall      )( NetworkClient *c                                     );
};

struct NetworkClientSettings
{
  char   *serverAddress;
  int     serverPort;
  float   timeoutTime; 

  void (* onConnection        )( int status                         );
  void (* onDisconnection     )( int status                         );
  void (* onData              )( unsigned char *data, ssize_t size  );
};

extern int            NetworkRole;
extern NetworkClient *ClientList;

bool SocketWorking();

void HostServer(struct NetworkHostSettings *settings);
void SendToClient(NetworkClient *c, unsigned char *datagram, ssize_t dataLength);
void CloseServer();

void Connect(struct NetworkClientSettings *settings);
void SendToServer(unsigned char *datagram, ssize_t dataLength);
void Disconnect();

#endif
