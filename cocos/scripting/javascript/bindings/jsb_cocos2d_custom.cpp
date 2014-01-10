//
//  jsb_cocos2d_custom.cpp
//  cocos2d_libs
//
//  Created by YangWenXin on 14-1-10.
//
//

#include "jsb_cocos2d_custom.h"
#include "cocos2d.h"
#include "cocos2d_specifics.hpp"

JSClass *jsb_String_class;
JSObject *jsb_String_prototype;

JSBool js_cocos2dx_String_constructor(JSContext *cx, uint32_t argc, jsval *vp);
void js_cocos2dx_String_finalize(JSContext *cx, JSObject *obj);
JSBool js_cocos2dx_String_createWithContentsOfFile(JSContext *cx, uint32_t argc, jsval *vp);

static JSBool empty_constructor(JSContext *cx, uint32_t argc, jsval *vp) {
	return JS_FALSE;
}

void js_cocos2dx_String_finalize(JSFreeOp *fop, JSObject *obj) {
    CCLOGINFO("jsbindings: finalizing JS object %p (String)", obj);
}

void js_register_cocos2dx_String(JSContext *cx, JSObject *global) {
	jsb_String_class = (JSClass *)calloc(1, sizeof(JSClass));
	jsb_String_class->name = "String";
	jsb_String_class->addProperty = JS_PropertyStub;
	jsb_String_class->delProperty = JS_DeletePropertyStub;
	jsb_String_class->getProperty = JS_PropertyStub;
	jsb_String_class->setProperty = JS_StrictPropertyStub;
	jsb_String_class->enumerate = JS_EnumerateStub;
	jsb_String_class->resolve = JS_ResolveStub;
	jsb_String_class->convert = JS_ConvertStub;
	jsb_String_class->finalize = js_cocos2dx_String_finalize;
	jsb_String_class->flags = JSCLASS_HAS_RESERVED_SLOTS(2);
    
	static JSPropertySpec properties[] = {
		{0, 0, 0, JSOP_NULLWRAPPER, JSOP_NULLWRAPPER}
	};
    
	static JSFunctionSpec funcs[] = {
        JS_FS_END
	};
    
	static JSFunctionSpec st_funcs[] = {
		JS_FN("createWithContentsOfFile", js_cocos2dx_String_createWithContentsOfFile, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FS_END
	};
    
	jsb_String_prototype = JS_InitClass(
                                                 cx, global,
                                                 NULL, // parent proto
                                                 jsb_String_class,
                                                 empty_constructor, 0,
                                                 properties,
                                                 funcs,
                                                 NULL, // no static properties
                                                 st_funcs);
	// make the class enumerable in the registered namespace
	JSBool found;
	JS_SetPropertyAttributes(cx, global, "String", JSPROP_ENUMERATE | JSPROP_READONLY, &found);
    
	// add the proto and JSClass to the type->js info hash table
	TypeTest<String> t;
	js_type_class_t *p;
	long typeId = t.s_id();
	if (_js_global_type_map.find(typeId) == _js_global_type_map.end())
	{
		p = (js_type_class_t *)malloc(sizeof(js_type_class_t));
		p->jsclass = jsb_String_class;
		p->proto = jsb_String_prototype;
		p->parentProto = NULL;
		_js_global_type_map.insert(std::make_pair(typeId, p));
	}
}

JSBool js_cocos2dx_String_createWithContentsOfFile(JSContext *cx, uint32_t argc, jsval *vp) {
	jsval *argv = JS_ARGV(cx, vp);
	JSBool ok = JS_TRUE;
    if (argc == 1) {
        std::string arg0;
        ok &= jsval_to_std_string(cx, argv[0], &arg0);
        JSB_PRECONDITION2(ok, cx, JS_FALSE, "js_cocos2dx_String_createWithContentsOfFile : Error processing arguments");
        cocos2d::String *ret = cocos2d::String::createWithContentsOfFile(arg0.c_str());
        jsval jsret;
        jsret = c_string_to_jsval(cx, ret->getCString());
        JS_SET_RVAL(cx, vp, jsret);
        return JS_TRUE;
    }
    JS_ReportError(cx, "js_cocos2dx_String_createWithContentsOfFile : wrong number of arguments");
	return JS_FALSE;
}

void register_all_cocos2dx_custom(JSContext* cx, JSObject* obj) {
	// first, try to get the ns
	JS::RootedValue nsval(cx);
	JSObject *ns;
	JS_GetProperty(cx, obj, "cc", &nsval);
	if (nsval == JSVAL_VOID) {
		ns = JS_NewObject(cx, NULL, NULL, NULL);
		nsval = OBJECT_TO_JSVAL(ns);
		JS_SetProperty(cx, obj, "cc", nsval);
	} else {
		JS_ValueToObject(cx, nsval, &ns);
	}
	obj = ns;
    
	js_register_cocos2dx_String(cx, obj);
}