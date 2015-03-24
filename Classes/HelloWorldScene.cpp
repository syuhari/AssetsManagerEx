#include "HelloWorldScene.h"
#include "ResourceManager.h"

USING_NS_CC;
USING_NS_CC_EXT;

HelloWorld::HelloWorld()
:_progress(nullptr)
{
    
}

HelloWorld::~HelloWorld()
{
    // remove custom events
    getEventDispatcher()->removeCustomEventListeners(ResourceManager::EVENT_PROGRESS);
    getEventDispatcher()->removeCustomEventListeners(ResourceManager::EVENT_FINISHED);
}

Scene* HelloWorld::createScene()
{
    auto scene = Scene::create();
    auto layer = HelloWorld::create();
    scene->addChild(layer);
    return scene;
}

bool HelloWorld::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    // set search paths
    std::vector<std::string> paths;
    paths.push_back(FileUtils::getInstance()->getWritablePath());
    FileUtils::getInstance()->setSearchPaths(paths);
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    auto sprite = Sprite::create("HelloWorld.png");
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
    this->addChild(sprite, 0);
    
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    // disabled button
    auto item = dynamic_cast<MenuItemImage*>(pSender);
    if (item!=nullptr) {
        item->setEnabled(false);
    }
    
    // label for progress
    auto size = Director::getInstance()->getWinSize();
    TTFConfig config("fonts/arial.ttf", 30);
    _progress = Label::createWithTTF(config, "0%", TextHAlignment::CENTER);
    _progress->setPosition( Vec2(size.width/2, 50) );
    this->addChild(_progress);
    
    // progress event
    getEventDispatcher()->addCustomEventListener(ResourceManager::EVENT_PROGRESS, [this](EventCustom* event){
        auto data = (Value*)event->getUserData();
        float percent = data->asFloat();
        std::string str = StringUtils::format("%.2f", percent) + "%";
        CCLOG("%.2f Percent", percent);
        if (this->_progress != nullptr) {
            this->_progress->setString(str);
        }
    });
    
    // fnished updating event
    getEventDispatcher()->addCustomEventListener(ResourceManager::EVENT_FINISHED, [this](EventCustom* event){
        // clear cache
        Director::getInstance()->getTextureCache()->removeAllTextures();
        // reload scene
        auto scene = HelloWorld::createScene();
        Director::getInstance()->replaceScene(scene);
    });
    
    // update resources
    ResourceManager::getInstance()->updateAssets("res/local.manifest");
}