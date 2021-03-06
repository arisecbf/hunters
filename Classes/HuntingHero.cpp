//
//  HuntingHero.cpp
//  hunters
//
//  Created by chenbingfeng on 15/11/24.
//
//

#include "HuntingHero.hpp"

#include "format.h"
#include "intersection.h"
#include "HuntingHeroMetaSuitManage.hpp"
#include "QuestDef.hpp"

USING_NS_CC;
void HuntingHero::init(cocos2d::Layer *mainLayer, cocos2d::Camera *mainCamera)
{
    _mainLayer = mainLayer;
    _mainCamera = mainCamera;

    initHubThings();
    initHeroThings();
}

void HuntingHero::initHubThings()
{
    _hubNode = Node::create();
    _hubNode->setPosition3D({0,0,0});
    _hubNode->setScale(0.15);
    _hubNode->setCameraMask(_mainCamera->getCameraMask());
    _hubNode->setZOrder(9);
    _hubNode->setRotation3D({90,0,-90});
    _mainLayer->addChild(_hubNode);
    //    _hubNode->setVisible(false);
}

void HuntingHero::initHeroThings()
{
    _dpxNode = DynamicPixelNode::create();
    _dpxNode->setPosition3D({0,0,0});
    _hubNode->addChild(_dpxNode);
    _dpxNode->setCameraMask(_mainCamera->getCameraMask(), true);
}

int HuntingHero::boneIndexType2sopxId(int boneIndexType)
{
    int r = 0;
    switch (boneIndexType) {
        case BT_HEAD:
            r = 2;
            break;

        case BT_HAIR:
            r = 0;
            break;

        case BT_BODY:
            r = 5;
            break;

        case BT_HAND_L:
            r = 4;
            break;

//        case BT_HAND_R:
//            r = 6;
//            break;

        case BT_LEG_L:
            r = 8;
            break;

        case BT_LEG_R:
            r = 8;
            break;

        default:
            break;
    }
    return r;
}

void HuntingHero::op_configHeroTypeAndGrade(HeroType heroType, int grade) // 由英雄类型和等级 来指定穿着套装和弓
{
    bool gradeOnly = _heroType == heroType;
    _heroType = heroType;
    _heroGrade = grade;
    //会清空 VBO 后，重新写数据
    _dpxNode->configClear();
    assert( grade >= 0 && grade <3);

    int suitId = 0;
    char bowType = 'm';
    if (heroType == HeroType::HT_META) {
        if (!gradeOnly) {
            suitId = HuntingHeroMetaSuitManage::s()->borrow();
        } else {
            suitId = _suidId;
        }
        bowType = 'm';
    } else {
        switch (heroType) {
            case HeroType::HT_SLOW_DOWN:
                suitId = 4;
                bowType = 'b';
                break;

            case HeroType::HT_HIGH_ATTACK:
                suitId = 5;
                bowType = 'e';
                break;

            case HeroType::HT_MULTI_ATTACK:
                suitId = 6;
                bowType = 'a';
                break;

            case HeroType::HT_BOMB_ATTACK:
                suitId = 7;
                bowType = 'f';
                break;
                
            default:
                assert(false);
                break;
        }
    }

    _suidId = suitId;
    // 穿衣服
    for (int i = 0; i < BT_BOW_MAX; i++) {
        _dpxNode->configAddSopx(fmt::sprintf("hunters/heros/%d.png.%d.so.png.sopx", suitId, boneIndexType2sopxId(i)), i, boneIndex2relativePosition(i));
    }

    // 箭
    _dpxNode->configAddSopx(fmt::sprintf("hunters/arrows/%02d.png.sopx", static_cast<int>(huntingHeroType2huntingArrowType(heroType, grade))), BT_ARROW, boneIndex2relativePosition(BT_ARROW));

    // 穿上弓
    _dpxNode->configAddSopx(fmt::sprintf("hunters/bows/%c%d.png.sopx", bowType, grade), BT_BOW_MAX, boneIndex2relativePosition(BT_BOW_MAX));

    _dpxNode->configSpecialDoubleBoneRotateFirst(BT_BOW_MAX, {0,0,45});

    _hubNode->setCameraMask(_mainCamera->getCameraMask(), true);
    ani_run();



}
void HuntingHero::op_configRelativeAngle(float angle) //逆时针变大，设置单个英雄的相对设计角度
{

}
void HuntingHero::op_startAiming() //开始瞄准，会举起弓到水平位置
{
    ani_idle();
    op_configAiming(35, 0);
}
void HuntingHero::op_stopAiming() //取消瞄准，会放下弓
{
    ani_idle();
}
void HuntingHero::op_toastUnderAttack() //播放被攻击动画
{

}
void HuntingHero::op_toastDead() //播放死亡
{

}
void HuntingHero::op_toastShield(float interval) //播放一段时间护盾
{

}
void HuntingHero::op_toastBoost(float interval) //播放一段时间的暴击增益
{

}
void HuntingHero::op_toastUpgrade() //播放升级动画
{
    ani_idle();
    const float jump_tiem = 0.2f;
    _hubNode->runAction( Sequence::create(
                                          EaseOut::create(MoveBy::create(jump_tiem, Vec3{0, 0, 5}), 2.f), EaseIn::create(MoveBy::create(jump_tiem, Vec3{0, 0, -5}), 2.f),

                                          EaseOut::create(MoveBy::create(jump_tiem, Vec3{0, 0, 5}), 2.f), EaseIn::create(MoveBy::create(jump_tiem, Vec3{0, 0, -5}), 2.f),

                                          NULL));
}

void HuntingHero::op_show()
{
    _hubNode->setVisible(true);
}

void HuntingHero::op_hide()
{
    _hubNode->setVisible(false);
}

void HuntingHero::op_move(HeroPositionType desPositionType, bool direct)
{
    _heroPositionType = desPositionType;
    float ypos = heroPositionType2floatYposition(desPositionType);
    const float move_speed = 10;
    if (direct) {
        _hubNode->setPosition3D({0, ypos, 0});
        ani_idle();
    } else {
        _hubNode->stopAllActions();
        ani_run();
        float time = std::abs(_hubNode->getPositionY() - ypos) / move_speed;
        _hubNode->runAction(MoveTo::create(time, Vec3{0, ypos, 0}));
        _hubNode->scheduleOnce([this](float dt) {
            this->ani_idle();
        }, time, "move over to idle");
    }
}

cocos2d::Vec3 HuntingHero::boneIndex2relativePosition(int boneIndexType)
{
    Vec3 r = {0,0,0};
    switch (boneIndexType) {

        case BT_HEAD:
            r = {0,10,0};
            break;

        case BT_HAIR:
            r = {0,-20.f/3,0};
            break;

        case BT_BOW_MAX:
            r = {1.f,1.f,0};
            break;
        case BT_ARROW:
            r = {5.f,5.f,0};
            break;
        case BT_LEG_L:
        case BT_LEG_R:
            r = {2.f/3,-7.f/3,0};
            break;
        default:
            break;
    }
    return r;
}

cocos2d::Vec3 HuntingHero::help_calcBonePosition(int boneIndexType)
{
    Vec3 r = {0,0,0};
    const float head_pos = 70;

    switch (boneIndexType) {

        case BT_HEAD:
            r =  {0,head_pos-30,0};
            break;

        case BT_HAIR:
            r = {-20,head_pos-15,-0.5};
            break;
        case BT_BODY:
            r = {-10,head_pos-47,0};
            break;

        case BT_LEG_L:
            r = {-13-2,head_pos-48,0};
            break;
        case BT_LEG_R:
            r = {-3-2,head_pos-48,0};
            break;
        case BT_HAND_L:
            r = {-10,head_pos-40,2.0};
            break;
        case BT_BOW_MAX:
            r = {-10+4*3,head_pos-20-4*3,1};
            break;

        case BT_ARROW:
            r = {-10+4*3,head_pos-20-4*3,1};
            break;
        default:
            assert(false);
            break;
    }
    return r;
}


void HuntingHero::op_toastShoot(float angle, float strenth) //瞄准完毕后的发射动作，发射后会放下弓
{
    const float shoot_time = 0.1;
    const float reset_time = 0.3;
    _dpxNode->configAction(BT_HEAD, help_calcBonePosition(BT_HEAD), {0,0,0}, 1.f*3,1.f*3, Sequence::create(DelayTime::create(shoot_time), Spawn::create(RotateTo::create(reset_time, {0,0,0}), MoveTo::create(reset_time, help_calcBonePosition(BT_HEAD)), NULL), NULL), false);

    _dpxNode->configAction(BT_HAIR, help_calcBonePosition(BT_HAIR), {0,0,0}, 0.9f*3,0.9f*3, Sequence::create(DelayTime::create(shoot_time), Spawn::create(RotateTo::create(reset_time, {0,0,0}), MoveTo::create(reset_time, help_calcBonePosition(BT_HAIR)), NULL), NULL), false);

    _dpxNode->configAction(BT_BODY, help_calcBonePosition(BT_BODY), {0,0,10}, 1.f*3,1.f*3, RepeatForever::create(Sequence::create( MoveBy::create(0.5, {0,-1,0}), MoveBy::create(0.5, {0,1,0}), NULL)), false);

    _dpxNode->configAction(BT_LEG_L, help_calcBonePosition(BT_LEG_L), {0,0,0}, 1.f*3,1.f*3, RepeatForever::create(Sequence::create( MoveBy::create(0.5, {0,0,0}), MoveBy::create(0.5, {0,0,0}), NULL)), false);

    _dpxNode->configAction(BT_LEG_R, help_calcBonePosition(BT_LEG_R), {0,0,0}, 1.f*3,1.f*3, RepeatForever::create(Sequence::create( MoveBy::create(0.5, {0,0,0}), MoveBy::create(0.5, {0,0,0}), NULL)), false);

    _dpxNode->configAction(BT_HAND_L, help_calcBonePosition(BT_HAND_L), {0,0,0}, 1.0f*3,1.0f*3,  Sequence::create(DelayTime::create(shoot_time), Spawn::create(RotateTo::create(reset_time, {0,0,0}), MoveTo::create(reset_time, help_calcBonePosition(BT_HAND_L)), NULL), NULL), false);

    _dpxNode->configAction(BT_BOW_MAX, help_calcBonePosition(BT_BOW_MAX), {0,0,100-45}, 3.f,  3.f,Sequence::create(ScaleTo::create(shoot_time, 3.f,3.f), Spawn::create(RotateTo::create(reset_time, {0,0,100-45}), MoveTo::create(reset_time, help_calcBonePosition(BT_BOW_MAX)), NULL), NULL), false);
    _dpxNode->configAction(BT_ARROW, help_calcBonePosition(BT_HEAD), {0,0,0}, 1.f*3,1.f*3, Sequence::create(DelayTime::create(shoot_time), Spawn::create(RotateTo::create(reset_time*0.01, {0,0,0}), MoveTo::create(reset_time*0.01, {0,0,-10000}), NULL), NULL), false);

    _mainCamera->scheduleOnce([this](float dt){ani_idle();}, 0.5f, "shoot to idle") ;

    _huntingArrowManageProtocal->op_shootArrow(huntingHeroType2huntingArrowType(_heroType, _heroGrade), _heroPositionType, angle, strenth);
}


void HuntingHero::op_configAiming(float angle, float strenth)
{
    auto emptyac = DelayTime::create(0);
    _dpxNode->configAction(BT_HEAD, help_calcBonePosition(BT_HEAD) + Vec3{-angle/6,0,0}, {0,0,-angle}, 1.f*3,1.f*3, emptyac->clone());

    _dpxNode->configAction(BT_HAIR, help_calcBonePosition(BT_HAIR), {0,0,0}, 0.9f*3,0.9f*3, emptyac->clone());

    _dpxNode->configAction(BT_BODY, help_calcBonePosition(BT_BODY), {0,0,10+(-angle)/2}, 1.f*3,1.f*3, emptyac->clone());


    _dpxNode->configAction(BT_LEG_L,help_calcBonePosition(BT_LEG_L), {0,0,0}, 1.f*3,1.f*3, emptyac->clone());

    _dpxNode->configAction(BT_LEG_R, help_calcBonePosition(BT_LEG_R), {0,0,0}, 1.f*3,1.f*3, emptyac->clone());

    _dpxNode->configAction(BT_HAND_L, help_calcBonePosition(BT_HAND_L), {0,0,-angle}, 1.0f*3,1.0f*3, emptyac->clone());

    _dpxNode->configAction(BT_BOW_MAX, help_calcBonePosition(BT_BOW_MAX), {0,0,45-angle-45}, 3.f*(1+strenth*0.8),3.f-strenth, emptyac->clone());
    _dpxNode->configAction(BT_ARROW, help_calcBonePosition(BT_ARROW), {0,0,45-angle}, 3.f,3.f, emptyac->clone());


}

void HuntingHero::ani_idle() //空闲动作
{
    //设置各 bone 的动作

    _dpxNode->configAction(BT_HEAD, help_calcBonePosition(BT_HEAD), {0,0,0}, 1.f*3,1.f*3, RepeatForever::create(Sequence::create( MoveBy::create(0.5, {0,2,0}), MoveBy::create(0.5, {0,-2,0}), NULL)));

    _dpxNode->configAction(BT_HAIR, help_calcBonePosition(BT_HAIR), {0,0,0}, 0.9f*3,0.9f*3, RepeatForever::create(Sequence::create( RotateBy::create(0.5, Vec3{0,0,-15}),RotateBy::create(0.5, Vec3{0,0,15}), NULL)));

    _dpxNode->configAction(BT_BODY, help_calcBonePosition(BT_BODY), {0,0,10}, 1.f*3,1.f*3, RepeatForever::create(Sequence::create( MoveBy::create(0.5, {0,-1,0}), MoveBy::create(0.5, {0,1,0}), NULL)));

    _dpxNode->configAction(BT_LEG_L, help_calcBonePosition(BT_LEG_L), {0,0,0}, 1.f*3,1.f*3, RepeatForever::create(Sequence::create( MoveBy::create(0.5, {0,0,0}), MoveBy::create(0.5, {0,0,0}), NULL)));

    _dpxNode->configAction(BT_LEG_R, help_calcBonePosition(BT_LEG_R), {0,0,0}, 1.f*3,1.f*3, RepeatForever::create(Sequence::create( MoveBy::create(0.5, {0,0,0}), MoveBy::create(0.5, {0,0,0}), NULL)));

    _dpxNode->configAction(BT_HAND_L, help_calcBonePosition(BT_HAND_L), {0,0,0}, 1.0f*3,1.0f*3, RepeatForever::create(Sequence::create( MoveBy::create(0.5, {0,2,0}), MoveBy::create(0.5, {0,-2,0}), NULL)));

    _dpxNode->configAction(BT_BOW_MAX, help_calcBonePosition(BT_BOW_MAX), {0,0,100-45}, 3.f,  3.f,RepeatForever::create(Sequence::create( MoveBy::create(0.5, {0,2,0}), MoveBy::create(0.5, {0,-2,0}), NULL)));

}

void HuntingHero::ani_run() //跑步动作
{
    //设置各 bone 的动作
    float head_pos = 70;

    const float run_time = 0.6;
    const float run_length = 8;
    const float angle_forward = 15;
    const float angle_back = 105;
    auto bodyMove =Sequence::create(MoveBy::create(run_time*0.5, {0,5,0}), MoveBy::create(run_time*0.5, {0,-5,0}), NULL);

    _dpxNode->configAction(BT_HEAD, help_calcBonePosition(BT_HEAD), {0,0,0}, 1.f*3, 1.f*3,RepeatForever::create(Sequence::create(MoveBy::create(run_time*0.5, {-2,6,0}), MoveBy::create(run_time*0.5, {2,-6,0}), NULL)));

    _dpxNode->configAction(BT_HAIR, help_calcBonePosition(BT_HAIR), {0,0,15}, 0.9f*3,0.9f*3, RepeatForever::create(Spawn::create( bodyMove->clone(), Sequence::create( RotateBy::create(run_time*0.5, Vec3{0,0,15}),RotateBy::create(run_time*0.5, Vec3{0,0,-15}), NULL), NULL)));

    _dpxNode->configAction(BT_BODY, help_calcBonePosition(BT_BODY), {0,0,10}, 1.f*3,1.f*3, RepeatForever::create(bodyMove->clone()));


    _dpxNode->configAction(BT_LEG_L, {-8,head_pos-48,0}, {0,0,-angle_forward}, 1.f*3,1.f*3, RepeatForever::create(Spawn::create(
                                                                                                            Sequence::create(                                                                                                                             MoveBy::create(run_time, {-run_length,0,0}), MoveBy::create(run_time, {run_length,0,0}), NULL),
                                                                                                            Sequence::create(                                                                                                                             RotateBy::create(run_time, {0,0,(angle_back+angle_forward)}), RotateBy::create(run_time, {0,0,-(angle_back+angle_forward)}), NULL)
                                                                                                            ,NULL)));

    _dpxNode->configAction(BT_LEG_R, {-8,head_pos-48,0}, {0,0,angle_back}, 1.f*3,1.f*3, RepeatForever::create(Spawn::create(
                                                                                                             Sequence::create(  MoveBy::create(run_time, {run_length,0,0}),                                                                                                                            MoveBy::create(run_time, {-run_length,0,0}), NULL),
                                                                                                             Sequence::create(   RotateBy::create(run_time, {0,0,-(angle_back+angle_forward)}),                                                                                                                           RotateBy::create(run_time, {0,0,(angle_back+angle_forward)}), NULL)
                                                                                                             ,NULL)));

    _dpxNode->configAction(BT_HAND_L, help_calcBonePosition(BT_HAND_L), {0,0,0}, 1.0f*3,1.0f*3, RepeatForever::create(Spawn::create( bodyMove->clone(), Sequence::create( RotateBy::create(run_time*0.5, Vec3{0,0,-35}),RotateBy::create(run_time*0.5, Vec3{0,0,35}), NULL), NULL)));

    _dpxNode->configAction(BT_BOW_MAX, help_calcBonePosition(BT_BOW_MAX), {0,0,100-45}, 3.f,3.f, RepeatForever::create(RepeatForever::create(Spawn::create( bodyMove->clone(), Sequence::create( RotateBy::create(run_time*0.5, Vec3{0,0,-15}),RotateBy::create(run_time*0.5, Vec3{0,0,15}), NULL), NULL))));
}

void HuntingHero::op_configPaused(bool pause)
{
    if (pause) {
        _dpxNode->pause();
    } else {
        _dpxNode->resume();
    }
}
