//
//  jsb_cocos2d_custom.h
//  cocos2d_libs
//
//  Created by YangWenXin on 14-1-10.
//
//

#ifndef cocos2d_libs_jsb_cocos2d_custom_h
#define cocos2d_libs_jsb_cocos2d_custom_h

#include "jsapi.h"
#include "jsfriendapi.h"


extern JSClass  *jsb_String_class;
extern JSObject *jsb_String_prototype;

void register_all_cocos2dx_custom(JSContext* cx, JSObject* obj);

#endif
