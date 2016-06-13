#ifndef HLDSDUMPER_HPP
#define HLDSDUMPER_HPP

#include "HybridLargeDataStorage.hpp"

#include <ostream>
#include <memory>
#include <algorithm>
#include <bitset>
#include <cstring>
#include <vector>

template<typename Key, typename Value>
class HLDSDumperBase{

	virtual std::vector<char> dumpRecord(const Key &key, const Value &value) const = 0;

public:
	virtual void dump(HybridLargeDataStorage<Key, Value> &hlds, std::ostream &o) const{
		for(typename HybridLargeDataStorage<Key, Value>::iterator it = hlds.begin(); it != hlds.end(); ++it){
			const std::vector<char> recordDump = this->dumpRecord(it.getKey(), *it);
			o.write(&recordDump[0], recordDump.size());
		}
	}

};


template<typename Key, typename Value>
class HLDSTextDumper : public HLDSDumperBase<Key, Value>{
	virtual std::vector<char> dumpRecord(const Key &key, const Value &value) const{
		const std::string result_s = key.toString() + '\t' + std::to_string(value) + '\n';

		std::vector<char> result(result_s.size());
		std::copy(result_s.cbegin(), result_s.cend(), result.begin());

		return result;
	}
};

template<typename Key, typename Value>
class HLDSBinaryDumper : public HLDSDumperBase<Key, Value>{
	virtual std::vector<char> dumpRecord(const Key &key, const Value &value) const{
		const size_t requiredBufferSize_bit = key.size() * Key::value_type::binarySize + sizeof(value) * 8;
		const size_t requiredBufferSize_byte = requiredBufferSize_bit / 8 + (requiredBufferSize_bit % 8 ? 1 : 0);

		std::vector<char> result(requiredBufferSize_byte, 0);

		size_t currentPos = 0;

		for(size_t i = 0; i < key.size(); ++i){
			const std::bitset<Key::value_type::binarySize> current = key.at(i).toBitset();
			for(size_t i = 0; i < current.size(); ++i){
				if(current.test(i)){
					const size_t bitPos = currentPos % 8;
					result.at(currentPos / 8) |= 1 >> bitPos;
				}
			}
		}

		memcpy(&result.at(requiredBufferSize_byte - sizeof(value)) , &value, sizeof(value));

		return result;
	}
};

#endif // HLDSDUMPER_HPP

