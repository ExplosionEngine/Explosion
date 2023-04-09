//
// Created by johnk on 2022/8/3.
//

#pragma once

namespace Runtime{
    class InputManager {
    public:
        ~InputManager();

        void MouseMove(double x, double y);
        // TODO

    private:
        friend class Engine;

        InputManager();
    };
}
