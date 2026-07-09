//

#ifndef MARIO_SIMPLERENDERER_H
#define MARIO_SIMPLERENDERER_H

#include "view/EntityRenderer.h"

/// 纯色矩形渲染 —— demo 阶段占位，将来替换为 SpriteRenderer
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
    sf::RectangleShape rect_;
};

#endif  // MARIO_SIMPLERENDERER_H