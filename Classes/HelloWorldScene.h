#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"

USING_NS_CC;

class HelloWorld : public cocos2d::Layer, public b2ContactListener
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

	// a selector callback
	void update(float dt);

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);

	void addBoxBodyForSprite(Sprite *sprite);
	void addBoxBodyByVertex(Sprite *sprite);
	void BeginContact(b2Contact* contact);

private:
	int game_is_over;
	Sprite *mBgSprite1;
	Sprite *mBgSprite2;

	Sprite *upPipe[3];
	Sprite *downPipe[3];

	Sprite *birdSprite;

	cocos2d::__Array* pipeUpList;
	cocos2d::__Array* pipeDownList;

	int pipeInterval;

	b2World* world;
};

#endif // __HELLOWORLD_SCENE_H__
