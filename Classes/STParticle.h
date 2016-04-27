#ifndef _STPARTICLE_H_
#define _STPARTICLE_H_
#include <cocos2d.h>
class STParticle : public cocos2d::Node
{
public:
	STParticle();
	virtual ~STParticle();
	CREATE_FUNC(STParticle);

	//inline bool isActive() const;
	
	//void setActive(bool active);

private:
	//bool _isActive = false;
};

#endif