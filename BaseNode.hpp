#ifndef BASENODE_HPP
#define BASENODE_HPP

#include "NodeFactory.hpp"

#include <boost/optional.hpp>

template<typename Key, typename Value>
class NodeFactory;

template<typename Key, typename Value>
class BaseNode{
public:
	BaseNode(){}
	virtual ~BaseNode(){}

	virtual void addTail(Key &&key, const Value &value, NodeFactory<Key, Value> &nodeFactory) = 0;
	virtual void addTail(const Key &key, const Value &value, NodeFactory<Key, Value> &nodeFactory) = 0;


	virtual boost::optional<const Value &> getValue(Key &&key) const = 0;
	virtual boost::optional<const Value &> getValue(const Key &key) const = 0;

	virtual boost::optional<Value &> getValue(Key &&key) = 0;
	virtual boost::optional<Value &> getValue(const Key &key) = 0;
};

#endif // BASENODE_HPP

