/*
 * STCamera.cpp
 *
 *  Created on: 2015. 10. 14.
 *      Author: ������
 */

#include "STCamera.h"
#include "IngameScene.h"

USING_NS_CC;

bool STCamera::init() {

	if(!Node::init()) {
		return false;
	}

	auto dir = Director::getInstance();

	_visibleSize = dir->getVisibleSize();



	return true;

}

void STCamera::updateCamera(float delta)
{


}

void STCamera::setField(cocos2d::Node* field){
	if(_field != nullptr) {
		_field->release();
	}
	_field = field;
	_field -> retain();


}
/*
void STCamera::setAbsoluteBorderRect(const cocos2d::Rect& rect){

}

void STCamera::setAbsoluteBorderRect(float x, float y, float width, float height){

}
void STCamera::setAbsoluteBorderRect(float ax, float ay, float bx, float by){

}
*/
void STCamera::setCameraPosition(const Vec2& cameraPos){

	_cameraPos = cameraPos;

	if(_field) {

		float offsetX = _visibleSize.width * _cameraAnchorPoint.x;
		float offsetY = _visibleSize.height * _cameraAnchorPoint.y;

		_field->setPosition(-_cameraPos.x + offsetX + _vibrationOffset.x, -_cameraPos.y + offsetY + _vibrationOffset.y);


	}

}

void STCamera::setVibrationOffset(const cocos2d::Vec2& offset) {
	_vibrationOffset = offset;
}

void STCamera::vibrateCameraDirection(float power, float duration, float radian) {

	if (isScheduled(_vibScheduleKey)) {
		unschedule(_vibScheduleKey);
	}
	float originalDuration = duration;
	duration = 0;
	float sita = radian;
	bool isFlip = true;
	float deltaElapsed = _vibrationInterval;
	schedule([deltaElapsed, isFlip, sita, originalDuration, duration, power, this](float delta) mutable-> void {
		delta = IngameScene::_timeScale * delta;
		deltaElapsed += delta;
		duration += delta;
		if (deltaElapsed < _vibrationInterval) {
			return;
		}
		deltaElapsed -= _vibrationInterval;

		float ratio = 1.f - duration / originalDuration;
		float realPower = power * ratio * ratio * ratio * ratio * ratio;

		//std::uniform_real_distribution<> ldis(-0.2f, 0.2f);
		sita += M_PI;
		/*
		if(isFlip) {

		isFlip = false;
		} else {
		sita += ldis(gen);
		isFlip = true;
		}*/

		_vibrationOffset.x = std::cos(sita) * realPower;
		_vibrationOffset.y = std::sin(sita) * realPower;



		if (duration > originalDuration) {
			unschedule(_vibScheduleKey);
			this->_vibrationOffset.x = 0;
			this->_vibrationOffset.y = 0;
		}
	}, _vibScheduleKey);
}



void STCamera::setVibrationInterval(float r){
	_vibrationInterval = r;
}

cocos2d::Vec2 STCamera::getCameraPosition() const {
	return _cameraPos;
}

void STCamera::addCameraPosition(const Vec2& relativePos) {

	setCameraPosition(relativePos + _cameraPos);
}

void STCamera::setCameraAnchorPoint(const cocos2d::Vec2& point) {

	_cameraAnchorPoint = point;
	setCameraPosition(_cameraPos);
}



STCamera::STCamera() {
	// TODO Auto-generated constructor stub

}

STCamera::~STCamera() {
	cocos2d::log("stcamera destructor start");

	if(_field != nullptr) {
		_field->release();
	}
	// TODO Auto-generated destructor stub
	cocos2d::log("stcamera destructor end");

}
