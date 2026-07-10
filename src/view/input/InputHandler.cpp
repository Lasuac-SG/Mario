//

#include "InputHandler.h"

#include "common/Type.h"

void InputHandler::pollEvents(sf::RenderWindow& window) {
    while (const auto ev = window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) {
            window.close();
        } else if (const auto* kp = ev->getIf<sf::Event::KeyPressed>()) {
            auto code = static_cast<int>(kp->code);
            if (code >= 0 && code < 256) keys_[code] = true;
        } else if (const auto* kr = ev->getIf<sf::Event::KeyReleased>()) {
            auto code = static_cast<int>(kr->code);
            if (code >= 0 && code < 256) keys_[code] = false;
        }
    }
}

void InputHandler::dispatchInput() {
    using sf::Keyboard::Key;
    bool left = keys_[static_cast<int>(Key::Left)] || keys_[static_cast<int>(Key::A)];
    bool right = keys_[static_cast<int>(Key::Right)] || keys_[static_cast<int>(Key::D)];
    bool jump =
        keys_[static_cast<int>(Key::Space)] || keys_[static_cast<int>(Key::W)] || keys_[static_cast<int>(Key::Up)];
    bool restart = keys_[static_cast<int>(Key::R)];

    // 左右冲突 → 停止
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

    // 跳跃边沿触发
    if (jump && !prevJump_) {
        InputActionParameter p(InputAction::JUMP);
        actionCmd_->exec(&p);
    }
    prevJump_ = jump;

    // 重启边沿触发
    if (restart && !prevRestart_) {
        InputActionParameter p(InputAction::RESTART);
        actionCmd_->exec(&p);
    }
    prevRestart_ = restart;
}
