#include "EPollPoller.h"
#include "Logger.h"
#include "Channel.h"
#include<sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

// channel 未添加到  poller 中
const int kNew = -1;  // channel 的成员 index_ = -1
// channel 已添加到  poller 中
const int kAdded = 1;
// channel 从 poller 中删除
const int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop * loop)
    : Poller(loop),
      epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
      events_(kInitEventListSize)  // vector<epoll_event>
{
    if (epollfd_ < 0)
    {
        LOG_FATAL("epoll_create error: %d\n", errno);
    }
}

EPollPoller::~EPollPoller()
{
    ::close(epollfd_);
}

// 重写基类 Poller 的抽象方法
Timestamp EPollPoller::poll(int timeoutMs, ChannelList *activeChannels)
{
    // 实际上应该用  LOG_DEBUG 输出日志更为合理
    LOG_INFO("func=>%s fd total count: %d\n", __FUNCTION__, channels_.size());
    int numEvents = ::epoll_wait(epollfd_, 
                                 &*events_.begin(),
                                 static_cast<int>(events_.size()),
                                 timeoutMs);
    int savedErrno = errno;
    Timestamp now(Timestamp::now());

    if (numEvents > 0)
    {
        LOG_INFO("%d events happened\n", numEvents);
        fillActiveChannels(numEvents, activeChannels);

        if (static_cast<size_t>(numEvents) == events_.size())
        {
            events_.resize(events_.size() * 2);
        }
    }
    else if (0 == numEvents)
    {
        LOG_INFO("%s timeout, nothing happened", __FUNCTION__);
    }
    else
    {
        // 错误发生
        if (savedErrno != EINTR)
        {
            errno = savedErrno;
            LOG_FATAL("EPollPoller::poll() error");
        }
    }

    return now;
}


// channel update remove => EventLoop upateChannel removeChannel
/* EventLoop
 * ChannelList   Poller 
 *               ChannelMap<fd, channel *> epollfd
*/
void EPollPoller::updateChannel(Channel *channel)
{
    const int index = channel->index();
    LOG_INFO("func = %s =>fd = %d events= %d index = %d\n", __FUNCTION__, channel->fd(), channel->events(), channel->index());

    if (index == kNew || index == kDeleted)
    {
        int fd = channel->fd();

        if (index == kNew)
        {
            channels_[fd] = channel;
        }

        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        //  channel 已经在 poller 上注册过了
        int fd = channel->fd();

        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

// 从 poller 中删除 channel
void EPollPoller::removeChannel(Channel *channel)
{
    int fd = channel->fd();
    LOG_INFO("func = %s =>fd = %d\n", __FUNCTION__, fd);

    int index = channel->index();
    channels_.erase(fd);

    if (index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }

    channel->set_index(kNew);
}

// 填写活跃的连接
void EPollPoller::fillActiveChannels(int numEvents, ChannelList * activeChannels) const
{
    for (int i = 0; i < numEvents; ++i)
    {
        Channel * channel = static_cast<Channel *>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);  // EventLoop 就拿到了它的 poller 给它返回的所有发生事件的 channel 列表
    }
}

// 更新 channel 通道，epoll_modify
void EPollPoller::update(int operation, Channel * channel)
{
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    int fd = channel->fd();

    event.events = channel->events();
    event.data.fd = fd;
    event.data.ptr = channel;

    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {
        if (operation == EPOLL_CTL_DEL)
        {
            LOG_ERROR("epoll_ctl del error: %d", errno);
        }
        else
        {
            LOG_FATAL("epoll_ctl add/mod error: %d", errno);
        }
    }
}