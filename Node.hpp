#ifndef NODE_HPP
#define NODE_HPP

#include "BaseNode.hpp"
#include "NodeFactory.hpp"

#include <array>
#include <utility>
#include <cassert>
#include <boost/optional.hpp>

template<typename Key, typename Value>
class NodeFactory;

template<typename Key, typename Value>
class BaseNode;

template<typename Key, typename Value>
class Node : public BaseNode<Key, Value>{
	std::array<BaseNode<Key, Value> *, Key::alphabetSize> tails;

public:
	Node(){
		this->tails.fill(nullptr);
	}

	virtual ~Node(){
		for(auto &tail : this->tails){
			delete tail;
		}
	}

	virtual void addTail(Key &&key, const Value &value, NodeFactory<Key, Value> &nodeFactory){
		Key currentKey(key);
		assert(key.empty() == false);

		const size_t index = currentKey.front().toIndex();
		currentKey.pop_front();

		if(this->tails.at(index) == nullptr){
			this->tails.at(index) = nodeFactory.createNode(currentKey, value);
		}

		if(currentKey.size() > 0){
			this->tails.at(index)->addTail(std::move(currentKey), value, nodeFactory);
		}
	}

	virtual void addTail(const Key &key, const Value &value, NodeFactory<Key, Value> &nodeFactory){
		Key nonConstKey(key);
		this->addTail(std::move(nonConstKey), value, nodeFactory);
	}


	virtual boost::optional<const Value &> getValue(Key &&key) const{
		Key currentKey(key);
		assert(key.empty() == false);

		const size_t index = currentKey.front().toIndex();
		if(this->tails.at(index) == nullptr){
			return boost::optional<const Value &>();
		}

		currentKey.pop_front();

		const BaseNode<Key, Value> *tail = this->tails.at(index);
		return tail->getValue(std::move(currentKey));
	}

	virtual boost::optional<const Value &> getValue(const Key &key) const{
		Key nonConstKey(key);
		return this->getValue(std::move(nonConstKey));
	}



	virtual boost::optional<Value &> getValue(Key &&key){
		Key currentKey(key);
		assert(key.empty() == false);

		const size_t index = currentKey.front().toIndex();
		if(this->tails.at(index) == nullptr){
			return boost::optional<Value &>();
		}

		currentKey.pop_front();

		return this->tails.at(index)->getValue(std::move(currentKey));
	}

	virtual boost::optional<Value &> getValue(const Key &key){
		Key nonConstKey(key);
		return this->getValue(std::move(nonConstKey));
	}
};

#endif // NODE_HPP

