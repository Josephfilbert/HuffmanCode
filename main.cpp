#include <iostream>
#include <queue>
#include <map>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <cmath>
#include <algorithm>
#include <bitset>
#include <limits>
#include <iomanip>
#include <cstring>

#define PARENT_NODE '\1'

void clear_cin() {
    //if(std::cin.eof()) return;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    //std::cin.clear();
}

template<typename K, typename V>
void insertMap(std::map<K, V>& mapRef, typename std::map<K, V>::iterator& hint, K key, V value) {
    if(mapRef.empty()) {
        hint = mapRef.insert(std::pair<K, V>(key, value)).first;
    } else {
        hint = mapRef.insert(hint, std::pair<K, V>(key, value));
    }
}

void invert_endian_int(unsigned int& num) {
    num = (num << 24) | ((num << 8) & 0x00ff0000) | ((num >> 8) & 0x0000ff00) | (num >> 24);
}



class HuffmanCode {
    struct Node {
        char character; //represent character or no character with number -1
        int weight;
        Node* left;
        Node* right;

        //creates Node without weight for decompression
        Node(char c): character(c), weight(0), left(nullptr), right(nullptr) {}

        //create new node for nodes priority queue
        Node(char c, int w): character(c), weight(w), left(nullptr), right(nullptr) {}

        //create new node as parent of two front nodes in queue
        Node(Node* leftChild, Node* rightChild):
            character(-1), weight(leftChild->weight + rightChild->weight),
            left(leftChild), right(rightChild) {}

        ~Node() {
            //recursive deletion
            if(left!=nullptr) delete left;
            if(right!=nullptr) delete right;
        }

        bool isLeaf() {
            return (left==nullptr && right==nullptr);
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
        bool modified;

        std::string bits_str;
        std::string bits_encoded;

    public:

        static bool is_bits(std::string str) {
            for(std::string::const_iterator iter = str.begin(); iter != str.end(); iter++) {
                if(*iter!='1' && *iter!='0')
                    return false;
            }
            return true;
        }

        bits_type(): modified(true) {}

        bits_type(std::string s): modified(true) {
            append_str(s);
        }

        std::size_t size() {
            return bits.size();
        }

        bits_type& reserve(std::size_t s) {
            bits.reserve(s);
            return *this;
        }

        bits_type& append(std::string bit_text) {
            if(is_bits(bit_text)) {
                modified = true;
                for(std::string::const_iterator iter = bit_text.begin(); iter != bit_text.end(); iter++) {
                    bits.push_back(*iter=='1'? true : false);
                }
            }
            return *this;
        }

        bits_type& append_str(std::string text) {
            modified = true;
            for(std::string::const_iterator iter = text.begin(); iter!= text.end(); iter++) {
                for(int i=0; i<8; i++) {
                    bits.push_back(static_cast<bool>((*iter >> (7 - i)) & 1));
                }
            }
            return *this;
        }

        bits_type& append_str(std::string text, int nBits) {
            modified = true;
            int i = 0;
            int j = 8;
            for(std::string::const_iterator iter = text.begin(); i < nBits && iter!= text.end(); iter++) {
                for(; i < j && i < nBits; i++) {
                    bits.push_back(static_cast<bool>((*iter >> (7 - (i % 8))) & 1));
                }
                j += 8;
            }
            return *this;
        }

        void clear() {
            bits.clear();
            modified = true;
            bits_str.clear();
            bits_encoded.clear();
        }

        bool isEmpty() {
            return bits.empty();
        }

        bits_type& append(const bits_type& rhs) {
            bits.insert(bits.end(), rhs.bits.begin(), rhs.bits.end());
            modified = true;
            return *this;
        }

        bits_type& append(int rhs) {
            return append(std::bitset<32>(rhs).to_string());
        }

        std::string getBits_str() {
            if(modified) {
                std::ostringstream oss;
                for(std::vector<bool>::const_iterator iter = bits.begin(); iter != bits.end(); iter++) {
                    oss << (*iter == true? '1' : '0');
                }

                bits_str = oss.str();
                modified = false;
            }
            return bits_str;
        }

        std::string getEncodedBits() {

            if(modified) {
                std::ostringstream oss;
                int length = 0;
                unsigned char process = 0;

                for(std::vector<bool>::const_iterator iter = bits.begin(); iter != bits.end(); iter++) {
                    //set the bits
                    if(*iter)
                        process |= 1 << (7 - (length % 8));
                        //process |= 1 << (length % 8);

                    //check length, if reach a byte, process new
                    if(++length % 8 == 0) {
                        oss.put(process);
                        process = 0;
                    }
                }

                //if there is still left, flush it
                if(length % 8 > 0) {
                    oss.put(process);
                }

                bits_encoded = oss.str();
                modified = false;
            }
            return bits_encoded;
        }


        bits_type& setBool(std::size_t index, bool value) {
            bits.at(index) = value;
            modified = true;
            return *this;
        }

        bool getBool(std::size_t index) {
            return bits.at(index);
        }

    };

    class FileHandler {

        //file structure
        /*
        num_bits: int
        tree_length: int (number of bytes)
        tree_structure
        data_payload
        */

        bits_type tree;
        bits_type payload;

        std::string filename;

        bool output_mode;
        bits_type output_payload;

    public:

        //first constructor is for output to file.
        FileHandler(std::string fname, bits_type serial_tree, bits_type payload_bits): output_mode(true), filename(fname), tree(serial_tree), payload(payload_bits) {


            //first, set the number of bits
            output_payload.append(std::bitset<32>(payload_bits.size()).to_string())

                //then add the tree length
                //because it's size is always divisible by 8
                .append(static_cast<int>(tree.size()) / 8)

                //insert the tree structure
                .append(tree)

                //the payload
                .append(payload_bits);



        }

        //For input
        FileHandler(std::string fname): filename(fname), output_mode(false) {

        }

        void write() {

            if(!output_mode) return;

            //write to file
            std::ofstream outfile(filename, std::ios::out | std::ios::trunc | std::ios::binary);
            std::istringstream iss(output_payload.getEncodedBits());

            std::copy(std::istreambuf_iterator<char>(iss), std::istreambuf_iterator<char>(), std::ostreambuf_iterator<char>(outfile));

            outfile.close();
        }

        bool read(std::string& serial_tree, bits_type& compressed_bits) {
            if(output_mode) return false;

            //write to file_content and close stream
            std::ifstream infile(filename, std::ios::binary);
            std::stringstream file_content;
            std::copy(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>(), std::ostreambuf_iterator<char>(file_content));
            infile.close();

            //get the number of bits
            unsigned int num_bits;
            file_content.read(reinterpret_cast<char*>(&num_bits), 4);
            invert_endian_int(num_bits);

            //number of tree length in bytes
            unsigned int tree_length;
            file_content.read(reinterpret_cast<char*>(&tree_length), 4);
            invert_endian_int(tree_length);

            //read the tree
            char *tree_content = new char[tree_length + 1];
            file_content.read(tree_content, tree_length);
            tree_content[tree_length] = 0;

            //read the rest which is the payload
            std::string payload_content;
            payload_content.insert(payload_content.begin(), std::istreambuf_iterator<char>(file_content), std::istreambuf_iterator<char>());

            //insert it to compressed_bits
            compressed_bits.reserve(payload_content.size());
            compressed_bits.append_str(payload_content, num_bits);

            //write the serialized tree
            serial_tree.assign(tree_content);
            delete[] tree_content;

            if(compressed_bits.size() != num_bits) return false;
            return true;

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
        while (nodes_queue.size() > 1) {
            //get first node
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
        if(rootnode->isLeaf()) {
            /*if(code_map.empty()) {
                //this will return iterator to newly inserted data as hint for next insertion
                code_map_iterator = code_map.insert(std::pair<char, std::string>(rootnode->character, code)).first;
            }
            else { //with hint
                code_map_iterator = code_map.insert(code_map_iterator, std::pair<char, std::string>(rootnode->character, code));
            }*/
            insertMap(code_map, code_map_iterator, rootnode->character, code);
        }
    }

    void compress() {

        for(std::string::const_iterator iter = payload.begin(); iter!= payload.end(); iter++) {
            compressed.append(code_map.at(*iter));
        }
    }

    void decompress() {

        //if tree is empty or the only node, do nothing
        if(root==nullptr || root->isLeaf()) return;

        //decompress directly from tree
        Node* ptr = root;

        bool isNextNull = false;

        payload.clear();
        std::cout << "Compressed.size = " << compressed.size() << std::endl;

        for(int i=0; i<compressed.size(); i++) {
            if(compressed.getBool(i)==false) {
                if(ptr->left != nullptr)
                    ptr = ptr->left;
                else
                    isNextNull = true;
            } else {
                if(ptr->right != nullptr)
                    ptr = ptr->right;
                else
                    isNextNull = true;
            }

            if(isNextNull) {
                //we reached the leaf, decrypt it!
                payload += ptr->character;

                //reset variables
                ptr = root;
                isNextNull = false;
                i--;
            }




            /*if(ptr->isLeaf()) {
                payload += ptr->character;
                ptr = root;
            }

            if(compressed[i])
                ptr = ptr->right;
            else
                ptr = ptr->left;*/



        }

        //TODO: Revise the algorithm later
        //extract the last character
        if(ptr->isLeaf()) {
            payload.append(1, ptr->character);
        }

    }

    //use pre-order traversal
    //this will be used to output to file
    void serialize_tree(Node* rootnode) {
        // \1 used for parent

        if(rootnode == nullptr) return;

        //preorder write
        if(rootnode->character == -1)
            serialized_tree += PARENT_NODE;
        else
            serialized_tree += rootnode->character;

        serialize_tree(rootnode->left);
        serialize_tree(rootnode->right);

    }




    void deserialize_tree(Node*& rootPtr, std::istream& is) {

        if(!is.good()) return;

        //read token
        char c;
        is.read(&c, 1);

        if(c == PARENT_NODE) {
            rootPtr = new Node(static_cast<char>(-1));
            deserialize_tree(rootPtr->left, is);
            deserialize_tree(rootPtr->right, is);
        } else {
            rootPtr = new Node(c);
        }
    }

public:

    void do_compress_routine() {
        generate_map(); //create character frequency map
        generate_nodes_queue(); //push all from map to priority queue
        generate_tree(); //generate tree
        create_code_map(root); //create code map

        serialize_tree(root);
        compress();
    }

    HuffmanCode(std::string input): payload(input), root(nullptr), serialized_tree("") {
        do_compress_routine();
    }

    HuffmanCode() {

    }

    bool readfile(std::string fname) {
        FileHandler fh(fname);

        bool read_status = fh.read(serialized_tree, compressed);

        std::istringstream stStream(serialized_tree);
        deserialize_tree(root, stStream);

        //reserialize tree to test
        //serialized_tree.clear();
        //serialize_tree(root);

        decompress();

        //std::cout << "Payload addres : " << std::hex << reinterpret_cast<void*>(&payload) << std::endl;
        //std::system("pause");
        return read_status;
    }

    //output to file
    void writefile(std::string fname) {
        FileHandler(fname, bits_type(serialized_tree), compressed).write();
    }

    std::string getPayload() {
        return payload;
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

    //FOR TESTING PURPOSE ONLY
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

    std::string getSerializedTree_readable(char nullChar = '#', char parentChar = '*') {
        std::string result;
        result.reserve(serialized_tree.size());
        for(std::string::const_iterator c = serialized_tree.begin(); c != serialized_tree.end(); c++) {
            switch(*c) {
                case PARENT_NODE: result += parentChar; break;
                default: result += *c;
            }
        }
        return result;
    }

    int getCompressionRatio() {
        return std::ceil((payload.size() * 8 - compressed.size()) / static_cast<float>(payload.size() * 8) * 100);
    }

    std::string getCompressedString() {
        return compressed.getBits_str();
    }
};




int main(int argc, char **argv)
{
    //std::cout << "Hello world!" << std::endl;

    /*HuffmanCode test(std::string("Huffman Code Implementation in C++11 with STL"));
    std::cout << test.printCharFreq() << std::endl;
    std::cout << test.print_code_map() << std::endl;
    //std::cout << test.printQueue() << std::endl;



    std::cout << "Compressed : " << test.getCompressedString() << std::endl;
    std::cout << "Compression ratio : " << test.getCompressionRatio() << '%' << std::endl;

    test.writefile("keytest2.compressed");*/

    HuffmanCode* hfCode = nullptr; //reusable object

    int mainmenu;

    do {
        mainmenu = -1;

        std::cout << "\nHuffman Coding Compression\n";
        std::cout << "1. Compress a text\n"
                  << "2. Compress a file\n"
                  << "3. Decompress a file\n"
                  << "0. Exit\n";

        std::cout << "Choose : ";
        std::cin >> mainmenu;clear_cin();

        if(mainmenu == 1) {
            std::string text;
            std::cout << "Enter text and press enter :" << std::endl;

            std::getline(std::cin, text);
            clear_cin();

            //check if eligible to compress
            if(text.empty()) {
                std::cout << "Invalid input, try again!" << std::endl;
                continue;
            }

            std::string filename;
            do {
                std::cout << "Enter filename : ";
                std::getline(std::cin, filename);
                clear_cin();
            } while(filename.empty());

            //write the file
            hfCode = new HuffmanCode(text);
            hfCode->writefile(filename);

            std::cout << "Text written to " << filename << ".\n";
            std::cout << "Text size reduced by : " << hfCode->getCompressionRatio() << "%\n";

            delete hfCode;
        }

        else if(mainmenu == 2) {
            std::cout << "Will be implemented later. Thanks for the patience." << std::endl;
        }

        else if(mainmenu == 3) {
            std::cout << "Enter file name relative to this directory : ";
            std::string filename;
            std::getline(std::cin, filename);clear_cin();

            std::ifstream infile(filename);

            //check file exists
            if(!infile.good()) {
                std::cout << "File not found!" << std::endl;
                continue;
            }

            hfCode = new HuffmanCode;
            bool read_status = hfCode->readfile(filename);

            if(!read_status) std::cout << "Warning! There's a problem reading the file, the content may be truncated" << std::endl;

            std::cout << "The compression ratio was: " << hfCode->getCompressionRatio() << "%\n";
            std::cout << "File content for " << filename << ':' << std::endl;
            std::cout << hfCode->getPayload() << std::endl;

            delete hfCode;
        }


    } while(mainmenu!=0);

    /*HuffmanCode test2;
    test2.readfile("binarytest.txt.compressed");
    std::cout << "Serialized tree : " << test2.getSerializedTree_readable() << std::endl;

    //rserialize

    std::cout << "File content :\n" << test2.getPayload() << std::endl;*/



    return 0;
}
