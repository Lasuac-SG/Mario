//

#ifndef MARIO_GAMECOMMANDS_H
#define MARIO_GAMECOMMANDS_H

#include "common/ICommand.h"
#include "common/EventId.h"
#include "model/GameModel.h"

/// 携带 InputAction 的参数，View 创建后传给 InputCommand::exec
class InputActionParameter : public ICommandParameter {
public:
    InputAction action;
    explicit InputActionParameter(InputAction a) : action(a) {}
};

/// 输入命令 —— 根据 InputActionParameter 分派到 GameModel 的对应方法
class InputCommand : public ICommandBase {
    GameModel* model_;
public:
    explicit InputCommand(GameModel* m) : model_(m) {}
    int exec(ICommandParameter* p) override {
        auto* param = dynamic_cast<InputActionParameter*>(p);
        if (!param) return -1;

        switch (param->action) {
        case InputAction::MOVE_LEFT:  model_->setMoveLeft(true);  break;
        case InputAction::MOVE_RIGHT: model_->setMoveRight(true); break;
        case InputAction::STOP:       model_->setMoveStop();      break;
        case InputAction::JUMP:       model_->jump();             break;
        case InputAction::RESTART:    model_->reset();            break;
        }
        return 0;
    }
};

#endif  // MARIO_GAMECOMMANDS_H
