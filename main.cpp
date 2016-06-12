#include "HybridLargeDataStorage.hpp"
#include "../TinyTestFramework/TinyTestFramework.hpp"
#include "Key.hpp"
#include "../FASTQParser/Common.hpp"

#include <iostream>
#include <list>
#include <cassert>
#include <initializer_list>
#include <chrono>
#include <random>
#include <map>
#include <stdexcept>
#include <map>


typedef uint64_t Value;

constexpr size_t FASTQ::Common::Nucleotide::alphabetSize;
constexpr FASTQ::Common::Nucleotide::value_type FASTQ::Common::Nucleotide::min_value;
constexpr FASTQ::Common::Nucleotide::value_type FASTQ::Common::Nucleotide::max_value;

using Key = Key_<FASTQ::Common::Nucleotide>;


Key stringToKey(const char *str){
	return Key::fromString(std::string(str));
}

void keyTest(){
	const Key key1 = stringToKey("TNGA");

	const size_t index = key1.toIndex();
	assert(key1 == Key::fromIndex(index, key1.size()));

	const Key key2 = stringToKey("ACNG");

	const Key key12 = key1 + key2;

	size_t factor = 1;
	for(size_t i = 0; i < key2.size(); ++i){
		factor *= Key::value_type::alphabetSize;
	}

	const size_t index12 = key2.toIndex() + key1.toIndex() * factor;

	assert(key12.toIndex() == index12);
}

void insertionTest(){
	HybridLargeDataStorage<Key, Value> hlds(2, 2);
	Key key1 = stringToKey("AAAA");
	Value value1 = 42;

	hlds.insert(key1, value1);

	const auto res = hlds.find(key1);
	assert(res != hlds.end());
	assert(*res == value1);


	Key key2 = stringToKey("TTGC");

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

void iteratorTest(){
	const size_t keySize = 20;
	const size_t headSize = 9;
	const size_t tailSize = keySize - headSize;
	HybridLargeDataStorage<Key, Value> hlds(headSize, tailSize);

	std::map<size_t, Key> keys;

	constexpr size_t keysCount = 10000;

	Key current;
	for(size_t i = 0; i < keysCount; ++i){
		current = randomKey(keySize);
		const size_t index = current.toIndex();

		if(keys.find(index) == keys.cend()){
			keys.insert(std::make_pair(index, current));
		}
	}

	for(const std::pair<size_t, Key> &key : keys){
		hlds.insert(key.second, key.first);
	}


	for(auto it = hlds.begin(); it != hlds.end(); ++it){
		const Key &key = it.getKey();
		const size_t index = key.toIndex();
		const Value value = *it;

		assert(value == index);

		auto keyIt = keys.find(value);
		assert(keyIt != keys.end());
		keys.erase(keyIt);

	}

	assert(keys.empty());
}

void largeDataTest(){
	const size_t keySize = 25;
	const size_t headSize = 10;
	const size_t tailSize = keySize - headSize;
	HybridLargeDataStorage<Key, Value> hlds(headSize, tailSize);


	std::map<Key, Value> values;

	std::default_random_engine rg(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<size_t> distr;

	const size_t count = 100000;
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

void nodeCountTest(){
	const size_t keySize = 10;
	const size_t headSize = 5;
	const size_t tailSize = keySize - headSize;
	HybridLargeDataStorage<Key, Value> hlds(headSize, tailSize);

	size_t expectedNodeCount = 0;
	size_t expectedValueNodeCount = 0;

	hlds.insert(stringToKey("AAAAAAAAAA"), 0);
	expectedNodeCount += 5;
	expectedValueNodeCount += 1;

	const auto res1 = hlds.getProducedNodeCount();
	assert(res1.first == expectedNodeCount);
	assert(res1.second == expectedValueNodeCount);

	hlds.insert(stringToKey("TAAAAAAAAA"), 0);
	expectedNodeCount += 5;
	expectedValueNodeCount += 1;

	const auto res2 = hlds.getProducedNodeCount();
	assert(res2.first == expectedNodeCount);
	assert(res2.second == expectedValueNodeCount);

	hlds.insert(stringToKey("AAAAAAAAAT"), 0);
	expectedNodeCount += 0;
	expectedValueNodeCount += 1;

	const auto res3 = hlds.getProducedNodeCount();
	assert(res3.first == expectedNodeCount);
	assert(res3.second == expectedValueNodeCount);

	hlds.insert(stringToKey("AAAAAAAATA"), 0);
	expectedNodeCount += 1;
	expectedValueNodeCount += 1;

	const auto res4 = hlds.getProducedNodeCount();
	assert(res4.first == expectedNodeCount);
	assert(res4.second == expectedValueNodeCount);
}

void resetTest(){
	const size_t keySize = 10;
	const size_t headSize = 5;
	const size_t tailSize = keySize - headSize;
	HybridLargeDataStorage<Key, Value> hlds(headSize, tailSize);
	hlds.insert(stringToKey("AAAAAAAAAA"), 0);
	hlds.insert(stringToKey("AAAAAAAATA"), 0);
	hlds.insert(stringToKey("AAAAAAAAAC"), 0);
	hlds.insert(stringToKey("AAAAAAAATG"), 0);
	hlds.insert(stringToKey("AAAAAACAAA"), 0);
	hlds.insert(stringToKey("AAANAAAATA"), 0);

	assert(hlds.getProducedNodeCount().first != 0);
	assert(hlds.getProducedNodeCount().second != 0);

	hlds.clear();

	assert(hlds.getProducedNodeCount().first == 0);
	assert(hlds.getProducedNodeCount().second == 0);

	assert(hlds.find(stringToKey("AAAAAAAAAA")) == hlds.end());
	assert(hlds.find(stringToKey("AAAAAAAATA")) == hlds.end());
	assert(hlds.find(stringToKey("AAAAAAAAAC")) == hlds.end());
	assert(hlds.find(stringToKey("AAAAAAAATG")) == hlds.end());
	assert(hlds.find(stringToKey("AAAAAACAAA")) == hlds.end());
	assert(hlds.find(stringToKey("AAANAAAATA")) == hlds.end());

	assert(hlds.begin() == hlds.end());
}


int main(){
	TTF_TEST(keyTest);
	TTF_TEST(nodeCountTest);
	TTF_TEST(insertionTest);
	TTF_TEST(iteratorTest);
	TTF_TEST(largeDataTest);
	TTF_TEST(multiAccessTest);
	TTF_TEST(resetTest);
}


















