//

#ifndef MARIO_SIMPLERENDERER_H
#define MARIO_SIMPLERENDERER_H

#include "view/EntityRenderer.h"

/// 使用像素素材渲染玩家和地图碰撞块
class SimpleRenderer : public EntityRenderer {
public:
    SimpleRenderer();

    void drawPlayer(sf::RenderWindow& window,
                    const GameModel& model,
                    AssetManager& assets) override;

    void drawTile(sf::RenderWindow& window,
                  const Tile& tile,
                  AssetManager& assets) override;

private:
    void drawFallbackTile(sf::RenderWindow& window, const Tile& tile);

    float runAnimationTime_ = 0.0f;
    MarioState lastPlayerState_ = MarioState::IDLE;
    sf::Clock animationClock_;
    sf::RectangleShape rect_;
};

#endif  // MARIO_SIMPLERENDERER_H
