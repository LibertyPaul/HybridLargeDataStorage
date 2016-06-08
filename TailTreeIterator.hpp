#ifndef TAILTREEITERATOR_HPP
#define TAILTREEITERATOR_HPP

#include "BaseNode.hpp"
#include "Node.hpp"
#include "ValueNode.hpp"
#include "TailTree.hpp"

#include <vector>
#include <cassert>

template<typename Key, typename Value>
class BaseNode;

template<typename Key, typename Value>
class Node;

template<typename Key, typename Value>
class TailTree;

template<typename Key, typename Value>
class TailTreeIterator{
	Key tailKey;

	typedef std::vector<BaseNode<Key, Value> *> BranchHolder;
	BranchHolder branch;

protected:
	BaseNode<Key, Value> *root() const{
		return this->currentChain.front();
	}

private:
	TailTreeIterator(Key tailKey, std::vector<BaseNode<Key, Value> *> branch): tailKey(std::move(tailKey)), branch(std::move(branch)){
		assert(this->tailKey.size() + 1 == this->branch.size());

		for(size_t i = 0; i < this->branch.size() - 1; ++i){
			const Node<Key, Value> *currentNode = dynamic_cast<const Node<Key, Value> *>(this->branch.at(i));
			assert(currentNode != nullptr);

			const typename Key::value_type &currentKey = this->tailKey.at(i);

			assert(currentNode->tails.at(currentKey.toIndex()) == this->branch.at(i + 1));
		}
	}

	bool isValid() const{
		return branch.empty() == false;
	}

public:
	TailTreeIterator(){

	}

	bool operator==(const TailTreeIterator &it) const{
		return this->branch == it.branch;
	}

	bool operator!=(const TailTreeIterator &it) const{
		return !(*this == it);
	}

	const Value &operator*() const{
		if(this->isValid() == false){
			throw std::out_of_range("TailTreeIterator is invalid");
		}

		const BaseNode<Key, Value> * const last = this->branch.back();
		const ValueNode<Key, Value> * const valueNode = dynamic_cast<const ValueNode<Key, Value> * const>(last);
		assert(valueNode != nullptr);

		return valueNode->getValue();
	}

	Value &operator*(){
		if(this->isValid() == false){
			throw std::out_of_range("TailTreeIterator is invalid");
		}

		BaseNode<Key, Value> * const last = this->branch.back();
		ValueNode<Key, Value> * const valueNode = dynamic_cast<ValueNode<Key, Value> * const>(last);
		assert(valueNode != nullptr);

		return valueNode->getValue();
	}

	TailTreeIterator &operator++(){
		if(*this == false){
			return *this; // why not? :)
		}

		const typename Node<Key, Value>::Direction direction = Node<Key, Value>::Direction::higher;

		for(int i = this->branch.size() - 2; i > 0; --i){
			Node<Key, Value> *current = dynamic_cast<Node<Key, Value> *>(this->branch.at(i));
			assert(current != nullptr);

			const typename Key::value_type &keyItem = this->tailKey.at(i);
			typename Node<Key, Value>::BranchInfo closestExistingBranchInfo = current->getClosestExistingBranch(direction, keyItem.toIndex());

			if(closestExistingBranchInfo.first != nullptr){
				this->branch.erase(this->branch.begin() + i, this->branch.end());

				do{
					this->branch.push_back(closestExistingBranchInfo.first);
					this->tailKey.push_back(closestExistingBranchInfo.second);

					Node<Key, Value> *node = dynamic_cast<Node<Key, Value> *>(this->branch.back());
					if(node != nullptr){
						closestExistingBranchInfo = node->getClosestExistingBranch(direction, this->tailKey.back().toIndex());
					}
					else{
						break;
					}
				}while(closestExistingBranchInfo.first != nullptr);

				break;
			}
		}

		return *this;
	}



	friend void swap(TailTreeIterator &lhs, TailTreeIterator &rhs){
		std::swap(lhs.branch, rhs.branch);
		std::swap(lhs.tailKey, rhs.tailKey);
	}

	friend class TailTree<Key, Value>;
};


#endif // TAILTREEITERATOR_HPP

