#include "Channel.h"
#include "EventLoop.h"
#include "Logger.h"
#include <sys/epoll.h>
#include <poll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

// EventLoop 包含： ChannelList Poller
Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop),
      fd_(fd),
      events_(0),
      revents_(0),
      index_(-1),
      tied_(false)
{
}

Channel::~Channel()
{
}

// Channel 的 tie 方法什么时候调用过?
void Channel::tie(const std::shared_ptr<void> & obj)
{
    tie_ = obj;
    tied_ = true;
}

/*
 * 当改变 channel 所表示  fd 的 events 事件后， 
 * update 负责在 poller 里面更改 fd 相应的事件 epoll_ctl
 * EventLoop 包含了 ChannelList 和  Poller
*/
void Channel::update()
{
    // 通过 ChannelList 所属的 EventLoop，调用  Poller 的相应的方法，
    // 注册 fd 的events 事件
    // add code
    // addedToLoop_ = true;
    loop_->updateChannel(this);
}

// 在 channel 所属的 EventLoop 中，把当前的 channel 删除掉
void Channel::remove()
{
    // add code
    //addedToLoop_ = false;
    loop_->removeChannel(this);
}

// fd 得到 poller 通知以后，处理事件的
void Channel::handleEvent(Timestamp receiveTime)
{
    if (tied_)
    {
        std::shared_ptr<void> guard = tie_.lock();

        if (guard)
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }
}

// 根据 poller 通知的 channel 发生的具体事件，由 channel 负责调用具体的回调操作
void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    LOG_INFO("channel handleEvent revents: %d\n", revents_);

    if ((revents_ & EPOLLHUP) && !(revents_ & POLLIN))
    {
        if (closeCallback_)
        {
            closeCallback_();
        }
    }

    if (revents_ & POLLERR)
    {
        if (errorCallback_)
        {
            errorCallback_();
        }
    }

    if (revents_ & (POLLIN | POLLPRI))
    {
        if (readCallback_)
        {
            readCallback_(receiveTime);
        }
    }

    if (revents_ & POLLOUT)
    {
        if (revents_ & POLLOUT)
        {
            if (writeCallback_)
            {
                writeCallback_();
            }
        }
    }
}