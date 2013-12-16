//
//  EffectCamera.cpp
//  cocos2dx
//
//  Created by YangWenXin on 13-12-9.
//  Copyright (c) 2013年 cocos2d-x. All rights reserved.
//

#include "EffectCamera.h"
#include "kazmath/GL/matrix.h"
#include <stdlib.h>

NS_CC_BEGIN

EffectCamera::EffectCamera() {
    init();
}

bool EffectCamera::init() {
    _originalX = 0;
    _originalY = 0;
    _originalAnchorPointX = 0.5;
    _originalAnchorPointY = 0.5;
    _originalWidth = 0;
    _originalHeight = 0;
    
    _currentOffsetX = 0;
    _currentOffsetY = 0;
    _currentScaleX = 1;
    _currentScaleY = 1;
    _currrentRotation = 0;
    
    _targetOffsetX = 0;
    _targetOffsetY = 0;
    _targetRotation = 0;
    _targetScaleX = 1;
    _targetScaleY = 1;
    
    _cameraCenterX = -1;
    _cameraCenterY = -1;
    _dirtyCameraCenter = false;
    _checkOffset = false;
    _isShaking = false;
    
    kmMat4Identity(&_transformMatrix);
    kmMat4Identity(&_mTranslate);
    kmMat4Identity(&_mScale);
    kmMat4Identity(&_mRotation);
    kmMat4Identity(&_mTemp);
    
    return true;
}

EffectCamera::~EffectCamera() {
    
}

void EffectCamera::initWithParams(float originalX, float originalY, float originalAnchorPointX, float originalAnchorPointY, float originalWidth, float originalHeight, bool checkOffset) {
    _originalX = originalX;
    _originalY = originalY;
    _originalAnchorPointX = originalAnchorPointX;
    _originalAnchorPointY = originalAnchorPointY;
    _originalWidth = originalWidth;
    _originalHeight = originalHeight;
    _checkOffset = checkOffset;
}

void EffectCamera::visit() {
    _dirtyR = false;
    _dirtyS = false;
    _dirtyT = false;
    if (_isShaking) {
        if (_duration > 0) {
            srand((unsigned)time(NULL));
            _currentOffsetX = _targetOffsetX + rand()%((int)_peak+1) - _peak/2;
            _currentOffsetY = _targetOffsetY + rand()%((int)_peak+1) - _peak/2;
            _peak -= _peakReduceGap;
            _duration--;
        }
        else {
            _isShaking = false;
            _currentOffsetX = _targetOffsetX;
            _currentOffsetY = _targetOffsetY;
        }
        _dirtyT = true;
    }
    else {
        // scale
        adjustGap(_currentScaleX, _targetScaleX, _gapScale, _dirtyS);
        adjustGap(_currentScaleY, _targetScaleY, _gapScale, _dirtyS);
        // translate
        adjustGap(_currentOffsetX, _targetOffsetX, _gapTranslateX, _dirtyT);
        adjustGap(_currentOffsetY, _targetOffsetY, _gapTranslateY, _dirtyT);
        // rotation
        adjustGap(_currrentRotation, _targetRotation, _gapRotation, _dirtyR);
    }
    if (_dirtyT || _dirtyR || _dirtyS) {
        kmMat4Translation(&_mTranslate, _currentOffsetX, _currentOffsetY, 0);
        kmMat4Scaling(&_mScale, _currentScaleX, _currentScaleY, 0);
        kmMat4RotationZ(&_mRotation, _currrentRotation);
        if (!_dirtyR) {
            kmMat4Multiply(&_transformMatrix, &_mTranslate, &_mScale);
        }
        else {
            kmMat4Multiply(&_mTemp, &_mTranslate, &_mScale);
            kmMat4Multiply(&_transformMatrix, &_mTemp, &_mRotation);
        }
    }
    kmGLMultMatrix(&_transformMatrix);
}

void EffectCamera::calculateCameraCenter(float touchInScreenX, float touchInScreenY) {
    float leftBottomX, leftBottomY, rightTopX, rightTopY;
    leftBottomX = _originalX + _currentOffsetX - _currentScaleX*_originalWidth*_originalAnchorPointX;
    leftBottomY = _originalY + _currentOffsetY - _currentScaleY*_originalHeight*_originalAnchorPointY;
    rightTopX = _originalX + _currentOffsetX + _currentScaleX*_originalWidth*(1-_originalAnchorPointX);
    rightTopY = _originalY + _currentOffsetY + _currentScaleY*_originalHeight*(1-_originalAnchorPointY);
    if (touchInScreenX < leftBottomX) {
        touchInScreenX = leftBottomX;
    }
    else if (touchInScreenX > rightTopX) {
        touchInScreenX = rightTopX;
    }
    if (touchInScreenY < leftBottomY) {
        touchInScreenY = leftBottomY;
    }
    else if (touchInScreenY > rightTopY) {
        touchInScreenY = rightTopY;
    }
    _cameraCenterX = (touchInScreenX - leftBottomX)/(_currentScaleX*_originalWidth);
    _cameraCenterY = (touchInScreenY - leftBottomY)/(_currentScaleY*_originalHeight);
    _dirtyCameraCenter = true;
}

void EffectCamera::adjustGap(float &src, float &dst, const float &gap, bool &dirty) {
    if (src != dst) {
        dirty = true;
        if (fabsf(src - dst) <= gap) {
            src = dst;
        }
        else if (src < dst) {
            src += gap;
        }
        else {
            src -= gap;
        }
    }
}

void EffectCamera::scrollTo(float offsetX, float offsetY, int scrollDuration) {
    _gapTranslateX = fabsf(offsetX-_targetOffsetX)/scrollDuration;
    _gapTranslateY = fabsf(offsetY-_targetOffsetY)/scrollDuration;
    _targetOffsetX = offsetX;
    _targetOffsetY = offsetY;
}

void EffectCamera::scrollToInc(float incOffsetX, float incOffsetY, int scrollDuration) {
    _gapTranslateX = fabsf(incOffsetX)/scrollDuration;
    _gapTranslateY = fabsf(incOffsetY)/scrollDuration;
    _targetOffsetX += incOffsetX;
    _targetOffsetY += incOffsetY;
}

void EffectCamera::scaleTo(float scaleX, float scaleY) {
    _gapScale = fabsf(scaleX-_targetScaleX);
    if (_dirtyCameraCenter) {
        scrollToInc(-(scaleX-_targetScaleX)*(_cameraCenterX-_originalAnchorPointX)*_originalWidth,
                    -(scaleY-_targetScaleY)*(_cameraCenterY-_originalAnchorPointY)*_originalHeight, 1);
        _dirtyCameraCenter = false;
    }
    _targetScaleX = scaleX;
    _targetScaleY = scaleY;
}

void EffectCamera::scaleToInc(float incScaleX, float incScaleY) {
    _gapScale = fabsf(incScaleX);
    _targetScaleX += incScaleX;
    _targetScaleY += incScaleY;
    if (_dirtyCameraCenter) {
        scrollToInc(-incScaleX*(_cameraCenterX-_originalAnchorPointX)*_originalWidth,
                    -incScaleY*(_cameraCenterY-_originalAnchorPointY)*_originalHeight, 1);
        _dirtyCameraCenter = false;
    }
}

void EffectCamera::rotateTo(float rotation) {
    _gapRotation = fabsf(rotation-_targetRotation);
    if (_dirtyCameraCenter) {
        float lengthX = (_cameraCenterX-_originalAnchorPointX)*_originalWidth*_targetScaleX;
        float lengthY = (_cameraCenterY-_originalAnchorPointY)*_originalHeight*_targetScaleY;
        float length = sqrtf(lengthX*lengthX + lengthY*lengthY);
        float angle1 = _targetRotation - rotation;
        float angle2 = atan2f((_originalAnchorPointY-_cameraCenterY)*_originalHeight*_targetScaleY, (_originalAnchorPointX-_cameraCenterX)*_originalWidth*_targetScaleX);
        scrollToInc(length*(cosf(angle1+angle2)-cosf(angle2)),
                    length*(sinf(angle1+angle2)-sinf(angle2)), 1);
        _dirtyCameraCenter = false;
    }
    _targetRotation = rotation;
}

void EffectCamera::rotateToInc(float incRotation) {
    _gapRotation = fabsf(incRotation);
    _targetRotation += incRotation;
    if (_dirtyCameraCenter) {
        float lengthX = (_cameraCenterX-_originalAnchorPointX)*_originalWidth*_targetScaleX;
        float lengthY = (_cameraCenterY-_originalAnchorPointY)*_originalHeight*_targetScaleY;
        float length = sqrtf(lengthX*lengthX + lengthY*lengthY);
        float angle1 = incRotation;
        float angle2 = atan2f((_originalAnchorPointY-_cameraCenterY)*_originalHeight*_targetScaleY, (_originalAnchorPointX-_cameraCenterX)*_originalWidth*_targetScaleX);
        scrollToInc(length*(cosf(angle1+angle2)-cosf(angle2)),
                    length*(sinf(angle1+angle2)-sinf(angle2)), 1);
        _dirtyCameraCenter = false;
    }
}

void EffectCamera::shake(float peak, int duration) {
    _isShaking = true;
    _peak = peak;
    _duration = duration;
    _peakReduceGap = _peak/_duration;
}

void EffectCamera::reset() {
    _peak = 0;
    _duration = 0;
    _isShaking = false;
}

NS_CC_END