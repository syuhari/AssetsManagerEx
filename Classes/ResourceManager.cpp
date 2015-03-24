//
//  ResourceManager.h
//
//  Created by Akihiro Matsuura on 3/23/15.
//  Copyright (c) 2015 Syuhari, Inc. All rights reserved.
//

#include "ResourceManager.h"

USING_NS_CC;
USING_NS_CC_EXT;

// custom event name
const char* ResourceManager::EVENT_PROGRESS = "__cc_Resource_Event_Progress";
const char* ResourceManager::EVENT_FINISHED = "__cc_Resource_Event_Finished";


ResourceManager* ResourceManager::instance = nullptr;

ResourceManager::~ResourceManager() {
    CC_SAFE_RELEASE_NULL(_am);
}

ResourceManager::ResourceManager()
:_am(nullptr)
,_amListener(nullptr)
{
    
}

ResourceManager* ResourceManager::getInstance() {
    if (instance==nullptr) {
        instance = new ResourceManager();
    }
    return instance;
}

void ResourceManager::updateAssets(std::string manifestPath)
{
    std::string storagePath = FileUtils::getInstance()->getWritablePath();
    CCLOG("storage path = %s", storagePath.c_str());
    
    if (_am!=nullptr) {
        CC_SAFE_RELEASE_NULL(_am);
    }
    _am = AssetsManagerEx::create(manifestPath, storagePath);
    _am->retain();
    
    if (!_am->getLocalManifest()->isLoaded()) {
        CCLOG("Fail to update assets, step skipped.");
    } else {
        _amListener = EventListenerAssetsManagerEx::create(_am, [this](EventAssetsManagerEx* event){
            static int failCount = 0;
            switch (event->getEventCode())
            {
                case EventAssetsManagerEx::EventCode::ERROR_NO_LOCAL_MANIFEST:
                {
                    CCLOG("No local manifest file found, skip assets update.");
                    break;
                }
                case EventAssetsManagerEx::EventCode::UPDATE_PROGRESSION:
                {
                    std::string assetId = event->getAssetId();
                    float percent = event->getPercent();
                    std::string str;
                    if (assetId == AssetsManagerEx::VERSION_ID) {
                        // progress for version file
                    } else if (assetId == AssetsManagerEx::MANIFEST_ID) {
                        // progress for manifest file
                    } else {
                        // dispatch progress event
                        CCLOG("%.2f Percent", percent);
                        auto event = EventCustom(ResourceManager::EVENT_PROGRESS);
                        auto data = Value(percent);
                        event.setUserData(&data);
                        Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
                    }
                    
                    break;
                }
                case EventAssetsManagerEx::EventCode::ERROR_DOWNLOAD_MANIFEST:
                case EventAssetsManagerEx::EventCode::ERROR_PARSE_MANIFEST:
                {
                    CCLOG("Fail to download manifest file, update skipped.");
                    break;
                }
                case EventAssetsManagerEx::EventCode::ALREADY_UP_TO_DATE:
                case EventAssetsManagerEx::EventCode::UPDATE_FINISHED:
                {
                    CCLOG("Update finished. %s", event->getMessage().c_str());
                    CC_SAFE_RELEASE_NULL(_am);
                    // dispatch finished updating event
                    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(ResourceManager::EVENT_FINISHED);
                    break;
                }
                case EventAssetsManagerEx::EventCode::UPDATE_FAILED:
                {
                    CCLOG("Update failed. %s", event->getMessage().c_str());
                    
                    // retry 5 times, if error occured
                    failCount ++;
                    if (failCount < 5) {
                        _am->downloadFailedAssets();
                    } else {
                        CCLOG("Reach maximum fail count, exit update process");
                        failCount = 0;
                    }
                    break;
                }
                case EventAssetsManagerEx::EventCode::ERROR_UPDATING:
                {
                    CCLOG("Asset %s : %s", event->getAssetId().c_str(), event->getMessage().c_str());
                    break;
                }
                case EventAssetsManagerEx::EventCode::ERROR_DECOMPRESS:
                {
                    CCLOG("%s", event->getMessage().c_str());
                    break;
                }
                default:
                    break;
            }
        });
        
        // execute updating resources
        Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_amListener, 1);
        _am->update();
    }
}