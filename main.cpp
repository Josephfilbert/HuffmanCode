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

        //create new node for nodes priority queue
        Node(char c, int w): character(c), weight(w), left(nullptr), right(nullptr) {}

        //create new node as parent of two front nodes in queue
        Node(Node* leftChild, Node* rightChild): character(-1), weight(leftChild->weight + rightChild->weight), left(leftChild), right(rightChild) {}

        ~Node() {
            //recursive deletion
            if(left!=nullptr) delete left;
            if(right!=nullptr) delete right;
        }
    };

    //for custom comparator class
    struct Comparator {
        bool operator()(const Node* left, const Node* right) {
            return left->weight < right->weight;
        }
    };

    std::string payload; //the input string
    std::map<char, int /*, std::less<int>*/> charmap; //character frequency map
    std::priority_queue<Node*, std::vector<Node*>, Comparator> nodes; //nodes priority queue for processing

    typedef std::vector<bool> code_t; //code data type to represent bits

    std::map<char, code_t> code_map; //character code map

    Node *root; //root node of generated root after combined

    void generate_map() {
        for(std::string::const_iterator c = payload.begin(); c != payload.end(); c++) {
            /*if(!charmap.find(*c))
                charmap.insert(std::pair<char, int>(*c, 1));*/

            //tru to insert naively, and check its return value

            //this version of insert will return std::pair< std::map<char, int>::iterator, bool>
            //the first value is iterator to newly inserted data
            //or the position where the interference comes
            //the second value determines whether the insertion successful or not
            //if false, it means already exists

            std::pair< std::map<char, int>::iterator, bool> insert_status = charmap.insert(std::pair<char, int>(*c, 1));

            if(insert_status.second == false) {
                //if already exists, increase its number
                insert_status.first->second += 1;
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
        generate_map(); //create character frequency map
        //generate_nodes_queue(); //push all from map to priority queue
        //generate_tree(); //generate tree
    }

    ~HuffmanCode() {
        if(root!=nullptr) delete root;
    }

    //prints all character frequency
    std::string printCharMap() {

        std::ostringstream oss;

        for(std::map<char, int>::const_iterator iter = charmap.begin(); iter != charmap.end(); iter++) {
            oss << '(' << iter->first << "): " << iter->second << std::endl;
        }

        return oss.str();
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
    //std::cout << "Hello world!" << std::endl;

    HuffmanCode test(std::string("Hello world!"));
    std::cout << test.printCharMap() << std::endl;

    return 0;
}
