//

#ifndef MARIO_ASSETMANAGER_H
#define MARIO_ASSETMANAGER_H

/// 纹理资源缓存 —— 加载并缓存 PNG 等图片
/// 当前为空（demo 用纯色渲染），将来加载真实精灵素材
class AssetManager {
public:
    /// 加载纹理（若已缓存则直接返回）
    const sf::Texture& load(const std::string& path) {
        auto it = cache_.find(path);
        if (it != cache_.end())
            return it->second;

        sf::Texture tex;
        if (tex.loadFromFile(path)) {
            cache_[path] = std::move(tex);
            return cache_[path];
        }
        // 加载失败返回空纹理（调用者应处理）
        return fallback_;
    }

private:
    std::unordered_map<std::string, sf::Texture> cache_;
    sf::Texture fallback_;
};

#endif  // MARIO_ASSETMANAGER_H