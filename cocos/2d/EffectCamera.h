//
//  EffectCamera.h
//  cocos2dx
//
//  Created by YangWenXin on 13-12-9.
//  Copyright (c) 2013年 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_EffectCamera_h
#define cocos2dx_EffectCamera_h

#include "CCObject.h"
#include "kazmath/mat4.h"

NS_CC_BEGIN

class EffectCamera : public Object {
    private:
    float _originalX;
    float _originalY;
    float _originalAnchorPointX;
    float _originalAnchorPointY;
    float _originalWidth;
    float _originalHeight;
    
    float _currentOffsetX;
    float _currentOffsetY;
    float _currentScaleX;
    float _currentScaleY;
    float _currrentRotation;
    
    float _targetOffsetX;
    float _targetOffsetY;
    float _targetScaleX;
    float _targetScaleY;
    float _targetRotation;
    
    float _cameraCenterX;
    float _cameraCenterY;
    
    float _gapTranslateX;
    float _gapTranslateY;
    float _gapScale;
    float _gapRotation;
    
    /** checks position in case leaving blank area in sreen, like COC */
    bool _checkOffset;
    
    /** is camera shaking */
    bool _isShaking;
    float _peak;
    int _duration;
    float _peakReduceGap;
    
    kmMat4 _transformMatrix;
    kmMat4 _mTranslate;
    kmMat4 _mScale;
    kmMat4 _mRotation;
    kmMat4 _mTemp;
    
    bool _dirtyT;
    bool _dirtyS;
    bool _dirtyR;
    bool _dirtyCameraCenter;
    
    bool init();
    void adjustGap(float &src, float &dst, const float &gap, bool &dirty);
    
    public:
    EffectCamera();
    
    /** init effect camera, if you don't enable checkOffset feature, this function must not be called
     *  @originalX
     *  @originalY
     *  @originalAnchorPointX
     *  @originalAnchorPointY
     *  @originalWidth
     *  @originalHeight
     *  @checkOffset
     *
     */
    void initWithParams(float originalX, float originalY, float originalAnchorPointX, float originalAnchorPointY, float originalWidth, float originalHeight, bool checkOffset);
    
    void visit();
    
    ~EffectCamera();
    
    void calculateCameraCenter(float touchInScreenX, float touchInScreenY);
    
    void scrollTo(float offsetX, float offsetY, int scrollDuration=5);
    
    void scrollToInc(float incOffsetX, float incOffsetY, int scrollDuration=10);
    
    void scaleTo(float scaleX, float scaleY);
    
    void scaleToInc(float incScaleX, float incScaleY);
    
    void rotateTo(float rotation);
    
    void rotateToInc(float incRotation);
    
    void shake(float peak, int duration);
    
    void reset();
};

NS_CC_END

#endif
