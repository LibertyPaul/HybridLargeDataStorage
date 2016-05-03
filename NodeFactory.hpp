#ifndef NODEFACTORY_HPP
#define NODEFACTORY_HPP

#include "BaseNode.hpp"
#include "Node.hpp"
#include "ValueNode.hpp"

#include <boost/pool/object_pool.hpp>


template<typename Key, typename Value>
class NodeFactory{
	//boost::object_pool<Node<Key, Value>> nodePool;
	//boost::object_pool<ValueNode<Key, Value>> valueNodePool;
public:
	BaseNode<Key, Value> *createNode(const Key &key, const Value &value){
		if(key.empty()){
			//return this->valueNodePool.construct(value);
			return new ValueNode<Key, Value>(value);
		}
		else{
			//return this->nodePool.construct();
			return new Node<Key, Value>();
		}
	}
};

#endif // NODEFACTORY_HPP
