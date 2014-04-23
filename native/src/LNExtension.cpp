//
//  LNExtension.cpp
//  LeapNative
//
//  Created by Wouter Verweirder on 01/02/13.
//  Copyright (c) 2013 Wouter Verweirder. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "LNExtension.h"
#include "LNLeapDevice.h"
#include "LNFREUtilities.h"

extern "C" {
    
    FREObject LeapNative_isSupported(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		std::cout << "LeapNative_isSupported" << std::endl;
		FREObject isSupported;
		FRENewObjectFromBool(1, &isSupported);
		return isSupported;
	}
    
    FREObject LeapNative_getFrame(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        return device->getFrame();
    }
    
    FREObject LeapNative_enableGesture(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        int gestureClassType;
        FREGetObjectAsInt32(argv[0], &gestureClassType);
        
        bool gestureEnabled = createBoolFromFREObject(argv[1]);
        
        std::cout << "LeapNative_enableGesture called: " << gestureClassType << std::endl;
        
        switch (gestureClassType) {
            case 5:
                device->controller->enableGesture(Gesture::TYPE_SWIPE, gestureEnabled);
                break;
            case 6:
                device->controller->enableGesture(Gesture::TYPE_CIRCLE, gestureEnabled);
                break;
            case 7:
                device->controller->enableGesture(Gesture::TYPE_SCREEN_TAP, gestureEnabled);
                break;
            case 8:
                device->controller->enableGesture(Gesture::TYPE_KEY_TAP, gestureEnabled);
                break;
            default:
                std::cout << "LeapNative_enableGesture: invalid argument" << std::endl;
                break;
        }
        return NULL;
    }
    
    FREObject LeapNative_isGestureEnabled(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        int gestureClassType;
        FREGetObjectAsInt32(argv[0], &gestureClassType);
        
        bool gestureEnabled;

        switch (gestureClassType) {
            case 5:
                gestureEnabled = device->controller->isGestureEnabled(Gesture::TYPE_SWIPE);
                break;
            case 6:
                gestureEnabled = device->controller->isGestureEnabled(Gesture::TYPE_CIRCLE);
                break;
            case 7:
                gestureEnabled = device->controller->isGestureEnabled(Gesture::TYPE_SCREEN_TAP);
                break;
            case 8:
                gestureEnabled = device->controller->isGestureEnabled(Gesture::TYPE_KEY_TAP);
                break;
            default:
                std::cout << "LeapNative_isGestureEnabled: invalid argument" << std::endl;
                gestureEnabled = false;
                break;
        }
        
        return createFREObjectForBool(gestureEnabled);
    }
    
    FREObject LeapNative_hasFocus(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        return device->hasFocus();
    }
    
    FREObject LeapNative_getClosestScreenHitPointable(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        int pointableId;
        FREGetObjectAsInt32(argv[0], &pointableId);
        
        return device->getClosestScreenHitPointable(pointableId);
    }
    
    FREObject LeapNative_getClosestScreenHit(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        double pX;
        FREGetObjectAsDouble(argv[0], &pX);
        
        double pY;
        FREGetObjectAsDouble(argv[1], &pY);
        
        double pZ;
        FREGetObjectAsDouble(argv[2], &pZ);
        
        double dX;
        FREGetObjectAsDouble(argv[3], &dX);
        
        double dY;
        FREGetObjectAsDouble(argv[4], &dY);
        
        double dZ;
        FREGetObjectAsDouble(argv[5], &dZ);
        
        Vector position = Vector((float) pX, (float) pY, (float) pZ);
        Vector direction = Vector((float) dX, (float) dY, (float) dZ);
        
        return device->getClosestScreenHit(position, direction);
    }

    //start screen class
    FREObject LeapNative_getScreenDistanceToPoint(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);

        int screenId;
        double pX;
        double pY;
        double pZ;
        FREGetObjectAsInt32(argv[0], &screenId);
        FREGetObjectAsDouble(argv[1], &pX);
        FREGetObjectAsDouble(argv[2], &pY);
        FREGetObjectAsDouble(argv[3], &pZ);
        
        Vector point = Vector((float) pX, (float) pY, (float) pZ);

        return device->getScreenDistanceToPoint(screenId, point);
    }
    
    FREObject LeapNative_getScreenHeightPixels(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        int screenId;
        FREGetObjectAsInt32(argv[0], &screenId);
        
        return device->getScreenHeightPixels(screenId);
    }
    
    FREObject LeapNative_getScreenWidthPixels(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        int screenId;
        FREGetObjectAsInt32(argv[0], &screenId);
        
        return device->getScreenWidthPixels(screenId);
    }
    
    FREObject LeapNative_getScreenHorizontalAxis(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        int screenId;
        FREGetObjectAsInt32(argv[0], &screenId);
        
        return device->getScreenHorizontalAxis(screenId);
    }
    
    FREObject LeapNative_getScreenVerticalAxis(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        int screenId;
        FREGetObjectAsInt32(argv[0], &screenId);
        
        return device->getScreenVerticalAxis(screenId);
    }
    
    FREObject LeapNative_getScreenBottomLeftCorner(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        int screenId;
        FREGetObjectAsInt32(argv[0], &screenId);
        
        return device->getScreenBottomLeftCorner(screenId);
    }
    
    FREObject LeapNative_getScreenIntersect(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        int screenId;
        FREGetObjectAsInt32(argv[0], &screenId);
        
        double pX;
        FREGetObjectAsDouble(argv[1], &pX);
        
        double pY;
        FREGetObjectAsDouble(argv[2], &pY);
        
        double pZ;
        FREGetObjectAsDouble(argv[3], &pZ);
        
        double dX;
        FREGetObjectAsDouble(argv[4], &dX);
        
        double dY;
        FREGetObjectAsDouble(argv[5], &dY);
        
        double dZ;
        FREGetObjectAsDouble(argv[6], &dZ);
        
        bool normalize = createBoolFromFREObject(argv[7]);
        
        double clampRatio;
        FREGetObjectAsDouble(argv[8], &clampRatio);
        
        Vector position = Vector((float) pX, (float) pY, (float) pZ);
        Vector direction = Vector((float) dX, (float) dY, (float) dZ);
        
        return device->getScreenIntersect(screenId, position, direction, normalize, (float) clampRatio);
    }
    
    FREObject LeapNative_getScreenProject(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        int screenId;
        FREGetObjectAsInt32(argv[0], &screenId);
        
        double pX;
        FREGetObjectAsDouble(argv[1], &pX);
        
        double pY;
        FREGetObjectAsDouble(argv[2], &pY);
        
        double pZ;
        FREGetObjectAsDouble(argv[3], &pZ);
        
        bool normalize = createBoolFromFREObject(argv[4]);
        
        double clampRatio;
        FREGetObjectAsDouble(argv[5], &clampRatio);
        
        Vector position = Vector((float) pX, (float) pY, (float) pZ);
        
        return device->getScreenProject(screenId, position, normalize, (float) clampRatio);
    }
    
    FREObject LeapNative_getScreenIsValid(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        int screenId;
        FREGetObjectAsInt32(argv[0], &screenId);
        
        return device->getScreenIsValid(screenId);
    }
    
    FREObject LeapNative_getScreenNormal(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        int screenId;
        FREGetObjectAsInt32(argv[0], &screenId);
        
        return device->getScreenNormal(screenId);
    }
    //end screen class
    
    //start config class
    FREObject LeapNative_getConfigBool(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        uint32_t len;
        const uint8_t* key = 0;
        FREGetObjectAsUTF8(argv[0], &len, &key);
        
        return device->getConfigBool(len, key);
    }

    FREObject LeapNative_getConfigFloat(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        uint32_t len;
        const uint8_t* key = 0;
        FREGetObjectAsUTF8(argv[0], &len, &key);
        
        return device->getConfigFloat(len, key);
    }
    
    FREObject LeapNative_getConfigInt32(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        uint32_t len;
        const uint8_t* key = 0;
        FREGetObjectAsUTF8(argv[0], &len, &key);
        
        return device->getConfigInt32(len, key);
    }
    
    FREObject LeapNative_getConfigString(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        uint32_t len;
        const uint8_t* key = 0;
        FREGetObjectAsUTF8(argv[0], &len, &key);
        
        return device->getConfigString(len, key);
    }

    FREObject LeapNative_getConfigType(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        uint32_t len;
        const uint8_t* key = 0;
        FREGetObjectAsUTF8(argv[0], &len, &key);
        
        return device->getConfigType(len, key);
    }
    
    FREObject LeapNative_setConfigBool(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        uint32_t len;
        const uint8_t* key = 0;
        FREGetObjectAsUTF8(argv[0], &len, &key);
        
        uint32_t value;
        FREGetObjectAsBool(argv[1], &value);
        
        return device->setConfigBool(len, key, (value != 0));
    }
    
    FREObject LeapNative_setConfigFloat(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        uint32_t len;
        const uint8_t* key = 0;
        FREGetObjectAsUTF8(argv[0], &len, &key);
        
        double value;
        FREGetObjectAsDouble(argv[1], &value);
        
        return device->setConfigFloat(len, key, (float)value);
    }
    
    FREObject LeapNative_setConfigString(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        uint32_t len;
        const uint8_t* key = 0;
        FREGetObjectAsUTF8(argv[0], &len, &key);
        
        uint32_t valueLen;
        const uint8_t* valueArray = 0;
        FREGetObjectAsUTF8(argv[1], &valueLen, &valueArray);
        
        
        return device->setConfigString(len, key, valueLen, valueArray);
    }
    
    FREObject LeapNative_setConfigSave(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        return device->setConfigSave();
    }
    //end config class
    
    //start policy
    FREObject LeapNative_setPolicyFlags(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        uint32_t flags;
        FREGetObjectAsUint32(argv[0], &flags);
        
        return device->setPolicyFlags(flags);
    }
    FREObject LeapNative_getPolicyFlags(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        return device->getPolicyFlags();
    }
    //end policty
    
    //start frame class
    FREObject LeapNative_frameRotationProbability(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        int frameId;
        FREGetObjectAsInt32(argv[0], &frameId);
        
        int sinceFrameId;
        FREGetObjectAsInt32(argv[1], &sinceFrameId);
        
        return device->frameProbability(frameId, sinceFrameId, 0);
    }
    
    FREObject LeapNative_frameScaleProbability(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        int frameId;
        FREGetObjectAsInt32(argv[0], &frameId);
        
        int sinceFrameId;
        FREGetObjectAsInt32(argv[1], &sinceFrameId);
        
        return device->frameProbability(frameId, sinceFrameId, 1);
    }
    
    FREObject LeapNative_frameTranslationProbability(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        int frameId;
        FREGetObjectAsInt32(argv[0], &frameId);
        
        int sinceFrameId;
        FREGetObjectAsInt32(argv[1], &sinceFrameId);
        
        return device->frameProbability(frameId, sinceFrameId, 2);
    }
    //end frame class
    
    //start hand class
    FREObject LeapNative_handRotationProbability(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        int handId;
        FREGetObjectAsInt32(argv[0], &handId);
        
        int frameId;
        FREGetObjectAsInt32(argv[1], &frameId);
        
        int sinceFrameId;
        FREGetObjectAsInt32(argv[2], &sinceFrameId);
        
        return device->handProbability(handId, frameId, sinceFrameId, 0);
    }
    
    FREObject LeapNative_handScaleProbability(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        int handId;
        FREGetObjectAsInt32(argv[0], &handId);
        
        int frameId;
        FREGetObjectAsInt32(argv[1], &frameId);
        
        int sinceFrameId;
        FREGetObjectAsInt32(argv[2], &sinceFrameId);
        
        return device->handProbability(handId, frameId, sinceFrameId, 1);
    }
    
    FREObject LeapNative_handTranslationProbability(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        int handId;
        FREGetObjectAsInt32(argv[0], &handId);
        
        int frameId;
        FREGetObjectAsInt32(argv[1], &frameId);
        
        int sinceFrameId;
        FREGetObjectAsInt32(argv[2], &sinceFrameId);
        
        return device->handProbability(handId, frameId, sinceFrameId, 2);
    }
    //end hand class
    
    //start device class
    FREObject LeapNative_getDeviceDistanceToBoundary(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        
        double pX;
        double pY;
        double pZ;
        FREGetObjectAsDouble(argv[0], &pX);
        FREGetObjectAsDouble(argv[1], &pY);
        FREGetObjectAsDouble(argv[2], &pZ);

        Vector position = Vector((float) pX, (float) pY, (float) pZ);

        return device->getDeviceDistanceToBoundary(position);
    }
    
    FREObject LeapNative_getDeviceHorizontalViewAngle(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        return device->getDeviceHorizontalViewAngle();
    }
    
    FREObject LeapNative_getDeviceVerticalViewAngle(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        return device->getDeviceVerticalViewAngle();
    }
    
    FREObject LeapNative_getDeviceIsEmbedded(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        return device->getDeviceIsEmbedded();
    }
    
    FREObject LeapNative_getDeviceIsStreaming(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        return device->getDeviceIsStreaming();
    }
    
    FREObject LeapNative_getDeviceIsValid(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        return device->getDeviceIsValid();
    }
    
    FREObject LeapNative_getDeviceRange(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        return device->getDeviceRange();
    }
    //end device class
    
    FRENamedFunction _Shared_methods[] = {
        { (const uint8_t*) "isSupported", 0, LeapNative_isSupported }
	};
    
	FRENamedFunction _Instance_methods[] = {
  		{ (const uint8_t*) "getFrame", 0, LeapNative_getFrame },
  		{ (const uint8_t*) "frameRotationProbability", 0, LeapNative_frameRotationProbability },
  		{ (const uint8_t*) "frameScaleProbability", 0, LeapNative_frameScaleProbability },
  		{ (const uint8_t*) "frameTranslationProbability", 0, LeapNative_frameTranslationProbability },
  		{ (const uint8_t*) "handRotationProbability", 0, LeapNative_handRotationProbability },
  		{ (const uint8_t*) "handScaleProbability", 0, LeapNative_handScaleProbability },
  		{ (const uint8_t*) "handTranslationProbability", 0, LeapNative_handTranslationProbability },
  		{ (const uint8_t*) "enableGesture", 0, LeapNative_enableGesture },
  		{ (const uint8_t*) "isGestureEnabled", 0, LeapNative_isGestureEnabled },
  		{ (const uint8_t*) "hasFocus", 0, LeapNative_hasFocus },
  		{ (const uint8_t*) "getClosestScreenHitPointable", 0, LeapNative_getClosestScreenHitPointable },
  		{ (const uint8_t*) "getClosestScreenHit", 0, LeapNative_getClosestScreenHit },

  		{ (const uint8_t*) "getScreenDistanceToPoint", 0, LeapNative_getScreenDistanceToPoint },
  		{ (const uint8_t*) "getScreenHeightPixels", 0, LeapNative_getScreenHeightPixels },
  		{ (const uint8_t*) "getScreenWidthPixels", 0, LeapNative_getScreenWidthPixels },
  		{ (const uint8_t*) "getScreenHorizontalAxis", 0, LeapNative_getScreenHorizontalAxis },
  		{ (const uint8_t*) "getScreenVerticalAxis", 0, LeapNative_getScreenVerticalAxis },
  		{ (const uint8_t*) "getScreenBottomLeftCorner", 0, LeapNative_getScreenBottomLeftCorner },
  		{ (const uint8_t*) "getScreenIntersect", 0, LeapNative_getScreenIntersect },
  		{ (const uint8_t*) "getScreenProject", 0, LeapNative_getScreenProject },
  		{ (const uint8_t*) "getScreenIsValid", 0, LeapNative_getScreenIsValid },
  		{ (const uint8_t*) "getScreenNormal", 0, LeapNative_getScreenNormal },

  		{ (const uint8_t*) "getDeviceDistanceToBoundary", 0, LeapNative_getDeviceDistanceToBoundary },
  		{ (const uint8_t*) "getDeviceHorizontalViewAngle", 0, LeapNative_getDeviceHorizontalViewAngle },
  		{ (const uint8_t*) "getDeviceVerticalViewAngle", 0, LeapNative_getDeviceVerticalViewAngle },
  		{ (const uint8_t*) "getDeviceIsEmbedded", 0, LeapNative_getDeviceIsEmbedded },
  		{ (const uint8_t*) "getDeviceIsStreaming", 0, LeapNative_getDeviceIsStreaming },
  		{ (const uint8_t*) "getDeviceIsValid", 0, LeapNative_getDeviceIsValid },
  		{ (const uint8_t*) "getDeviceRange", 0, LeapNative_getDeviceRange },

  		{ (const uint8_t*) "getConfigBool", 0, LeapNative_getConfigBool },
  		{ (const uint8_t*) "getConfigFloat", 0, LeapNative_getConfigFloat },
  		{ (const uint8_t*) "getConfigInt32", 0, LeapNative_getConfigInt32 },
  		{ (const uint8_t*) "getConfigString", 0, LeapNative_getConfigString },
  		{ (const uint8_t*) "getConfigType", 0, LeapNative_getConfigType },
  		{ (const uint8_t*) "setConfigBool", 0, LeapNative_setConfigBool },
  		{ (const uint8_t*) "setConfigFloat", 0, LeapNative_setConfigFloat },
  		{ (const uint8_t*) "setConfigString", 0, LeapNative_setConfigString },
  		{ (const uint8_t*) "setConfigSave", 0, LeapNative_setConfigSave },
   		{ (const uint8_t*) "setPolicyFlags", 0, LeapNative_setPolicyFlags },
   		{ (const uint8_t*) "getPolicyFlags", 0, LeapNative_getPolicyFlags }
	};
    
    void contextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx, uint32_t* numFunctions, const FRENamedFunction** functions) {
        std::cout << "context initializer" << std::endl;
        if ( 0 == strcmp( (const char*) ctxType, "shared" ) )
		{
			*numFunctions = sizeof( _Shared_methods ) / sizeof( FRENamedFunction );
			*functions = _Shared_methods;
		}
		else
        {
            *numFunctions = sizeof( _Instance_methods ) / sizeof( FRENamedFunction );
            *functions = _Instance_methods;
            
            leapnative::LNLeapDevice* device = new leapnative::LNLeapDevice(ctx);
            FRESetContextNativeData(ctx, (void*) device);
        }
	}
    
	void contextFinalizer(FREContext ctx) {
        leapnative::LNLeapDevice* device;
        FREGetContextNativeData(ctx, (void **) &device);
        if(device != NULL)
        {
            delete device;
        }
        std::cout << "context finalizer" << std::endl;
		return;
	}
    
	void LeapNativeInitializer(void** extData, FREContextInitializer* ctxInitializer, FREContextFinalizer* ctxFinalizer) {
		*ctxInitializer = &contextInitializer;
		*ctxFinalizer = &contextFinalizer;
	}
    
	void LeapNativeFinalizer(void* extData) {
		return;
	}
}