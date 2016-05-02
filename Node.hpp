#ifndef NODE_HPP
#define NODE_HPP

#include "BaseNode.hpp"
#include "NodeFactory.hpp"

#include <array>
#include <utility>
#include <cassert>

template<typename Key, typename Value>
class NodeFactory;

template<typename Key, typename Value>
class Node : public BaseNode<Key, Value>{
	std::array<BaseNode<Key, Value> *, Key::alphabetSize> tails;

public:
	Node(){
		this->tails.fill(nullptr);
	}

	virtual ~Node(){
		for(const auto &tail : this->tails){
			delete tail;
		}
	}

	virtual void addTail(Key &&key, const Value &value){
		Key currentKey(key);
		assert(key.empty() == false);

		const size_t index = currentKey.front().toIndex();
		currentKey.pop_front();

		if(this->tails.at(index) == nullptr){
			this->tails.at(index) = NodeFactory<Key, Value>::createNode(currentKey, value);
		}

		if(currentKey.size() > 0){
			this->tails.at(index)->addTail(std::move(currentKey), value);
		}
	}

	virtual void addTail(const Key &key, const Value &value){
		Key nonConstKey(key);
		this->addTail(std::move(nonConstKey), value);
	}


	virtual Value getValue(Key &&key) const{
		Key currentKey(key);
		assert(key.empty() == false);

		const size_t index = currentKey.front().toIndex();
		currentKey.pop_front();

		return this->tails.at(index)->getValue(std::move(currentKey));
	}

	virtual Value getValue(const Key &key) const{
		Key nonConstKey(key);
		return this->getValue(std::move(nonConstKey));
	}
};

#endif // NODE_HPP

