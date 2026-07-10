//

#include "view/SimpleRenderer.h"
#include "view/AssetManager.h"

namespace {
constexpr const char* kTileAtlas = "picture/mario/地图/map.png";
const sf::IntRect kPlatformRect({0, 360}, {80, 40});

const char* choosePlayerFrame(MarioState state, float runAnimationTime) {
    static constexpr const char* kRunFrames[] = {
        "picture/mario/人物/player_1/red_9.png",
        "picture/mario/人物/player_1/red_10.png",
        "picture/mario/人物/player_1/red_11.png",
        "picture/mario/人物/player_1/red_12.png",
    };

    switch (state) {
    case MarioState::RUNNING: {
        constexpr float kRunFrameDuration = 0.09f;
        const int frameCount = static_cast<int>(std::size(kRunFrames));
        int frame = static_cast<int>(runAnimationTime / kRunFrameDuration) % frameCount;
        return kRunFrames[frame];
    }
    case MarioState::JUMPING:
        return "picture/mario/人物/player_1/red_6.png";
    case MarioState::FALLING:
        return "picture/mario/人物/player_1/red_7.png";
    case MarioState::IDLE:
    default:
        return "picture/mario/人物/player_1/red_1.png";
    }
}

void drawBrickGround(sf::RenderWindow& window, const TileInfo& tile) {
    constexpr float kBrickW = 32.0f;
    constexpr float kBrickH = 16.0f;

    sf::RectangleShape brick;
    brick.setOutlineThickness(-1.0f);
    brick.setOutlineColor(sf::Color(92, 52, 24));

    const int rows = static_cast<int>(std::ceil(tile.h / kBrickH));
    const int cols = static_cast<int>(std::ceil(tile.w / kBrickW));

    for (int row = 0; row < rows; ++row) {
        const float y = tile.y + row * kBrickH;
        const float rowHeight = std::min(kBrickH, tile.y + tile.h - y);
        const float offset = (row % 2 == 0) ? 0.0f : kBrickW * 0.5f;

        for (int col = -1; col < cols; ++col) {
            const float x = tile.x + offset + col * kBrickW;
            const float left = std::max(x, tile.x);
            const float right = std::min(x + kBrickW, tile.x + tile.w);
            if (right <= left) {
                continue;
            }

            brick.setPosition({left, y});
            brick.setSize({right - left, rowHeight});
            brick.setFillColor(((row + col) % 2 == 0)
                               ? sf::Color(181, 101, 29)
                               : sf::Color(160, 82, 45));
            window.draw(brick);
        }
    }
}

void drawPipe(sf::RenderWindow& window, const TileInfo& tile) {
    const float headH = std::min(16.0f, tile.h * 0.4f);
    const float headW = tile.w + 12.0f;
    const float bodyW = std::max(18.0f, tile.w - 8.0f);
    const float bodyX = tile.x + (tile.w - bodyW) * 0.5f;
    const float headX = tile.x - (headW - tile.w) * 0.5f;

    sf::RectangleShape part;
    part.setOutlineThickness(-1.0f);
    part.setOutlineColor(sf::Color(20, 90, 20));

    part.setPosition({bodyX, tile.y + headH});
    part.setSize({bodyW, std::max(0.0f, tile.h - headH)});
    part.setFillColor(sf::Color(40, 180, 40));
    window.draw(part);

    sf::RectangleShape bodyShadow;
    bodyShadow.setPosition({bodyX + bodyW * 0.65f, tile.y + headH + 1.0f});
    bodyShadow.setSize({bodyW * 0.18f, std::max(0.0f, tile.h - headH - 1.0f)});
    bodyShadow.setFillColor(sf::Color(20, 130, 20));
    window.draw(bodyShadow);

    part.setPosition({headX, tile.y});
    part.setSize({headW, headH});
    part.setFillColor(sf::Color(55, 205, 55));
    window.draw(part);

    sf::RectangleShape lipShadow;
    lipShadow.setPosition({headX + headW * 0.72f, tile.y + 1.0f});
    lipShadow.setSize({headW * 0.14f, std::max(0.0f, headH - 2.0f)});
    lipShadow.setFillColor(sf::Color(20, 130, 20));
    window.draw(lipShadow);

    sf::RectangleShape highlight;
    highlight.setPosition({headX + 3.0f, tile.y + 3.0f});
    highlight.setSize({std::max(4.0f, headW * 0.16f), std::max(4.0f, headH - 6.0f)});
    highlight.setFillColor(sf::Color(120, 245, 120));
    window.draw(highlight);
}
}

SimpleRenderer::SimpleRenderer() {
    rect_.setOutlineThickness(0.0f);
}

void SimpleRenderer::draw(sf::RenderWindow& window,
                          const PlayerInfo& player,
                          AssetManager& assets) {
    const float deltaSeconds = animationClock_.restart().asSeconds();
    if (player.state == MarioState::RUNNING) {
        if (lastPlayerState_ != MarioState::RUNNING) {
            runAnimationTime_ = 0.0f;
        }
        runAnimationTime_ += deltaSeconds;
    } else {
        runAnimationTime_ = 0.0f;
    }
    lastPlayerState_ = player.state;

    const sf::Texture& texture = assets.load(choosePlayerFrame(player.state, runAnimationTime_));
    if (texture.getSize().x == 0 || texture.getSize().y == 0) {
        rect_.setTexture(nullptr, true);
        rect_.setSize(sf::Vector2f(player.width, player.height));
        rect_.setPosition(sf::Vector2f(player.x, player.y));
        rect_.setFillColor(sf::Color::Red);
        window.draw(rect_);
        return;
    }

    sf::Sprite sprite(texture);
    const auto texSize = texture.getSize();
    const float scaleX = player.width / static_cast<float>(texSize.x);
    const float scaleY = player.height / static_cast<float>(texSize.y);
    sprite.setScale({scaleX, scaleY});
    sprite.setPosition({player.x, player.y});

    if (player.direction == Direction::LEFT) {
        sprite.setOrigin({static_cast<float>(texSize.x), 0.0f});
        sprite.setScale({-scaleX, scaleY});
        sprite.setPosition({player.x, player.y});
    }

    window.draw(sprite);
}

void SimpleRenderer::draw(sf::RenderWindow& window,
                          const TileInfo& tile,
                          AssetManager& assets) {
    if (tile.type == TileType::GROUND) {
        drawBrickGround(window, tile);
        return;
    }

    if (tile.type == TileType::PIPE) {
        drawPipe(window, tile);
        return;
    }

    const sf::Texture& atlas = assets.load(kTileAtlas);
    if (atlas.getSize().x == 0 || atlas.getSize().y == 0) {
        drawFallbackTile(window, tile);
        return;
    }

    rect_.setPosition({tile.x, tile.y});
    rect_.setSize({tile.w, tile.h});
    rect_.setTexture(&atlas, true);
    rect_.setFillColor(sf::Color::White);

    switch (tile.type) {
    case TileType::PLATFORM: {
        rect_.setTextureRect(sf::IntRect({kPlatformRect.position.x, kPlatformRect.position.y},
                                         {static_cast<int>(tile.w), static_cast<int>(tile.h)}));
        break;
    }
    default:
        drawFallbackTile(window, tile);
        return;
    }

    window.draw(rect_);
}

void SimpleRenderer::drawFallbackTile(sf::RenderWindow& window, const TileInfo& tile) {
    rect_.setTexture(nullptr, true);
    rect_.setSize({tile.w, tile.h});
    rect_.setPosition({tile.x, tile.y});

    switch (tile.type) {
    case TileType::GROUND:
        rect_.setFillColor(sf::Color(139, 90, 43));
        break;
    case TileType::PLATFORM:
        rect_.setFillColor(sf::Color(128, 128, 128));
        break;
    case TileType::PIPE:
        rect_.setFillColor(sf::Color(50, 205, 50));
        break;
    default:
        rect_.setFillColor(sf::Color::Magenta);
        break;
    }

    window.draw(rect_);
}
