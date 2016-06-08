#ifndef HYBRIDLARGEDATASTORAGE_HPP
#define HYBRIDLARGEDATASTORAGE_HPP

#include "BaseNode.hpp"
#include "TailTree.hpp"
#include "HLDSIterator.hpp"
#include "HeadsHolder.hpp"

#include <utility>
#include <vector>
#include <cassert>
#include <cstddef>
#include <algorithm>

template<typename Key, typename Value>
class TailTree;

template<typename Key, typename Value>
class HLDSIterator;

template<typename Key, typename Value>
class HybridLargeDataStorage{
public:
	typedef HLDSIterator<Key, Value> iterator;
	
private:
	const size_t headSize;
	const size_t tailSize;
	HeadsHolder<Key, Value> headsHolder;


public:
	HybridLargeDataStorage(const size_t headSize, const size_t tailSize): headSize(headSize), tailSize(tailSize), headsHolder(headSize, tailSize){

	}

	~HybridLargeDataStorage(){

	}

	void insert(Key key, const Value &value){
		assert(key.size() == this->headSize + this->tailSize);

		Key headKey(this->headSize);
		std::copy(key.cbegin(), key.cbegin() + this->headSize, headKey.begin());

		typename HeadsHolder<Key, Value>::iterator head = this->headsHolder.find(std::move(headKey));
		assert(head != this->headsHolder.end());

		Key tailKey(this->tailSize);
		std::copy(key.cbegin() + this->headSize, key.cend(), tailKey.begin());

		head->addTail(tailKey, value);
	}

	iterator find(const Key &key){
		assert(key.size() == this->headSize + this->tailSize);

		Key headKey(this->headSize);
		std::copy(key.begin(), key.begin() + this->headSize, headKey.begin());

		const typename HeadsHolder<Key, Value>::iterator headsIterator = this->headsHolder.find(headKey);
		assert(headsIterator != this->headsHolder.end());

		Key tailKey(this->tailSize);
		std::copy(key.begin() + this->headSize, key.end(), tailKey.begin());

		typename TailTree<Key, Value>::iterator tailIterator = headsIterator->find(std::move(tailKey));
		typename TailTree<Key, Value>::iterator tailTreeEnd = headsIterator->end();

		return iterator(std::move(headsIterator), this->headsHolder.end(), std::move(tailIterator), std::move(tailTreeEnd));
	}

	iterator begin(){
		const typename HeadsHolder<Key, Value>::iterator firstNotEmptyHead = std::find_if(this->headsHolder.begin(), this->headsHolder.end(), [](const typename HeadsHolder<Key, Value>::value_type &head){
			return head.begin() != head.end();
		});

		if(firstNotEmptyHead == this->headsHolder.end()){
			return this->end();
		}

		typename TailTree<Key, Value>::iterator tailTreeIterator = firstNotEmptyHead->begin();
		typename TailTree<Key, Value>::iterator tailTreeEnd = firstNotEmptyHead->end();

		return iterator(std::move(firstNotEmptyHead), this->headsHolder.end(), std::move(tailTreeIterator), std::move(tailTreeEnd));
	}

	iterator end(){
		return iterator();
	}

};

#endif // HYBRIDLARGEDATASTORAGE_HPP
