#pragma once

#include <vector>

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

                Node() {}
                Node(T data, Node* parentNode = NULL) : content(data), parent(parentNode) {}
                ~Node() {
                    for (auto it = childs.begin(); it != childs.end(); ++it) {
                        delete (*it);
                    }
                    childs.clear();
                }
            };

            NodeVectorType m_nodes;
            bool m_destroyed;

        public:
            // <ITERATOR>
            // Because trees are bidimensional structures, the Iterator is who keeps track of what is begin(), end()
            // as well as "Depth"begin() and "Depth"End(). (OBS: Draw a tree to understand if things get confused.)
            // - For breadth's begin() and end() methods, it returns the current siblings vector's begin() and end() methods, respectively.
            // - For depth's begin() and end() methods, it is just a bool that tells whether the iterator can't go any further.
            // Usage:
            // the ++it and --it operators moves between sibling, reaching the end() and begin() iterator of the current vector, respectively.
            // the StepDown() moves to the first child of the current node, and StepUp() moves to it's parent, but never moves past the first
            // or last node in the current depth chain.
            
            class Iterator {
            private:
                NodeVectorIteratorType m_current;
                NodeVectorType* m_currentNodes;

                Iterator() {}

            public:
                Iterator(NodeVectorIteratorType current, NodeVectorType* currentNodes)
                    :m_current(current), m_currentNodes(currentNodes) {}

                T& operator*() {
                    return (*m_current)->content;
                }

                bool operator==(const Iterator& rhs) const {
                    return m_current == rhs.m_current && m_currentNodes == rhs.m_currentNodes;
                }

                bool operator!=(const Iterator& rhs) const {
                    return m_current != rhs.m_current || m_currentNodes != rhs.m_currentNodes;
                }

                // Advances to the next sibling.
                void operator++() {
                    ++m_current;
                }

                // Returns to the previous sibling.
                void operator--() {
                    --m_current;
                }

                // Advances to the first child.
                void StepDown() {
                    if (IsDepthEnd()) return;
                    m_current = (*m_current)->childs.begin();
                }

                // Returns to it's parent.
                void StepUp() {
                    if (IsDepthBegin()) return;
                    m_current = (*m_current)->parent->parent->childs.begin();
                }


                // BEGIN / END METHODS
                NodeVectorIteratorType BreadthBegin() {
                    return m_currentNodes->begin();
                }

                NodeVectorIteratorType BreadthEnd() {
                    return m_currentNodes->end();
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
                Node* parentNode = *(parent.m_current);
                Node* node = new Node(data, parentNode);
                parentNode->childs.push_back(node);
                return Iterator(--parentNode->childs.end(), &(parentNode->childs));
            }

            // Push as the "childIndex"th child of a specific parent.
            Iterator Push(const T& data, Iterator parent, unsigned int childIndex) {
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

                delete (*position);
                return result;
            }

            Iterator Find(const T& data) const {
                
            }

            std::vector<Iterator> FindAll(const T& data) const {

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
