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
#include "CCGeometry.h"
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
    
    float _minScaleX;
    float _maxScaleX;
    float _minScaleY;
    float _maxScaleY;
    Rect _boundingArea;
    
    float _cameraCenterX;
    float _cameraCenterY;
    
    float _gapTranslateX;
    float _gapTranslateY;
    float _gapScaleX;
    float _gapScaleY;
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
    void adjustValue(float &value, const float &min, const float &max);
    void adjustIncValue(float &inc, const float &value, const float &min, const float &max);
    void adjustTargetOffset(float &targetOffsetX, float &targetOffsetY);
    
    public:
    EffectCamera();
    
    /** init effect camera parameters
     *  @param originalX              Node's original x position
     *  @param originalY              Node's original y position
     *  @param originalAnchorPointX   Node's original x anchor point (0-1)
     *  @param originalAnchorPointY   Node's original y anchor point (0-1)
     *  @param originalWidth          Node's content's original with
     *  @param originalHeight         Node's content's original height
     *  @param minScaleX              minimum scale to x (default 0.1)
     *  @param maxScaleX              maximum scale to x (default 0.1)
     *  @param minScaleY              minimum scale to y (default 2)
     *  @param maxScaleY              maximum scale to y (default 2)
     *  @param checkOffset            check node's position or not, if true should provide boundgingArea (default false)
     *  @param boundingArea           node's movement area (default CCRectZero)
     *
     */
    void initWithParams(float originalX, float originalY,
                        float originalAnchorPointX, float originalAnchorPointY,
                        float originalWidth, float originalHeight,
                        float minScaleX=0.1, float maxScaleX=2,
                        float minScaleY=0.1, float maxScaleY=2,
                        bool checkOffset=false, Rect boundingArea=Rect::ZERO);
    
    void visit();
    
    ~EffectCamera();
    
    void calculateCameraCenter(float touchInScreenX, float touchInScreenY);
    
    void scrollTo(float offsetX, float offsetY, int scrollDuration=1);
    
    void scrollToInc(float incOffsetX, float incOffsetY, int scrollDuration=1);
    
    void scaleTo(float scaleX, float scaleY);
    
    void scaleToInc(float incScaleX, float incScaleY);
    
    void rotateTo(float rotation);
    
    void rotateToInc(float incRotation);
    
    void shake(float peak, int duration);
    
    void reset();
};

NS_CC_END

#endif
