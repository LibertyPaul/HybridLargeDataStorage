#ifndef VALUENODE_HPP
#define VALUENODE_HPP

#include "BaseNode.hpp"

#include <cassert>
#include <utility>

template<typename Key, typename Value>
class ValueNode : public BaseNode<Key, Value>{
	Value value;

public:
	ValueNode(Value value): value(std::move(value)){}

	virtual const Value &getValue() const{
		return this->value;
	}

	virtual Value &getValue(){
		return this->value;
	}
};

#endif // VALUENODE_HPP

