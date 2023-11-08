#pragma once

#include "noncopyable.h"

class InetAddress;

// 封装 socket fd
class Socket : noncopyable
{
public:
    explicit Socket(int sockfd)
        : sockfd_(sockfd)
    {
    }

    ~Socket();

    int fd() const { return sockfd_; }
    void bindAddress(const InetAddress &localaddr);
    void listen();

    int accept(InetAddress *peeraddr);

    void shutdownWrite();

    // 开启/关闭 TCP_NODELAY (开启/关闭 Nagle 算法)
    void setTcpNoDelay(bool on);
    // 开启/关闭 SO_REUSEADDR
    void setReuseAddr(bool on);
    // 开启/关闭 SO_USEPORT
    void setReusePort(bool on);
    // 开启/关闭 SO_KEEPALIVE
    void setReuseAddr(bool on);
private:
    const int sockfd_;
};