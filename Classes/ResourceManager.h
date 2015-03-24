//
//  ResourceManager.h
//
//  Created by Akihiro Matsuura on 3/23/15.
//  Copyright (c) 2015 Syuhari, Inc. All rights reserved.
//

#ifndef __Syuhari__ResourceManager__
#define __Syuhari__ResourceManager__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"

class ResourceManager {
private:
    ResourceManager();
    static ResourceManager* instance;
    
    cocos2d::extension::AssetsManagerEx* _am;
    cocos2d::extension::EventListenerAssetsManagerEx* _amListener;
    
public:
    // custom event name
    static const char* EVENT_PROGRESS;
    static const char* EVENT_FINISHED;
    
    virtual ~ResourceManager();
    static ResourceManager* getInstance();
    
    void updateAssets(std::string manifestPath);
};

#endif /* defined(__Syuhari__ResourceManager__) */
