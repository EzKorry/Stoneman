#ifndef _STLABEL_H_
#define _STLABEL_H_

#include <cocos2d.h>
using namespace cocos2d;
class STLabel : public cocos2d::Label {

public:
	static STLabel* create();
	static STLabel * createWithTTF(const std::string& text, const std::string& fontFilePath, float fontSize,
		const Size& dimensions = Size::ZERO, TextHAlignment hAlignment = TextHAlignment::LEFT,
		TextVAlignment vAlignment = TextVAlignment::TOP);
	static STLabel* createWithTTF(const TTFConfig& ttfConfig, const std::string& text,
		TextHAlignment hAlignment = TextHAlignment::LEFT, int maxLineWidth = 0);
	

	virtual bool setTTFConfig(const TTFConfig& ttfConfig) override;
	virtual void setScaleX(float scaleX) override;
	virtual float getScaleX() const override;
	virtual void setScaleY(float scaleY) override;
	virtual float getScaleY() const override;
	virtual void setScaleZ(float scaleZ) override;
	virtual float getScaleZ() const override;
	virtual void setScale(float scale) override;
	virtual float getScale() const override;
	virtual void setScale(float scaleX, float scaleY) override;

	virtual Size getSizeCalcResolution();

CC_CONSTRUCTOR_ACCESS:
	/**
	* Constructor of Label.
	* @js NA
	*/
	STLabel(TextHAlignment hAlignment = TextHAlignment::LEFT,
		TextVAlignment vAlignment = TextVAlignment::TOP);

	/**
	* Destructor of Label.
	* @js NA
	* @lua NA
	*/
	virtual ~STLabel();


};

#endif
