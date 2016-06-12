#ifndef HYBRIDLARGEDATASTORAGE_HPP
#define HYBRIDLARGEDATASTORAGE_HPP

#include "BaseNode.hpp"
#include "Node.hpp"
#include "ValueNode.hpp"
#include "TailTree.hpp"
#include "HLDSIterator.hpp"
#include "HeadsHolder.hpp"
#include "CountingFactory.hpp"

#include <utility>
#include <vector>
#include <cassert>
#include <cstddef>
#include <algorithm>
#include <stdexcept>

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

	CountingFactory<Node<Key, Value>> nodeFactory;
	CountingFactory<ValueNode<Key, Value>> valueNodeFactory;

	HeadsHolder<Key, Value> headsHolder;



public:
	HybridLargeDataStorage(const size_t headSize, const size_t tailSize):
		headSize(headSize),
		tailSize(tailSize),
		nodeFactory(),
		valueNodeFactory(),
		headsHolder(headSize, tailSize, nodeFactory, valueNodeFactory){}

	HybridLargeDataStorage(const HybridLargeDataStorage &o):
		headSize(o.headSize),
		tailSize(o.tailSize),
		nodeFactory(o.nodeFactory),
		valueNodeFactory(o.valueNodeFactory),
		headsHolder(o.headsHolder){}

	HybridLargeDataStorage(HybridLargeDataStorage &&o):
		headSize(o.headSize),
		tailSize(o.tailSize),
		nodeFactory(std::move(o.nodeFactory)),
		valueNodeFactory(std::move(o.valueNodeFactory)),
		headsHolder(std::move(o.headsHolder)){}

	~HybridLargeDataStorage(){}

	HybridLargeDataStorage &operator=(HybridLargeDataStorage o){
		if(this->headSize != o.headSize){
			throw std::logic_error("this->headSize must be equal to o.headSize");
		}

		if(this->tailSize != o.tailSize){
			throw std::logic_error("this->tailSize must be equal to o.tailSize");
		}

		this->nodeFactory = std::move(o.nodeFactory);
		this->valueNodeFactory = std::move(o.valueNodeFactory);
		this->headsHolder = std::move(o.headsHolder);

		return *this;
	}

	void clear(){
		this->nodeFactory.reset();
		this->valueNodeFactory.reset();
		this->headsHolder.reset(this->nodeFactory, this->valueNodeFactory);
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
		const typename HeadsHolder<Key, Value>::iterator firstNotEmptyHead = this->headsHolder.first_not_empty();

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

	typedef size_t NodeCount;
	typedef size_t ValueNodeCount;

	std::pair<NodeCount, ValueNodeCount> getProducedNodeCount() const{
		return std::make_pair(this->nodeFactory.producedItemsCount(), this->valueNodeFactory.producedItemsCount());
	}

};

#endif // HYBRIDLARGEDATASTORAGE_HPP
