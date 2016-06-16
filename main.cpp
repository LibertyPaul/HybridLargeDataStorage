#include "HybridLargeDataStorage.hpp"
#include "../TinyTestFramework/TinyTestFramework.hpp"
#include "Key.hpp"
#include "../FASTQParser/Common.hpp"
#include "HLDSDump.hpp"
#include "HLDSBinaryDumpMerger.hpp"

#include <iostream>
#include <list>
#include <cassert>
#include <initializer_list>
#include <chrono>
#include <random>
#include <map>
#include <stdexcept>
#include <map>
#include <sstream>


typedef uint64_t Value;

constexpr size_t FASTQ::Common::Nucleotide::alphabetSize;
constexpr FASTQ::Common::Nucleotide::value_type FASTQ::Common::Nucleotide::min_value;
constexpr FASTQ::Common::Nucleotide::value_type FASTQ::Common::Nucleotide::max_value;

using Key = Key_<FASTQ::Common::Nucleotide>;


void keyTest(){
	const Key key1 = Key::fromString("TNGA");

	const size_t index = key1.toIndex();
	assert(key1 == Key::fromIndex(index, key1.size()));

	const Key key2 = Key::fromString("ACNG");

	const Key key12 = key1 + key2;

	size_t factor = 1;
	for(size_t i = 0; i < key2.size(); ++i){
		factor *= Key::value_type::alphabetSize;
	}

	const size_t index12 = key2.toIndex() + key1.toIndex() * factor;

	assert(key12.toIndex() == index12);

	assert(key2 < key1);
}

void insertionTest(){
	HybridLargeDataStorage<Key, Value> hlds(2, 2);
	Key key1 = Key::fromString("AAAA");
	Value value1 = 42;

	hlds.insert(key1, value1);

	const auto res = hlds.find(key1);
	assert(res != hlds.end());
	assert(*res == value1);


	Key key2 = Key::fromString("TTGC");

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

void RAMUsageTest(){
	const size_t keySize = 10;
	const size_t headSize = 5;
	const size_t tailSize = keySize - headSize;
	HybridLargeDataStorage<Key, Value> hlds(headSize, tailSize);

	size_t currentSize = hlds.getApproximateRAMUsage();

	hlds.insert(Key::fromString("AAAAAAAAAA"), 0);
	const size_t res1 = hlds.getApproximateRAMUsage();
	assert(res1 > currentSize);
}

void resetTest(){
	const size_t keySize = 10;
	const size_t headSize = 5;
	const size_t tailSize = keySize - headSize;
	HybridLargeDataStorage<Key, Value> hlds(headSize, tailSize);

	const size_t startRAMUsage = hlds.getApproximateRAMUsage();

	hlds.insert(Key::fromString("AAAAAAAAAA"), 0);
	hlds.insert(Key::fromString("AAAAAAAATA"), 0);
	hlds.insert(Key::fromString("AAAAAAAAAC"), 0);
	hlds.insert(Key::fromString("AAAAAAAATG"), 0);
	hlds.insert(Key::fromString("AAAAAACAAA"), 0);
	hlds.insert(Key::fromString("AAANAAAATA"), 0);

	assert(startRAMUsage != hlds.getApproximateRAMUsage());

	hlds.clear();

	assert(startRAMUsage == hlds.getApproximateRAMUsage());

	assert(hlds.find(Key::fromString("AAAAAAAAAA")) == hlds.end());
	assert(hlds.find(Key::fromString("AAAAAAAATA")) == hlds.end());
	assert(hlds.find(Key::fromString("AAAAAAAAAC")) == hlds.end());
	assert(hlds.find(Key::fromString("AAAAAAAATG")) == hlds.end());
	assert(hlds.find(Key::fromString("AAAAAACAAA")) == hlds.end());
	assert(hlds.find(Key::fromString("AAANAAAATA")) == hlds.end());

	assert(hlds.begin() == hlds.end());
}

void equalsTest(){
	const size_t keySize = 10;
	const size_t headSize = 5;
	const size_t tailSize = keySize - headSize;
	HybridLargeDataStorage<Key, Value> hlds1(headSize, tailSize);
	HybridLargeDataStorage<Key, Value> hlds2(headSize, tailSize);

	assert(hlds1 == hlds2);

	const std::vector<std::pair<Key, Value>> testData = {
		{randomKey(keySize), 42},
		{randomKey(keySize), 2},
		{randomKey(keySize), 1234},
		{randomKey(keySize), 3},
		{randomKey(keySize), 432},
		{randomKey(keySize), 11111},
		{randomKey(keySize), 0}
	};

	for(const auto &item : testData){
		hlds1.insert(item.first, item.second);
		hlds2.insert(item.first, item.second);
	}
	assert(hlds1 == hlds2);

	auto it = hlds2.find(testData.front().first);
	assert(it != hlds2.end());

	*it += 1;
	assert(hlds1 != hlds2);

	*it -= 1;
	assert(hlds1 == hlds2);

	const Key newKey = randomKey(keySize);
	const Value newValue = 42424242;

	hlds2.insert(newKey, newValue);
	assert(hlds1 != hlds2);

	hlds1.insert(newKey, newValue);
	assert(hlds1 == hlds2);
}

void keyItem2bitsetTest(){
	assert(Key::value_type::fromSymbol('A').toBitset() == std::bitset<Key::value_type::binarySize>("000"));
	assert(Key::value_type::fromSymbol('T').toBitset() == std::bitset<Key::value_type::binarySize>("001"));
	assert(Key::value_type::fromSymbol('G').toBitset() == std::bitset<Key::value_type::binarySize>("010"));
	assert(Key::value_type::fromSymbol('C').toBitset() == std::bitset<Key::value_type::binarySize>("011"));
	assert(Key::value_type::fromSymbol('N').toBitset() == std::bitset<Key::value_type::binarySize>("100"));

	assert(Key::value_type::fromSymbol('A') == Key::value_type::fromBitset(std::bitset<Key::value_type::binarySize>("000")));
	assert(Key::value_type::fromSymbol('T') == Key::value_type::fromBitset(std::bitset<Key::value_type::binarySize>("001")));
	assert(Key::value_type::fromSymbol('G') == Key::value_type::fromBitset(std::bitset<Key::value_type::binarySize>("010")));
	assert(Key::value_type::fromSymbol('C') == Key::value_type::fromBitset(std::bitset<Key::value_type::binarySize>("011")));
	assert(Key::value_type::fromSymbol('N') == Key::value_type::fromBitset(std::bitset<Key::value_type::binarySize>("100")));
}

void dumperTest(){
	const size_t keySize = 5;
	const size_t headSize = 2;
	const size_t tailSize = keySize - headSize;
	const size_t hldsId = 42424242;
	HybridLargeDataStorage<Key, Value> hlds(hldsId, headSize, tailSize);

	const Key key = randomKey(keySize);

	std::vector<std::pair<Key, Value>> testData = {
		{randomKey(keySize), keySize + headSize + hldsId},
		{randomKey(keySize), keySize + headSize + hldsId},
		{randomKey(keySize), keySize + headSize + hldsId},
		{randomKey(keySize), keySize + headSize + hldsId},
		{randomKey(keySize), keySize + headSize + hldsId},
		{randomKey(keySize), keySize + headSize + hldsId}
	};

	for(const auto item : testData){
		hlds.insert(item.first, item.second);
	}

	std::sort(testData.begin(), testData.end(), [](const std::pair<Key, Value> &lhs, const std::pair<Key, Value> &rhs){
		return lhs.first.toIndex() < rhs.first.toIndex();
	}); // hlds will sort them aswell


	std::stringstream buffer;
	HLDSDumpWriter<Key, Value> dumpWriter(buffer);
	dumpWriter.dumpAll(hlds);


	HLDSDumpReader<Key, Value> dumpReader(buffer);
	assert(dumpReader.getHeader().keySize == keySize);
	assert(dumpReader.getHeader().hldsId == hldsId);

	for(const auto item : testData){
		assert(dumpReader.hasNext());
		const HLDSDumpRecord<Key, Value> rec = dumpReader.read();

		assert(rec.key == item.first);
		assert(rec.value ==  item.second);
	}

	assert(dumpReader.hasNext() == false);
}

void mergeTest(){
	const size_t keySize = 5;
	const size_t headSize = 2;
	const size_t tailSize = keySize - headSize;
	const size_t hldsId = 42424242;
	HybridLargeDataStorage<Key, Value> hlds1(hldsId, headSize, tailSize);
	HybridLargeDataStorage<Key, Value> hlds2(hldsId, headSize, tailSize);

	const Value value = 100500;
	std::vector<std::pair<Key, Value>> testData1 = {
		{randomKey(keySize), value},
		{randomKey(keySize), value},
		{randomKey(keySize), value},
		{randomKey(keySize), value},
		{randomKey(keySize), value},
		{randomKey(keySize), value}
	};

	for(const auto item : testData1){
		hlds1.insert(item.first, item.second);
	}

	std::stringstream dump1;
	HLDSDumpWriter<Key, Value> writer1(dump1);
	writer1.dumpAll(hlds1);

	std::vector<std::pair<Key, Value>> testData2 = {
		{randomKey(keySize), value},
		{randomKey(keySize), value},
		{randomKey(keySize), value},
		{randomKey(keySize), value},
		{randomKey(keySize), value},
		{randomKey(keySize), value}
	};

	for(const auto item : testData2){
		hlds2.insert(item.first, item.second);
	}

	std::stringstream dump2;
	HLDSDumpWriter<Key, Value> writer2(dump2);
	writer2.dumpAll(hlds2);


	std::stringstream mergedDumps;
	HLDSBinaryDumpMerger<Key, Value> merger(dump1, dump2, mergedDumps);
	merger.run();

	std::vector<std::pair<Key, Value>> mergedTestData(testData1.size() + testData2.size());

	const auto comp = [](const std::pair<Key, Value> &lhs, const std::pair<Key, Value> &rhs) -> bool{
		return lhs.first < rhs.first;
	};

	std::sort(testData1.begin(), testData1.end(), comp);
	std::sort(testData2.begin(), testData2.end(), comp);
	std::merge(testData1.cbegin(), testData1.cend(), testData2.cbegin(), testData2.cend(), mergedTestData.begin(), comp);

	HLDSDumpReader<Key, Value> reader(mergedDumps);

	for(const auto item : mergedTestData){
		assert(reader.hasNext());
		assert(item.first == reader.peek().key);
		assert(item.second == reader.peek().value);

		reader.read();
	}


}


int main(){
	TTF_TEST(keyTest);
	TTF_TEST(keyItem2bitsetTest);
	TTF_TEST(RAMUsageTest);
	TTF_TEST(insertionTest);
	TTF_TEST(iteratorTest);
	TTF_TEST(largeDataTest);
	TTF_TEST(multiAccessTest);
	TTF_TEST(resetTest);
	TTF_TEST(dumperTest);
	TTF_TEST(equalsTest);
	TTF_TEST(mergeTest);
}


















