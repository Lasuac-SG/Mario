//

#include "view/View.h"

GameView::GameView(GameViewModel* vm, EntityRenderer* renderer)
    : window_(sf::VideoMode({LOGIC_W, LOGIC_H}), "Mario Demo",
              sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize),
      vm_(vm), renderer_(renderer) {
    assets_ = new AssetManager();
}

GameView::~GameView() {
    delete assets_;
}

void GameView::run() {
    sf::Clock clock;

    while (window_.isOpen()) {
        float dt = clock.restart().asSeconds();
        dt = std::min(dt, 1.0f / 30.0f);  // 防止 delta 螺旋

        processEvents();
        processInput();
        vm_->tick(dt);
        render();
    }
}

void GameView::processEvents() {
    while (const auto ev = window_.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) {
            window_.close();
        } else if (const auto* kp = ev->getIf<sf::Event::KeyPressed>()) {
            auto code = static_cast<int>(kp->code);
            if (code >= 0 && code < 256)
                keys_[code] = true;
        } else if (const auto* kr = ev->getIf<sf::Event::KeyReleased>()) {
            auto code = static_cast<int>(kr->code);
            if (code >= 0 && code < 256)
                keys_[code] = false;
        }
    }
}

void GameView::processInput() {
    using sf::Keyboard::Key;
    bool left  = keys_[static_cast<int>(Key::Left)]  || keys_[static_cast<int>(Key::A)];
    bool right = keys_[static_cast<int>(Key::Right)] || keys_[static_cast<int>(Key::D)];
    bool jump  = keys_[static_cast<int>(Key::Space)] || keys_[static_cast<int>(Key::W)] || keys_[static_cast<int>(Key::Up)];
    bool restart = keys_[static_cast<int>(Key::R)];

    ICommandBase& command = vm_->getInputCmd();

    if (left && right) {
        InputActionParameter p;
        p.v_ = InputAction::STOP;
        command.exec(&p);
    } else if (left) {
        InputActionParameter p;
        p.v_ = InputAction::MOVE_LEFT;
        command.exec(&p);
    } else if (right) {
        InputActionParameter p;
        p.v_ = InputAction::MOVE_RIGHT;
        command.exec(&p);
    } else {
        InputActionParameter p;
        p.v_ = InputAction::STOP;
        command.exec(&p);
    }

    if (jump && !prevJump_) {
        InputActionParameter p;
        p.v_ = InputAction::JUMP;
        command.exec(&p);
    }
    prevJump_ = jump;

    if (restart && !prevRestart_) {
        InputActionParameter p;
        p.v_ = InputAction::RESTART;
        command.exec(&p);
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

    const GameModel& model = vm_->gameModel();
    const float halfViewW = static_cast<float>(LOGIC_W) * 0.5f;
    const float levelW = model.levelWidthPx();
    float cameraCenterX = model.playerX() + model.playerW() * 0.5f;

    if (levelW > static_cast<float>(LOGIC_W)) {
        cameraCenterX = std::clamp(cameraCenterX, halfViewW, levelW - halfViewW);
    } else {
        cameraCenterX = levelW * 0.5f;
    }

    sf::View view(sf::FloatRect({cameraCenterX - halfViewW, 0.f},
                                {static_cast<float>(LOGIC_W), static_cast<float>(LOGIC_H)}));
    sf::FloatRect viewport(
        {(winW - LOGIC_W * scale) / (2.0f * winW),
         (winH - LOGIC_H * scale) / (2.0f * winH)},
        {(LOGIC_W * scale) / winW,
         (LOGIC_H * scale) / winH}
    );
    view.setViewport(viewport);
    window_.setView(view);

    const auto& tiles = model.tiles();
    for (const auto& tile : tiles) {
        renderer_->drawTile(window_, tile, *assets_);
    }

    renderer_->drawPlayer(window_, model, *assets_);

    window_.display();
}
