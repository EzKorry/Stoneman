/*
 * apDataManager.cpp
 *
 *  Created on: 2015. 11. 13.
 *      Author: ±èÅÂÈÆ
 */

#include "apDataManager.h"

namespace arphomod {

using namespace std;

std::shared_ptr<apDataManager> apDataManager::_sp = nullptr;

apDataManager::apDataManager() {
	// TODO Auto-generated constructor stub

}

apDataManager::~apDataManager() {
	// TODO Auto-generated destructor stub
}




void apDataManager::saveDataInt(const string& key, const int& data) {
	//_dataInt[key] = data;

}
void apDataManager::saveDataFloat(const string& key, const float& data) {
	//_dataFloat[key] = data;

}
void apDataManager::saveDataPointer(const string& key, void* data) {
	//_dataPointer[key] = data;
}

int apDataManager::getDataInt(const string& key) const {
//	if(_dataInt.find(key) != _dataInt.end()) {
//		return _dataInt[key];
//	} else {
//		return 0;
//	}
	return 0;
}
float apDataManager::getDataFloat(const string& key) const {
//
//	if(_dataFloat.find(key) != _dataFloat.end()) {
//		return _dataFloat[key];
//	} else {
//		return 0.f;
//	}
	return 0.f;

}
void* apDataManager::getDataPointer(const string& key) const {
//	if(_dataPointer.find(key) != _dataPointer.end()) {
//		return _dataPointer[key];
//	} else {
//		return nullptr;
//	}
	return nullptr;

}

} /* namespace arphomod */
