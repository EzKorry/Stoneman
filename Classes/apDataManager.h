/*
 * apDataManager.h
 *
 *  Created on: 2015. 11. 13.
 *      Author: ±Ë≈¬»∆
 */

#ifndef APDATAMANAGER_H_
#define APDATAMANAGER_H_

#include <string>
#include <functional>
#include <vector>
#include <memory>
#include <typeinfo>
#include <unordered_map>
#include <cocos2d.h>
#include <json/rapidjson.h>
#include <json/document.h>


namespace arphomod {

using namespace std;
using namespace cocos2d;


class apDataManager {
public:
	apDataManager();
	virtual ~apDataManager();

	std::vector<int> i;


	// singleton.
	static shared_ptr<apDataManager> getInstance() {

		if (_sp == nullptr) {
			_sp = make_shared<apDataManager>();
		}
		return _sp;
	}

	void saveData();
	void setPath(const string& path);
	void loadData(const string& path);

	void set(const std::string& key, const char* data) {
		set(key, string(data));
	}
	
	void set(const std::string& key, const string& data) {
		_dataString[key] = data;
	}

	void set(const std::string& key, float data) {
		_dataFloat[key] = data;
	}

	void set(const std::string& key, double data) {
		_dataFloat[key] = data;
	}

	void set(const std::string& key, int data) {
		_dataInt[key] = data;
	}

	void set(const std::string& key, bool data) {
		_dataBool[key] = data;
	}

	// data getter
	template<class T>
	T get(const std::string& key) {
		T t;
		return t;
	}

	template<>
	int get<int>(const std::string& key) {
		return _dataInt[key];
	}

	template<>
	float get<float>(const std::string& key) {
		return _dataFloat[key];
	}

	template<>
	double get<double>(const std::string& key) {
		return _dataFloat[key];
	}

	template<>
	bool get<bool> (const std::string& key) {
		CCASSERT(_dataBool.find(key) != _dataBool.end(), "bool data not exist");
		return _dataBool[key];
	}

	template<>
	string get<string>(const std::string& key) {
		return _dataString[key];
	}

	// remove data
	template<class T>
	void remove(const std::string& key) {
	}

	template<>
	void remove<int>(const std::string& key) {
		_dataInt.erase(key);
	}
	template<>
	void remove<float>(const std::string& key) {
		_dataFloat.erase(key);
	}
	template<>
	void remove<double>(const std::string& key) {
		remove<float>(key);
	}
	template<>
	void remove<bool>(const std::string& key) {
		_dataBool.erase(key);
	}
	template<>
	void remove<string>(const std::string& key) {
		_dataString.erase(key);
	}

private:
	std::unordered_map<string, string> _dataString;
	std::unordered_map<string, bool> _dataBool;
	std::unordered_map<string, int> _dataInt;
	std::unordered_map<string, float> _dataFloat;


	static shared_ptr<apDataManager> _sp;
	rapidjson::Document _document;
	string _path;

};

} /* namespace arphomod */

#endif /* APDATAMANAGER_H_ */
