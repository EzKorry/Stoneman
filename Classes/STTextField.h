/*
 * STTextfield.h
 *
 *  Created on: 2015. 10. 15.
 *      Author: ±Ë≈¬»∆
 */

#ifndef STTEXTFIELD_H_
#define STTEXTFIELD_H_

#include <cocos2d.h>
// æ»µÂ∑Œ¿ÃµÂ¥¬ #include "extension/cocos-ext.h"
#include "cocos-ext.h"
#include "ui/CocosGUI.h"
#include "ui/UIEditBox/UIEditBox.h"
using namespace cocos2d::ui;

class STTextField: public cocos2d::Node, public cocos2d::ui::EditBoxDelegate {
public:
	STTextField();
	virtual ~STTextField();

	// callback: std::function<void(cocos2d::ui::EditBox*)>

	static STTextField* create(const std::function<void(cocos2d::ui::EditBox*)>& callback, const cocos2d::Size& size, const std::string& placeHolder)	{

	    STTextField *pRet = new(std::nothrow) STTextField();
	    if (pRet && pRet->initWithCallback(callback, size, placeHolder))
	    {
	        pRet->autorelease();
	        return pRet;
	    }
	    else
	    {
	        delete pRet;
	        pRet = NULL;
	        return NULL;
	    }
	}

	bool initWithCallback(const std::function<void(cocos2d::ui::EditBox*)>& callback, const cocos2d::Size& size, const std::string& placeHolder);


	virtual void editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox) override;
	virtual void editBoxEditingDidEnd(cocos2d::ui::EditBox* editBox) override;
	virtual void editBoxTextChanged(cocos2d::ui::EditBox* editBox,
			const std::string& text) override;
	virtual void editBoxReturn(cocos2d::ui::EditBox* editBox) override;
private:

	cocos2d::ui::EditBox* _editName { nullptr };
	std::function<void(cocos2d::ui::EditBox*)> _callback { nullptr };
};

#endif /* STTEXTFIELD_H_ */
