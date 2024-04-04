#pragma once
#include <queue>

namespace Sandbox
{
    struct MouseButtonEvent
    {
        int    button;
        int    action;
        int    mods;
        double timestamp;  // 时间戳，记录事件发生的具体时间
        double xPos;  // 鼠标指针的X位置
        double yPos;  // 鼠标指针的Y位置
    };

    class InputQueue
    {
    public:
        std::queue<MouseButtonEvent> mouseButtonQueue;
    };
}
