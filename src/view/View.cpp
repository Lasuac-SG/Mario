//

#include "view/View.h"

#include "common/Type.h"
#include "view/SimpleRenderer.h"

GameView::GameView()
    : window_(sf::VideoMode({LOGIC_W, LOGIC_H}), "Mario Demo",
              sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize) {
    renderer_ = std::make_unique<SimpleRenderer>();
    assets_ = new AssetManager();
}

GameView::~GameView() { delete assets_; }

void GameView::run() {
    sf::Clock clock;

    while (window_.isOpen()) {
        float dt = clock.restart().asSeconds();
        dt = std::min(dt, 1.0f / 30.0f);  // 防止 delta 螺旋
        processEvents();
        processInput();
        updateFrameFunction_(dt);
        render();
    }
}

void GameView::processEvents() {
    while (const auto ev = window_.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) {
            window_.close();
        } else if (const auto* kp = ev->getIf<sf::Event::KeyPressed>()) {
            auto code = static_cast<int>(kp->code);
            if (code >= 0 && code < 256) keys_[code] = true;
        } else if (const auto* kr = ev->getIf<sf::Event::KeyReleased>()) {
            auto code = static_cast<int>(kr->code);
            if (code >= 0 && code < 256) keys_[code] = false;
        }
    }
}

void GameView::processInput() {
    using sf::Keyboard::Key;
    bool left = keys_[static_cast<int>(Key::Left)] || keys_[static_cast<int>(Key::A)];
    bool right = keys_[static_cast<int>(Key::Right)] || keys_[static_cast<int>(Key::D)];
    bool jump =
        keys_[static_cast<int>(Key::Space)] || keys_[static_cast<int>(Key::W)] || keys_[static_cast<int>(Key::Up)];
    bool restart = keys_[static_cast<int>(Key::R)];

    // 左右冲突 → 停止
    if (left && right) {
        InputActionParameter p(InputAction::STOP);
        actionParam_->exec(&p);
    } else if (left) {
        InputActionParameter p(InputAction::MOVE_LEFT);
        actionParam_->exec(&p);
    } else if (right) {
        InputActionParameter p(InputAction::MOVE_RIGHT);
        actionParam_->exec(&p);
    } else {
        InputActionParameter p(InputAction::STOP);
        actionParam_->exec(&p);
    }

    // 跳跃边沿触发
    if (jump && !prevJump_) {
        InputActionParameter p(InputAction::JUMP);
        actionParam_->exec(&p);
    }
    prevJump_ = jump;

    // 重启边沿触发
    if (restart && !prevRestart_) {
        InputActionParameter p(InputAction::RESTART);
        actionParam_->exec(&p);
    }
    prevRestart_ = restart;
}

void GameView::render() {
    window_.clear(sf::Color(107, 140, 255));  // 天空蓝

    // 设置逻辑坐标视口（保持等比缩放）
    float winW = static_cast<float>(window_.getSize().x);
    float winH = static_cast<float>(window_.getSize().y);
    float scaleX = winW / LOGIC_W;
    float scaleY = winH / LOGIC_H;
    float scale = std::min(scaleX, scaleY);

    sf::View view(sf::FloatRect({0.f, 0.f}, {static_cast<float>(LOGIC_W), static_cast<float>(LOGIC_H)}));
    view.setCenter({*cameraX_, *cameraY_});
    sf::FloatRect viewport({(winW - LOGIC_W * scale) / (2.0f * winW), (winH - LOGIC_H * scale) / (2.0f * winH)},
                           {(LOGIC_W * scale) / winW, (LOGIC_H * scale) / winH});
    view.setViewport(viewport);
    window_.setView(view);

    // 渲染 Tile —— 通过 ViewModel 数据绑定
    for (const auto& tile : *tileInfos_) {
        renderer_->draw(window_, tile, *assets_);
    }

    // 渲染玩家 —— 通过 ViewModel 数据绑定
    renderer_->draw(window_, *playerInfo_, *assets_);

    window_.display();
}