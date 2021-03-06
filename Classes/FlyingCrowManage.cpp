//
//  FlyingCrowManage.cpp
//  hunters
//
//  Created by chenbingfeng on 15/12/7.
//
//

#include "FlyingCrowManage.hpp"
USING_NS_CC;


void FlyingCrowManage::init(cocos2d::Layer *mainLayer, cocos2d::Camera *mainCamera)
{
    _mainLayer = mainLayer;
    _mainCamera = mainCamera;
}

void FlyingCrowManage::op_dealCollision(ArrowUnit& arrow)
{
    for (auto iter = _crows.begin(); iter != _crows.end(); ) {
        if ((*iter)->op_dealWithArrow(arrow) ){
            iter = _crows.erase(iter);
        } else {
            iter++;
        }
    }
}

void FlyingCrowManage::update(float dt)
{
    _timeLeft -= dt;
    if (_timeLeft <= 0) {
        addBatchOfCrows();
        _batchCount++;
        _timeLeft = random(25.f, 55.f);
    }
    for (auto crow : _crows) {
        crow->update(dt);
    }
    for (auto iter = _crows.begin(); iter != _crows.end(); ) {
        if ((*iter)->isDead()) {
            iter = _crows.erase(iter);
        } else {
            iter++;
        }
    }
}

static const bool _X_  = true;
static const bool ___  = false;
struct Pat
{
    const bool* pa;
    int h;
    int w;
};


static const bool A_0[1*5] = {
    _X_,___,_X_,___,_X_,
};

static const bool A_1[1*5] = {
    _X_,___,_X_,___,___,
};

static const bool A_2[1*5] = {
    _X_,___,___,_X_,_X_,
};

static const bool A_3[1*5] = {
    ___,_X_,_X_,___,_X_,
};

static Pat PatsA[4] = {
    {A_0, 1, 5},
    {A_1, 1, 5},
    {A_2, 1, 5},
    {A_3, 1, 5},
};

static const bool B_0[2*5] = {
    _X_,___,___,___,_X_,
    ___,___,_X_,___,___,
};

static const bool B_1[2*5] = {
    ___,___,_X_,___,___,
    _X_,___,___,___,_X_,
};

static const bool B_3[2*5] = {
    ___,_X_,___,_X_,___,
    _X_,___,_X_,___,_X_,
};

static const bool B_4[2*5] = {
    _X_,_X_,_X_,___,___,
    ___,___,___,_X_,_X_,
};

static const bool B_5[2*5] = {
    _X_,___,_X_,___,___,
    ___,_X_,___,_X_,___,
};

static const bool B_2[2*5] = {
    ___,_X_,___,_X_,___,
    _X_,___,___,___,_X_,
};

static Pat PatsB[6] = {
    {B_0, 2, 5},
    {B_1, 2, 5},
    {B_2, 2, 5},
    {B_3, 2, 5},
    {B_4, 2, 5},
    {B_5, 2, 5},
};
static const bool C_0[3*6] = {
    _X_,___,_X_,___,___,___,
    ___,_X_,___,_X_,___,___,
    ___,___,_X_,___,_X_,___,
};

static const bool C_1[3*6] = {
    _X_,___,_X_,___,_X_,___,
    ___,_X_,___,_X_,___,_X_,
    _X_,___,_X_,___,_X_,___,
};

static const bool C_2[3*6] = {
    ___,___,_X_,___,_X_,___,
    ___,_X_,___,_X_,___,___,
    _X_,___,_X_,___,___,___,
};

static Pat PatsC[3] = {
    {C_0, 3, 6},
    {C_1, 3, 6},
    {C_2, 3, 6},
};

void FlyingCrowManage::addBatchOfCrows()
{
    float radioOfA = 0.6f;
    float radioOfB = 0.15f;
    float r = rand_0_1();
    Pat p;
    if (r < radioOfA) {
        p = PatsA[random(0, 4)];
    } else if (r < radioOfA + radioOfB) {
        p = PatsB[random(0, 6)];
    } else {
        p = PatsC[random(0, 3)];
    }
    bool first = true;
    for (int x = 0; x < p.w; x++) {
        for (int y = 0; y < p.h; y++) {
            if (p.pa[y*p.w + x]) {
                addCrow(x, y, p.h, first);
                first = false;
            }
        }
    }

}

void FlyingCrowManage::addCrow(int x, int y, int h, bool cry)
{
    float rAcce = 0.05f + 0.25f*std::min(1.f, (_batchCount*1.f/20));
    float rShield = 0.05f + 0.4f*std::min(1.f, (_batchCount*1.f/20));

    bool acce = rand_0_1() < rAcce;
    bool shield = rand_0_1() < rShield;

    auto sp = FlyingCrow::create();
    sp->init(_mainLayer, _mainCamera);
    sp->configProtocals(_topIconsProtocal, _particleManageProtocal);
    sp->op_config(acce, shield, Vec2{x*20.f, (y-h*1.f/2)*20.f});
    if (cry) sp->op_configCry();
    _crows.push_back(sp);

}

void FlyingCrowManage::op_configPaused(bool pause)
{
    for (auto& c : _crows) {
        c->op_configPaused(pause);
    }
}