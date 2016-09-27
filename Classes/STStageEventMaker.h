#ifndef _ST_STAGEEVENTMAKER_H_
#define _ST_STAGEEVENTMAKER_H_

#include <string>
#include <cocos2d.h>

using namespace std;
using namespace cocos2d;
class STStageEventMaker
{
public:
	static void makeStageEvent(const string& level);
	STStageEventMaker();
	~STStageEventMaker();

private:

	static void makeLevel1();
};

#endif