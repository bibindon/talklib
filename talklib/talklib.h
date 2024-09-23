// CSVファイル読み込みは保留。このクラスを使う機会が3回くらいしかないなら、作っても意味ない。
#pragma once
#include <string>
#include <vector>

class ISprite
{
public:
    virtual void DrawImage(const int x, const int y, const int transparency = 255) = 0;
    virtual void Load(const std::string& filepath) = 0;
    virtual ~ISprite() {};
};

class IFont
{
public:
    virtual void DrawText_(const std::string& msg, const int x, const int y) = 0;
    virtual void Init() = 0;
    virtual ~IFont() {};
};

class ISoundEffect
{
public:
    virtual void PlayMessage() = 0;
    virtual void Stop() = 0;
    virtual void Init() = 0;
    virtual ~ISoundEffect() {};
};

class ICamera
{
public:
    virtual void SetPosAndRot(const float posX, const float posY, const float posZ,
                              const float AtX, const float AtY, const float AtZ) = 0;
    virtual ~ICamera() {};
};

class TalkBall
{
public:
    void Init(const std::vector<std::string>& csvOneLine,
              IFont* font,
              ISprite* sprite,
              ISoundEffect* SE,
              ICamera* camera);
    void Update();
    void Render();
    bool IsFinish();

private:
    std::vector<std::string> m_textShow;
    std::vector<std::string> m_text;
    int m_textIndex = 0;
    int m_counter = 0;
    int m_charCount = 0;
    IFont* m_font = nullptr;
    ISprite* m_sprite = nullptr;
    ISoundEffect* m_SE = nullptr;
    ICamera* m_camera = nullptr;

    float m_EyeX = 0.f;
    float m_EyeY = 0.f;
    float m_EyeZ = 0.f;
    float m_LookAtX = 0.f;
    float m_LookAtY = 0.f;
    float m_LookAtZ = 0.f;

    bool m_isSEPlay = false;
    bool m_isFinish = false;
};

class Talk
{
public:

    void Init(const std::string& csvFilename,
              IFont* font,
              ISoundEffect* SE,
              ISprite* sprTextBack,
              ISprite* sprFade,
              ICamera* camera,
              const float eyeX,
              const float eyeY,
              const float eyeZ,
              const float lookAtX,
              const float lookAtY,
              const float lookAtZ);
    std::vector<TalkBall> CreateTalkList();
    void Next();
    bool Update();
    void Render();
    void Finalize();

private:
    std::string m_csvfilepath;
    ISprite* m_sprTextBack;
    IFont* m_font;
    ISoundEffect* m_SE;
    std::vector<TalkBall> m_talkBallList;
    int m_talkBallIndex = 0;

    ISprite* m_sprFade;
    // 30フレームかけて表示する。
    // 30フレームではなく500ミリ秒、でやるべきだが、それほど大きな問題とならないのでよしとする。
    const int FADE_FRAME_MAX = 30;
    bool m_isFadeIn = false;
    int m_FadeInCount = 0;
    bool m_isFadeOut = false;
    int m_FadeOutCount = 0;

    const int WAIT_NEXT_FRAME = 60;
    int m_waitNextCount = 0;

    ICamera* m_camera = nullptr;

    float m_restoreEyeX = 0.f;
    float m_restoreEyeY = 0.f;
    float m_restoreEyeZ = 0.f;
    float m_restoreLookAtX = 0.f;
    float m_restoreLookAtY = 0.f;
    float m_restoreLookAtZ = 0.f;

};

