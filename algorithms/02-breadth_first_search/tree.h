#ifndef TREE_H
#define TREE_H

#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <queue>

using namespace std;

#include "node.h"

template <class Key>
class Tree : unique_ptr<Node<Key>> {
   public:
        using unique_ptr<Node<Key>>::unique_ptr;

        /* Constructor & Destructor  */
        Tree(){};
        Tree(unique_ptr<Node<Key>>&& k) : unique_ptr<Node<Key>>(move(k)) { };
        Tree(const vector<Key> &keys) {
            for (int i = 0; i < keys.size(); i++) {
                this->add(keys[i]);
            }
        };
        ~Tree() = default;

        /* Copy constructor & operator  */
        Tree(const Tree<Key> &other) = default;
        Tree &operator=(const Tree<Key> &other) = default;

        /* Move constructor & operator */
        Tree(Tree<Key> &&other) = default;
        Tree &operator=(Tree<Key> &&other) = default;

        /* Functions */
        void add(const Key &key) {

            Node<Key>* parent = nullptr;
            Tree<Key>* current = this;
            while (current->get() != nullptr) {
                parent = current->get();
                if (key < (*current).get()->key) {
                    current = &(current->get()->left);
                } 
                else {
                    current = &(current->get()->right);
                }
            }

            Tree<Key> add = make_unique<Node<Key>>(key, parent);
            (*current) = move(add);
        };

        void breadth_first_search() {
            queue<Tree<Key>*> q;
            q.push(this);

            while (!q.empty()) {
                Tree<Key>* current = q.front();
                q.pop();

                cout << current->get()->key << " ";
                if (current->get()->left) q.push( &(current->get()->left) );
                if (current->get()->right) q.push( &(current->get()->right) );
            }
        }

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
                out << content.str() << "[label=\"" << this->get()->key << "\"]";
                out << ";\n";

                string left_child = this->get()->left.drawRecursive(out, counter);
                string right_child = this->get()->right.drawRecursive(out, counter);

                out << content.str() << " -> " << left_child << ";\n";
                out << content.str() << " -> " << right_child << ";\n";
            };

            return content.str();
        };
};

#endif
