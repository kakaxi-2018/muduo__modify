#include "Socket.h"
#include "Logger.h"
#include "InetAddress.h"
#include <unistd.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/tcp.h>

Socket::~Socket()
{
    close(sockfd_);
}

void Socket::bindAddress(const InetAddress &localaddr)
{
    if (0 != ::bind(sockfd_, (struct sockaddr *)localaddr.getSockAddr(), sizeof(sockaddr_in)))
    {
        LOG_FATAL("bind sockfd: %d failed\n", sockfd_);
    }
}

void Socket::listen()
{
    if (0 != ::listen(sockfd_, 1024))
    {
        LOG_FATAL("listen sockfd: %d failed\n", sockfd_);
    }
}

int Socket::accept(InetAddress *peeraddr)
{
    struct sockaddr_in addr;
    socklen_t len;
    bzero(&addr, sizeof(addr));
    int connfd = ::accept(sockfd_, (sockaddr *)&addr, &len);
    // todo
    if (connfd > 0)
    {
        peeraddr->setSockAddr(addr);
    }

    return connfd;
}

void Socket::shutdownWrite()
{
    if (::shutdown(sockfd_, SHUT_WR) < 0)
    {
        LOG_ERROR("shutdownWrite error");
    }
}

// 开启/关闭 TCP_NODELAY (开启/关闭 Nagle 算法)
void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1: 0;
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
                &optval, sizeof(optval));
}

// 开启/关闭 SO_REUSEADDR
void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1: 0;
    ::setsockopt(sockfd_, SQL_SOCKET, SO_REUSEADDR,
            &optval, sizeof(optval));
}

// 开启/关闭 SO_USEPORT
void Socket::setReusePort(bool on)
{
    int optval = on ? 1: 0;
    ::setsockopt(sockfd_, SQL_SOCKET, SO_REUSEPORT,
            &optval, sizeof(optval));
}

// 开启/关闭 SO_KEEPALIVE
void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1: 0;
    ::setsockopt(sockfd_, SQL_SOCKET, SO_KEEPALIVE,
            &optval, sizeof(optval));
}