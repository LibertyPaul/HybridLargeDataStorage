#ifndef HEADSHOLDER_HPP
#define HEADSHOLDER_HPP

#include "TailTree.hpp"
#include "HeadsIterator.hpp"
#include "CountingFactory.hpp"
#include "Node.hpp"
#include "ValueNode.hpp"

#include <vector>
#include <functional>

template<typename Key, typename Value>
class HeadsIterator;

template<typename Key, typename Value>
using HeadsContainer = std::vector<TailTree<Key, Value>>;

template<typename Key, typename Value>
class HeadsHolder : protected HeadsContainer<Key, Value>{
	const size_t headKeyLength;
	const size_t tailKeyLength;

public:

	typedef HeadsIterator<Key, Value> iterator;
	typedef typename HeadsContainer<Key, Value>::value_type value_type;


	HeadsHolder(
		const size_t headKeyLength,
		const size_t tailKeyLength,
		const CountingFactory<Node<Key, Value>> &nodeFactory,
		const CountingFactory<ValueNode<Key, Value>> &valueNodeFactory
	):
		headKeyLength(headKeyLength), tailKeyLength(tailKeyLength)
	{
		size_t headCount = 1;
		for(size_t i = 0; i < this->headKeyLength; ++i){
			headCount *= Key::value_type::alphabetSize;
		}

		const TailTree<Key, Value> emptyTailTree(this->tailKeyLength + 1, nodeFactory, valueNodeFactory);

		HeadsContainer<Key, Value> &base = *this;
		base.resize(headCount, emptyTailTree);
	}

	HeadsHolder(const HeadsHolder &o):
		HeadsContainer<Key, Value>(o),
		headKeyLength(o.headKeyLength),
		tailKeyLength(o.tailKeyLength){}

	HeadsHolder(HeadsHolder &&o):
		HeadsContainer<Key, Value>(std::move(o)),
		headKeyLength(o.headKeyLength),
		tailKeyLength(o.tailKeyLength){}

	HeadsHolder &operator=(HeadsHolder o){
		assert(this->headKeyLength == o.headKeyLength);
		assert(this->tailKeyLength == o.tailKeyLength);

		this->HeadsContainer<Key, Value>::operator=(std::move(o));

		return *this;
	}

	void reset(){
		for(TailTree<Key, Value> &tailTree : *this){
			tailTree.clear();
		}
	}

	size_t size() const{
		return this->HeadsContainer<Key, Value>::size();
	}

	iterator begin(){
		Key headKey = Key::fromIndex(0, this->headKeyLength);

		HeadsContainer<Key, Value> &base = *this;
		typename HeadsContainer<Key, Value>::iterator it = base.begin();

		return iterator(it, headKey);
	}

	iterator end()/* const*/{
		HeadsContainer<Key, Value> &base = *this;
		typename HeadsContainer<Key, Value>::iterator it = base.end();

		return iterator(it, Key());
	}

	iterator find(Key headKey){
		assert(headKey.size() == this->headKeyLength);

		const size_t index = headKey.toIndex();

		HeadsContainer<Key, Value> &base = *this;
		typename HeadsContainer<Key, Value>::iterator it = base.begin() + index;

		return iterator(it, std::move(headKey));
	}

	iterator first_not_empty(){
		HeadsContainer<Key, Value> &base = *this;

		auto res = std::find_if(base.begin(), base.end(), [](const typename HeadsContainer<Key, Value>::value_type &value){
			return value.isEmpty() == false;
		});

		if(res == base.end()){
			return this->end();
		}

		const size_t index = res - base.begin();

		return iterator(std::move(res), Key::fromIndex(index, this->headKeyLength));
	}
};

#endif // HEADSHOLDER_HPP












