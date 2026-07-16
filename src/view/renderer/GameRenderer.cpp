#include "GameRenderer.h"

namespace {

constexpr const char* kTileAtlas = "picture/mario/map/map.png";
constexpr const char* kEnemyFrameA = "picture/mario/enemy/monster_44.png";
constexpr const char* kEnemyFrameB = "picture/mario/enemy/monster_45.png";
const sf::IntRect kPlatformRect({0, 360}, {80, 40});

sf::Vector2f effectiveViewportSize(const sf::Vector2u& windowSize) {
    const float rawW = static_cast<float>(windowSize.x);
    const float rawH = static_cast<float>(windowSize.y);

    constexpr float kGrowthFactor = 0.2f;
    const float viewW = (rawW <= DefaultViewWidth) ? rawW : DefaultViewWidth + (rawW - DefaultViewWidth) * kGrowthFactor;
    const float viewH = (rawH <= DefaultViewHeight) ? rawH : DefaultViewHeight + (rawH - DefaultViewHeight) * kGrowthFactor;

    return {viewW, viewH};
}

const std::unordered_map<char, std::vector<std::string>>& glyphs() {
    static const std::unordered_map<char, std::vector<std::string>> kGlyphs = {
        {'A', {"01110", "10001", "10001", "11111", "10001", "10001", "10001"}},
        {'C', {"01111", "10000", "10000", "10000", "10000", "10000", "01111"}},
        {'D', {"11110", "10001", "10001", "10001", "10001", "10001", "11110"}},
        {'E', {"11111", "10000", "10000", "11110", "10000", "10000", "11111"}},
        {'G', {"01111", "10000", "10000", "10011", "10001", "10001", "01110"}},
        {'I', {"11111", "00100", "00100", "00100", "00100", "00100", "11111"}},
        {'L', {"10000", "10000", "10000", "10000", "10000", "10000", "11111"}},
        {'M', {"10001", "11011", "10101", "10101", "10001", "10001", "10001"}},
        {'N', {"10001", "11001", "10101", "10011", "10001", "10001", "10001"}},
        {'O', {"01110", "10001", "10001", "10001", "10001", "10001", "01110"}},
        {'R', {"11110", "10001", "10001", "11110", "10100", "10010", "10001"}},
        {'S', {"01111", "10000", "10000", "01110", "00001", "00001", "11110"}},
        {'T', {"11111", "00100", "00100", "00100", "00100", "00100", "00100"}},
        {'U', {"10001", "10001", "10001", "10001", "10001", "10001", "01110"}},
        {'V', {"10001", "10001", "10001", "10001", "10001", "01010", "00100"}},
        {'W', {"10001", "10001", "10001", "10101", "10101", "10101", "01010"}},
        {'Y', {"10001", "10001", "01010", "00100", "00100", "00100", "00100"}},
        {'0', {"01110", "10001", "10011", "10101", "11001", "10001", "01110"}},
        {'1', {"00100", "01100", "00100", "00100", "00100", "00100", "01110"}},
        {'2', {"01110", "10001", "00001", "00010", "00100", "01000", "11111"}},
        {'3', {"11110", "00001", "00001", "01110", "00001", "00001", "11110"}},
        {'4', {"00010", "00110", "01010", "10010", "11111", "00010", "00010"}},
        {'5', {"11111", "10000", "10000", "11110", "00001", "00001", "11110"}},
        {'6', {"01110", "10000", "10000", "11110", "10001", "10001", "01110"}},
        {'7', {"11111", "00001", "00010", "00100", "01000", "01000", "01000"}},
        {'8', {"01110", "10001", "10001", "01110", "10001", "10001", "01110"}},
        {'9', {"01110", "10001", "10001", "01111", "00001", "00001", "01110"}},
        {'-', {"00000", "00000", "00000", "11111", "00000", "00000", "00000"}},
        {' ', {"00000", "00000", "00000", "00000", "00000", "00000", "00000"}},
    };
    return kGlyphs;
}

const char* choosePlayerFrame(MarioState state, float runAnimationTime) {
    static constexpr const char* kRunFrames[] = {
        "picture/mario/player_1/red_9.png",
        "picture/mario/player_1/red_10.png",
        "picture/mario/player_1/red_11.png",
        "picture/mario/player_1/red_12.png",
    };

    switch (state) {
        case MarioState::RUNNING: {
            constexpr float kRunFrameDuration = 0.09f;
            const int frameCount = static_cast<int>(std::size(kRunFrames));
            int frame = static_cast<int>(runAnimationTime / kRunFrameDuration) % frameCount;
            return kRunFrames[frame];
        }
        case MarioState::JUMPING:
            return "picture/mario/player_1/red_6.png";
        case MarioState::FALLING:
            return "picture/mario/player_1/red_7.png";
        case MarioState::DEAD:
            return "picture/mario/player_1/red_27.png";
        case MarioState::IDLE:
        default:
            return "picture/mario/player_1/red_1.png";
    }
}

const char* chooseEnemyFrame(float enemyAnimationTime) {
    constexpr float kEnemyFrameDuration = 0.18f;
    const int frame = static_cast<int>(enemyAnimationTime / kEnemyFrameDuration) % 2;
    return frame == 0 ? kEnemyFrameA : kEnemyFrameB;
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
            if (right <= left) continue;

            brick.setPosition({left, y});
            brick.setSize({right - left, rowHeight});
            brick.setFillColor(((row + col) % 2 == 0) ? sf::Color(181, 101, 29) : sf::Color(160, 82, 45));
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

}  // namespace

GameRenderer::GameRenderer() { rect_.setOutlineThickness(0.0f); }

void GameRenderer::render(sf::RenderWindow& window, float dt) {
    enemyAnimationTime_ += std::max(0.0f, dt);
    if (won_ && *won_) {
        winAnimationTime_ += std::max(0.0f, dt);
    } else {
        winAnimationTime_ = 0.0f;
    }

    window.clear(sf::Color(107, 140, 255));

    const auto viewSize = effectiveViewportSize(window.getSize());
    sf::View view(sf::FloatRect({0.0f, 0.0f}, viewSize));
    view.setCenter({*cameraX_, *cameraY_});
    window.setView(view);

    for (const auto& tile : *tileInfos_) {
        drawTile(window, tile);
    }

    drawGoal(window);

    if (enemyInfos_) {
        for (const auto& enemy : *enemyInfos_) {
            drawEnemy(window, enemy);
        }
    }

    drawPlayer(window, dt);

    window.setView(window.getDefaultView());
    drawHud(window);
    if (won_ && *won_) {
        drawWinOverlay(window);
    }
    if (gameOver_ && *gameOver_) {
        drawGameOverOverlay(window);
    }
    window.display();
}

void GameRenderer::drawHud(sf::RenderWindow& window) {
    const std::string score = hudInfo_ ? std::to_string(hudInfo_->score) : "0";
    const std::string coins = hudInfo_ ? std::to_string(hudInfo_->coins) : "0";
    const std::string world = hudInfo_ ? hudInfo_->world : "1-1";
    const std::string time = hudInfo_ ? std::to_string(hudInfo_->timeRemaining) : "300";
    const std::string lives = hudInfo_ ? std::to_string(hudInfo_->lives) : "3";

    drawHudBlock(window, "SCORE", score, 90.0f);
    drawHudBlock(window, "COINS", coins, 250.0f);
    drawHudBlock(window, "WORLD", world, 410.0f);
    drawHudBlock(window, "TIME", time, 570.0f);
    drawHudBlock(window, "LIVES", lives, 720.0f);
}

void GameRenderer::drawHudBlock(sf::RenderWindow& window, const std::string& label, const std::string& value, float centerX) {
    constexpr float kLabelScale = 3.0f;
    constexpr float kValueScale = 3.0f;
    constexpr float kLabelY = 10.0f;
    constexpr float kValueY = 46.0f;

    const float labelX = centerX - measurePixelText(label, kLabelScale) * 0.5f;
    const float valueX = centerX - measurePixelText(value, kValueScale) * 0.5f;

    drawPixelText(window, label, labelX, kLabelY, kLabelScale);
    drawPixelText(window, value, valueX, kValueY, kValueScale);
}

void GameRenderer::drawPixelText(sf::RenderWindow& window, const std::string& text, float x, float y, float scale) {
    constexpr float kPixelGap = 1.0f;
    constexpr float kLetterGap = 2.0f;
    const auto& font = glyphs();

    sf::RectangleShape pixel({scale, scale});

    float cursorX = x;
    for (char ch : text) {
        auto it = font.find(ch);
        if (it == font.end()) {
            cursorX += 6.0f * scale;
            continue;
        }

        const auto& rows = it->second;
        for (std::size_t row = 0; row < rows.size(); ++row) {
            for (std::size_t col = 0; col < rows[row].size(); ++col) {
                if (rows[row][col] != '1') continue;

                const float px = cursorX + col * (scale + kPixelGap);
                const float py = y + row * (scale + kPixelGap);

                pixel.setPosition({px + scale * 0.35f, py + scale * 0.35f});
                pixel.setFillColor(sf::Color::Black);
                window.draw(pixel);

                pixel.setPosition({px, py});
                pixel.setFillColor(sf::Color::White);
                window.draw(pixel);
            }
        }

        cursorX += rows.front().size() * (scale + kPixelGap) + kLetterGap * scale;
    }
}

float GameRenderer::measurePixelText(const std::string& text, float scale) const {
    constexpr float kPixelGap = 1.0f;
    constexpr float kLetterGap = 2.0f;
    const auto& font = glyphs();

    float width = 0.0f;
    for (char ch : text) {
        auto it = font.find(ch);
        const std::size_t glyphWidth = (it == font.end()) ? 5 : it->second.front().size();
        width += glyphWidth * (scale + kPixelGap) + kLetterGap * scale;
    }

    return width > 0.0f ? width - kLetterGap * scale : 0.0f;
}

void GameRenderer::drawPlayer(sf::RenderWindow& window, float dt) {
    const auto& player = *playerInfo_;
    if (player.state == MarioState::RUNNING) {
        if (lastPlayerState_ != MarioState::RUNNING) runAnimationTime_ = 0.0f;
        runAnimationTime_ += dt;
    } else {
        runAnimationTime_ = 0.0f;
    }
    lastPlayerState_ = player.state;

    const sf::Texture& texture = assets_.load(choosePlayerFrame(player.state, runAnimationTime_));
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

    if (player.direction == Direction::LEFT && player.state != MarioState::DEAD) {
        sprite.setOrigin({static_cast<float>(texSize.x), 0.0f});
        sprite.setScale({-scaleX, scaleY});
        sprite.setPosition({player.x, player.y});
    }

    window.draw(sprite);
}

void GameRenderer::drawEnemy(sf::RenderWindow& window, const EnemyInfo& enemy) {
    const sf::Texture& texture = assets_.load(chooseEnemyFrame(enemyAnimationTime_));
    if (texture.getSize().x == 0 || texture.getSize().y == 0) {
        rect_.setTexture(nullptr, true);
        rect_.setSize({enemy.w, enemy.h});
        rect_.setPosition({enemy.x, enemy.y});
        rect_.setFillColor(sf::Color(146, 88, 44));
        window.draw(rect_);
        return;
    }

    sf::Sprite sprite(texture);
    const auto texSize = texture.getSize();
    const float texW = static_cast<float>(texSize.x);
    const float texH = static_cast<float>(texSize.y);
    const float uniformScale = std::min(enemy.w / texW, enemy.h / texH) * 1.25f;
    const float drawW = texW * uniformScale;
    const float drawH = texH * uniformScale;
    const float drawX = enemy.x + (enemy.w - drawW) * 0.5f;
    const float drawY = enemy.y + (enemy.h - drawH);

    sprite.setScale({uniformScale, uniformScale});
    sprite.setPosition({drawX, drawY});

    if (enemy.direction == Direction::LEFT) {
        sprite.setOrigin({texW, 0.0f});
        sprite.setScale({-uniformScale, uniformScale});
        sprite.setPosition({drawX + drawW, drawY});
    }

    window.draw(sprite);
}

void GameRenderer::drawGoal(sf::RenderWindow& window) {
    if (!goalInfo_ || goalInfo_->w <= 0.0f || goalInfo_->h <= 0.0f) return;

    const float poleX = goalInfo_->x + goalInfo_->w * 0.45f;
    const float poleY = goalInfo_->y;
    const float poleW = std::max(4.0f, goalInfo_->w * 0.18f);
    const float poleH = goalInfo_->h;

    sf::RectangleShape pole({poleW, poleH});
    pole.setPosition({poleX, poleY});
    pole.setFillColor(sf::Color(234, 236, 238));
    pole.setOutlineThickness(-1.0f);
    pole.setOutlineColor(sf::Color(130, 140, 150));
    window.draw(pole);

    sf::CircleShape topper(std::max(5.0f, goalInfo_->w * 0.22f));
    topper.setFillColor(sf::Color(245, 245, 210));
    topper.setOutlineThickness(-1.0f);
    topper.setOutlineColor(sf::Color(130, 140, 150));
    topper.setPosition({poleX - topper.getRadius() + poleW * 0.5f, poleY - topper.getRadius() * 1.2f});
    window.draw(topper);

    const float flagW = std::max(20.0f, goalInfo_->w * 1.4f);
    const float flagH = std::max(16.0f, goalInfo_->h * 0.12f);
    const float slide = (won_ && *won_) ? std::min(goalInfo_->h - flagH - 10.0f, winAnimationTime_ * 90.0f) : 0.0f;
    const float flagY = poleY + 8.0f + slide;

    sf::ConvexShape flag;
    flag.setPointCount(3);
    flag.setPoint(0, {poleX + poleW, flagY});
    flag.setPoint(1, {poleX + poleW + flagW, flagY + flagH * 0.5f});
    flag.setPoint(2, {poleX + poleW, flagY + flagH});
    flag.setFillColor(sf::Color(220, 52, 52));
    flag.setOutlineThickness(-1.0f);
    flag.setOutlineColor(sf::Color(120, 20, 20));
    window.draw(flag);

    sf::RectangleShape base({goalInfo_->w + 10.0f, 6.0f});
    base.setPosition({goalInfo_->x - 5.0f, goalInfo_->y + goalInfo_->h - 6.0f});
    base.setFillColor(sf::Color(160, 82, 45));
    window.draw(base);
}

 sf::FloatRect GameRenderer::restartButtonBounds(const sf::RenderWindow& window) const {
    const sf::View overlayView = window.getDefaultView();
    const sf::Vector2f size = overlayView.getSize();
    const sf::Vector2f center = overlayView.getCenter();
    const float left = center.x - size.x * 0.5f;
    const float top = center.y - size.y * 0.5f;
    const float buttonW = 280.0f;
    const float buttonH = 64.0f;
    const float buttonX = left + (size.x - buttonW) * 0.5f;
    const float buttonY = top + size.y * 0.66f;
    const float hoverScale = restartHovered_ ? 0.94f : 1.0f;
    const float drawW = buttonW * hoverScale;
    const float drawH = buttonH * hoverScale;
    const float drawX = buttonX + (buttonW - drawW) * 0.5f;
    const float drawY = buttonY + (buttonH - drawH) * 0.5f;
    return {{drawX, drawY}, {drawW, drawH}};
}

void GameRenderer::drawWinOverlay(sf::RenderWindow& window) {
    const sf::View overlayView = window.getView();
    const sf::Vector2f size = overlayView.getSize();
    const sf::Vector2f center = overlayView.getCenter();
    const float left = center.x - size.x * 0.5f;
    const float top = center.y - size.y * 0.5f;

    const float bandW = size.x;
    const float bandH = std::max(110.0f, size.y * 0.15f);
    const float bandY = top + size.y * 0.32f;

    sf::RectangleShape shade({bandW, bandH});
    shade.setPosition({left, bandY});
    shade.setFillColor(sf::Color(0, 0, 0, 185));
    window.draw(shade);

    sf::RectangleShape edge({bandW, 4.0f});
    edge.setFillColor(sf::Color(245, 245, 245, 210));
    edge.setPosition({left, bandY});
    window.draw(edge);
    edge.setPosition({left, bandY + bandH - 4.0f});
    window.draw(edge);

    constexpr float kScale = 4.0f;
    const float bob = std::sin(winAnimationTime_ * 3.5f) * 2.0f;

    const std::string topText = "YOU";
    const std::string bottomText = "WIN";
    const float topX = left + (bandW - measurePixelText(topText, kScale)) * 0.5f;
    const float bottomX = left + (bandW - measurePixelText(bottomText, kScale)) * 0.5f;

    drawPixelText(window, topText, topX, bandY + 18.0f + bob, kScale);
    drawPixelText(window, bottomText, bottomX, bandY + 62.0f + bob, kScale);
}

void GameRenderer::drawGameOverOverlay(sf::RenderWindow& window) {
    const sf::View overlayView = window.getDefaultView();
    const sf::Vector2f size = overlayView.getSize();
    const sf::Vector2f center = overlayView.getCenter();
    const float left = center.x - size.x * 0.5f;
    const float top = center.y - size.y * 0.5f;

    sf::RectangleShape shade(size);
    shade.setPosition({left, top});
    shade.setFillColor(sf::Color(0, 0, 0, 170));
    window.draw(shade);

    const float bandW = size.x;
    const float bandH = std::max(110.0f, size.y * 0.15f);
    const float bandY = top + size.y * 0.28f;

    sf::RectangleShape band({bandW, bandH});
    band.setPosition({left, bandY});
    band.setFillColor(sf::Color(0, 0, 0, 210));
    window.draw(band);

    sf::RectangleShape edge({bandW, 4.0f});
    edge.setFillColor(sf::Color(245, 245, 245, 220));
    edge.setPosition({left, bandY});
    window.draw(edge);
    edge.setPosition({left, bandY + bandH - 4.0f});
    window.draw(edge);

    constexpr float kScale = 4.0f;
    const std::string topText = "GAME";
    const std::string bottomText = "OVER";
    const float topX = left + (bandW - measurePixelText(topText, kScale)) * 0.5f;
    const float bottomX = left + (bandW - measurePixelText(bottomText, kScale)) * 0.5f;
    drawPixelText(window, topText, topX, bandY + 18.0f, kScale);
    drawPixelText(window, bottomText, bottomX, bandY + 62.0f, kScale);

    const sf::FloatRect button = restartButtonBounds(window);
    sf::RectangleShape buttonShape(button.size);
    buttonShape.setPosition(button.position);
    buttonShape.setFillColor(restartHovered_ ? sf::Color(250, 220, 120) : sf::Color(240, 200, 90));
    buttonShape.setOutlineThickness(-2.0f);
    buttonShape.setOutlineColor(restartHovered_ ? sf::Color(150, 90, 25) : sf::Color(110, 70, 20));
    window.draw(buttonShape);

    const std::string restartText = "RESTART";
    constexpr float kButtonScale = 2.6f;
    const float textX = button.position.x + (button.size.x - measurePixelText(restartText, kButtonScale)) * 0.5f;
    const float textY = button.position.y + (button.size.y - (7.0f * (kButtonScale + 1.0f))) * 0.5f - 1.0f;
    drawPixelText(window, restartText, textX, textY, kButtonScale);
}

void GameRenderer::drawTile(sf::RenderWindow& window, const TileInfo& tile) {
    if (tile.type == TileType::GROUND) {
        drawBrickGround(window, tile);
        return;
    }
    if (tile.type == TileType::PIPE) {
        drawPipe(window, tile);
        return;
    }

    const sf::Texture& atlas = assets_.load(kTileAtlas);
    if (atlas.getSize().x == 0 || atlas.getSize().y == 0) {
        drawFallbackTile(window, tile);
        return;
    }

    rect_.setPosition({tile.x, tile.y});
    rect_.setSize({tile.w, tile.h});
    rect_.setTexture(&atlas, true);
    rect_.setFillColor(sf::Color::White);

    if (tile.type == TileType::PLATFORM) {
        rect_.setTextureRect(sf::IntRect({kPlatformRect.position.x, kPlatformRect.position.y},
                                         {static_cast<int>(tile.w), static_cast<int>(tile.h)}));
    } else {
        drawFallbackTile(window, tile);
        return;
    }

    window.draw(rect_);
}

void GameRenderer::drawFallbackTile(sf::RenderWindow& window, const TileInfo& tile) {
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
