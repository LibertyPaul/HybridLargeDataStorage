#include "HybridLargeDataStorage.hpp"

#include <iostream>
#include <list>
#include <cassert>
#include <initializer_list>
#include <boost/optional.hpp>
#include <chrono>
#include <random>
#include <map>

using namespace std;


enum class Alphabet{
	A = 0, T, G, C
};

class KeyItem{
	Alphabet index;
public:
	typedef Alphabet value_type;
	KeyItem(const Alphabet index): index(index){}

	size_t toIndex() const{
		return static_cast<size_t>(index);
	}

	bool operator==(const KeyItem &o) const{
		return this->index == o.index;
	}

	bool operator!=(const KeyItem &o) const{
		return !this->operator==(o);
	}

	bool operator<(const KeyItem &o) const{
		return this->index < o.index;
	}
};

class Key : public std::vector<KeyItem>{
public:
	static constexpr size_t alphabetSize = 4;

	Key(){}
	Key(std::initializer_list<KeyItem> il): std::vector<KeyItem>(il){}


	KeyItem at(const size_t index) const{
		assert(index < this->size());
		return static_cast<const std::vector<KeyItem> *>(this)->at(index);
	}

	void pop_front(){
		this->erase(this->begin());
	}

	size_t size() const{
		return static_cast<const std::vector<KeyItem> *>(this)->size();
	}

	bool empty() const{
		return static_cast<const std::vector<KeyItem> *>(this)->empty();
	}
};

bool operator<(const Key &lhs, const Key &rhs){
	assert(lhs.size() == rhs.size());

	for(size_t i = 0; i < lhs.size(); ++i){
		if(lhs[i] != rhs[i]){
			return lhs[i] < rhs[i];
		}
	}

	return false;
}

typedef uint64_t Value;

void insertionTest(){
	HybridLargeDataStorage<Key, Value> hlds(2, 2);
	Key key1 = {
		{KeyItem(Alphabet::A)},
		{KeyItem(Alphabet::A)},
		{KeyItem(Alphabet::A)},
		{KeyItem(Alphabet::A)}
	};
	Value value1 = 42;

	hlds.insert(key1, value1);

	const boost::optional<Value &> res = hlds.getValue(key1);
	assert(res.get() == value1);


	Key key2 = {
		{KeyItem(Alphabet::T)},
		{KeyItem(Alphabet::T)},
		{KeyItem(Alphabet::G)},
		{KeyItem(Alphabet::C)}
	};

	const boost::optional<Value &> invRes = hlds.getValue(key2);

	assert(!invRes);


	hlds.insert(key2, 100);

	boost::optional<Value &> res1 = hlds.getValue(key1);
	boost::optional<Value &> res2 = hlds.getValue(key2);
	assert(res1.get() != res2.get());

	res1.get() = 200;
	res2.get() = 200;

	assert(hlds.getValue(key1).get() == 200);
	assert(hlds.getValue(key2).get() == 200);
}

Key randomKey(const size_t size){
	static std::default_random_engine rg(std::chrono::system_clock::now().time_since_epoch().count());
	static std::uniform_int_distribution<uint8_t> distr(0, Key::alphabetSize - 1);

	Key key;

	for(size_t i = 0; i < size; ++i){
		const Key::value_type current(static_cast<Key::value_type::value_type>(distr(rg)));
		key.push_back(current);
	}

	return key;
}

void largeDataTest(){
	const size_t keySize = 25;
	const size_t headSize = 10;
	const size_t tailSize = keySize - headSize;
	HybridLargeDataStorage<Key, Value> hlds(headSize, tailSize);


	std::map<Key, Value> values;

	std::default_random_engine rg(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<size_t> distr;

	const size_t count = 10000000;
	for(size_t i = 0; i < count; ++i){
		const Key currentKey = randomKey(keySize);
		const Value currentValue = distr(rg);
		hlds.insert(currentKey, currentValue);
		values[currentKey] = currentValue;
	}

	for(auto value = values.cbegin(); value != values.cend(); ++value){
		assert(hlds.getValue(value->first).get() == value->second);
	}

}

void multiAccessTest(){

}

#define TEST(function){\
	std::cout << #function << " was started" << std::endl;\
	const auto startTime = std::chrono::steady_clock::now();\
	function();\
	const auto endTime = std::chrono::steady_clock::now();\
	std::cout << #function << " [PASSED]. duration: " << std::fixed << std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() / 1000000. << "s" << std::endl;\
	}\



int main(){
	TEST(insertionTest);
	TEST(largeDataTest);


}

