#ifndef TAILTREE_HPP
#define TAILTREE_HPP

#include "BaseNode.hpp"
#include "TailTreeIterator.hpp"

#include <stdexcept>

template<typename Key, typename Value>
class TailTree{
	BaseNode<Key, Value> *root = nullptr;
	const size_t depth;

public:
	typedef TailTreeIterator<Key, Value> iterator;

	TailTree(const size_t depth): depth(depth){}

	TailTree(const TailTree &tailTree): root(tailTree.root), depth(tailTree.depth){}

	TailTree(TailTree &&tailTree): depth(tailTree.depth){
		this->root = tailTree.root;
		tailTree.root = nullptr;
	}

	void addTail(const Key &key, Value value){
		assert(key.size() == depth - 1);

		BaseNode<Key, Value> **current = &this->root;
		for(const auto keyItem : key){
			Node<Key, Value> *node = nullptr;

			if(*current == nullptr){
				node = new Node<Key, Value>();
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

		*current = new ValueNode<Key, Value>(std::move(value));
	}

protected:
	bool isEmpty() const{
		return this->root == nullptr;
	}

public:
	iterator begin() const{
		if(this->isEmpty()){
			return iterator();
		}

		std::vector<BaseNode<Key, Value> *> branch;
		branch.reserve(this->depth);
		branch.push_back(this->root);

		Key key;
		key.reserve(this->depth);

		Node<Key, Value> *node = nullptr;

		while((node = dynamic_cast<Node<Key, Value> *>(branch.back())) != nullptr){
			const auto branchInfo = node->getClosestExistingBranch(-1);
			branch.push_back(branchInfo.first);
			key.push_back(branchInfo.second);
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

		typename iterator::BranchHolder branch;
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

		this->root = tailTree.root;

		return *this;
	}
};

#endif // TAILTREE_HPP

