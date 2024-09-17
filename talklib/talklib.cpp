#include "talklib.h"
#include <sstream>

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

void Talk::Init(
    IFont* font,
    ISoundEffect* SE,
    ISprite* sprTextBack,
    ISprite* sprFade,
    const std::vector<TalkBall>& pageList,
    ICamera* restore)
{
    m_font = font;
    m_SE = SE;
    m_sprTextBack = sprTextBack;
    m_sprFade = sprFade;
    m_talkBallList = pageList;
    m_isFadeIn = true;
    m_restore = restore;
}

void Talk::Next()
{
    if (m_waitNextCount < WAIT_NEXT_FRAME)
    {
        return;
    }
    int textIndex = m_talkBallList.at(m_pageIndex).GetTextIndex();
    int textIndexMax = (int)m_talkBallList.at(m_pageIndex).GetTextList().size();
    if (textIndex < textIndexMax - 1)
    {
        textIndex++;
    }
    else
    {
        if (m_pageIndex <= (int)m_talkBallList.size() - 2)
        {
            textIndex = 0;
            m_pageIndex++;
        }
        else
        {
            m_FadeOutCount = 0;
            m_isFadeOut = true;
        }
    }
    m_talkBallList.at(m_pageIndex).SetTextIndex(textIndex);
    m_SE->PlayMove();
    m_waitNextCount = 0;
}

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
    return isFinish;
}

void Talk::Render()
{
    m_talkBallList.at(m_pageIndex).GetCamera()->SetPosAndRot();

    m_sprTextBack->DrawImage(0, 0);
    std::vector<std::vector<std::string>> vss = m_talkBallList.at(m_pageIndex).GetTextList();
    int textIndex = m_talkBallList.at(m_pageIndex).GetTextIndex();
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
}

void Talk::Finalize()
{
    m_restore->SetPosAndRot();

    for (std::size_t i = 0; i < m_talkBallList.size(); ++i)
    {
        delete m_talkBallList.at(i).GetCamera();
        m_talkBallList.at(i).SetCamera(nullptr);
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

std::vector<std::vector<std::string>> TalkBall::GetTextList() const
{
    return m_textList;
}

void TalkBall::SetTextList(const std::vector<std::vector<std::string>>& textList)
{
    m_textList = textList;
}

int TalkBall::GetTextIndex() const
{
    return m_textIndex;
}

void TalkBall::SetTextIndex(const int index)
{
    m_textIndex = index;
}

ICamera* TalkBall::GetCamera() const
{
    return m_camera;
}

void TalkBall::SetCamera(ICamera* const camera)
{
    m_camera = camera;
}

