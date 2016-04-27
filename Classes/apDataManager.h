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


namespace arphomod {

using namespace std;


class apBaseData {
public:

	apBaseData() {}
	virtual ~apBaseData() {}

};
template <class T>
class apData : apBaseData {

public:
	function<T(string)> getData;
	typedef T data_type;

private:
	T _data;
};

class apDataManager {
public:
	apDataManager();
	virtual ~apDataManager();

	std::vector<int> i;


	// singleton.
	static shared_ptr<apDataManager> getInstance() {

		if(_sp == nullptr) {
			_sp = make_shared<apDataManager>();
		}
		return _sp;
	}


	void saveDataInt(const string& key, const int& data);
	void saveDataFloat(const string& key, const float& data);
	void saveDataPointer(const string& key, void* data);

	// if key doesn't exist, then return 0
	int getDataInt(const string& key) const;

	// if key doesn't exist, then return 0.f
	float getDataFloat(const string& key) const;

	// if key doesn't exist, then return nullptr
	void* getDataPointer(const string& key) const;

	template <class T>
	void saveData(const string& key, const typename std::remove_reference<T>::type & data) {


	}


	template <class T>
	T loadData() {


	}




private:

	std::unordered_map<string, int> _dataInt;
	std::unordered_map<string, float> _dataFloat;
	std::unordered_map<string, void*> _dataPointer;


	static shared_ptr<apDataManager> _sp;

};

} /* namespace arphomod */

#endif /* APDATAMANAGER_H_ */
