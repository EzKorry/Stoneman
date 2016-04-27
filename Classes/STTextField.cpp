/*
 * STTextfield.cpp
 *
 *  Created on: 2015. 10. 15.
 *      Author: 源��깭�썕
 */

#include "STTextField.h"

USING_NS_CC;
using namespace cocos2d::ui;


bool STTextField::initWithCallback(const std::function<void(cocos2d::ui::EditBox*)>& callback, const Size& size, const std::string& placeHolder) {


	auto visibleSize = Director::getInstance()->getVisibleSize();


	_editName = ui::EditBox::create(size, "bg.png",TextureResType::LOCAL);
	_editName->setAnchorPoint(Vec2(0.5f,0.5f));
	_editName->setPosition(Vec2::ZERO);
	_editName->setFontColor(Color3B::WHITE);
	_editName->setPlaceHolder(placeHolder.c_str());
	_editName->setPlaceholderFontColor(Color3B(127,127,127));
	_editName->setFont("fonts/SpoqaHsRegular.ttf", size.height / 2);
	//_editName->setText("yo");
	_editName->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	_editName->setDelegate(this);
	addChild(_editName);

	_callback = callback;


	return true;

}

void STTextField::editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox) {

}
void STTextField::editBoxEditingDidEnd(cocos2d::ui::EditBox* editBox) {
	/*if(_callback != nullptr) {
		_callback(editBox);
		cocos2d::log("editBoxCalled!");
	}
*/
}
void STTextField::editBoxTextChanged(cocos2d::ui::EditBox* editBox,
		const std::string& text) {

}
void STTextField::editBoxReturn(cocos2d::ui::EditBox* editBox) {
	if (_callback != nullptr) {
		_callback(editBox);
		cocos2d::log("editBoxCalled!");

	}
}

STTextField::STTextField() {
	// TODO Auto-generated constructor stub

}

STTextField::~STTextField() {
	// TODO Auto-generated destructor stub
}
