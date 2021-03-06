#include <vector>
#include <set>
#include <string>
#include <map>
#include <cassert>
#include <fstream>
#include <algorithm>
#include <iostream>

 enum Direction { DIRECTED=1, UNDIRECTED=0 }; 

 template<Direction direction, class T>
class Weighted_Graph {
    public:

         // Constructor
        Weighted_Graph(int nodes = 0) : connections(nodes) { };

         // Functions
        Direction getDirection() const {
            return direction;
        };

        int addNode() {
            int number_of_nodes = connections.size();
            this->connections.resize(number_of_nodes + 1);
        };          

        void addConnection(int from, int to, T weight) {
            if (!isValidNode(from) || !isValidNode(to)) {
                cout << "Unvalid nodes! \n";
            }

             if (direction == Direction::DIRECTED) {
                this->connections[from].insert(make_pair(to, weight));
            }
            else {
                this->connections[from].insert(make_pair(to, weight));
                this->connections[to].insert(make_pair(from, weight));
            }
        }; 

        void deleteConnection(int from, int to) {
            if (!isValidNode(from) || !isValidNode(to)) {
                throw "Unvalid nodes!";
            }

             if (direction == Direction::DIRECTED) {
                this->connections[from].erase(to);
            }
            else {
                this->connections[from].erase(to);
                this->connections[to].erase(from);
            }
        };

        int countNodes() const {
            return this->connections.size();
        };

        int countConnections() const {
            int connections = 0;
            for (int i = 0; i < this->connections.size(); i++) {
                connections += this->connections[i].size();
            }

             return connections;
        };

        vector<int> bellman_ford_algorithm(int node = 0) {

            int nodes = countNodes();
            vector<bool> init(nodes, false);
            vector<int> shortest_paths(nodes, 0);
            
            init[node] = true;

            bool changes = true;
            int maxIterations = nodes - 1;
            while (changes && maxIterations != 0) {
                changes = bellman_ford(node, shortest_paths, init);
                maxIterations--;
            }

            return shortest_paths;
        };

         void draw(string filename) const {
            ofstream out(filename);
            assert(out);

             out << "digraph {\n";

             int nodes = countNodes();
            while (--nodes != -1) {
                out << '"' << nodes << "\"[label=\"" << nodes << "\"];" << endl;
            }

             for (int node = 0; node < countNodes(); node++) {

                 for (auto iter = this->connections[node].begin(); iter != this->connections[node].end(); iter++) {
                    out << '"' << node << "\" -> \"" << iter->first << "\";" << endl;
                }
            }

              out << "}";
        };

     private:

        // Functions for verification
        bool isValidNode(int number) const {
            if (number >= 0 && number < this->connections.size()) {
                return true;
            }

             return false;
        };

        bool bellman_ford(int node, vector<int>& solution, vector<bool>& init) {
            bool changes = false;
            int nodes = countNodes();

            // visit all nodes
            int i = node;
            do {

                 // check all connections with their neighbors
                for (auto iter = this->connections[i].begin(); iter != this->connections[i].end(); iter++) {           
                    int startNode = i;
                    int endNode = iter->first;
                    int weight = iter->second;

                    if (!init[startNode]) {
                        continue;
                    }

                     // first time?
                    if (!init[endNode]) {
                        solution[endNode] = solution[startNode] + weight;
                        init[endNode] = true;
                        changes = true;
                        continue;
                    }

                     // if (begin_node's score + connection's weight < end_node's score) => change score
                    if (solution[startNode] + weight < solution[endNode]) {
                        solution[endNode] = solution[startNode] + weight;
                        changes = true;
                    }
                }

                i = (i+1) % nodes;
            } while (i != node);

            return changes;
        };

         void visit() {

         };

         // Fields
        vector<map<int, T>> connections;
};