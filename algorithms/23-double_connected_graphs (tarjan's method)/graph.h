#include <vector>
#include <set>
#include <string>
#include <cassert>
#include <fstream>
#include <algorithm>

enum Direction { DIRECTED=1, UNDIRECTED=0 }; 

template<Direction direction>
class Graph {
    public:

        // Constructor
        Graph(int nodes = 0) : found(false), bridges(0) {
            this->connections.resize(nodes);

        };

        // Functions
        Direction getDirection() const {
            return direction;
        };

        int addNode() {
            int number_of_nodes = connections.size();
            this->connections.resize(number_of_nodes + 1);

            this->found = false;
            this->bridges.clear();
            this->bridges.shrink_to_fit();
        };          

        void addConnection(int from, int to) {
            if (!isValidNode(from) || !isValidNode(to)) {
                cout << "Unvalid nodes!\n";
                return;
            }

            if (direction == Direction::DIRECTED) {
                this->connections[from].insert(to);
            }
            else {
                this->connections[from].insert(to);
                this->connections[to].insert(from);
            }

            this->found = false;
            this->bridges.clear();
            this->bridges.shrink_to_fit();
        }; 

        void deleteConnection(int from, int to) {
            if (!isValidNode(from) || !isValidNode(to)) {
                cout << "Unvalid nodes!\n";
                return;
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

        // Graph should be connected!
        void isDoubleConnected() {
            if (!this->found) {
                depthFirstSearch();
                this->found = true;
            }

            for (int i = 0; i < this->bridges.size(); i+=2) {
                cout << "\tBridge found between node " << this->bridges[i] << " and node " << this->bridges[i+1]  << endl;
            }

            for (auto iter = this->articulation_points.begin(); iter != this->articulation_points.end(); iter++) {
                cout << "\tArticulation point found: node " << *iter  << endl;
            }
        }

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
                    out << '"' << node << "\" -> \"" << *iter << "\";" << endl;
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

        vector<int> depthFirstSearch(int startNode = 0) {
            int counter = 0;
            int nodes = countNodes();

            // for bridge/articulation point searching (Tarjan's algorithm)
            vector<int> visited(nodes, -1);
            vector<int> low_values(nodes);    
            
            // add for-loop if start graph is not connected
            visit(startNode, -1, visited, low_values, counter);

            return visited;
        };

        void visit(int node, int parent, vector<int>& visited, vector<int>& lows, int& counter) {
            set<int> neighbors = this->connections[node];
            
            visited[node] = counter;
            lows[node] = counter;

            counter++;

            for (auto iter = neighbors.begin(); iter != neighbors.end(); iter++) {
                if (*iter == parent) continue;

                // first visit
                if (visited[*iter] < 0) {
                    visit(*iter, node, visited, lows, counter);

                    // update lows
                    lows[node] = min(lows[node], lows[*iter]);

                    // bridge is found when parents discovery number is smaller than his childs low number
                    // articulation point is found when parents discovery number is equal or smaller than his childs low number (FAILS FOR ROOT)

                    // bridge found
                    if (visited[node] < lows[*iter]) {
                        this->bridges.push_back(node);
                        this->bridges.push_back(*iter);

                        // check for articulation point
                        // because we found a bridge, one of the nodes could be an articulation point
                        if (this->connections[node].size() > 1) {
                            this->articulation_points.insert(node);
                        }
                        if (this->connections[*iter].size() > 1) {
                            this->articulation_points.insert(*iter);
                        }
                    }

                    // articulation point found
                    // too solve the root issue, check for articulation points when a bridge is found
                    if (visited[node] <= lows[*iter] && node != 0) {
                        this->articulation_points.insert(node);
                    }
                }
                // already visited
                else {

                    // update lows
                    lows[node] = min(lows[node], visited[*iter]);
                }
            }
        };
        
        // Fields
        vector<set<int>> connections;

        bool found;
        vector<int> bridges;
        set<int> articulation_points;
};
