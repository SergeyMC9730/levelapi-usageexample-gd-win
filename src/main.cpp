#include <matdash.hpp>

// defines add_hook to use minhook
#include <matdash/minhook.hpp>

// lets you use mod_main
#include <matdash/boilerplate.hpp>

// matdash::create_console
#include <matdash/console.hpp>

// gd.h includes cocos2d.h
#include <gd.h>
#include <cocos-ext.h>

// compile options
#include "compile.h"

#include <stdlib.h>

using namespace cocos2d::extension;
using namespace cocos2d;
using namespace gd;

cocos2d::CCObject* t;

void sendLevelAPIMessage(const char* title, const char* description) {
    AchievementNotifier * not = AchievementNotifier::sharedState();
    not->notifyAchievement(title, description, "GJ_downloadBtn_001.png", true);
}

class NetworkAction {
public:
    static void post(std::string url, SEL_HttpResponse callback) {
        if (DEBUG) std::cout << "[DEBUG] Send " << url << std::endl;
        CCHttpClient* gdNetworking = CCHttpClient::getInstance();
        CCHttpRequest* req = new CCHttpRequest();
        req->setRequestType(CCHttpRequest::HttpRequestType::kHttpPost);
        req->setUrl(url.c_str());
        req->setResponseCallback(t, callback);
        req->setRequestData(nullptr, 0);
        gdNetworking->send(req);
        req->release();
        return;
    }
};

class NTest {
public:
    void respCallback(CCHttpClient* client, CCHttpResponse* response) {
        if (DEBUG) std::cout << "[LOG] " << response->isSucceed() << std::endl;
        if (!response->isSucceed()) {
            if (DEBUG) std::cout << "[LOG] " << response->getErrorBuffer() << std::endl;
        }
        else {
            if (DEBUG) std::cout << "[LOG] " << response->getResponseData()->data() << std::endl;
        }
        CCScene *scn = CCDirector::sharedDirector()->getRunningScene();
        gd::TextAlertPopup* p = gd::TextAlertPopup::create("Got something", 0.5f, 0.3f);
        p->setPositionY(25.f);
        scn->addChild(p, 1024);
    }
};

namespace MenuLayerX {
    bool MenuLayer_init(gd::MenuLayer* self) {
        if (!matdash::orig<&MenuLayer_init>(self)) return false;
        //NetworkAction::post("https://gd.dogotrigger.xyz/api/v1/search/lid/128", httpresponse_selector(NTest::respCallback));
        return true;
    }
};
namespace LevelInfoX {
    int currentLevelID = 0;

    class LIX {
    public:
        void respCallback(CCHttpClient* client, CCHttpResponse* response) {
            if (response->isSucceed()) {
                sendLevelAPIMessage("LevelAPI", "Level is queued!");
            }
            else {
                sendLevelAPIMessage("LevelAPI", "LevelAPI is down");
            }
            return;
        }
        void onUpload(CCObject* target) {
            sendLevelAPIMessage("LevelAPI", "Uploading level...");
            std::string str = "https://gd.dogotrigger.xyz/api/v1/upload/level/";
            if (DEBUG) std::cout << "[DEBUG] Upload level " << currentLevelID << std::endl;
            str.append(std::to_string(currentLevelID));
            NetworkAction::post(str, httpresponse_selector(LIX::respCallback));
        }
    };
    bool init(gd::LevelInfoLayer *self, gd::GJGameLevel* selectedLevel) {
        if (!matdash::orig<&init>(self, selectedLevel)) return false;

        CCMenu* bmenu = CCMenu::create();
        CCSprite* sprite = CCSprite::createWithSpriteFrameName("GJ_downloadBtn_001.png");
        gd::CCMenuItemSpriteExtra* bbtn = gd::CCMenuItemSpriteExtra::create(sprite, self, menu_selector(LIX::onUpload));
        bbtn->setPosition({ 150, 134 });
        //bbtn->setScale(0.35f);
        bmenu->addChild(bbtn);
        
        self->addChild(bmenu, 1024);

        currentLevelID = selectedLevel->m_nLevelID;

        return true;
    }
}

namespace ProfilePageX {
    int accountID;

    class PIX {
    public:
        void respCallback(CCHttpClient* client, CCHttpResponse* response) {
            if (response->isSucceed()) {
                sendLevelAPIMessage("LevelAPI", "Level is queued!");
            }
            else {
                sendLevelAPIMessage("LevelAPI", "LevelAPI is down");
            }
            return;
        }
        void onUpload(CCObject* target) {
            sendLevelAPIMessage("LevelAPI", "Moving levels into the queue...");
            std::string str = "https://gd.dogotrigger.xyz/api/v1/upload/user/";
            str.append(std::to_string(accountID));
            NetworkAction::post(str, httpresponse_selector(PIX::respCallback));
        }
    };

    bool init(gd::ProfilePage* self, int m_accountID, bool idk) {
        if (!matdash::orig<&init>(self, accountID, idk)) return false;

        accountID = m_accountID;

        CCMenu* bmenu = CCMenu::create();
        CCSprite* sprite = CCSprite::createWithSpriteFrameName("GJ_downloadBtn_001.png");
        CCMenuItemSpriteExtra* bbtn = CCMenuItemSpriteExtra::create(sprite, self, menu_selector(PIX::onUpload));
        bbtn->setPosition({ -194, -80 });
        bmenu->addChild(bbtn);
        self->addChild(bmenu, 1024);

        return true;
    }
}

namespace LevelBrowserX {
    uint32_t* levelIDS = nullptr;
    uint32_t levelIDSArraySize = 0;
    bool buttonWasCreated = false;
    bool UploadClassIsRetried = false;

    bool init(gd::LevelBrowserLayer* self, GJSearchObject* searchParameters) {
        if (!matdash::orig<&init>(self, searchParameters)) return false;

        buttonWasCreated = false;

        return true;
    }

}

void mod_main(HMODULE) {
    // this creates a console window whenever the mod is injected
    // which is very useful for debugging, but make sure to remove
    // on release builds! :D
    if(DEBUG) matdash::create_console();

    t = new cocos2d::CCObject();

    matdash::add_hook<&MenuLayerX::MenuLayer_init>(gd::base + 0x1907B0);
    matdash::add_hook<&LevelInfoX::init>(gd::base + 0x175DF0);
    matdash::add_hook<&ProfilePageX::init>(gd::base + 0x20EF00);

    if (DEBUG) std::cout << "[LOG] Early init success!" << std::endl;
}