//
// Copyright (c) 2013, Christian Speich
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#import <CoreSystem/CommonTypes.h>
#import "Error/Assert.h"
#import "Utils/KObject.h"

#pragma once

// If you want either key or value to be
// retained use Ptr<Key> as Key
template<class Key, class Value>
class Dictionary : public KObject {
	class DictionaryNode : public KObject {
	public:
		Key key;
		Value value;
	
		Ptr<DictionaryNode> leftChild;
		Ptr<DictionaryNode> rightChild;
		
		DictionaryNode(Key _key, Value _value) : key(_key), value(_value) {}
	};

private:
	Ptr<DictionaryNode> headNode;
	
public:
	Dictionary() {}
	
	void set(Key _key, Value _value)
	{
		if (!this->headNode) {
			this->headNode = new DictionaryNode(_key, _value);
		}
		else {
			Ptr<DictionaryNode> node = this->headNode;
			
			while (node) {
				if (node->key == _key) {
					node->value = _value;
					return;
				}
				else if (node->key < _key) {
					if (node->leftChild)
						node = node->leftChild;
					else {
						node->leftChild = new DictionaryNode(_key, _value);
						return;
					}
				}
				else if (node->key > _key) {
					if (node->rightChild)
						node = node->rightChild;
					else {
						node->rightChild = new DictionaryNode(_key, _value);
						return;
					}
				}
			}
		}
	}
	
	Value get(Key _key) const
	{
		Ptr<DictionaryNode> node = this->headNode;
			
		while (node) {
			if (node->key == _key)
				break;
			else if (node->key < _key)
				node = node->leftChild;
			else if (node->key > _key)
				node = node->rightChild;
		}
		
		if (node)
			return node->value;
		return 0;
	}
	
	void remove(Key _key)
	{
		Ptr<DictionaryNode> parent;
		Ptr<DictionaryNode> node = this->headNode;
		
		while (node) {
			if (node->key == _key) {
				if (parent) {
					// We have both childs so move right one up
					// and make left the new left leaf of the right child
					if (node->leftChild && node->rightChild) {
						Ptr<DictionaryNode> n = node->rightChild;
						
						while (n->leftChild) n = n->leftChild;
						
						n->leftChild = node->leftChild;
						node->leftChild = NULL;
					}
					
					if (node->leftChild) {
						if (parent->leftChild == node)
							parent->leftChild = node->leftChild;
						else
							parent->rightChild = node->leftChild;
					}
					else if (node->rightChild) {
						if (parent->leftChild == node)
							parent->leftChild = node->rightChild;
						else
							parent->rightChild = node->rightChild;
					}
				}
				else
					this->headNode = NULL;
			}
			else if (node->key < _key) {
				parent = node;
				node = node->leftChild;
			}
			else if (node->key > _key) {
				parent = node;
				node = node->rightChild;
			}
		}
	}
};
