#include "HelloWorldScene.h"
#include "AppMacros.h"
#include "renderer/CCGLProgram.h"

#include <string>

USING_NS_CC;

using namespace std;

class TutorialLayer : public Layer
{
public:
	virtual bool init();

	Vec4 m_rect;
	Sprite* m_pMask;

	CREATE_FUNC(TutorialLayer)
};

bool TutorialLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	GLchar* pszFragSource = "#ifdef GL_ES \n \
							precision lowp float; \n \
							#endif \n \
							varying vec4 v_fragmentColor; \n \
							varying vec2 v_texCoord; \n \
							uniform vec2 u_resolution; \n \
							uniform vec4 u_rect; \n \
							void main(void) \n \
							{ \n \
							vec4 finalColor = vec4(0.0, 0.0, 0.0, 0.77); \n \
							if ( v_texCoord.x >= u_rect.x / u_resolution.x && v_texCoord.x <= (u_rect.x + u_rect.z) / u_resolution.x \n \
							&& v_texCoord.y >= u_rect.y / u_resolution.y && v_texCoord.y <= (u_rect.y + u_rect.w) / u_resolution.y) \n \
								finalColor.w = 0.0; \n \
							gl_FragColor = finalColor; \n \
							}";

	auto visibleSize = Director::getInstance()->getVisibleSize();
	m_pMask = Sprite::create("test.png");
	m_pMask->setAnchorPoint(Vec2(0, 0));
	m_pMask->setScale(visibleSize.width, visibleSize.height);
	this->addChild(m_pMask);

	GLProgram* pProgram = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, pszFragSource);
	GLProgramState* pState = GLProgramState::getOrCreateWithGLProgram(pProgram);
	m_pMask->setGLProgramState(pState);

	auto pp = Sprite::create("bang.png");
	pp->setPosition(visibleSize / 2);
	this->addChild(pp);

	Vec2 pos = pp->convertToWorldSpace(Vec2(0, 0));
	m_rect = Vec4(pos.x, visibleSize.height - pos.y - pp->getContentSize().height, pp->getContentSize().width, pp->getContentSize().height);
	m_pMask->getGLProgramState()->setUniformVec2("u_resolution", visibleSize);
	m_pMask->getGLProgramState()->setUniformVec4("u_rect", m_rect);

	return true;
}


Scene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
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
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        CC_CALLBACK_1(HelloWorld::menuCloseCallback,this));
    
    closeItem->setPosition(origin + Vec2(visibleSize) - Vec2(closeItem->getContentSize() / 2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    
    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = LabelTTF::create("Hello World", "Arial", TITLE_FONT_SIZE);
    
    // position the label on the center of the screen
    label->setPosition(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height);

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize / 2) + origin);

    // add the sprite as a child to this layer
    this->addChild(sprite);

	auto ss = TutorialLayer::create();
	addChild( ss, 999);

    
    return true;
}

void HelloWorld::menuCloseCallback(Ref* sender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
