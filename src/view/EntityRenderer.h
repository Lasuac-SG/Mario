//

#ifndef MARIO_ENTITYRENDERER_H
#define MARIO_ENTITYRENDERER_H

#include "model/GameModel.h"

class AssetManager;

/// 渲染策略基类 —— SimpleRenderer(占位矩形) / SpriteRenderer(将来精灵)
class EntityRenderer {
public:
    virtual ~EntityRenderer() = default;

    virtual void drawPlayer(sf::RenderWindow& window,
                            const GameModel& model,
                            AssetManager& assets) = 0;

    virtual void drawTile(sf::RenderWindow& window,
                          const Tile& tile,
                          AssetManager& assets) = 0;
};

#endif  // MARIO_ENTITYRENDERER_H