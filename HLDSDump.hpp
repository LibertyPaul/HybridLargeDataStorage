#ifndef HLDSDUMP_HPP
#define HLDSDUMP_HPP

#include "HLDSIterator.hpp"

#include <ostream>
#include <istream>
#include <memory>

template<typename T>
void writeBinary(const T &t, std::ostream &o){
	o.write(reinterpret_cast<const char *>(&t), sizeof(t));
}

template<typename T>
T readBinary(std::istream &i){
	T t;
	i.read(reinterpret_cast<char *>(&t), sizeof(t));

	return t;
}

struct HLDSDumpHeader{
	size_t hldsId;
	size_t keySize;

	HLDSDumpHeader(){}
	HLDSDumpHeader(const size_t hldsId, const size_t keySize): hldsId(hldsId), keySize(keySize){}

	static constexpr size_t serializedSize(){
		return sizeof(size_t) * 2;
	}

	void toStream(std::ostream &o) const{
		writeBinary<size_t>(this->hldsId, o);
		writeBinary<size_t>(this->keySize, o);
	}

	static HLDSDumpHeader fromStream(std::istream &i){
		const size_t hldsId = readBinary<size_t>(i);
		const size_t keySize = readBinary<size_t>(i);

		return HLDSDumpHeader(hldsId, keySize);
	}
};

template<typename Key, typename Value>
struct HLDSDumpRecord{
	Key key;
	Value value;

	HLDSDumpRecord(){}
	HLDSDumpRecord(Key key, Value value): key(std::move(key)), value(std::move(value)){
		static_assert(std::is_integral<Value>::value, "Value must be of an integer type");
	}

	static size_t serializedSize(const size_t keySize){
		const size_t keySize_bit = keySize * Key::value_type::binarySize;
		const size_t keySize_byte = keySize_bit / 8 + (keySize_bit % 8 ? 1 : 0);
		const size_t valueSize = sizeof(Value);

		return keySize_byte + valueSize;
	}

	void toStream(std::ostream &o) const{
		const size_t keySize_bit = this->key.size() * Key::value_type::binarySize;
		const size_t keySize_byte = keySize_bit / 8 + (keySize_bit % 8 ? 1 : 0);

		std::unique_ptr<char[]> binaryKey(new char[keySize_byte]);
		std::fill(binaryKey.get(), binaryKey.get() + keySize_byte, 0);

		for(size_t keyIndex = 0; keyIndex < this->key.size(); ++keyIndex){
			const std::bitset<Key::value_type::binarySize> current = this->key.at(keyIndex).toBitset();
			for(size_t bitsetIndex = 0; bitsetIndex < current.size(); ++bitsetIndex){
				if(current.test(bitsetIndex)){
					const size_t currentPos = keyIndex * current.size() + bitsetIndex;
					const size_t bitPos = currentPos % 8;
					const char value = 1 << bitPos;
					binaryKey[currentPos / 8] |= value;
				}
			}
		}

		o.write(binaryKey.get(), keySize_byte);
		writeBinary<Value>(this->value, o);
	}

	static HLDSDumpRecord fromStream(std::istream &i, const size_t keySize){
		const size_t keySize_bit = keySize * Key::value_type::binarySize;
		const size_t keySize_byte = keySize_bit / 8 + (keySize_bit % 8 ? 1 : 0);

		std::unique_ptr<char[]> binaryKey(new char[keySize_byte]);
		i.read(binaryKey.get(), keySize_byte);

		Key key;

		for(size_t keyIndex = 0; keyIndex < keySize; ++keyIndex){
			std::bitset<Key::value_type::binarySize> current;
			for(size_t bitsetPos = 0; bitsetPos < current.size(); ++bitsetPos){
				const size_t currentPos = keyIndex * current.size() + bitsetPos;
				const char mask = 1 << (currentPos % 8);
				const char value = binaryKey[currentPos / 8];
				if((value & mask) != 0){
					current.set(bitsetPos);
				}
			}

			key.push_back(Key::value_type::fromBitset(current));
		}

		Value value = readBinary<Value>(i);

		return HLDSDumpRecord(key, value);
	}
};


template<typename Key, typename Value>
class HLDSDumpWriter{
	std::ostream &dst;

public:
	HLDSDumpWriter(std::ostream &dst): dst(dst){
		this->dst.exceptions(std::ios_base::failbit | std::ios_base::badbit);
	}

	void writeHeader(const HLDSDumpHeader &header){
		header.toStream(this->dst);
	}

	void write(const HLDSDumpRecord<Key, Value> &record){
		record.toStream(this->dst);
	}

	void dumpAll(HybridLargeDataStorage<Key, Value> &hlds){
		this->writeHeader(HLDSDumpHeader(hlds.getId(), hlds.keySize()));

		for(auto it = hlds.begin(); it != hlds.end(); ++it){
			const HLDSDumpRecord<Key, Value> record(it.getKey(), *it);
			this->write(record);
		}
	}
};


template<typename Key, typename Value>
class HLDSDumpReader{
	std::istream &src;
	HLDSDumpHeader header;

	HLDSDumpRecord<Key, Value> buffer;
	bool alive = true;

public:
	HLDSDumpReader(std::istream &src): src(src){
		this->src.exceptions(std::ios_base::failbit | std::ios_base::badbit);
		this->readHeader();
		this->readRecord();
	}

private:
	void readHeader(){
		this->header = HLDSDumpHeader::fromStream(this->src);
	}

	void readRecord(){
		try{
			buffer = HLDSDumpRecord<Key, Value>::fromStream(this->src, this->header.keySize);
		}
		catch(std::ios_base::failure &){
			this->alive = false;
		}
	}

public:
	const HLDSDumpHeader &getHeader() const{
		return this->header;
	}

	size_t recordCount() const{
		const size_t previousPos = this->src.tellg();
		this->src.seekg(0, std::ios_base::end);
		const size_t lastPos = this->src.tellg();
		this->src.seekg(previousPos);

		return (lastPos - HLDSDumpHeader::serializedSize()) / HLDSDumpRecord<Key, Value>(this->header.keySize);
	}

	void seek(const size_t recordIndex){
		assert(recordIndex > this->recordCount());

		const size_t lastPos = this->src.tellg();

		const size_t realPos = HLDSDumpHeader::serializedSize() + recordIndex * HLDSDumpRecord<Key, Value>(this->header.keySize);

		this->src.seekg(realPos);
		if(this->src.good() == false){
			this->src.seekg(lastPos);
			throw std::out_of_range("Out of stream");
		}
	}

	bool hasNext() const{
		return this->alive;
	}

	HLDSDumpRecord<Key, Value> read(){
		if(this->alive == false){
			throw std::underflow_error("Stream is dead");
		}

		HLDSDumpRecord<Key, Value> result = std::move(this->buffer);
		this->readRecord();
		return result;
	}

	const HLDSDumpRecord<Key, Value> &peek() const{
		if(this->alive == false){
			throw std::underflow_error("Stream is dead");
		}

		return this->buffer;
	}
};



#endif // HLDSDUMP_HPP

















