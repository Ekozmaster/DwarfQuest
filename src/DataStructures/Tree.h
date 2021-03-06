#pragma once

#include <vector>
#include <stdexcept>
#include <algorithm>

#include <src/Utils/Testing.h>

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
        private:
            Tree* m_ownerTree;

            Node() = delete;
            Node(const Node& node) = delete;
            Node(Node&& node) = delete;
        public:
            T content;
            Node* parent;
            NodeVectorType children;

            Node(Tree* ownerTree) : m_ownerTree(ownerTree) {
                m_ownerTree->m_size++;
                COUNT_CONSTRUCTOR_CALL("Tree::Node");
            }

            Node(Tree* ownerTree, const T& data, Node* parentNode = NULL) 
                : m_ownerTree(ownerTree), content(data), parent(parentNode) {
                m_ownerTree->m_size++;
                COUNT_CONSTRUCTOR_CALL("Tree::Node");
            }
            
            ~Node() {
                for (auto it = children.begin(); it != children.end(); ++it) {
                    delete (*it);
                }
                children.clear();
                m_ownerTree->m_size--;
                COUNT_DESTRUCTOR_CALL("Tree::Node");
            }
        };

        friend class Node;

        NodeVectorType m_nodes;
        bool m_destroyed;
        unsigned int m_size;

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
                if (node->parent != NULL) m_nodeSiblingsVector = &(node->parent->children);
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
            Iterator& operator++() {
                if (!IsBreadthEnd()) ++m_nodeIt;
                return *this;
            }

            // Returns to the previous sibling.
            Iterator& operator--() {
                if (!IsBreadthBegin()) --m_nodeIt;
                return *this;
            }

            // Advances to the first child.
            Iterator& StepDown() {
                if (!IsDepthEnd()) {
                    m_nodeSiblingsVector = &((*m_nodeIt)->children);
                    m_nodeIt = (*m_nodeIt)->children.begin();
                }
                return *this;
            }

            // Returns to it's parent.
            Iterator& StepUp() {
                if (!IsDepthBegin()) {
                    Node* parentNode = (*m_nodeIt)->parent;
                    if (parentNode->parent == NULL) m_nodeSiblingsVector = &(m_ownerTree->m_nodes);
                    else m_nodeSiblingsVector = &(parentNode->parent->children);
                    m_nodeIt = std::find(m_nodeSiblingsVector->begin(), m_nodeSiblingsVector->end(), parentNode);
                }
                return *this;
            }

            int ChildCount() {
                return (*m_nodeIt)->children.size();
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
                return (*m_nodeIt)->children.size() == 0;
            }

            friend class Tree;
        };
        // </ITERATOR>
        

        Tree() {
            m_destroyed = false;
            m_size = 0;
        }

        ~Tree() {
            Destroy();
        }

        // Push to the back of the root nodes.
        Iterator Push(const T& data) {
            Node* node = new Node(this, data);
            m_nodes.push_back(node);
            return Iterator(this, --m_nodes.end(), &m_nodes);
        }

        // Push as the last child of a specific parent.
        Iterator Push(const T& data, Iterator parent) {
            if (parent == End()) throw std::invalid_argument("Passing invalid 'parent' argument in Three.Push");
            Node* parentNode = *(parent.m_nodeIt);
            Node* node = new Node(this, data, parentNode);
            parentNode->children.push_back(node);
            return Iterator(this, --parentNode->children.end(), &(parentNode->children));
        }

        // Push as the "childIndex"th child of a specific parent.
        Iterator Push(const T& data, Iterator parent, unsigned int childIndex) {
            if (parent == End()) throw std::invalid_argument("Passing invalid 'parent' argument in Three.Push");
            Node* parentNode = *(parent.m_nodeIt);
            Node* node = new Node(this, data, parentNode);
            auto it = parentNode->children.insert(parentNode->children.begin() + childIndex, node);
            return Iterator(this, it, &(parentNode->children));
        }

        // Push as the "childIndex"th child of the root nodes.
        Iterator Push(const T& data, unsigned int childIndex) {
            Node* node = new Node(this, data);
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
            
            position.m_nodeSiblingsVector->erase(position.m_nodeIt);
            if (!node->parent) result = End();
            delete node;

            return result;
        }

    private:
        Iterator DepthFind(const T& data, Iterator currentIt) {
            if (*currentIt == data) return currentIt;
            // Step down into currentNode's children.
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

        Iterator Back() {
            if (m_nodes.size() == 0) return End();
            return --End();
        }

        unsigned int Size() {
            return m_size;
        }

        void Clear() {
            for (Iterator it = Begin(); it != End(); ++it) {
                delete* (it.m_nodeIt);
            }
            m_nodes.clear();
            m_size = 0;
        }

        void Destroy() {
            if (m_destroyed) return;
            Clear();
            m_destroyed = true;
        }
        //*/
    };

}

