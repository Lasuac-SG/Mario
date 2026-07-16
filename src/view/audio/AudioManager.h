#ifndef MARIO_AUDIOMANAGER_H
#define MARIO_AUDIOMANAGER_H

class AudioManager {
   public:
    AudioManager() = default;
    ~AudioManager() = default;

    bool playBGM(const std::string& path, float volume = 30.f);
    void stopBGM();
    void setBGMVolume(float volume);
    bool isBGMPlaying() const;

   private:
    sf::Music bgm_;
};

#endif  // MARIO_AUDIOMANAGER_H
