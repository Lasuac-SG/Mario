#ifndef MARIO_INPUTHANDLER_H
#define MARIO_INPUTHANDLER_H

#include "common/ICommand.h"

class InputHandler {
   public:
    void setActionCommand(ICommandBase* cmd) { actionCmd_ = cmd; }

    // 每帧实时轮询按键状态；窗口事件由 View 统一处理。
    void pollEvents(sf::RenderWindow& window);

    void dispatchInput();
    void triggerRestart();

   private:
    ICommandBase* actionCmd_ = nullptr;
    bool keys_[256]{false};
    bool prevJump_ = false;
    bool prevRestart_ = false;
};

#endif  // MARIO_INPUTHANDLER_H
