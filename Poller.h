#pragma once

#include "noncopyable.h"
#include "Timestamp.h"
#include <vector>
#include <unordered_map>

class Channel;
class EventLoop;

// muduo 库多路事件分发器的核心IO利用模块
class Poller: noncopyable
{
public:
    using ChannelList = std::vector<Channel *>;

    Poller(EventLoop * looop);
    virtual ~Poller();

    // 给所有的 I/O 复用保留统一的接口
    virtual Timestamp poll(int timeoutMs, ChannelList * activeChannels) = 0;
    virtual void updateChannel(Channel * channel) = 0;
    virtual void removeChannel(Channel * channel) = 0;
    // 判断 参数 channel 是否在当前 Polle 当中
    virtual bool hasChannel(Channel * channel) const;

    // EventLoop 可以通过该接口获取默认的 IO 复用的具体实现
    static Poller * newDefaultPoller(EventLoop * loop);
protected:
    using ChannelMap = std::unordered_map<int, Channel *>;
    ChannelMap channels_;
private:
    EventLoop * ownerLoop_;   // 定义 Poller 所属的事件循环 EventLoop;
};