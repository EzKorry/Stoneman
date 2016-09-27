/*
 * apDataManager.cpp
 *
 *  Created on: 2015. 11. 13.
 *      Author: ±Ë≈¬»∆
 */

#include "apDataManager.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include <sstream>


namespace arphomod {

using namespace std;

std::shared_ptr<apDataManager> apDataManager::_sp = nullptr;

apDataManager::apDataManager() {
	// TODO Auto-generated constructor stub

}

apDataManager::~apDataManager() {
	// TODO Auto-generated destructor stub
}

void apDataManager::saveData()
{
	// clean up
	
	auto fu = FileUtils::getInstance();
	auto& alloc = _document.GetAllocator();

	for (auto& item : _dataInt) {
		rapidjson::Value name(item.first.c_str(), alloc);
		int value = item.second;
		if (!_document.HasMember(name)) {
			_document.AddMember(name.Move(), value, alloc);
		}
	}
	for (auto& item : _dataString) {
		rapidjson::Value name(item.first.c_str(), alloc);
		rapidjson::Value value(item.second.c_str(), alloc);
		if (!_document.HasMember(name)) {
			_document.AddMember(name.Move(), value.Move(), alloc);
		}
	}
	for (auto& item : _dataBool) {
		rapidjson::Value name(item.first.c_str(), alloc);
		bool value = item.second;
		if (!_document.HasMember(name)) {
			_document.AddMember(name.Move(), value, alloc);
		}
	}	
	for (auto& item : _dataFloat) {
		rapidjson::Value name(item.first.c_str(), alloc);
		auto value = item.second;
		if (!_document.HasMember(name)) {
			_document.AddMember(name.Move(), value, alloc);
		}
	}
	rapidjson::StringBuffer buf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
	_document.Accept(writer);

	fu->writeStringToFile(buf.GetString(), _path);

	_document.RemoveAllMembers();
}

void apDataManager::setPath(const string & path)
{
	_path = path;
}

void apDataManager::loadData(const string & path)
{

	auto fu = FileUtils::getInstance();
	CCASSERT(fu->isFileExist(path), "user data file not exist");
	_path = path;
	auto js = fu->getStringFromFile(path);
	_document.Parse(js.c_str());
	

	for (auto it = _document.MemberBegin(); it != _document.MemberEnd(); it++) {

		string name = it->name.GetString();
		auto& value = it->value;

		if (value.IsBool()) { 
			//*************
			_dataBool[name] = value.GetBool(); 
		}
		else if (value.IsNumber()) {
			if (value.IsInt()) {
				//*************
				_dataInt[name] = value.GetInt();
			}
			else {
				//*************
				_dataFloat[name] = value.GetDouble();
			}
		}
		else if (value.IsString()) { 
			//*************
			_dataString[name] = value.GetString(); 
		}
		else { 
			// error
			CCASSERT(false, "user data element is not available. type wrong. type is array or object"); 
		}

	}
	_document.RemoveAllMembers();

}





} /* namespace arphomod */
