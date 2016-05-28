/*
 * STCamera.h
 *
 *  Created on: 2015. 10. 14.
 *      Author: ±Ë≈¬»∆
 */

#ifndef STCAMERA_H_
#define STCAMERA_H_
#include <cocos2d.h>
#include <string>

class STCamera : public cocos2d::Node {
public:
	STCamera();
	void setField(cocos2d::Node* field);
/*
	void setAbsoluteBorderRect(const cocos2d::Rect& rect);
	void setAbsoluteBorderRect(float x, float y, float width, float height);
	void setAbsoluteBorderRect(float ax, float ay, float bx, float by);
*/
	void setCameraPosition(const cocos2d::Vec2& point);
	void setCameraZoom(float zoom);
	cocos2d::Vec2 getCameraPosition() const;
	void setCameraAnchorPoint(const cocos2d::Vec2& point);
	void addCameraPosition(const cocos2d::Vec2& relativePos);
	void setVibrationOffset(const cocos2d::Vec2& offset);
	bool init() override;

	// called by main scene update.
	void updateCamera(float delta);

	/**
	* Vibrate camera with specific power, duration, angle.
	*
	* @param power first viberation pixel radius.
	* @param duration second.
	* @param radian vibrate direction
	*/
	void vibrateCameraDirection(float power, float duration, float radian);
	void setVibrationInterval(float r);

	CREATE_FUNC(STCamera);
	virtual ~STCamera();

private:

	cocos2d::Vec2 _cameraPos{cocos2d::Vec2::ZERO};
	float _zoom = 1.0f;
	cocos2d::Vec2 _vibrationOffset {cocos2d::Vec2::ZERO };
	cocos2d::Vec2 _cameraAnchorPoint{cocos2d::Vec2(0.5f,0.5f)};
	cocos2d::Node* _field{ nullptr };
	cocos2d::Size _visibleSize;
	const std::string _vibScheduleKey{ "ez_camera_vibration_schedule_key" };
	float _vibrationInterval{ 0.04f };


};

#endif /* STCAMERA_H_ */
