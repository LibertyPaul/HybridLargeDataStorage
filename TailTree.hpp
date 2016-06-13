#ifndef TAILTREE_HPP
#define TAILTREE_HPP

#include "BaseNode.hpp"
#include "Node.hpp"
#include "ValueNode.hpp"
#include "TailTreeIterator.hpp"
#include "CountingFactory.hpp"

#include <stdexcept>

template<typename Key, typename Value>
class TailTree{
	BaseNode<Key, Value> *root = nullptr;
	const size_t depth;

	const CountingFactory<Node<Key, Value>> &nodeFactory;
	const CountingFactory<ValueNode<Key, Value>> &valueNodeFactory;

public:
	typedef TailTreeIterator<Key, Value> iterator;

	TailTree(
		const size_t depth,
		const CountingFactory<Node<Key, Value>> &nodeFactory,
		const CountingFactory<ValueNode<Key, Value>> &valueNodeFactory
	):
		depth(depth),
		nodeFactory(nodeFactory),
		valueNodeFactory(valueNodeFactory)
	{

	}

	TailTree(const TailTree &tailTree):
		root(tailTree.root),
		depth(tailTree.depth),
		nodeFactory(tailTree.nodeFactory),
		valueNodeFactory(tailTree.valueNodeFactory)
	{

	}

	TailTree(TailTree &&tailTree):
		root(tailTree.root),
		depth(tailTree.depth),
		nodeFactory(tailTree.nodeFactory),
		valueNodeFactory(tailTree.valueNodeFactory)
	{
		tailTree.root = nullptr;
	}

	~TailTree(){
		delete this->root;
	}

	void addTail(const Key &key, Value value){
		assert(key.size() == depth - 1);

		BaseNode<Key, Value> **current = &this->root;
		for(const auto keyItem : key){
			Node<Key, Value> *node = nullptr;

			if(*current == nullptr){
				node = this->nodeFactory.create();
				*current = node;
			}
			else{
				node = dynamic_cast<Node<Key, Value> *>(*current);
			}

			assert(node != nullptr);

			current = &node->tails.at(keyItem.toIndex());
		}

		if(*current != nullptr){
			throw std::runtime_error("Node with this key already exists");
		}

		*current = this->valueNodeFactory.create(std::move(value));
	}

public:
	bool isEmpty() const{
		return this->root == nullptr;
	}

	iterator begin() const{
		if(this->isEmpty()){
			return iterator();
		}

		BranchHolder<Key, Value> branch;
		branch.reserve(this->depth);
		branch.push_back(this->root);

		Key key;
		key.reserve(this->depth);

		Node<Key, Value> *node = nullptr;

		while((node = dynamic_cast<Node<Key, Value> *>(branch.back())) != nullptr){
			const auto branchInfo = node->getClosestExistingBranch(Node<Key, Value>::Direction::higher, -1);
			branch.push_back(branchInfo.first);
			key.push_back(Key::value_type::fromIndex(branchInfo.second));
		}


		return iterator(std::move(key), std::move(branch));
	}

	iterator end() const{
		return iterator();
	}

	iterator find(Key key) const{
		if(this->root == nullptr){
			return iterator();
		}

		BranchHolder<Key, Value> branch;
		branch.reserve(key.size() + 1);

		branch.push_back(this->root);
		for(const auto value : key){
			Node<Key, Value> *node = dynamic_cast<Node<Key, Value> *>(branch.back());
			assert(node != nullptr);

			BaseNode<Key, Value> *next = node->tails.at(value.toIndex());
			if(next == nullptr){
				return iterator();
			}

			branch.push_back(next);
		}

		return iterator(std::move(key), std::move(branch));
	}

	TailTree &operator=(const TailTree &tailTree){ // TODO check if deep copy is needed
		assert(this->depth == tailTree.depth);

		delete this->root;

		this->root = tailTree.root;

		return *this;
	}

	void clear(){
		delete this->root;
		this->root = nullptr;
	}
};

#endif // TAILTREE_HPP

