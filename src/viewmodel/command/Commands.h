//

#ifndef MARIO_COMMANDS_H
#define MARIO_COMMANDS_H

#include "common/EventId.h"
#include "common/Type.h"
#include "model/GameModel.h"

typedef TypeParameter<InputAction> InputActionParameter;

class InputCommand : public ICommandBase {
   public:
    explicit InputCommand(GameModel* m) : model_(m) {}
    int exec(ICommandParameter* p) override;
   private:
    GameModel* model_;
};

#endif  // MARIO_COMMANDS_H
