#include "talklib.h"
#include <sstream>
#include "HeaderOnlyCsv.hpp"

std::vector<std::string> split(const std::string& s, char delim)
{
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, delim))
    {
        result.push_back(item);
    }

    return result;
}

void Talk::Init(const std::string& csvfilepath,
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
                const float lookAtZ)
{
    m_csvfilepath = csvfilepath;
    m_font = font;
    m_SE = SE;
    m_sprTextBack = sprTextBack;
    m_sprFade = sprFade;
    m_camera = camera;
    m_restoreEyeX = eyeX;
    m_restoreEyeY = eyeY;
    m_restoreEyeZ = eyeZ;
    m_restoreLookAtX = lookAtX;
    m_restoreLookAtY = lookAtY;
    m_restoreLookAtZ = lookAtZ;

    m_font->Init();
    m_SE->Init();
    m_sprTextBack->Load("textBack.png");
    m_sprFade->Load("black.png");

    m_isFadeIn = true;

    std::vector<TalkBall> talkList = CreateTalkList();
    m_talkBallList = talkList;

}

std::vector<TalkBall> Talk::CreateTalkList()
{
    std::vector<TalkBall> talkList;
    std::vector<std::vector<std::string> > vss = csv::Read(m_csvfilepath);

    for (std::size_t i = 1; i < vss.size(); ++i)
    {
        TalkBall talkBall;
        talkBall.Init(vss.at(i), m_font, m_sprTextBack, m_SE, m_camera);
        talkList.push_back(talkBall);
    }
    return talkList;
}

void Talk::Next()
{
    if (m_waitNextCount < WAIT_NEXT_FRAME)
    {
        return;
    }

    if (m_talkBallList.at(m_talkBallIndex).IsFinish() == false)
    {
        return;
    }

    if (m_talkBallIndex < (int)m_talkBallList.size() - 1)
    {
        m_talkBallIndex++;
    }
    else
    {
        m_FadeOutCount = 0;
        m_isFadeOut = true;
    }
    m_waitNextCount = 0;

}

// 戻り値は会話終了フラグ
bool Talk::Update()
{
    bool isFinish = false;
    m_waitNextCount++;
    if (m_isFadeIn)
    {
        if (m_FadeInCount < FADE_FRAME_MAX)
        {
            m_FadeInCount++;
        }
        else
        {
            m_isFadeIn = false;
            m_FadeInCount = 0;
        }
    }
    if (m_isFadeOut)
    {
        if (m_FadeOutCount < FADE_FRAME_MAX)
        {
            m_FadeOutCount++;
        }
        else
        {
            isFinish = true;
        }
    }
    m_talkBallList.at(m_talkBallIndex).Update();

    return isFinish;
}

void Talk::Render()
{
    m_talkBallList.at(m_talkBallIndex).Render();

    if (m_isFadeIn)
    {
        m_sprFade->DrawImage(0, 0, 255 - m_FadeInCount*255/FADE_FRAME_MAX);
    }
    if (m_isFadeOut)
    {
        m_sprFade->DrawImage(0, 0, m_FadeOutCount*255/FADE_FRAME_MAX);
    }
}

void Talk::Finalize()
{
    if (m_camera != nullptr)
    {
        m_camera->SetPosAndRot(m_restoreEyeX,    m_restoreEyeY,    m_restoreEyeZ,
                               m_restoreLookAtX, m_restoreLookAtY, m_restoreLookAtZ);
    }

    delete m_camera;
    m_camera = nullptr;

    delete m_sprFade;
    m_sprFade = nullptr;

    delete m_sprTextBack;
    m_sprTextBack = nullptr;

    delete m_SE;
    m_SE = nullptr;

    delete m_font;
    m_font = nullptr;
}

void TalkBall::Init(const std::vector<std::string>& csvOneLine,
                    IFont* font,
                    ISprite* sprite,
                    ISoundEffect* SE,
                    ICamera* camera)
{
    m_font = font;
    m_sprite = sprite;
    m_SE = SE;
    m_camera = camera;

    std::vector<std::string> vs;

    std::string work = csvOneLine.at(1);
    work.erase(remove(work.begin(), work.end(), '\"'), work.end());
    vs = split(work, '\n');
    m_text = vs;
    m_text.resize(3);

    vs = split(csvOneLine.at(2), ':');

    m_EyeX = (float)std::atof(vs.at(0).c_str());
    m_EyeY = (float)std::atof(vs.at(1).c_str());
    m_EyeZ = (float)std::atof(vs.at(2).c_str());

    vs = split(csvOneLine.at(3), ':');

    m_LookAtX = (float)std::atof(vs.at(0).c_str());
    m_LookAtY = (float)std::atof(vs.at(1).c_str());
    m_LookAtZ = (float)std::atof(vs.at(2).c_str());

    m_textShow.resize(3);
}

void TalkBall::Update()
{
    m_camera->SetPosAndRot(m_EyeX, m_EyeY, m_EyeZ, m_LookAtX, m_LookAtY, m_LookAtZ);
    m_textIndex++;

    bool finish = false;

    // 文字送り処理
    m_textShow.at(0).clear();
    m_textShow.at(1).clear();
    m_textShow.at(2).clear();

    // 30フレーム経過してから文字の表示を始める
    m_counter++;
    if (m_counter < 30)
    {
        return;
    }

    if (m_isSEPlay == false)
    {
        m_isSEPlay = true;
        m_SE->PlayMessage();
    }

    m_charCount++;
    // 一行目
    if (m_charCount < (int)m_text.at(0).size())
    {
        // マルチバイト文字は1文字で2バイトであることを考慮する
        if (m_charCount % 2 == 0)
        {
            m_textShow.at(0) = m_text.at(0).substr(0, m_charCount);
        }
        else
        {
            m_textShow.at(0) = m_text.at(0).substr(0, m_charCount - 1);
        }
    }
    else
    {
        m_textShow.at(0) = m_text.at(0);
    }

    int total = 0;

    // 二行目
    total = m_text.at(0).size() + m_text.at(1).size();
    int secondLineCount = m_charCount - m_text.at(0).size();
    if (m_charCount < total)
    {
        if (secondLineCount >= 0)
        {
            // マルチバイト文字は1文字で2バイトであることを考慮する
            if (secondLineCount % 2 == 0)
            {
                m_textShow.at(1) = m_text.at(1).substr(0, secondLineCount);
            }
            else
            {
                m_textShow.at(1) = m_text.at(1).substr(0, secondLineCount - 1);
            }
        }
    }
    else
    {
        m_textShow.at(1) = m_text.at(1);
    }

    // 三行目
    total = m_text.at(0).size() + m_text.at(1).size() + m_text.at(2).size();

    int thirdLineCount = m_charCount - m_text.at(0).size() - m_text.at(1).size();
    if (m_charCount < total)
    {
        if (thirdLineCount >= 0)
        {
            // マルチバイト文字は1文字で2バイトであることを考慮する
            if (thirdLineCount % 2 == 0)
            {
                m_textShow.at(2) = m_text.at(2).substr(0, thirdLineCount);
            }
            else
            {
                m_textShow.at(2) = m_text.at(2).substr(0, thirdLineCount - 1);
            }
        }
    }
    else
    {
        m_textShow.at(2) = m_text.at(2);
        m_isFinish = true;
        m_SE->Stop();
    }

}

void TalkBall::Render()
{
    m_sprite->DrawImage(0, 0);
    if (m_textShow.size() >= 1)
    {
        m_font->DrawText_(m_textShow.at(0), 100, 730);
    }

    if (m_textShow.size() >= 2)
    {
        m_font->DrawText_(m_textShow.at(1), 100, 780);
    }

    if (m_textShow.size() >= 3)
    {
        m_font->DrawText_(m_textShow.at(2), 100, 830);
    }
}

bool TalkBall::IsFinish()
{
    return m_isFinish;
}
