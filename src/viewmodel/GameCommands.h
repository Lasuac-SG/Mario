//

#ifndef MARIO_GAMECOMMANDS_H
#define MARIO_GAMECOMMANDS_H

#include "common/EventId.h"
#include "common/Type.h"
#include "model/GameModel.h"

typedef TypeParameter<InputAction> InputActionParameter;

/// 输入命令 —— 根据 InputActionParameter 分派到 GameModel 的对应方法
class InputCommand : public ICommandBase {
    GameModel* model_;

   public:
    explicit InputCommand(GameModel* m) : model_(m) {}
    int exec(ICommandParameter* p) override {
        auto* param = dynamic_cast<InputActionParameter*>(p);
        if (!param) return -1;

        switch (param->v_) {
            case InputAction::MOVE_LEFT:
                model_->setMoveLeft(true);
                break;
            case InputAction::MOVE_RIGHT:
                model_->setMoveRight(true);
                break;
            case InputAction::STOP:
                model_->setMoveStop();
                break;
            case InputAction::JUMP:
                model_->jump();
                break;
            case InputAction::RESTART:
                model_->reset();
                break;
        }
        return 0;
    }
};

#endif  // MARIO_GAMECOMMANDS_H
