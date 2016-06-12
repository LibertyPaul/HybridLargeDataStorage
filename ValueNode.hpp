#ifndef VALUENODE_HPP
#define VALUENODE_HPP

#include "BaseNode.hpp"
#include "CountingFactory.hpp"

#include <cassert>
#include <utility>

template<typename Key, typename Value>
class ValueNode : public BaseNode<Key, Value>{
	Value value;

	ValueNode(Value value): value(std::move(value)){}
	~ValueNode(){}

public:
	virtual const Value &getValue() const{
		return this->value;
	}

	virtual Value &getValue(){
		return this->value;
	}

	friend class CountingFactory<ValueNode>;
};

#endif // VALUENODE_HPP

