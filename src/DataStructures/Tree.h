#pragma once

#include <vector>
#include <src/Utils/Testing.h>

namespace DwarfQuest {
    namespace DataStructures {

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
                    COUNT_CONSTRUCTOR_CALL(this);
                }

                Node(T data, Node* parentNode = NULL) : content(data), parent(parentNode) {
                    COUNT_CONSTRUCTOR_CALL(this);
                }
                
                ~Node() {
                    for (auto it = childs.begin(); it != childs.end(); ++it) {
                        delete (*it);
                    }
                    childs.clear();
                    COUNT_DESTRUCTOR_CALL(this);
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
                NodeVectorIteratorType m_current;
                NodeVectorType* m_currentNodes;

                Iterator() {}

            public:
                Iterator(NodeVectorIteratorType current)
                    :m_current(current) {
                    Node* node = *current;
                    if (node->parent != NULL) m_currentNodes = &(node->parent->childs);
                    else m_currentNodes = &m_nodes;
                }

                Iterator(NodeVectorIteratorType current, NodeVectorType* currentNodes)
                    :m_current(current), m_currentNodes(currentNodes) {}

                T& operator*() {
                    return (*m_current)->content;
                }

                bool operator==(const Iterator& rhs) const {
                    return m_currentNodes == rhs.m_currentNodes && m_current == rhs.m_current;
                }

                bool operator!=(const Iterator& rhs) const {
                    return m_currentNodes != rhs.m_currentNodes || m_current != rhs.m_current;
                }

                // Advances to the next sibling.
                void operator++() {
                    if (IsBreadthEnd()) return;
                    ++m_current;
                }

                // Returns to the previous sibling.
                void operator--() {
                    if (IsBreadthBegin()) return;
                    --m_current;
                }

                // Advances to the first child.
                void StepDown() {
                    if (IsDepthEnd()) return;
                    m_currentNodes = &((*m_current)->childs);
                    m_current = (*m_current)->childs.begin();
                }

                // Returns to it's parent.
                void StepUp() {
                    if (IsDepthBegin()) return;
                    Node* parentNode = (*m_current)->parent;
                    if (parentNode->parent == NULL) m_currentNodes = &m_nodes;
                    else m_currentNodes = &(parentNode->parent->childs);
                    m_current = m_currentNodes->begin();
                }


                // BEGIN / END METHODS
                bool IsBreadthBegin() {
                    return m_current == m_currentNodes->begin();
                }

                bool IsBreadthEnd() {
                    return m_currentNodes->size() == 0 || m_current == m_currentNodes->end();
                }

                bool IsDepthBegin() {
                    return (*m_current)->parent == NULL;
                }

                bool IsDepthEnd() {
                    return (*m_current)->childs.size() == 0;
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
                return Iterator(--m_nodes.end(), &m_nodes);
            }

            // Push as the last child of a specific parent.
            Iterator Push(const T& data, Iterator parent) {
                if (parent == End()) throw std::invalid_argument("Passing invalid 'parent' argument in Three.Push");
                Node* parentNode = *(parent.m_current);
                Node* node = new Node(data, parentNode);
                parentNode->childs.push_back(node);
                return Iterator(--parentNode->childs.end(), &(parentNode->childs));
            }

            // Push as the "childIndex"th child of a specific parent.
            Iterator Push(const T& data, Iterator parent, unsigned int childIndex) {
                if (parent == End()) throw std::invalid_argument("Passing invalid 'parent' argument in Three.Push");
                Node* parentNode = *(parent.m_current);
                Node* node = new Node(data, parentNode);
                auto it = parentNode->childs.insert(parentNode->childs.begin() + childIndex, node);
                return Iterator(it, &(parentNode->childs));
            }

            // Push as the "childIndex"th child of the root nodes.
            Iterator Push(const T& data, unsigned int childIndex) {
                Node* node = new Node(data);
                auto it = m_nodes.insert(m_nodes.begin() + childIndex, node);
                return Iterator(it, &m_nodes);
            }

            // Node's destructor causes a chain reaction of destruction, so just "delete node" is enough to recursively clear everything.
            // This method returns a iterator for the parent of the destroyed item, if there is one, otherwise return End().
            Iterator Erase(const Iterator& position) {
                Iterator result = position;
                if (result.IsDepthBegin()) result = End();
                else result.StepUp();

                delete *(position.m_current);
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

            std::vector<Iterator> FindAll(const T& data) {

            }

            Iterator Begin() {
                return Iterator(m_nodes.begin(), &m_nodes);
            }

            Iterator End() {
                return Iterator(m_nodes.end(), &m_nodes);
            }

            void Destroy() {
                if (m_destroyed) return;
                // else Do Magic
            }
            //*/
        };

    }
}
