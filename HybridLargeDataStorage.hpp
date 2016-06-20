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
#include <random>
#include <chrono>

template<typename Key, typename Value>
class TailTree;

template<typename Key, typename Value>
class HLDSIterator;

template<typename Key, typename Value>
class HybridLargeDataStorage{
public:
	typedef HLDSIterator<Key, Value> iterator;
	
private:
	size_t id;
	size_t headSize;
	size_t tailSize;
	size_t itemCount = 0;

	CountingFactory<Node<Key, Value>> nodeFactory;
	CountingFactory<ValueNode<Key, Value>> valueNodeFactory;

	HeadsHolder<Key, Value> headsHolder;


	static size_t generateRandomId(){
		static std::default_random_engine re(std::chrono::system_clock::now().time_since_epoch().count());
		static std::uniform_int_distribution<size_t> distr;

		return distr(re);
	}


public:
	HybridLargeDataStorage(const size_t id, const size_t headSize, const size_t tailSize):
		id(id),
		headSize(headSize),
		tailSize(tailSize),
		nodeFactory(),
		valueNodeFactory(),
		headsHolder(headSize, tailSize, nodeFactory, valueNodeFactory){}

	HybridLargeDataStorage(const size_t headSize, const size_t tailSize):
		HybridLargeDataStorage(HybridLargeDataStorage::generateRandomId(), headSize, tailSize){}

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
		this->headsHolder.reset();
	}

	size_t keySize() const{
		return this->headSize + this->tailSize;
	}

	size_t getId() const{
		return this->id;
	}

private:
	std::pair<Key, Key> splitKey(Key key) const{
		Key tailKey(this->tailSize);
		std::copy(key.cbegin() + this->headSize, key.cend(), tailKey.begin());

		key.resize(this->headSize);

		return std::make_pair(std::move(key), std::move(tailKey));
	}

public:
	void insert(Key key, const Value &value){
		assert(key.size() == this->keySize());

		std::pair<Key, Key> splittedKey = this->splitKey(std::move(key));

		typename HeadsHolder<Key, Value>::iterator head = this->headsHolder.find(std::move(splittedKey.first));
		assert(head != this->headsHolder.end());

		head->addTail(std::move(splittedKey.second), value);
		++this->itemCount;
	}

	iterator find(const Key &key){
		assert(key.size() == this->keySize());

		std::pair<Key, Key> splittedKey = this->splitKey(std::move(key));

		const typename HeadsHolder<Key, Value>::iterator headsIterator = this->headsHolder.find(std::move(splittedKey.first));
		assert(headsIterator != this->headsHolder.end());

		typename TailTree<Key, Value>::iterator tailIterator = headsIterator->find(std::move(splittedKey.second));
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

	size_t getApproximateRAMUsage() const{
		const size_t headsHolderSize = this->headsHolder.size() * sizeof(typename HeadsHolder<Key, Value>::value_type);
		const size_t nodesSize = this->nodeFactory.producedItemsCount() * sizeof(Node<Key, Value>);
		const size_t valueNodesSize = this->valueNodeFactory.producedItemsCount() * sizeof(ValueNode<Key, Value>);

		return headsHolderSize + nodesSize + valueNodesSize;
	}

	bool operator==(/*const */HybridLargeDataStorage &o)/*const*/{
		if(this->itemCount != o.itemCount){
			return false;
		}

		return std::equal(this->begin(), this->end(), o.begin());
	}

	bool operator!=(/*const */HybridLargeDataStorage &o)/*const*/{
		return !(*this == o);
	}
};

#endif // HYBRIDLARGEDATASTORAGE_HPP
