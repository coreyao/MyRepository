#include "HelloWorldScene.h"
#include "ConvertUTF/ConvertUTF.h"

USING_NS_CC;

#include <vector>
#include <string>
using namespace std;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

cocos2d::Vector<cocos2d::SpriteFrame *> CreateAnimationFrame(const char *animation_name, cocos2d::Rect texture_info)
{
	int x = texture_info.origin.x;
	int y = texture_info.origin.y;

	int cx = texture_info.size.width / y;
	int cy = texture_info.size.height / x;

	cocos2d::Vector<cocos2d::SpriteFrame *> frame_list;

	for(int i = 0 ;i < x; i++)
	{
		for(int j = 0; j < y; j++)
		{
			cocos2d::SpriteFrame *frame = cocos2d::SpriteFrame::create(animation_name,cocos2d::Rect(j * cx,i * cy,cx,cy));
			frame_list.pushBack(frame);
		}
	}
	return frame_list;
}

class CSuccessiveAnimPlayer
{
public:
	struct SAnimInfo
	{
		SAnimInfo()
			: m_fDuration(0.0f)
			, m_fAnimDelay(1.0f / 30)
			, m_iStartFrame(0)
			, m_iEndFrame(-1)
		{
			m_tScale = Vec2(1.0f, 1.0f);
		}

		SAnimInfo( const string& sAnimName, const cocos2d::Rect& textureInfo, float fDuration, 
				const Vec2& scale = Vec2(1.0f, 1.0f), float fAnimDelay = 1.0f / 30, int iStartFrame = 0, int iEndFrame = -1 )
		{
			m_sAnimName = sAnimName;
			m_tTextureInfo = textureInfo;
			m_fDuration = fDuration;
			m_fAnimDelay = fAnimDelay;
			m_iStartFrame = iStartFrame;
			m_iEndFrame = iEndFrame;
			m_tScale = scale;
		}

		string m_sAnimName;
		cocos2d::Rect m_tTextureInfo;
		float m_fDuration;
		float m_fAnimDelay;
		int m_iStartFrame;
		int m_iEndFrame;
		Vec2 m_tScale;
	};
	
	static void Play(Node* pParent, const Vec2& pos, const vector<SAnimInfo>& vAnimInfo);
};

void CSuccessiveAnimPlayer::Play( Node* pParent, const Vec2& pos, const vector<SAnimInfo>& vAnimInfo )
{
	Node* pController = Node::create();
	pController->setPosition(pos);
	pParent->addChild(pController);

	Sprite* pEffectSprite = Sprite::create();
	pController->addChild(pEffectSprite);

	auto BeginPlay = [pEffectSprite]( const SAnimInfo& animInfo )
	{
		cocos2d::Vector<cocos2d::SpriteFrame*> vFinalFrames;
		cocos2d::Vector<cocos2d::SpriteFrame*> vFrames = CreateAnimationFrame(animInfo.m_sAnimName.c_str(), animInfo.m_tTextureInfo);

		int iStartFrame = animInfo.m_iStartFrame;
		int iEndFrame = animInfo.m_iEndFrame;

		if ( iEndFrame < 0 )
			iEndFrame = vFrames.size() - 1;
		for ( int i = iStartFrame; i <= iEndFrame; ++i )
		{
			vFinalFrames.pushBack(vFrames.at(i));
		}

		pEffectSprite->setScale(animInfo.m_tScale.x, animInfo.m_tScale.y);
		pEffectSprite->runAction(RepeatForever::create(Animate::create(Animation::createWithSpriteFrames(vFinalFrames, animInfo.m_fAnimDelay))));
	};

	auto StopPlay = [pEffectSprite]()
	{
		pEffectSprite->stopAllActions();
	};

	Vector<FiniteTimeAction*> arrayOfActions;
	for (auto& animInfo : vAnimInfo)
	{
		arrayOfActions.pushBack(CallFunc::create(std::bind(BeginPlay, animInfo)));
		arrayOfActions.pushBack(DelayTime::create(animInfo.m_fDuration));
		arrayOfActions.pushBack(CallFunc::create(std::bind(StopPlay)));
	}
	arrayOfActions.pushBack(RemoveSelf::create());

	pController->runAction(Sequence::create(arrayOfActions));
}

char* HelloWorld::G2U(const char* gb2312)  
{  
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);  
	wchar_t* wstr = new wchar_t[len+1];  
	memset(wstr, 0, len+1);  
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);  
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);  
	char* str = new char[len+1];  
	memset(str, 0, len+1);  
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);  
	if(wstr) delete[] wstr;  
	return str;
} 

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	this->scheduleUpdate();

	Sprite* pSprite11 = Sprite::create("HelloWorld.png");
	pSprite11->setPosition(visibleSize / 2);
	this->addChild(pSprite11);

	Sprite* pSprite = Sprite::create("CloseNormal.png");
	pSprite->setPosition(visibleSize / 2);
	this->addChild(pSprite);

	RichText* pRichText = RichText::create();
	pRichText->setPosition(visibleSize / 2);
	pRichText->setContentSize(Size(50, 200));
	pRichText->setString("<color=255,0,0>abcdefg</>hijklmnopqrst");
	this->addChild(pRichText);

	/*vector<CSuccessiveAnimPlayer::SAnimInfo> vAnimInfo;
	vAnimInfo.push_back(CSuccessiveAnimPlayer::SAnimInfo("warning.png", Rect(1, 8, 160, 150), 1.0f, Vec2(5.0f, 5.0f), 1.0f / 30, 0, 1));
	vAnimInfo.push_back(CSuccessiveAnimPlayer::SAnimInfo("warning.png", Rect(1, 8, 160, 150), 1.0f, Vec2(5.0f, 5.0f), 1.0f / 30, 2, 2));
	vAnimInfo.push_back(CSuccessiveAnimPlayer::SAnimInfo("warning.png", Rect(1, 8, 160, 150), 1.0f, Vec2(5.0f, 5.0f), 1.0f / 30, 3, 7));
	CSuccessiveAnimPlayer::Play(this, visibleSize / 2, vAnimInfo);

	vAnimInfo.clear();
	vAnimInfo.push_back(CSuccessiveAnimPlayer::SAnimInfo("warningcircle.png", Rect(1, 8, 1200, 150), 3.0f, Vec2(1.0f, 1.0f), 1.0f / 10));
	CSuccessiveAnimPlayer::Play(this, Vec2(visibleSize.width / 2, visibleSize.height - 100), vAnimInfo);

	vAnimInfo.clear();
	vAnimInfo.push_back(CSuccessiveAnimPlayer::SAnimInfo("protectbegin.png", Rect(2, 3, 1500, 500), 2 * 3 * 1.0f / 30));
	vAnimInfo.push_back(CSuccessiveAnimPlayer::SAnimInfo("protectpause.png", Rect(1, 3, 1500, 250), 1 * 2 * 1.0f / 30 * 5));
	vAnimInfo.push_back(CSuccessiveAnimPlayer::SAnimInfo("protectstop.png", Rect(1, 4, 2000, 250), 1 * 4 * 1.0f / 30));
	CSuccessiveAnimPlayer::Play(this, visibleSize / 2, vAnimInfo);*/

	//auto pLabel = cocos2d::Label::createWithTTF(G2U("好"), "fonts/simyou.ttf", 20);
	////pLabel->setString("a");
	//pLabel->setPosition(visibleSize / 2);
	//this->addChild(pLabel);


    return true;
}

void HelloWorld::update( float deltaTime )
{
	//Size visibleSize = Director::getInstance()->getVisibleSize();
	//DrawPrimitives::setPointSize(10);
	//DrawPrimitives::drawPoint(visibleSize / 2);
}

void RichText::setString( const std::string& text )
{
	if (text.compare(_originalUTF8String))
	{
		_originalUTF8String = text;

		std::u16string utf16String;
		if (StringUtils::UTF8ToUTF16(_originalUTF8String, utf16String))
		{
			_currentUTF16String  = utf16String;
		}
	}

	this->removeAllChildren();

	vector<pair<u16string, u16string>> vStr;
	u16string temp = _currentUTF16String;
	while (!temp.empty())
	{
		u16string::size_type spos = temp.find_first_of('<');
		if ( spos != u16string::npos )
		{
			u16string::size_type epos = temp.find_first_of('>');
			u16string configureStr = temp.substr(spos + 1, epos - spos - 1);

			u16string out;
			StringUtils::UTF8ToUTF16("</>", out);
			u16string::size_type iIndex = temp.find(out);
			u16string contentStr = temp.substr(epos + 1, iIndex - epos - 1);

			vStr.push_back( pair<u16string, u16string>( contentStr, configureStr ) );

			temp = temp.substr(iIndex + 3);
		}
		else
		{
			vStr.push_back( pair<u16string, u16string>( temp, u16string() ) );
			temp = u16string();
		}
	}

	Size tSize = this->getContentSize();
	float fStartX = 0;
	float fStartY = 0;
	for (auto& pPair : vStr)
	{
		int strLen = static_cast<int>(pPair.first.length());
		string sConfigure;
		StringUtils::UTF16ToUTF8(pPair.second, sConfigure);
		Color3B color = Color3B::WHITE;

		u16string::size_type iIndex = sConfigure.find("color");
		if ( iIndex != u16string::npos )
			color = Color3B::RED;

		for (int i = 0; i < strLen; ++i)
		{
			string utf8Str;
			StringUtils::UTF16ToUTF8(u16string(1, pPair.first[i]), utf8Str);
			Label* pLabel = Label::create();
			this->addChild(pLabel);
			pLabel->setString(utf8Str);
			pLabel->setAnchorPoint(Vec2(0, 1));
			pLabel->setPosition(fStartX, fStartY);
			pLabel->setScale(this->getScale());
			pLabel->setColor(color);

			fStartX += pLabel->getContentSize().width * pLabel->getScaleX();
			if ( fStartX >= tSize.width )
			{
				fStartX = 0;
				fStartY -= pLabel->getContentSize().height * pLabel->getScaleY();
			}
		}
	}
}
