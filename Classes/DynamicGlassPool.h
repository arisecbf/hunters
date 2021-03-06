// (C) 2015 Turnro.com

#ifndef __Turnroll__DynamicGlassPool__
#define __Turnroll__DynamicGlassPool__

#include "PixelPlane.h"

class DynamicGlassPool
{
public:
    // 注意glass的生命是由layer托管的，特别是其内存关系。
    void init(cocos2d::Layer* layer, cocos2d::Camera* camera);

    // brrow/giveback时会设置好visibity，其它的参数需要用户自己设置。
    PixelPlane* brrow();
    void giveback(PixelPlane* glass);

private:
    int _zorderIndex = 10000;
    cocos2d::Layer* _layer;//glass的宿主
    cocos2d::Camera* _camera;//为了camera mask
    static const int N = 64;
    std::vector<PixelPlane*> _idleGlasses;
    PixelPlane* genIdleGlass();
};

#endif /* defined(__Turnroll__DynamicGlassPool__) */
