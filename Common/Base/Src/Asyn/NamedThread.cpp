//
// Created by LiZhen on 2021/8/24.
//

#include <Common/Asyn/NamedThread.h>

namespace Explosion {

    NamedThread::NamedThread(std::string_view str)
        : name(str)
    {
    }

    NamedThread::~NamedThread()
    {
    }

    void NamedThread::ExitThread()
    {
        exit.store(true);
        {
            std::lock_guard<std::mutex> lock(mtx);
        }
        Notify();
        thd->join();
    }

    void NamedThread::Notify()
    {
        cv.notify_one();
    }

}