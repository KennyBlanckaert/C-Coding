#ifndef __RBTree_H
#define __RBTree_H

#include <cstdlib>
#include <iostream>
#include <queue>
#include <memory>
#include <functional>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cassert>

using namespace std;

// Enumeration color
enum RBColor {red, black};

// Node class
template <class Key>
class RBNode;

// Tree class
template <class Key>
class RBTree : public unique_ptr<RBNode<Key>>{
    public:
        using unique_ptr<RBNode<Key>>::unique_ptr;

        // Constructors
        RBTree() { };
        RBTree(unique_ptr<RBNode<Key>>&& k) : unique_ptr<RBNode<Key>>(move(k)) { };
        RBTree(const vector<Key>& keys, const vector<Key>& red_keys) { 
            for (int i = 0; i < keys.size(); i++) {
                this->add(keys[i], RBColor::black);
            }

            convertToRed(red_keys);
        };

        // Function declarations
        void rotate(bool left) {
            if (left) {
                RBTree<Key> child = move((*this)->right);

                (*this)->right = move(child->left);
                child.get()->left = move(*this);
                *this = move(child);

                (*this)->parent = (*this)->left->parent;
                (*this)->left->parent = this->get();
                if ((*this)->left->right) {
                    (*this)->left->right->parent = (*this)->left.get();
                }
            }
            else {
                RBTree<Key> child = move((*this)->left);

                (*this)->left = move(child->right);
                child.get()->right = move(*this);
                *this = move(child);

                (*this)->parent = (*this)->right->parent;
                (*this)->right->parent = this->get();
                if ((*this)->right->left) {
                    (*this)->right->left->parent = (*this)->right.get();
                }
            }
        }
        
        void add(const Key& key, RBColor color = RBColor::red) {

            RBTree<Key>* current = this;
            RBNode<Key>* parent = 0;
            while (*current && current->get()->key != key) {
                parent = current->get();
                if (key < current->get()->key) {
                    current = &(current->get()->left);
                }
                else {
                    current = &(current->get()->right);
                }
            }

            RBTree<Key> add = make_unique<RBNode<Key>>(key, color, parent);
            *current = move(add);

            this->checkColors(current);
        }

        void convertToRed(const vector<Key>& red_keys) {
            
            for (int i = 0; i < red_keys.size(); i++) {
                Key k = red_keys[i];

                RBTree<Key>* current = this;
                RBNode<Key>* parent = 0;
                while (*current && current->get()->key != k) {
                    parent = current->get();
                    if (k < current->get()->key) {
                        current = &(current->get()->left);
                    }
                    else {
                        current = &(current->get()->right);
                    }
                }

                current->get()->color = RBColor::red;
            }
        }

        void getFamily(const RBNode<Key>* c, RBTree<Key>*& p, RBTree<Key>*& g, RBTree<Key>*& u) {
            
            RBNode<Key>* p_node = c->parent;
            RBNode<Key>* g_node = p_node->parent;

            // Get grandparent tree (if exists)
            if (g_node == this->get()) {
                g = this; 
            }
            else {
                RBNode<Key>* top_node = g_node->parent;
                if (g_node == top_node->left.get()) {
                    g = &(top_node->left);
                }
                else if (g_node == top_node->right.get()) {
                    g = &(top_node->right);
                }
            }

            if (p_node == g_node->left.get()) {
                p = &(g_node->left);
                u = &(g_node->right);
            }
            else if (p_node == g_node->right.get()) {
                p = &(g_node->right);
                u = &(g_node->left);
            }
        }

        void checkColors(RBTree<Key>* current) {
            
            if ( (*current)->color == RBColor::red && (*current)->parent->color == RBColor::red ) {                

                // Nodes: current, parent, grandparent, uncle
                RBNode<Key>* c = current->get();
                RBTree<Key>* p = nullptr;
                RBTree<Key>* g = nullptr;
                RBTree<Key>* u = nullptr;

                // Get nodes
                this->getFamily(c, p, g, u);
                cout << "Current node: " << c->key << " (" << (c->color == RBColor::red ? "red" : "black") << ")" << endl;
                cout << "p node: " << p->get()->key << " (" << (p->get()->color == RBColor::red ? "red" : "black") << ")" << endl;
                cout << "g node: " << g->get()->key << " (" << (g->get()->color == RBColor::red ? "red" : "black") << ")" << endl;
                cout << "u node: " << u->get()->key << " (" << (u->get()->color == RBColor::red ? "red" : "black") << ")" << endl;

                // First scenario
                if (u->get()->color == RBColor::red) {
                    u->get()->color = RBColor::black;
                    p->get()->color = RBColor::black;
                    g->get()->color = RBColor::red;
                }
                // Second scenario
                else if (u->get()->color == RBColor::black) {

                    // p is linkerchild van g
                    if (g->get()->left.get()->key == p->get()->key) {
                        
                        // c is right child van p
                        if (p->get()->right && p->get()->right.get()->key == c->key) {
                            p->rotate(true);
                        }

                        p->get()->color = RBColor::black;
                        g->get()->color = RBColor::red;
                        g->rotate(false);

                    }
                    // p is right child van g
                    else if (g->get()->right.get()->key == p->get()->key) {
                        
                        // c is left child van p
                        if (p->get()->left && p->get()->left.get()->key == c->key) {
                            p->rotate(false);
                        }

                        p->get()->color = RBColor::black;
                        g->get()->color = RBColor::red;
                        g->rotate(true);

                    }
                }

                // Repeat bottom-p
                if (g != this) {    
                    this->checkColors(g);
                }
            }
        };

        void draw(const char* filename) {

            ofstream out(filename);
            assert(out);

            int counter = 0;

            out << "digraph {\n";
            this->drawRecursive(out, counter);
            out << "}";
        };

        string drawRecursive(ostream& out, int& counter) {

            ostringstream content;

            if (!*this){
                content << "null" << ++counter;
                out << content.str() << " [shape=point];\n";
            }
            else {
                content << '"' << this->get()->key << '"';
                if (this->get()->color == RBColor::black) {
                    out << content.str() << "[fillcolor=black style=filled fontcolor=white label=\"" << this->get()->key << "\"]";
                }
                else if (this->get()->color == RBColor::red) {
                    out << content.str() << "[fillcolor=red style=filled label=\"" << this->get()->key << "\"]";
                }
                out << ";\n";

                string left_child = this->get()->left.drawRecursive(out, counter);
                string right_child = this->get()->right.drawRecursive(out, counter);

                out << content.str() << " -> " << left_child << ";\n";
                out << content.str() << " -> " << right_child << ";\n";
            };

            return content.str();
        };
};

template <class Key>
class RBNode {
    friend class RBTree<Key>;

    public:

        // Constructors
        RBNode() : parent(0) { };
        RBNode(const Key& sl) : key{sl}, parent(0), color(red) { };
        RBNode(const Key& sl, RBColor k) : key(sl), parent(0), color(k) { };
        RBNode(const Key& sl, RBColor k, RBNode<Key>* o) : key(sl), parent(o), color(k) { };
        RBNode(Key&& sl) : key{move(sl)}, parent(0), color(red) { };

        // Fields
        Key key;
        RBNode<Key>* parent;
        RBTree<Key> left, right;
        RBColor color;
};

#endif
