#include "Poller.h"
#include "Channel.h"

Poller::Poller(EventLoop *loop)
    :ownerLoop_(loop)
{
}

Poller::~Poller() = default;

// 判断 参数 channel 是否在当前 Polle 当中
bool Poller::hasChannel(Channel *channel) const
{
    ChannelMap::const_iterator it = channels_.find(channel->fd());
    return it != channels_.end() && it->second == channel;
}

