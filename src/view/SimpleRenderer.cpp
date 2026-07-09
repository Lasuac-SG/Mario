//

#include "view/SimpleRenderer.h"
#include "view/AssetManager.h"

SimpleRenderer::SimpleRenderer() {
    rect_.setOutlineThickness(0.0f);
}

void SimpleRenderer::drawPlayer(sf::RenderWindow& window,
                                const GameModel& model,
                                AssetManager&) {
    rect_.setSize(sf::Vector2f(model.playerW(), model.playerH()));
    rect_.setPosition(sf::Vector2f(model.playerX(), model.playerY()));
    rect_.setFillColor(sf::Color::Red);
    window.draw(rect_);

    // 方向指示点（面向方向画一个小白块）
    const float dotSize = 8.0f;
    float dx = (model.playerFacing() == Direction::RIGHT) ? 2.0f : model.playerW() - 2.0f - dotSize;
    rect_.setSize(sf::Vector2f(dotSize, dotSize));
    rect_.setPosition(sf::Vector2f(model.playerX() + dx, model.playerY() + model.playerH() / 2.0f));
    rect_.setFillColor(sf::Color::White);
    window.draw(rect_);
}

void SimpleRenderer::drawTile(sf::RenderWindow& window,
                              const Tile& tile,
                              AssetManager&) {
    rect_.setSize(sf::Vector2f(tile.w, tile.h));
    rect_.setPosition(sf::Vector2f(tile.x, tile.y));

    switch (tile.type) {
    case TileType::GROUND:
        rect_.setFillColor(sf::Color(139, 90, 43));     // 棕色
        break;
    case TileType::PLATFORM:
        rect_.setFillColor(sf::Color(128, 128, 128));   // 灰色
        break;
    case TileType::PIPE:
        rect_.setFillColor(sf::Color(50, 205, 50));     // 绿色
        break;
    default:
        rect_.setFillColor(sf::Color::Magenta);
        break;
    }

    window.draw(rect_);
}