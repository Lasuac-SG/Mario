//

#include "viewmodel/command/Commands.h"

int InputCommand::exec(ICommandParameter* p) {
    auto* param = dynamic_cast<InputActionParameter*>(p);
    if (!param) return -1;

    switch (param->v_) {
        case InputAction::MOVE_LEFT:  model_->setMoveLeft(true);  break;
        case InputAction::MOVE_RIGHT: model_->setMoveRight(true); break;
        case InputAction::STOP:       model_->setMoveStop();      break;
        case InputAction::JUMP:       model_->jump();             break;
        case InputAction::RESTART:    model_->reset();            break;
    }
    return 0;
}