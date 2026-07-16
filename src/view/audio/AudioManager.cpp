#include "view/audio/AudioManager.h"

bool AudioManager::playBGM(const std::string& path, float volume) {
    if (!bgm_.openFromFile(path)) return false;
    bgm_.setLooping(true);
    bgm_.setVolume(volume);
    bgm_.play();
    return true;
}

void AudioManager::stopBGM() { bgm_.stop(); }

void AudioManager::setBGMVolume(float volume) { bgm_.setVolume(volume); }

bool AudioManager::isBGMPlaying() const { return bgm_.getStatus() == sf::Music::Status::Playing; }
