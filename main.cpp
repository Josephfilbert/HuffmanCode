#include <iostream>
#include <queue>
#include <map>
#include <fstream>
#include <sstream>

class HuffmanCode {
    struct Node {
        char character; //represent character or no character with number -1
        int weight;
        Node* left;
        Node* right;

        Node(char c, int w): character(c), weight(w), left(nullptr), right(nullptr) {}
        Node(Node* leftChild, Node* rightChild): character(-1), weight(leftChild->weight + rightChild->weight), left(leftChild), right(rightChild) {}

        ~Node() {
            if(left!=nullptr) delete left;
            if(right!=nullptr) delete right;
        }
    };

    struct compare {
        bool operator()(Node* left, Node* right) {
            return left->weight < right->weight;
        }
    };

    Node* root;

    std::string payload; //the input string
    std::map<char, int /*, std::less<int>*/> charmap; //character frequency map
    std::priority_queue<Node*, std::vector<Node*>, compare> nodes; //nodes priority queue for processing

    void generate_map() {
        for(std::string::const_iterator c = payload.begin(); c != payload.end(); c++) {
            /*if(!charmap.find(*c))
                charmap.insert(std::pair<char, int>(*c, 1));*/

            //tru to insert naively, and check its return value

            //this version of insert will return std::pair< std::map<char, int>::iterator, bool>
            //the second value determines whether the insertion successful or not
            //if false, it means already exists
            if((charmap.insert(std::pair<char, int>(*c, 1))).second == false) {
                //if already exists, increase its number
                charmap[*c]++;
            }
        }
    }

    //called after map generated
    void generate_nodes_queue() {
        for(std::map<char, int>::const_iterator iter = charmap.begin(); iter != charmap.end(); iter++) {
            nodes.push(new Node(iter->first, iter->second));
        }
    }

    void generate_tree(std::string input) {
        //use reference for easy processing
        while (nodes.size() > 1) {
            Node* firstNode = nodes.top();

            //pop and get the second node
            nodes.pop();
            Node* secondNode = nodes.top();
            nodes.pop();

            //new node based on total number of weight
            Node* newNode = new Node(firstNode, secondNode);

            //place it back to the priority queue
            nodes.push(newNode);
        }

    }

public:
    HuffmanCode(std::string input): payload(input) {
        generate_map();
    }

    std::string printCharMap() {

    }

};

//file structure
/*
num_bits: int
data_start_pos: int
tree_structure
data_payload
*/

int main()
{
    std::cout << "Hello world!" << std::endl;


    return 0;
}
