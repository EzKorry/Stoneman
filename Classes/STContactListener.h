/*
 * STCharacterContactListener.h
 *
 *  Created on: 2015. 11. 6.
 *      Author: ±Ë≈¬»∆
 */

#ifndef STCONTACTLISTENER_H_
#define STCONTACTLISTENER_H_

#include <Box2D/Box2D.h>
#include "apHookActionManager.h"
#include "apDataManager.h"
#include <vector>
#include <unordered_map>

using namespace arphomod;
class IngameScene;

enum class STContactCallback {
	PreSolve,
	PostSolve,
	Begin,
	End
};
// last Param is other fixture when contact.
using C_BeginContact = std::function<void(b2Contact*, b2Fixture*)>;
using C_EndContact = std::function<void(b2Contact*, b2Fixture*)>;
using C_PreSolve = std::function<void(b2Contact*, const b2Manifold*, b2Fixture*)>;
using C_PostSolve = std::function<void(b2Contact*, const b2ContactImpulse*, b2Fixture*)>;

class STContactListener: public b2ContactListener {
public:

	STContactListener(IngameScene* p);
	virtual ~STContactListener();
	virtual void BeginContact(b2Contact* contact);
	virtual void EndContact(b2Contact* contact);
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

	void setBeginContact(b2Fixture* fixture, const C_BeginContact& callback);
	void setBeginContact(b2Body* body, const C_BeginContact& callback);
	void setEndContact(b2Fixture* fixture, const C_EndContact& callback);
	void setEndContact(b2Body* body, const C_EndContact& callback);
	void setPreSolve(b2Fixture* fixture, const C_PreSolve& callback);
	void setPreSolve(b2Body* body, const C_PreSolve& callback);
	void setPostSolve(b2Fixture* fixture, const C_PostSolve& callback);
	void setPostSolve(b2Body* body, const C_PostSolve& callback);

	void removeFixture(b2Fixture* fixture);
	void removeBody(b2Body* body);

private:

	std::unordered_map<b2Fixture*, C_BeginContact> _c_beginContact;
	std::unordered_map<b2Fixture*, C_EndContact> _c_endContact;
	std::unordered_map<b2Fixture*, C_PreSolve> _c_preSolve;
	std::unordered_map<b2Fixture*, C_PostSolve> _c_postSolve;
	std::unordered_map<b2Body*, std::set<b2Fixture*>> _bodyToFixture;




	//std::shared_ptr<apHookActionManager> _actionManager;
	//std::shared_ptr<apDataManager> _dataManager;

	IngameScene* _mainScene {nullptr};
};

#endif /* STCONTACTLISTENER_H_ */
