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
    m_talkBallIndex++;
    /*
    if (m_waitNextCount < WAIT_NEXT_FRAME)
    {
        return;
    }
    int textIndex = m_talkBallList.at(m_talkBallIndex).GetTextIndex();
    int textIndexMax = (int)m_talkBallList.at(m_talkBallIndex).GetTextList().size();
    if (textIndex < textIndexMax - 1)
    {
        textIndex++;
    }
    else
    {
        if (m_talkBallIndex <= (int)m_talkBallList.size() - 2)
        {
            textIndex = 0;
            m_talkBallIndex++;
        }
        else
        {
            m_FadeOutCount = 0;
            m_isFadeOut = true;
        }
    }
    m_talkBallList.at(m_talkBallIndex).SetTextIndex(textIndex);
    m_SE->Play();
    m_waitNextCount = 0;
    */
}

// 戻り値は会話終了フラグ
bool Talk::Update()
{
    /*
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
    return isFinish;
    */
    bool finish = false;
    if (m_talkBallIndex <= m_talkBallList.size() - 1)
    {
        m_talkBallList.at(m_talkBallIndex).Update();
    }
    else
    {
        finish = true;
    }
    return finish;
}

void Talk::Render()
{
    m_talkBallList.at(m_talkBallIndex).Render();
    /*
    m_talkBallList.at(m_talkBallIndex).GetCamera()->SetPosAndRot();

    m_sprTextBack->DrawImage(0, 0);
    std::vector<std::vector<std::string>> vss = m_talkBallList.at(m_talkBallIndex).GetTextList();
    int textIndex = m_talkBallList.at(m_talkBallIndex).GetTextIndex();
    if (vss.at(textIndex).size() >= 1)
    {
        m_font->DrawText_(vss.at(textIndex).at(0), 100, 730);
    }

    if (vss.at(textIndex).size() >= 2)
    {
        m_font->DrawText_(vss.at(textIndex).at(1), 100, 780);
    }

    if (vss.at(textIndex).size() >= 3)
    {
        m_font->DrawText_(vss.at(textIndex).at(2), 100, 830);
    }

    if (m_isFadeIn)
    {
        m_sprFade->DrawImage(0, 0, 255 - m_FadeInCount*255/FADE_FRAME_MAX);
    }
    if (m_isFadeOut)
    {
        m_sprFade->DrawImage(0, 0, m_FadeOutCount*255/FADE_FRAME_MAX);
    }
    */
}

void Talk::Finalize()
{
    // TODO
//    m_restore->SetPosAndRot();

    for (std::size_t i = 0; i < m_talkBallList.size(); ++i)
    {
//        delete m_talkBallList.at(i).GetCamera();
//        m_talkBallList.at(i).SetCamera(nullptr);
    }
    delete m_sprTextBack;
    m_sprTextBack = nullptr;
    delete m_sprFade;
    m_sprFade = nullptr;
    delete m_font;
    m_font = nullptr;
    delete m_SE;
    m_SE = nullptr;
}

void TalkBall::Init(const std::vector<std::string>& csvOneLine,
                    IFont* font,
                    ISprite* sprite,
                    ISoundEffect* SE,
                    ICamera* camera)
{
    m_text = csvOneLine;
    m_font = font;
    m_sprite = sprite;
    m_SE = SE;
    m_camera = camera;

    std::vector<std::string> vs;

    std::string work = csvOneLine.at(1);
    work.erase(remove(work.begin(), work.end(), '\"'), work.end());
    vs = split(work, '\n');
    m_text = vs;

    vs = split(csvOneLine.at(2), ':');

    m_EyeX = (float)std::atof(vs.at(0).c_str());
    m_EyeY = (float)std::atof(vs.at(1).c_str());
    m_EyeZ = (float)std::atof(vs.at(2).c_str());

    vs = split(csvOneLine.at(3), ':');

    m_LookAtX = (float)std::atof(vs.at(0).c_str());
    m_LookAtY = (float)std::atof(vs.at(1).c_str());
    m_LookAtZ = (float)std::atof(vs.at(2).c_str());
}

void TalkBall::Update()
{
    m_camera->SetPosAndRot(m_EyeX, m_EyeY, m_EyeZ, m_LookAtX, m_LookAtY, m_LookAtZ);
}

void TalkBall::Render()
{
    m_sprite->DrawImage(0, 0);
    if (m_text.size() >= 1)
    {
        m_font->DrawText_(m_text.at(0), 100, 730);
    }

    if (m_text.size() >= 2)
    {
        m_font->DrawText_(m_text.at(1), 100, 780);
    }

    if (m_text.size() >= 3)
    {
        m_font->DrawText_(m_text.at(2), 100, 830);
    }
}
