#include "InputHandler.h"

#include "common/Type.h"

void InputHandler::pollEvents(sf::RenderWindow& window) {
    (void)window;

    using sf::Keyboard::Key;
    keys_[static_cast<int>(Key::Left)] = sf::Keyboard::isKeyPressed(Key::Left);
    keys_[static_cast<int>(Key::A)] = sf::Keyboard::isKeyPressed(Key::A);
    keys_[static_cast<int>(Key::Right)] = sf::Keyboard::isKeyPressed(Key::Right);
    keys_[static_cast<int>(Key::D)] = sf::Keyboard::isKeyPressed(Key::D);
    keys_[static_cast<int>(Key::Space)] = sf::Keyboard::isKeyPressed(Key::Space);
    keys_[static_cast<int>(Key::W)] = sf::Keyboard::isKeyPressed(Key::W);
    keys_[static_cast<int>(Key::Up)] = sf::Keyboard::isKeyPressed(Key::Up);
    keys_[static_cast<int>(Key::R)] = sf::Keyboard::isKeyPressed(Key::R);
}

void InputHandler::dispatchInput() {
    using sf::Keyboard::Key;
    bool left = keys_[static_cast<int>(Key::Left)] || keys_[static_cast<int>(Key::A)];
    bool right = keys_[static_cast<int>(Key::Right)] || keys_[static_cast<int>(Key::D)];
    bool jump =
        keys_[static_cast<int>(Key::Space)] || keys_[static_cast<int>(Key::W)] || keys_[static_cast<int>(Key::Up)];
    bool restart = keys_[static_cast<int>(Key::R)];

    if (left && right) {
        InputActionParameter p(InputAction::STOP);
        actionCmd_->exec(&p);
    } else if (left) {
        InputActionParameter p(InputAction::MOVE_LEFT);
        actionCmd_->exec(&p);
    } else if (right) {
        InputActionParameter p(InputAction::MOVE_RIGHT);
        actionCmd_->exec(&p);
    } else {
        InputActionParameter p(InputAction::STOP);
        actionCmd_->exec(&p);
    }

    if (jump && !prevJump_) {
        InputActionParameter p(InputAction::JUMP);
        actionCmd_->exec(&p);
    }
    prevJump_ = jump;

    if (restart && !prevRestart_) {
        triggerRestart();
    }
    prevRestart_ = restart;
}

void InputHandler::triggerRestart() {
    if (!actionCmd_) return;
    InputActionParameter p(InputAction::RESTART);
    actionCmd_->exec(&p);
}
