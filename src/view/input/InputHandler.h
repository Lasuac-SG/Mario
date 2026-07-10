//

#ifndef MARIO_INPUTHANDLER_H
#define MARIO_INPUTHANDLER_H

#include "common/ICommand.h"

class InputHandler {
   public:
    // 命令绑定（等价 BattleBoard::set_move_red_command）
    void setActionCommand(ICommandBase* cmd) { actionCmd_ = cmd; }

    // 每帧调用：轮询 SFML 事件 → 更新内部 key state
    void pollEvents(sf::RenderWindow& window);

    // 每帧调用：读取 key state → 派发 InputAction 命令
    void dispatchInput();

   private:
    ICommandBase* actionCmd_ = nullptr;
    bool keys_[256]{false};
    bool prevJump_ = false;
    bool prevRestart_ = false;
};

#endif  // MARIO_INPUTHANDLER_H
