//

#ifndef MARIO_SIMPLERENDERER_H
#define MARIO_SIMPLERENDERER_H

#include "view/EntityRenderer.h"

/// 使用像素素材渲染玩家和地图碰撞块
class SimpleRenderer : public EntityRenderer {
public:
    SimpleRenderer();

    void draw(sf::RenderWindow& window,
              const PlayerInfo& player,
              AssetManager& assets) override;

    void draw(sf::RenderWindow& window,
              const TileInfo& tile,
              AssetManager& assets) override;
private:
    void drawFallbackTile(sf::RenderWindow& window, const TileInfo& tile);

    float runAnimationTime_ = 0.0f;
    MarioState lastPlayerState_ = MarioState::IDLE;
    sf::Clock animationClock_;
    sf::RectangleShape rect_;
};

#endif  // MARIO_SIMPLERENDERER_H
