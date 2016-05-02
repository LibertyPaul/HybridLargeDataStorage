#ifndef HYBRIDLARGEDATASTORAGE_HPP
#define HYBRIDLARGEDATASTORAGE_HPP

#include "BaseNode.hpp"
#include "NodeFactory.hpp"

#include <utility>
#include <vector>
#include <cassert>
#include <cstddef>
#include <boost/optional.hpp>

template<typename Key, typename Value>
class HybridLargeDataStorage{
	const size_t headSize;
	const size_t tailSize;
	std::vector<BaseNode<Key, Value> *> heads;


	size_t getIndex(const Key &key) const{
		assert(key.size() >= this->headSize);
		size_t result = 0;
		size_t multiplier = 1;

		for(size_t i = 0; i < this->headSize; ++i){
			result += key.at(i).toIndex() * multiplier;
			multiplier *= Key::alphabetSize;
		}

		return result;
	}

public:
	HybridLargeDataStorage(const size_t headSize, const size_t tailSize): headSize(headSize), tailSize(tailSize){
		size_t headCount = 1;
		for(size_t i = 0; i < this->headSize; ++i){
			headCount *= Key::alphabetSize;
		}

		this->heads.resize(headCount);
	}

	void insert(Key key, const Value &value){
		assert(key.size() == this->headSize + this->tailSize);

		const size_t index = this->getIndex(key);
		for(size_t i = 0; i < this->headSize; ++i){
			key.pop_front();
		}

		if(this->heads.at(index) == nullptr){
			this->heads.at(index) = NodeFactory<Key, Value>::createNode(key, value);
		}

		this->heads.at(index)->addTail(std::move(key), value);
	}

	boost::optional<const Value &> getValue(Key key) const{
		assert(key.size() == this->headSize + this->tailSize);

		const size_t index = this->getIndex(key);
		for(size_t i = 0; i < this->headSize; ++i){
			key.pop_front();
		}

		if(this->heads.at(index) == nullptr){
			return boost::optional<Value &>();
		}

		return this->heads.at(index)->getValue(std::move(key));

	}


	boost::optional<Value &> getValue(Key key){
		assert(key.size() == this->headSize + this->tailSize);

		const size_t index = this->getIndex(key);
		for(size_t i = 0; i < this->headSize; ++i){
			key.pop_front();
		}

		if(this->heads.at(index) == nullptr){
			return boost::optional<Value &>();
		}

		return this->heads.at(index)->getValue(std::move(key));

	}

};

#endif // HYBRIDLARGEDATASTORAGE_HPP
