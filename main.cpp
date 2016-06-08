#include "HybridLargeDataStorage.hpp"
#include "TinyTestFramework/TinyTestFramework.hpp"

#include <iostream>
#include <list>
#include <cassert>
#include <initializer_list>
#include <chrono>
#include <random>
#include <map>
#include <stdexcept>

using namespace std;


enum class Alphabet{
	A = 0, T, G, C, N
};

class KeyItem{
	Alphabet index;
public:
	static constexpr size_t alphabetSize = 5;
	static constexpr Alphabet min_value = Alphabet::A;
	static constexpr Alphabet max_value = Alphabet::N;

	typedef Alphabet value_type;

	explicit KeyItem(const Alphabet index): index(index){}

	size_t toIndex() const{
		return static_cast<size_t>(index);
	}

	static KeyItem fromIndex(const size_t index){
		if(index > static_cast<size_t>(max_value)){
			throw std::overflow_error("");
		}

		return KeyItem(static_cast<Alphabet>(index));
	}

	KeyItem &operator=(const KeyItem &keyItem){
		this->index = keyItem.index;
		return *this;
	}

	bool operator==(const Alphabet &alphabet) const{
		return this->index == alphabet;
	}

	bool operator==(const KeyItem &o) const{
		return *this == o.index;
	}

	bool operator!=(const KeyItem &o) const{
		return !(*this == o);
	}

	bool operator<(const KeyItem &o) const{
		return this->index < o.index;
	}

	KeyItem &operator++(){
		*this = KeyItem::fromIndex(this->toIndex() + 1);
		return *this;
	}
};

class Key : public std::vector<KeyItem>{
public:

	Key(){}
	Key(Key &&key): std::vector<KeyItem>(static_cast<std::vector<KeyItem> &&>(key)){}
	Key(const Key &key): std::vector<KeyItem>(key){}
	Key(const size_t size): std::vector<KeyItem>(size, KeyItem(value_type::min_value)){}
	Key(std::initializer_list<KeyItem> il): std::vector<KeyItem>(il){}

	void resize(const size_t size){
		std::vector<KeyItem> &base = *this;
		base.resize(size, KeyItem(value_type::min_value));
	}

	size_t toIndex() const{
		size_t result = 0;
		size_t multiplier = 1;

		for(auto it = this->crbegin(); it != this->crend(); ++it){
			result += it->toIndex() * multiplier;
			multiplier *= value_type::alphabetSize;
		}

		return result;
	}

	static Key fromIndex(size_t index, const size_t size){
		Key resultReversed;
		resultReversed.reserve(size);

		while(index != 0){
			const size_t currentIndex = index % value_type::alphabetSize;
			const KeyItem currentKeyItem = KeyItem::fromIndex(currentIndex);
			resultReversed.push_back(currentKeyItem);
		}

		if(resultReversed.size() != size){
			throw std::overflow_error("");
		}

		Key result;
		result.resize(resultReversed.size());

		std::copy(resultReversed.crbegin(), resultReversed.crend(), result.begin());

		return result;
	}

	Key &operator=(Key key){
		std::vector<KeyItem> &base1 = *this;
		std::vector<KeyItem> &base2 = key;

		base1 = std::move(base2);

		return *this;
	}

	Key &operator++(){
		Key keyCopy = *this;
		auto it = keyCopy.rbegin();
		while(it != keyCopy.rend()){
			if(*it == value_type::max_value){
				*it = KeyItem(value_type::min_value);
			}
			else{
				++*it;
				break;
			}

			++it;
		}

		if(it == keyCopy.rend()){
			throw std::overflow_error("");
		}

		*this = std::move(keyCopy);

		return *this;
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

	const auto res = hlds.find(key1);
	assert(res != hlds.end());
	assert(*res == value1);


	Key key2 = {
		{KeyItem(Alphabet::T)},
		{KeyItem(Alphabet::T)},
		{KeyItem(Alphabet::G)},
		{KeyItem(Alphabet::C)}
	};

	const auto invRes = hlds.find(key2);

	assert(invRes == hlds.end());


	hlds.insert(key2, 100);

	auto res1 = hlds.find(key1);
	auto res2 = hlds.find(key2);
	assert(*res1 != *res2);

	*res1 = 200;
	*res2 = 200;

	assert(*hlds.find(key1) == 200);
	assert(*hlds.find(key2) == 200);
}

Key randomKey(const size_t size){
	static std::default_random_engine rg(std::chrono::system_clock::now().time_since_epoch().count());
	static std::uniform_int_distribution<uint8_t> distr(0, Key::value_type::alphabetSize - 1);

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

	const size_t count = 1000000;
	for(size_t i = 0; i < count; ++i){
		const Key currentKey = randomKey(keySize);
		const Value currentValue = distr(rg);
		hlds.insert(currentKey, currentValue);
		values[currentKey] = currentValue;
	}

	for(auto value = values.cbegin(); value != values.cend(); ++value){
		auto it = hlds.find(value->first);
		assert(it != hlds.end());
		assert(*it == value->second);
	}

}

void multiAccessTest(){
	const size_t keySize = 25;
	const size_t headSize = 10;
	const size_t tailSize = keySize - headSize;
	HybridLargeDataStorage<Key, Value> hlds(headSize, tailSize);


	const size_t count = 1000;
	std::vector<Key> keys;
	for(size_t i = 0; i < count; ++i){
		keys.push_back(randomKey(keySize));
	}


	for(const auto &key : keys){
		assert(hlds.find(key) == hlds.end());
	}

	const Value startValue = 0;
	for(const auto &key : keys){
		hlds.insert(key, startValue);
	}

	for(const auto &key : keys){
		assert(*hlds.find(key) == startValue);
	}

	for(const auto &key : keys){
		*hlds.find(key) += 1;
	}

	for(const auto &key : keys){
		assert(*hlds.find(key) == startValue + 1);
	}

}



int main(){
	TTF_TEST(insertionTest);
	TTF_TEST(largeDataTest);
	TTF_TEST(multiAccessTest);


}

