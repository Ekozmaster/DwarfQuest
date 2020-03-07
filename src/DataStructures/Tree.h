#pragma once

#include <vector>
#include <src/Utils/Testing.h>

namespace DwarfQuest {
    namespace DataStructures {

        // For a while this structure does only supports unique values.
        // Trying to use it with repeated values may result in unexpected behaviour.
        template <class T>
        class Tree {
        private:
            class Node;
            typedef typename std::vector<Node*> NodeVectorType;
            typedef typename NodeVectorType::iterator NodeVectorIteratorType;

            class Node {
            public:
                T content;
                Node* parent;
                NodeVectorType childs;

                Node() {
                    COUNT_CONSTRUCTOR_CALL;
                }

                Node(T data, Node* parentNode = NULL) : content(data), parent(parentNode) {
                    COUNT_CONSTRUCTOR_CALL;
                }
                
                ~Node() {
                    for (auto it = childs.begin(); it != childs.end(); ++it) {
                        delete (*it);
                    }
                    childs.clear();
                    COUNT_DESTRUCTOR_CALL;
                }
            };

            NodeVectorType m_nodes;
            bool m_destroyed;

        public:
            // <ITERATOR>
            // Because trees are bidimensional structures, the Iterator is who keeps track of what is begin(), end()
            // as well as "Depth"begin() and "Depth"End().
            // Usage:
            // the ++it and --it operators moves between sibling, while StepDown moves to the first child of the current node and StepUp
            // moves to the current node's parent.
            // different from std::vector, Iterators can't move past the edges, neither depth nor breadth-wise.
            
            class Iterator {
            private:
                Tree* m_ownerTree;
                NodeVectorIteratorType m_nodeIt;
                NodeVectorType* m_nodeSiblingsVector;

                Iterator() {}

                Iterator(Tree* ownerTree, NodeVectorIteratorType current)
                    :m_ownerTree(ownerTree), m_nodeIt(current) {
                    Node * node = *current;
                    if (node->parent != NULL) m_nodeSiblingsVector = &(node->parent->childs);
                    else m_nodeSiblingsVector = &(m_ownerTree->m_nodes);
                }

                Iterator(Tree *ownerTree, NodeVectorIteratorType current, NodeVectorType* currentNodes)
                    :m_ownerTree(ownerTree), m_nodeIt(current), m_nodeSiblingsVector(currentNodes) {}
            public:

                T& operator*() {
                    return (*m_nodeIt)->content;
                }

                bool operator==(const Iterator& rhs) const {
                    return m_nodeSiblingsVector == rhs.m_nodeSiblingsVector && m_nodeIt == rhs.m_nodeIt;
                }

                bool operator!=(const Iterator& rhs) const {
                    return m_nodeSiblingsVector != rhs.m_nodeSiblingsVector || m_nodeIt != rhs.m_nodeIt;
                }

                // Advances to the next sibling.
                void operator++() {
                    if (IsBreadthEnd()) return;
                    ++m_nodeIt;
                }

                // Returns to the previous sibling.
                void operator--() {
                    if (IsBreadthBegin()) return;
                    --m_nodeIt;
                }

                // Advances to the first child.
                void StepDown() {
                    if (IsDepthEnd()) return;
                    m_nodeSiblingsVector = &((*m_nodeIt)->childs);
                    m_nodeIt = (*m_nodeIt)->childs.begin();
                }

                // Returns to it's parent.
                void StepUp() {
                    if (IsDepthBegin()) return;
                    Node* parentNode = (*m_nodeIt)->parent;
                    if (parentNode->parent == NULL) m_nodeSiblingsVector = &(m_ownerTree->m_nodes);
                    else m_nodeSiblingsVector = &(parentNode->parent->childs);
                    m_nodeIt = std::find(m_nodeSiblingsVector->begin(), m_nodeSiblingsVector->end(), parentNode);
                }


                // BEGIN / END METHODS
                bool IsBreadthBegin() {
                    return m_nodeIt == m_nodeSiblingsVector->begin();
                }

                bool IsBreadthEnd() {
                    return m_nodeSiblingsVector->size() == 0 || m_nodeIt == m_nodeSiblingsVector->end();
                }

                bool IsDepthBegin() {
                    return (*m_nodeIt)->parent == NULL;
                }

                bool IsDepthEnd() {
                    return (*m_nodeIt)->childs.size() == 0;
                }

                friend class Tree;
            };
            // </ITERATOR>
            

            Tree() {
                m_destroyed = false;
            }

            ~Tree() {
                Destroy();
            }

            // Push to the back of the root nodes.
            Iterator Push(const T& data) {
                Node* node = new Node(data);
                m_nodes.push_back(node);
                return Iterator(this, --m_nodes.end(), &m_nodes);
            }

            // Push as the last child of a specific parent.
            Iterator Push(const T& data, Iterator parent) {
                if (parent == End()) throw std::invalid_argument("Passing invalid 'parent' argument in Three.Push");
                Node* parentNode = *(parent.m_nodeIt);
                Node* node = new Node(data, parentNode);
                parentNode->childs.push_back(node);
                return Iterator(this, --parentNode->childs.end(), &(parentNode->childs));
            }

            // Push as the "childIndex"th child of a specific parent.
            Iterator Push(const T& data, Iterator parent, unsigned int childIndex) {
                if (parent == End()) throw std::invalid_argument("Passing invalid 'parent' argument in Three.Push");
                Node* parentNode = *(parent.m_nodeIt);
                Node* node = new Node(data, parentNode);
                auto it = parentNode->childs.insert(parentNode->childs.begin() + childIndex, node);
                return Iterator(this, it, &(parentNode->childs));
            }

            // Push as the "childIndex"th child of the root nodes.
            Iterator Push(const T& data, unsigned int childIndex) {
                Node* node = new Node(data);
                auto it = m_nodes.insert(m_nodes.begin() + childIndex, node);
                return Iterator(this, it, &m_nodes);
            }

            // Node's destructor causes a chain reaction of destruction, so just "delete node" is enough to recursively clear everything.
            // This method returns a iterator for the parent of the destroyed item, if there is one, otherwise return End().
            Iterator Erase(const Iterator& position) {
                if (position == End()) return End();
                Iterator result = position;
                Node* node = *(position.m_nodeIt);
                if (node->parent) result.StepUp();
                else result = End();
                
                position.m_nodeSiblingsVector->erase(position.m_nodeIt);
                delete node;

                return result;
            }

        private:
            Iterator DepthFind(const T& data, Iterator currentIt) {
                if (*currentIt == data) return currentIt;
                // Step down into currentNode's childs.
                if (!currentIt.IsDepthEnd()) {
                    currentIt.StepDown();
                    for (Iterator it = currentIt; !it.IsBreadthEnd(); ++it) {
                        Iterator nextIt = DepthFind(data, it);
                        if (nextIt != End() && *nextIt == data) return nextIt;
                    }
                }
                return End();
            }

        public:
            Iterator Find(const T& data) {
                if (m_nodes.size() == 0) return End();

                for (Iterator it = Begin(); it != End(); ++it) {
                    Iterator searchIt = DepthFind(data, it);
                    if (searchIt != End()) return searchIt;
                }
                return End();
            }

            Iterator Begin() {
                return Iterator(this, m_nodes.begin(), &m_nodes);
            }

            Iterator End() {
                return Iterator(this, m_nodes.end(), &m_nodes);
            }

            void Destroy() {
                if (m_destroyed) return;
                for (Iterator it = Begin(); it != End(); ++it) {
                    delete *(it.m_nodeIt);
                }
                m_nodes.clear();
                m_destroyed = true;
            }
            //*/
        };

    }
}
