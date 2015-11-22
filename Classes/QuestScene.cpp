// (C) 2015 Arisecbf


#include "QuestScene.h"
#include "format.h"
#include "PixelDataCache.hpp"
#include "HeroGraphicUnitAnimationDef.hpp"
#include "RivalGraphicAnimationDef.hpp"
#include "StoryInfo.hpp"
#include "BattleState.hpp"
USING_NS_CC;


bool QuestScene::init()
{
    assert(TRBaseScene::init());

    // preload  TODO 移到load界面中

    StoryInfo::s()->loadConfig();
    HeroGraphicAnimationDef::loadConfig();
    RivalGraphicAnimationDef::loadConfig();
    PixelDataCache::s()->preload("beats/a.png.sopx");
    PixelDataCache::s()->preload("beats/b.png.sopx");
    PixelDataCache::s()->preloadChars();
    addCommonBtn({0.9,0.5}, "reset", [](){Director::getInstance()->replaceScene(QuestScene::create());});


    addCommonBtn({0.9,0.4}, "upg", [this](){_beatField.op_showUpgrade();});
/*
    addCommonBtn({0.35,0.95}, "battle cam", [this](){
        _cameraManage.op_switchToPosition(QuestCameraPosition::BATTLE);
    });
    addCommonBtn({0.2,0.95}, "forward cam", [this](){
        _cameraManage.op_switchToPosition(QuestCameraPosition::FORWARD);
    });

    addCommonBtn({0.9,0.9}, "roles newbatt", [this](){
        _battleRoles.op_newBattleConfig();
    });
    addCommonBtn({0.75,0.9}, "roles battleIn", [this](){
        _battleRoles.op_battleIn();
    });

    addCommonBtn({0.6,0.9}, "battle begin", [this](){
        battleBegin();
    });

    addCommonBtn({0.45,0.9}, "scene forword", [this](){
        _battleRoad.op_moveForword();
    });

    addCommonBtn({0.3,0.9}, "test bloodline", [this](){
        _bloodBar.op_hero_configBlood(1000, random(0, 1000));
        _bloodBar.op_rival_configBlood(1000, random(0,1000), 4);
    });

    addCommonBtn({0.15,0.9}, "start beating", [this](){
        _beatField.op_startBeating();
    });
*/
    addCommonBtn({0.9,0.8}, "road shadow", [this](){
        _battleRoad.op_toastDarkShadow(1.0, 2);
    });
    addCommonBtn({0.9,0.7}, "enegy test", [this](){
        _energyBar.op_addEnergy(0.1);
    });
    addCommonBtn({0.9,0.6}, "gold number", [this](){
        _topIcons.op_configGold(random(0, 99999));
    });


    auto layer = Layer::create();
    this->addChild(layer);

    auto size = Director::getInstance()->getVisibleSize();

    auto camera = Camera::createPerspective(60, size.width/size.height, 0.1, 1000000);
    camera->setCameraFlag(CameraFlag::USER1);
    layer->addChild(camera);
    camera->setCameraMask((unsigned short)CameraFlag::USER1);

    _mainLayer = layer;
    _mainCamera = camera;

    // init
    _cameraManage.init(_mainCamera);
    _battleRoad.init(_mainLayer, _mainCamera);
    _battleRoles.init(_mainLayer, _mainCamera);
    _energyBar.init(_mainLayer, _mainCamera);
    _bloodBar.init(_mainLayer, _mainCamera);
    _beatField.init(_mainLayer, _mainCamera);
    initPauseShadowThings();
    _topIcons.init(_mainLayer, _mainCamera);

    // config
    _cameraManage.configProtocals(&_battleRoad, &_battleRoles, &_bloodBar);
    _beatField.configProtocals(&_battleRoles, &_battleRoad);
    BattleState::s()->configProtocals(&_bloodBar, &_beatField,
                                      &_battleRoles, &_battleRoad, &_cameraManage, _mainCamera, this);

    // init actions
    _cameraManage.op_switchToPosition(QuestCameraPosition::FORWARD, true);

//    // hero 穿好衣服
//    _battleRoles.op_heroConfig();
//    // 新关卡开始 hero 跑进来
//    _battleRoles.op_heroRunIn();
//    // 然后 hero 原地跑，背景往后移
//    scheduleOnce([this](float dt) {
//        _battleRoad.op_moveForword(QuestDef::HERO_RUN_IN_TIME2 + QuestDef::CAMERA_MOVE_TIME);
//    }, QuestDef::HERO_RUN_IN_TIME, "after hero run in");
//
//    scheduleOnce([this](float dt) {
//        _cameraManage.op_switchToPosition(QuestCameraPosition::BATTLE, false);
//        _battleRoles.op_switchToBattle();
//    }, QuestDef::HERO_RUN_IN_TIME + QuestDef::HERO_RUN_IN_TIME2, "after hero run in2");
//
//    scheduleOnce([this](float dt) {
//        this->battleBegin();
//        BattleState::s()->heroSetBloodBar();
//    }, QuestDef::HERO_RUN_IN_TIME + QuestDef::HERO_RUN_IN_TIME2 + QuestDef::CAMERA_MOVE_TIME, "after hero run in3");

    
    scheduleUpdate();
    return true;
}

void QuestScene::initPauseShadowThings()
{
    _pauseShadow = Sprite::create("images/pause_shadow.png");
    _pauseShadow->setPosition(genPos({0.5,0.5}));
    _pauseShadow->setScale(1000);
    _defaultLayer->addChild(_pauseShadow);
    _pauseShadow->setVisible(false);

    auto listener = EventListenerTouchOneByOne::create();

    listener->onTouchBegan = [this](Touch* touch, Event* event){

        if (_waiting2startBattle && _pauseShadow->isVisible()) {
            _waiting2startBattle = false;
            _pauseShadow->setVisible(false);
            _beatField.op_startBeating();
        }
        return false;
    };

    listener->onTouchMoved = [this](Touch* touch, Event* event){

    };

    listener->onTouchEnded = [this](Touch* touch, Event* event){

    };

    listener->onTouchCancelled = [this](Touch* touch, Event* event){
    };

    _mainLayer->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, _mainLayer);
}

void QuestScene::op_showPauseShadow()
{
    _pauseShadow->setVisible(true);
    _waiting2startBattle = true;
}

void QuestScene::battleBegin()
{
    BattleState::s()->initAsNextRival(); //初始化首个rival的数据
}

void QuestScene::update(float dt)
{
//    _battleRoad.update(dt);
//    _beatField.update(dt);
    _topIcons.update(dt);
    _energyBar.update(dt);
}