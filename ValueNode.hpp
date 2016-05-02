#ifndef VALUENODE_HPP
#define VALUENODE_HPP

#include "BaseNode.hpp"

#include <cassert>
#include <utility>

template<typename Key, typename Value>
class ValueNode : public BaseNode<Key, Value>{
	Value value;

public:
	ValueNode(const Value &value): value(value){}

	virtual void addTail(Key &&, const Value &){
		assert(true);
	}

	virtual void addTail(const Key &key, const Value &value){
		Key nonConstKey(key);
		this->addTail(std::move(nonConstKey), value);
	}

	virtual Value getValue(Key &&key) const{
		Key currentKey(key);
		assert(currentKey.empty());

		return this->value;
	}

	virtual Value getValue(const Key &key) const{
		Key nonConstKey(key);
		return this->getValue(std::move(key));
	}
};

#endif // VALUENODE_HPP

