#include "HelloWorldScene.h"
#include "GB2ShapeCache-x.h"

USING_NS_CC;

#define PTM_RATIO			32
#define PIPE_INTERVAL		110
#define PIPE_EXT_SPACE		20

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

	Size winSize = Director::getInstance()->getWinSize();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();	

	log("winSize = (%f, %f)", winSize.width, winSize.height);
	log("visibleSize = (%f, %f)", visibleSize.width, visibleSize.height);
	log("origin = (%f, %f)", origin.x, origin.y);

	/* 创建一个物理世界 */
	b2Vec2 gravity;
	gravity.Set(0.0f, 0.0f);
	world = new b2World(gravity);

	// Do we want to let bodies sleep?
	world->SetAllowSleeping(false);

	world->SetContinuousPhysics(true);

	world->SetContactListener(this);

	GB2ShapeCache::sharedGB2ShapeCache()->addShapesWithFile("shapedefs.plist");

	// Define the ground body.
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0, 0); // bottom-left corner

	// Call the body factory which allocates memory for the ground body
	// from a pool and creates the ground box shape (also from a pool).
	// The body is also added to the world.
	b2Body* groundBody = world->CreateBody(&groundBodyDef);

	// Define the ground box shape.
	b2EdgeShape groundBox;

	// bottom
	groundBox.Set(b2Vec2(origin.x / PTM_RATIO,
		origin.y / PTM_RATIO),
		b2Vec2((origin.x + visibleSize.width) / PTM_RATIO,
		origin.y / PTM_RATIO));
	groundBody->CreateFixture(&groundBox, 0);

#if 0
	auto sprite1 = Sprite::create("bird1_0.png");
	sprite1->setPosition(ccp(0,0));
	addChild(sprite1, 2);

	auto sprite2 = Sprite::create("bird1_1.png");
	sprite2->setPosition(ccp(origin.x + sprite2->boundingBox().size.width / 2, origin.y + sprite2->boundingBox().size.height / 2));
	addChild(sprite2, 2);
#endif

	mBgSprite1 = Sprite::create("bg_day.png");
	mBgSprite1->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	
	mBgSprite2 = Sprite::create("bg_day.png");
	mBgSprite2->setPosition(Vec2(visibleSize.width + visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	mBgSprite2->setFlipX(true);
	
	addChild(mBgSprite1,1);
	addChild(mBgSprite2,1);

#if 0
	pipeUpList = __Array::createWithCapacity(10);
	pipeUpList->retain();

	pipeDownList = __Array::createWithCapacity(10);
	pipeDownList->retain();

	for (int i = 0; i < 3; i++)
	{
		auto *upPipe = Sprite::create("pipe_up.png");
		static int  minY = winSize.height - upPipe->getContentSize().height - PIPE_INTERVAL;
		upPipe->setAnchorPoint(Vec2(0.5, 1));
		int y = (int)(upPipe->getContentSize().height * CCRANDOM_0_1());
		log("y = %d", y);
		y = y < minY ? minY : y;
		upPipe->setPosition(Vec2(winSize.width / 2 * (i + 1), y));

		auto *downPipe = Sprite::create("pipe_down.png");
		downPipe->setAnchorPoint(Vec2(0.5, 0));
		downPipe->setPosition(Vec2(winSize.width / 2 * (i + 1), y + PIPE_INTERVAL));

		addChild(upPipe, 1);
		addChild(downPipe, 1);

		pipeUpList->addObject(upPipe);
		pipeDownList->addObject(downPipe);
	}
#endif

	int minY;
	for (int i = 0; i < 3; i++)
	{
		upPipe[i] = Sprite::create("pipe_up.png");
		minY = winSize.height - upPipe[i]->getContentSize().height - PIPE_INTERVAL;
		upPipe[i]->setAnchorPoint(Vec2(0.5, 1));
		int y = (int)(upPipe[i]->getContentSize().height * CCRANDOM_0_1());
		log("y = %d", y);
		y = y < minY ? minY : y;
		upPipe[i]->setPosition(Vec2((winSize.width / 2 + PIPE_EXT_SPACE)* (i + 4), y));

		downPipe[i] = Sprite::create("pipe_down.png");
		downPipe[i]->setAnchorPoint(Vec2(0.5, 0));
		downPipe[i]->setPosition(Vec2((winSize.width / 2 + PIPE_EXT_SPACE) * (i + 4), y + PIPE_INTERVAL));

		addChild(upPipe[i], 1);
		addChild(downPipe[i], 1);

		/* 添加到物理世界 */
		addBoxBodyForSprite(upPipe[i]);
		addBoxBodyForSprite(downPipe[i]);

	}

	scheduleUpdate();
    
	pipeInterval = visibleSize.width / 2;

	/* 添加小鸟 */
	birdSprite = Sprite::create("bird0_0.png");
	birdSprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(birdSprite, 2);
	//addBoxBodyForSprite(birdSprite);
	addBoxBodyByVertex(birdSprite);

	/* 添加触摸 */
	setTouchEnabled(true);
	setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
	auto oneTouch = EventListenerTouchOneByOne::create();
	oneTouch->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
	oneTouch->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
	//oneTouch->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
	//oneTouch->onTouchCancelled = CC_CALLBACK_2(HelloWorld::onTouchCancelled, this);
	
	EventDispatcher* eventDispatcher = Director::getInstance()->getEventDispatcher();
	eventDispatcher->addEventListenerWithSceneGraphPriority(oneTouch, this);

	game_is_over = false;

    return true;
}

void HelloWorld::BeginContact(b2Contact* contact)
{
	if (contact->GetFixtureA()->GetBody()->GetUserData() == birdSprite || 
		contact->GetFixtureB()->GetBody()->GetUserData() == birdSprite)
	{
		log("Someting on the bird!");
		game_is_over = true;
	}
}

void HelloWorld::update(float dt)
{
	if (game_is_over)
	{
		return;
	}
#if 1
	static int speed = 2;
	int x1, x2, y;
	Size winSize = Director::getInstance()->getWinSize();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	/*log("winSize = (%f, %f)", winSize.width, winSize.height);
	log("visibleSize = (%f, %f)", visibleSize.width, visibleSize.height);
	log("origin = (%f, %f)", origin.x, origin.y);*/

	x1 = mBgSprite1->getPosition().x;
	x2 = mBgSprite2->getPosition().x;
	y = mBgSprite1->getPosition().y;

	x1 -= speed;
	x2 -= speed;

	if (x1 < -winSize.width / 2)
	{
		x1 += 2 * winSize.width;
		//x2 = winSize.width / 2;
	}

	if (x2 < -winSize.width / 2)
	{
		x2 += 2 * winSize.width;
		//x2 = winSize.width + winSize.width / 2;
	}

	mBgSprite1->setPosition(x1, y);
	mBgSprite2->setPosition(x2, y);
	
#if 0
	int pipeX, pipeY;
	Sprite *killUpPipe = NULL, *killDownPipe = NULL;
	for (int i = 0; i < pipeUpList->count(); i++)
	{
		Size pipeSize;
		Sprite *upPipe = (Sprite *)pipeUpList->objectAtIndex(i);
		Sprite *downPipe = (Sprite *)pipeDownList->objectAtIndex(i);
		
		pipeX = upPipe->getPosition().x - speed;
		pipeY = upPipe->getPosition().y;
		pipeSize = upPipe->getContentSize();

		if (pipeX < - pipeSize.width)
		{
			killUpPipe = upPipe;
			killDownPipe = downPipe;
			//pipeUpList->removeObject(upPipe);
			//pipeDownList->removeObject(downPipe);
			continue;
		}

		upPipe->setPosition(pipeX, pipeY);
		downPipe->setPosition(pipeX, pipeY + PIPE_INTERVAL);
	}

	if (killUpPipe != NULL && killDownPipe != NULL)
	{
		pipeUpList->removeObject(killUpPipe);
		pipeDownList->removeObject(killDownPipe);
	}

	// 最后一个pipe已经显示 产生一个新的pipe
	if (pipeX < winSize.width)
	{
		pipeInterval = winSize.width / 2;// -5 + CCRANDOM_0_1() * 10;

		auto *backUpPipe = Sprite::create("pipe_up.png");
		backUpPipe->setAnchorPoint(Vec2(0.5, 1));
		y = (int)(backUpPipe->getContentSize().height * CCRANDOM_0_1());
		int minY = winSize.height - backUpPipe->getContentSize().height - PIPE_INTERVAL;
		y = y < minY ? minY : y;
		backUpPipe->setPosition(Vec2(pipeX + pipeInterval, y));

		auto *backDownPipe = Sprite::create("pipe_down.png");
		backDownPipe->setAnchorPoint(Vec2(0.5, 0));
		backDownPipe->setPosition(Vec2(winSize.width + pipeInterval, y + PIPE_INTERVAL));

		addChild(backUpPipe, 1);
		addChild(backDownPipe, 1);

		pipeUpList->addObject(backUpPipe);
		pipeDownList->addObject(backDownPipe);
	}
#endif

	int pipeX, pipeY;
	Sprite *killUpPipe = NULL, *killDownPipe = NULL;
	for (int i = 0; i < 3; i++)
	{
		Size pipeSize;

		pipeX = upPipe[i]->getPosition().x - speed;
		pipeY = upPipe[i]->getPosition().y;
		pipeSize = upPipe[i]->getContentSize();

		if (pipeX < -pipeSize.width)
		{
			pipeX += 3 * (winSize.width / 2 + PIPE_EXT_SPACE);
		}

		upPipe[i]->setPosition(pipeX, pipeY);
		downPipe[i]->setPosition(pipeX, pipeY + PIPE_INTERVAL);
	}

	/* 更新物理世界 */
	int velocityIterations = 8;
	int positionIterations = 1;

	world->Step(dt, velocityIterations, positionIterations);
	for (b2Body *b = world->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetUserData() != NULL) {
			Sprite *sprite = (Sprite *)b->GetUserData();
			Vec2 anchor = sprite->getAnchorPoint() - Vec2(0.5, 0.5);
			Size size = sprite->getContentSize();
			b2Vec2 b2Position = b2Vec2((sprite->getPosition().x - anchor.x * size.width) / PTM_RATIO,
				(sprite->getPosition().y - anchor.y * size.height) / PTM_RATIO);
			float32 b2Angle = -1 * CC_DEGREES_TO_RADIANS(sprite->getRotation());

			b->SetTransform(b2Position, b2Angle);
		}
	}


#endif
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

bool HelloWorld::onTouchBegan(Touch* touch, Event* event)
{
	Size winSize = Director::getInstance()->getWinSize();
	log("onTouchBegan");

	birdSprite->stopAllActions();
	/* 添加飞行动画 */
	auto animation = Animation::create();
	for (int i = 0; i < 3; i++)
	{
		char animation_name[50] = { 0 };
		sprintf(animation_name, "bird0_%d.png", i);
		animation->addSpriteFrameWithFile(animation_name);
	}
	animation->setDelayPerUnit(0.3f / 3);
	animation->setRestoreOriginalFrame(true);
	auto fly_animation = Animate::create(animation);
	
	auto jump = JumpBy::create(0.6f, ccp(0, 0/*-winSize.width*/), 60, 1);
	auto fall = MoveBy::create(1.5f, ccp(0, -winSize.height));
	auto fly_action = Sequence::create(jump, fall, NULL);

	auto up_rotate = RotateTo::create(0.2f, -30);
	auto down_rotate = RotateTo::create(0.4f, 80);
	auto fly_rotate = Sequence::create(up_rotate, down_rotate, NULL);
	
	auto fly = Spawn::create(fly_action, fly_animation, fly_rotate, NULL);
	//birdSprite->runAction(RepeatForever::create(CCAnimate::create(animation)));
	birdSprite->runAction(fly);

	return true;
}

void HelloWorld::onTouchMoved(Touch* touch, Event* event)
{
	log("onTouchMoved");
}

void HelloWorld::addBoxBodyForSprite(Sprite *sprite)
{
	b2BodyDef spriteBodyDef;
	spriteBodyDef.type = b2_dynamicBody;
	Vec2 anchor = sprite->getAnchorPoint() - Vec2(0.5, 0.5);
	Size size = sprite->getContentSize();
	spriteBodyDef.position.Set((sprite->getPosition().x - anchor.x * size.width) / PTM_RATIO,
		(sprite->getPosition().y - anchor.y * size.height) / PTM_RATIO);
	spriteBodyDef.userData = sprite;
	b2Body *spriteBody = world->CreateBody(&spriteBodyDef);

	b2PolygonShape spriteShape;
	spriteShape.SetAsBox(sprite->getContentSize().width / PTM_RATIO / 2,
		sprite->getContentSize().height / PTM_RATIO / 2);
	b2FixtureDef spriteShapeDef;
	spriteShapeDef.shape = &spriteShape;
	spriteShapeDef.density = 10.0;
	spriteShapeDef.isSensor = true;
	spriteBody->CreateFixture(&spriteShapeDef);
}


void HelloWorld::addBoxBodyByVertex(Sprite *sprite)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;

	//bodyDef.position.Set(p.x / PTM_RATIO, p.y / PTM_RATIO);
	bodyDef.position.Set(sprite->getPosition().x / PTM_RATIO, sprite->getPosition().y / PTM_RATIO);
	bodyDef.userData = sprite;
	b2Body *body = world->CreateBody(&bodyDef);

	// add the fixture definitions to the body  
	GB2ShapeCache *sc = GB2ShapeCache::sharedGB2ShapeCache();
	sc->addFixturesToBody(body, "bird0_1");
	//sprite->setAnchorPoint(sc->anchorPointForShape(name.c_str()));
}