/*
 * STCharacterContactListener.cpp
 *
 *  Created on: 2015. 11. 6.
 *      Author: ±Ë≈¬»∆
 */

#include "STContactListener.h"

#include <cocos2d.h>
#include <IngameScene.h>
#include <list>


void STContactListener::BeginContact(b2Contact* contact) {
	auto fa = contact->GetFixtureA();
	auto fb = contact->GetFixtureB();

	if(_c_beginContact.find(fa) != _c_beginContact.end()) {
		for (auto& func : _c_beginContact[fa]) {
			func(contact, fb);
		}
		//_c_beginContact[fa](contact, fb);

	}

	if(_c_beginContact.find(fb) != _c_beginContact.end()) {
		for (auto& func : _c_beginContact[fb]) {
			func(contact, fa);
		}
		//_c_beginContact[fb](contact, fa);

	}





}
// no reason worldManifold.normal.x, worldManifold.normal.y.
void STContactListener::EndContact(b2Contact* contact) {
	auto fa = contact->GetFixtureA();
	auto fb = contact->GetFixtureB();

	if(_c_endContact.find(fa) != _c_endContact.end()) {
		for (auto& func : _c_endContact[fa]) {
			func(contact, fb);
		}
		//_c_endContact[fa](contact, fb);

	}

	if(_c_endContact.find(fb) != _c_endContact.end()) {
		for (auto& func : _c_endContact[fb]) {
			func(contact, fa);
		}
		//_c_endContact[fb](contact, fa);

	}


}

	/// This is called after a contact is updated. This allows you to inspect a
	/// contact before it goes to the solver. If you are careful, you can modify the
	/// contact manifold (e.g. disable contact).
	/// A copy of the old manifold is provided so that you can detect changes.
	/// Note: this is called only for awake bodies.
	/// Note: this is called even when the number of contact points is zero.
	/// Note: this is not called for sensors.
	/// Note: if you set the number of contact points to zero, you will not
	/// get an EndContact callback. However, you may get a BeginContact callback
	/// the next step.
void STContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {

	auto fa = contact->GetFixtureA();
	auto fb = contact->GetFixtureB();

	if(_c_preSolve.find(fa) != _c_preSolve.end()) {
		for (auto& func : _c_preSolve[fa]) {
			func(contact, oldManifold, fb);
		}
		//_c_preSolve[fa](contact, oldManifold, fb);

	}

	if(_c_preSolve.find(fb) != _c_preSolve.end()) {
		for (auto& func : _c_preSolve[fb]) {
			func(contact, oldManifold, fa);
		}
		//_c_preSolve[fb](contact, oldManifold, fa);

	}

	//auto av = contact->GetFixtureA()->GetBody()->GetLinearVelocity();
	//auto bv = contact->GetFixtureB()->GetBody()->GetLinearVelocity();

	//cocos2d::log("avx:%.2f, avy:%.2f, bvx:%.2f, bvy:%.2f", av.x, av.y, bv.x, bv.y);
}

	/// This lets you inspect a contact after the solver is finished. This is useful
	/// for inspecting impulses.
	/// Note: the contact manifold does not include time of impact impulses, which can be
	/// arbitrarily large if the sub-step is small. Hence the impulse is provided explicitly
	/// in a separate data structure.
	/// Note: this is only called for contacts that are touching, solid, and awake.
void STContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {

	auto fa = contact->GetFixtureA();
	auto fb = contact->GetFixtureB();

	if(_c_postSolve.find(fa) != _c_postSolve.end()) {
		for (auto& func : _c_postSolve[fa]) {
			func(contact, impulse, fb);
		}
		//_c_postSolve[fa](contact, impulse, fb);

	}

	if(_c_postSolve.find(fb) != _c_postSolve.end()) {
		for (auto& func : _c_postSolve[fb]) {
			func(contact, impulse, fa);
		}
		//_c_postSolve[fb](contact, impulse, fa);

	}
}

void STContactListener::setBeginContact(b2Fixture* fixture,
		const C_BeginContact& callback) {

	_c_beginContact[fixture].push_back(callback);

}
void STContactListener::setBeginContact(b2Body* body,
		const C_BeginContact& callback) {
	for (auto fixture = body->GetFixtureList(); fixture != nullptr;
			fixture = fixture->GetNext()) {
		//_c_beginContact[fixture] = callback;
		_c_beginContact[fixture].push_back(callback);
		_bodyToFixture[body].emplace(fixture);
	}

}
void STContactListener::setEndContact(b2Fixture* fixture,
		const C_EndContact& callback) {
	//_c_endContact[fixture] = callback;
	_c_endContact[fixture].push_back(callback);
}
void STContactListener::setEndContact(b2Body* body,
		const C_EndContact& callback) {
	for (auto fixture = body->GetFixtureList(); fixture != nullptr;
			fixture = fixture->GetNext()) {

		_c_endContact[fixture].push_back(callback);
		_bodyToFixture[body].emplace(fixture);
	}

}
void STContactListener::setPreSolve(b2Fixture* fixture,
		const C_PreSolve& callback) {
	_c_preSolve[fixture].push_back(callback);

}
void STContactListener::setPreSolve(b2Body* body, const C_PreSolve& callback) {
	for (auto fixture = body->GetFixtureList(); fixture != nullptr;
			fixture = fixture->GetNext()) {
		_c_preSolve[fixture].push_back(callback);
		_bodyToFixture[body].emplace(fixture);
	}

}
void STContactListener::setPostSolve(b2Fixture* fixture,
		const C_PostSolve& callback) {
	_c_postSolve[fixture].push_back(callback);

}
void STContactListener::setPostSolve(b2Body* body,
		const C_PostSolve& callback) {
	for (auto fixture = body->GetFixtureList(); fixture != nullptr;
			fixture = fixture->GetNext()) {
		_c_postSolve[fixture].push_back(callback);
		_bodyToFixture[body].emplace(fixture);
	}

}

void STContactListener::removeFixture(b2Fixture* fixture) {
	_c_beginContact.erase(fixture);
	_c_endContact.erase(fixture);
	_c_preSolve.erase(fixture);
	_c_postSolve.erase(fixture);

	std::list<b2Body*> removeBodyList;

	for(auto& item : _bodyToFixture) {
		auto body = item.first;
		auto& fixtures = item.second;
		fixtures.erase(fixture);
		if(fixtures.empty()) {
			removeBodyList.emplace_back(body);
		}
	}

	for(auto& body : removeBodyList) {
		_bodyToFixture.erase(body);
	}
}

void STContactListener::removeBody(b2Body* body) {

	if(_bodyToFixture.find(body) != _bodyToFixture.end()) {
		for(auto& fixture :_bodyToFixture[body]) {
			_c_beginContact.erase(fixture);
			_c_endContact.erase(fixture);
			_c_preSolve.erase(fixture);
			_c_postSolve.erase(fixture);
		}

		_bodyToFixture.erase(body);
	}
}

STContactListener::STContactListener() 
{
	//_actionManager = apHookActionManager::getInstance();
	//_dataManager = apDataManager::getInstance();

}

STContactListener::~STContactListener() {

}
