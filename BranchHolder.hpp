#ifndef BRANCHHOLDER_HPP
#define BRANCHHOLDER_HPP

#include "BaseNode.hpp"
#include "Node.hpp"
#include "ValueNode.hpp"

#include <vector>

template<typename Key, typename Value>
using BranchContainer = std::vector<BaseNode<Key, Value> *>;

template<typename Key, typename Value>
class BranchHolder : public BranchContainer<Key, Value>{
public:
	typedef typename BranchContainer<Key, Value>::value_type value_type;

	BranchHolder(){}
	BranchHolder(const BranchHolder &bh): BranchContainer<Key, Value>(bh){}
	BranchHolder(BranchHolder &&bh): BranchContainer<Key, Value>(bh){}

	BranchHolder &operator=(const BranchHolder &bh){
		static_cast<BranchContainer<Key, Value> &>(*this) = bh;
		return *this;
	}

	BranchHolder &operator=(BranchHolder &&bh){
		static_cast<BranchContainer<Key, Value> &>(*this) = bh;
		return *this;
	}

	size_t size() const{
		return this->BranchContainer<Key, Value>::size();
	}

	void reserve(const size_t size){
		this->BranchContainer<Key, Value>::reserve(size);
	}

	void push_back(const value_type &value){
		if(this->complete()){
			throw std::logic_error("Can't insert anything after another ValueNode");
		}

		this->BranchContainer<Key, Value>::push_back(value);
	}

	bool complete() const{
		return this->empty() == false && dynamic_cast<const ValueNode<Key, Value> *>(this->back()) != nullptr;
	}

	const value_type &root() const{
		return this->front();
	}

	value_type &root(){
		return this->front();
	}

	const ValueNode<Key, Value> *valueNode() const{
		const ValueNode<Key, Value> *result = dynamic_cast<const ValueNode<Key, Value> *>(this->back());

		assert(result != nullptr);

		return result;
	}

	ValueNode<Key, Value> *valueNode(){
		ValueNode<Key, Value> *result = dynamic_cast<ValueNode<Key, Value> *>(this->back());

		assert(result != nullptr);

		return result;
	}

};

#endif // BRANCHHOLDER_HPP

