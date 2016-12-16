#include <iostream>
#include <queue>
#include <map>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <cmath>

class HuffmanCode {
    struct Node {
        char character; //represent character or no character with number -1
        int weight;
        Node* left;
        Node* right;

        //create new node for nodes priority queue
        Node(char c, int w): character(c), weight(w), left(nullptr), right(nullptr) {}

        //create new node as parent of two front nodes in queue
        Node(Node* leftChild, Node* rightChild):
            character(-1), weight(leftChild->weight + rightChild->weight), left(leftChild), right(rightChild) {}

        ~Node() {
            //recursive deletion
            if(left!=nullptr) delete left;
            if(right!=nullptr) delete right;
        }

        //for custom comparator class
        struct Comparator {
            bool operator()(const Node* left, const Node* right) {
                return left->weight > right->weight;
            }
        };
    };

    class bits_type {
        std::vector<bool> bits;

    public:
        std::size_t size() {
            return bits.size();
        }

        bits_type& append(std::string bit_text) {
            for(std::string::const_iterator iter = bit_text.begin(); iter != bit_text.end(); iter++) {
                bits.push_back(*iter=='1'? true : false);
            }
            return *this;
        }

        std::string getBits() {
            std::ostringstream oss;
            for(std::vector<bool>::const_iterator iter = bits.begin(); iter != bits.end(); iter++) {
                oss << (*iter == true? '1' : '0');
            }
            return oss.str();
        }
    };

    std::string payload; //the input string
    bits_type compressed; //the compressed

    std::string serialized_tree; //serialized tree, obtained from Breadth First Search

    std::map<char, int> charmap; //character frequency map
    std::priority_queue<Node*, std::vector<Node*>, Node::Comparator> nodes_queue; //nodes priority queue for processing


    std::map<char, std::string> code_map; //character code map
    std::map<char, std::string>::iterator code_map_iterator; //for use when generating encode map to improve performance

    Node *root; //root node of generated root after combined

    void generate_map() {
        std::map<char, int>::iterator map_iter;
        for(std::string::const_iterator c = payload.begin(); c != payload.end(); c++) {
            /*if(!charmap.find(*c))
                charmap.insert(std::pair<char, int>(*c, 1));*/

            //tru to insert naively, and check its return value

            //this version of insert will return std::pair< std::map<char, int>::iterator, bool>
            //the first value is iterator to newly inserted data
            //or the position where the interference comes
            //the second value determines whether the insertion successful or not
            //if false, it means already exists

            /*std::pair< std::map<char, int>::iterator, bool> insert_status = charmap.insert(std::pair<char, int>(*c, 1));

            if(insert_status.second == false) {
                //if already exists, increase its number
                insert_status.first->second += 1;
            }*/

            //with iterator
            //insert first element

            if(charmap.empty()) {

                //first insertion will always successful, it will return iterator to inserted data
                map_iter = charmap.insert(std::pair<char, int>(*c, 1)).first;

            } else {
                map_iter = charmap.insert(map_iter, std::pair<char, int>(*c, 0));

                //if fail, it will be increased, if not yet exists it will be 0 + 1
                map_iter->second += 1;
            }
        }
    }

    //called after map generated
    void generate_nodes_queue() {
        for(std::map<char, int>::const_iterator iter = charmap.begin(); iter != charmap.end(); iter++) {
            nodes_queue.push(new Node(iter->first, iter->second));
        }
    }

    void generate_tree() {
        //use reference for easy processing
        while (nodes_queue.size() > 1) {
            Node* firstNode = nodes_queue.top();

            //pop and get the second node
            nodes_queue.pop();
            Node* secondNode = nodes_queue.top();
            nodes_queue.pop();

            //new node based on total number of weight
            Node* newNode = new Node(firstNode, secondNode);

            //place it back to the priority queue
            nodes_queue.push(newNode);
        }

        //the root node is the only node in the queue
        root = nodes_queue.top();

        //dequeue the last node
        nodes_queue.pop();
    }

    void create_code_map(Node* rootnode, std::string code = "") {
        //using in order traversal, recursive

        //base condition: if the node is null, then done
        if(rootnode==nullptr) return;

        if(rootnode->left != nullptr) create_code_map(rootnode->left, code + '0');
        if(rootnode->right != nullptr) create_code_map(rootnode->right, code + '1');

        //if leaf node
        if(rootnode->left == nullptr && rootnode->right == nullptr) {
            if(code_map.empty()) {
                //this will return iterator to newly inserted data as hint for next insertion
                code_map_iterator = code_map.insert(std::pair<char, std::string>(rootnode->character, code)).first;
            }
            else { //with hint
                code_map_iterator = code_map.insert(code_map_iterator, std::pair<char, std::string>(rootnode->character, code));
            }
        }
    }

    void compress() {
        for(std::string::const_iterator iter = payload.begin(); iter!= payload.end(); iter++) {
            compressed.append(code_map.at(*iter));
        }
    }



    //use pre-order traversal
    void serialize_tree(Node* rootnode) {
        if(rootnode == nullptr) serialized_tree += "# ";
        else {
            //preorder write
            if(rootnode->character == -1)
                serialized_tree += "* ";
            else
                serialized_tree += std::string(1, rootnode->character) + ' ';

            serialize_tree(rootnode->left);
            serialize_tree(rootnode->right);
        }
    }

public:

    HuffmanCode(std::string input): payload(input), root(nullptr) {
        generate_map(); //create character frequency map
        generate_nodes_queue(); //push all from map to priority queue
        generate_tree(); //generate tree
        create_code_map(root); //create code map

        serialize_tree(root);
        serialized_tree.pop_back();
        compress();
    }

    ~HuffmanCode() {
        if(root!=nullptr) delete root;

        while(nodes_queue.size() > 0) {
            delete nodes_queue.top();
            nodes_queue.pop();
        }
    }

    //prints all character frequency
    std::string printCharFreq() {

        std::ostringstream oss;

        for(std::map<char, int>::const_iterator iter = charmap.begin(); iter != charmap.end(); iter++) {
            oss << '(' << iter->first << "): " << iter->second << std::endl;
        }

        return oss.str();
    }

    //FOR DEBUGGING PURPOSE ONLY
    //Prints all Priority Queue contents after initial queue creation
    //This will delete all created nodes in queue
    std::string printQueue() {
        std::ostringstream oss;

        while(nodes_queue.size() > 0) {
            oss << '(' << nodes_queue.top()->character << "): " << nodes_queue.top()->weight << std::endl;

            //we should deallocate it immediately, because the only pointer to the object will lost after this
            delete nodes_queue.top();

            nodes_queue.pop();
        }

        return oss.str();
    }

    std::string print_code_map() {

        std::ostringstream oss;

        for(std::map<char, std::string>::const_iterator iter = code_map.begin(); iter != code_map.end(); iter++) {
            oss << '(' << iter->first << "): " << iter->second << std::endl;
        }

        return oss.str();
    }

    std::string getSerializedTree() {
        return serialized_tree;
    }

    int getCompressionRatio() {
        return std::ceil((payload.size() * 8 - compressed.size()) / static_cast<float>(payload.size() * 8) * 100);
    }

    std::string getCompressedString() {
        return compressed.getBits();
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

    HuffmanCode test(std::string("Hello world"));
    std::cout << test.printCharFreq() << std::endl;
    std::cout << test.print_code_map() << std::endl;
    //std::cout << test.printQueue() << std::endl;

    std::cout << "Serialized tree : " << test.getSerializedTree() << std::endl;
    std::cout << "Compressed : " << test.getCompressedString() << std::endl;
    std::cout << "Compression ratio : " << test.getCompressionRatio() << std::endl;

    return 0;
}
