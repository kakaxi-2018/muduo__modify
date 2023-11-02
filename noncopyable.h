// 防止头文件被多次包含，比较轻便
#pragma once

/*
 ** noncopyable 被继承以后，派生类对象可以正常地构造和析构，但是派生类对象无法拷贝构造
 ** 和赋值操作
*/
class noncopyable
{
public:
    noncopyable(const noncopyable &) = delete;
    noncopyable & operator=(const noncopyable &) = delete;
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};