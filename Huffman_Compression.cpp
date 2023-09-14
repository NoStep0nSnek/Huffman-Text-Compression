#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>

#define bitset(byte,nbit)   ((byte) |=  (1<<(nbit)))
#define bitclear(byte,nbit) ((byte) &= ~(1<<(nbit)))
#define bitflip(byte,nbit)  ((byte) ^=  (1<<(nbit)))
#define bitcheck(byte,nbit) ((byte) &   (1<<(nbit))) // returns bit value (i.e: 1,2,4,8,16,32,64)

// code read offset (starts at 1)
#define OFFSET 1
// CHAR OFFSET (+25)
#define OFFSET2 25

using namespace std;

/*class compressed_txt_file() {

)*/

struct node {
    
    char data;
    
    int index;

    unsigned freq;

    struct node *left, *right;

};

struct char_record {

    int times_used;

    char data;
};

// clamps an int between upper bound and lower bound
int clamp (int num, int LB = -9999999, int UB = 9999999) {
    if (num < LB) {
        return LB;
    }
    if (num > UB) {
        return UB;
    }
    return num;
}

// takes a string of bits and converts it to bytes
vector<char> bits_to_bytes(string bits) {
    vector<char> bytes;
    char byte = 0;

    for (int i = 0; i < ceil(float(bits.size()) / 8); i++) {
        for (int j = 0; j < 8; j++) {
            if (bits[j + (i * 8)] == '1') {
                bitset(byte, j);
            }
        }
        bytes.push_back(byte);
        byte = 0;
    }

    return bytes;
}

// takes bytes and puts its bits in a string
string bytes_to_bits(vector<char> bytes) {
    string bits;

    for (int i = 0; i < bytes.size(); i++) {
        for (int j = 0; j < 8; j++) {
            if (bitcheck(bytes[i], j)) {
                bits.push_back('1');
            } else {
                bits.push_back('0');
            }
        }
    }

    return bits;
}

// void insert_sorted(vector<node> &tree, node* temp_node){
//     vector<node> copy_tree;
//     int highest = 0;
//     for (auto z : tree) {
//         if (temp_node->freq < z.freq) {
//             highest = z.freq;
//         }
//     }
//     for (auto z : tree) {
//         if (z.freq == highest) {
//             copy_tree.push_back(*temp_node);
//         }
//         copy_tree.push_back(z);
//     }
//     tree = copy_tree;
// }

// writes huffman bits to file
// codes go first, seperated by a space and from the main text with a newline

// codes (D:e, F;10, C:1, 0, 1) & (D: , F;10, C:1, 1, 0) are missing from the read tree - caused when the file is being written - see the compressed file?
string decompress_huffman(string filename) {

    std::ifstream in_file (filename, ios::in | ios::binary);
    cout << "\n\nNAME:" << filename;
    cout << "\nOPEN:" << in_file.is_open();
    string text;
    vector<string> codes; // code[0] == letter, code[1] == frequency
    vector<int> freqs;
    vector<char> chars;
    // gets huffman codes from text
    int max_iterations = 1000;
    int iterations = 0;
    string str_text_length;
    int text_length;
    while (in_file.is_open()) {
        char temp_char;
        in_file.read(&temp_char, 1);
        if (temp_char == '\n') {
            break;
        }
        str_text_length.push_back(temp_char);
    }
    cout << "\nLEN: " << str_text_length << "\n";
    text_length = stoi(str_text_length) - 1;

    // gets codes from file
    string code;
    int matches_a_row = 0; // prevents the code from thinking that code[0] is the seperator character between codes
    while (in_file.is_open()) {
        char temp_char;
        string end;
        in_file.read(&temp_char, 1);
        //in_file.read(&end[0], 4);
        if (temp_char == '\n') {
            break;
        }
        if (temp_char == ' ' && matches_a_row == 0) {
            matches_a_row++;
            // offets letter val of code
            code[0] -= OFFSET2;
            codes.push_back(code);
            code = "";
        } else {
            matches_a_row = 0;
            code.push_back(temp_char);
        }
    }

    cout << "\n\nREAD CODES: ";
    // prints read codes
    for (auto z : codes) {
        cout << "(";
        for (int i = 0; i < z.size(); i++) {
            if (i == 0) {
                cout << "D:" << char(z[i]) << ",";
            } else {
                cout << int(z[i]-48) << ",";
            }
        }
        cout << ") ";
    }


    // gets frequencies & chars from codes & stores them
    // for (auto z : codes) {
    //     chars.push_back(z[0]);
    //     freqs.push_back(z[1]);
    //     char_record record;
    //     record.data = z[0];
    //     record.times_used = 0;
    // }

    string text_buffer;
    int len = 0;
    while (in_file.is_open() && in_file) { // reads text bytes
        char temp_char;
        in_file.read(&temp_char, 1);
        text_buffer.push_back(temp_char);
        len++;
    }
    
    cout << "\nTEXT BUFFER: ";
    for (auto z : text_buffer) {
        cout << z << ",";
    }
    text_buffer.pop_back();
    vector<char> v;
    for (int i = 0; i < len-1; i++) {
        v.push_back(text_buffer[i]);
    }

    string bits = bytes_to_bits(v); // converts read bytes into bits

    cout << "\nBITS: " << bits << "\n";

    // records frequency of used chars
    // index is the index of the char in code
    // value is the freq
    
    vector<int> used_characters(codes.size());

    // scans through bits and compares it codes for match, adds one to chars used and
    // makes sure that it hasn't been used more than frequency times
    for (int i = 0; i < bits.size();) {
        if (text.length() > text_length) {
            break;
        }
        // scans through codes
        int max_matches = 0;
        char max_matches_char;
        for (int j = 0; j < codes.size(); j++) {
            string code = codes[j];
            int matches = 0;
            for (int k = OFFSET; k < code.size(); k++) {
                if (bits[i+k-OFFSET] == code[k]) {
                    matches++;
                } else {
                    break;
                }
            }

            if (matches >= max_matches) {
                max_matches = matches;
                max_matches_char = code[0];
            }
        }
        i += max_matches;
        text.push_back(max_matches_char);
    }
    in_file.close();

    return text;
}

// writes:
// text_size code code code
// huffman_bytes
// issue is with the writing of the codes
// The issue is that the frequency of E & D are 10, the ASCII value of '\n' is 10
// see: https://www.asciitable.com/
// solution: add 127 to freq and then sub 100 to freq upon reading it
void write_huffman(vector<string> codes, string text, string filename, string endFolder) {

    // I should first check to see if the file exists
    ofstream out_file (endFolder + filename, ios::binary);

    // writes size of text
    out_file << to_string(text.length());
    cout << "LEN:" << to_string(text.length());
    out_file << '\n';

    cout << "\n\n";

    // writes plain codes, with the character in front of them
    for (int i = 0; i < codes.size(); i++) {
        string copy = (codes[i]);
        copy[0] += OFFSET2;
        out_file << (&copy[0]);
        out_file << ' '; // SET TO A FOR FILE TESTING AS SPACE ISNT WORKING NOR IS THIS
    }
    out_file << endl;

    string bits;
    //cout << "\n";
    // searches through text & finds their respective huffman bits, then adds it to the bits string
    for (int i = 0; i < text.size(); i++) {
        for (int j = 0; j < codes.size(); j++) {
            if (codes[j][0] == text[i]) {
                bits.append(codes[j], OFFSET); // 1 is because bits starts at char 1 not char 0
            }
        }
    }

    vector<char> bytes = bits_to_bytes(bits);

    for (auto z : bytes) {
        out_file << z;
    }

    out_file.close();
}

// If it value of the right is greater write a '1', otherwise right a '0'.
vector<string> get_codes(vector<node> tree, vector<char> chars, vector<int> frequencies) {

    vector<string> codes;

    //find which node points to letter, then find which node points that one and repeat
    for (int i = 0; i < chars.size(); i++) {
        string code;
        code.push_back(chars[i]);
            

        node last_point;


        //int index = 0;
        // finds which node points to letter
        for (int j = 0; j < tree.size(); j++) {
            if (tree[j].data == chars[i]) {
                last_point = tree[j];
                //index = j;
                break;
            }
        }

        //cout << "\nLETTER FREQ: " << letter_node.freq;
        //cout << "\nLETTER DATA: " << letter_node.data;
        //int last_point_index = index;
        for (int j = 0; j < tree.size(); j++) {
            if (tree[j].right) {
                // cout << "\nR:" << tree[j].right->freq << "," << last_point.freq;
                // cout << "\nR:" << tree[j].right->data << "," << last_point.data;
                // cout << "\nR:" << tree[j].right->index << "," << last_point.index;
                //if (tree[j].right == last_point && tree[j].right == last_point) {
                if (tree[j].right->index == last_point.index) {
                    // cout << "\nRI";
                    code.insert(code.begin() + 1, '1');
                    last_point = tree[j];
                }
            }
            if (tree[j].left) {
                // cout << "\nL:" << tree[j].left->freq << "," << last_point.freq;
                // cout << "\nL:" << tree[j].left->data << "," << last_point.data;
                // cout << "\nL:" << tree[j].left->index << "," << last_point.index;
                //if (tree[j].left->freq == last_point->freq && tree[j].left->data == last_point->data) {
                if (tree[j].left->index == last_point.index) {
                    // cout << "\nLI";
                    code.insert(code.begin() + 1, '0');
                    last_point = tree[j];
                }
            }
        }
        codes.push_back(code);
    }

    return codes;
}

// inserts into sorted list
void insert_sorted(vector<node> &tree, node* temp_node) {
    vector<node> tree_copy;
    bool pushed_back = false;
    for (int i = 0; i < tree.size(); i++) {
        if (i == 0 && temp_node->freq < tree[i].freq && !pushed_back) { //  checks beginning
            tree_copy.push_back(*temp_node);
            pushed_back = true;
        } else if (temp_node->freq >= tree[i-1].freq && temp_node->freq <= tree[i].freq && !pushed_back) {
            tree_copy.push_back(*temp_node);
            pushed_back = true;
        } 
        tree_copy.push_back(tree[i]);
        if (i == tree.size() - 1 && temp_node->freq > tree[i].freq && !pushed_back) { // checks end
            tree_copy.push_back(*temp_node);
            pushed_back = true;
        }
    }
    tree = tree_copy;
}

void remove_element(vector<node> &tree, int index) {
    vector<node> tree_copy;
    for (int i = 0; i < tree.size(); i++) {
        if (i != index) {
            tree_copy.push_back(tree[i]);
        }
    }
    tree = tree_copy;
}

void huffman_compression(string text, vector<char> chars, vector<int> freqs, string filename, string endFolder) {

    vector<char> sorted_chars = chars;
    vector<int> sorted_freqs = freqs;

    // sorts frequencies
    for (int i = 0; i < sorted_freqs.size(); i++) {
        for (int j = 0; j < sorted_freqs.size(); j++) {
            if (sorted_freqs[i] < sorted_freqs[j]) {
                swap(sorted_freqs[i], sorted_freqs[j]);
                swap(sorted_chars[i], sorted_chars[j]);
            }
        }
    }

    cout << "\nSORTED FREQS: ";
    for (auto z : sorted_freqs) {
        cout << z << ",";
    }

    cout << "\nSORTED CHARS: ";
    for (auto z : sorted_chars) {
        cout << z << ",";
    }

    vector<node> heap_tree(sorted_freqs.size());

    // init heap tree
    int index = 0;
    for (int i = 0; i < sorted_freqs.size(); i++) {
        heap_tree[i].freq = sorted_freqs[i];
        heap_tree[i].data = sorted_chars[i];
        index++;
        heap_tree[i].index = index;
    }

    vector<node> heap_tree_copy = heap_tree;

    // once max frequency is reached, break

    int min_int = 0;
    int merge = heap_tree.size();
    int i = 0;
    vector<string> codes;

    while (true) {
        node* node_left = new node();
        node_left->freq = heap_tree_copy[i].freq;
        node_left->data = heap_tree_copy[i].data;
        node_left->index = heap_tree_copy[i].index;
        remove_element(heap_tree_copy, i);
        //i++;

        node* node_right = new node();
        node_right->freq = heap_tree_copy[i].freq;
        node_right->data = heap_tree_copy[i].data;
        node_right->index = heap_tree_copy[i].index;
        remove_element(heap_tree_copy, i);
        //i++;

        node* sum_node = new node();
        index++;
        sum_node->index = index;
        sum_node->freq = node_right->freq + node_left->freq;
        sum_node->data = 0;
        sum_node->left = node_left;
        sum_node->right = node_right;
        insert_sorted(heap_tree_copy, sum_node);
        insert_sorted(heap_tree, sum_node);
        //insert(heap_tree, i, *sum_node);
        cout << "\nLEFT: " << sum_node->left->data << "," << sum_node->left->freq;
        cout << "\nRIGHT: " << sum_node->right->data << "," << sum_node->right->freq;
        cout << "\nSUM: " << sum_node->data << "," << int(sum_node->freq);
        //cout << "\nI: " << i;
        cout << "\n";
        for (auto z : heap_tree_copy) {
            cout << z.freq << "F,";
            //cout << z.data << "D,";
        }
        cout << "\n";
        for (auto z : heap_tree_copy) {
            cout << z.data << "D,";
        }

        merge--;
        if (heap_tree_copy.size() == 0) {
            break;
        }
    }

    codes = get_codes(heap_tree, sorted_chars, sorted_freqs);
    cout << "\n#Codes: " << codes.size();
    cout << "\nCodes: ";

    for (auto z : codes) {
        cout << "(";
        for (int i = 0; i < z.size(); i++) {
            if (i == 0) {
                cout << "D:";
                cout << char(z[i]) << ',';
            } else if (i == 1) {
                cout << "F:" << int(z[i]) << ", C:";
            } else {
                cout << int(z[i]-48) << ',';
            }
        }
        cout << ") ";
    }

    write_huffman(codes, text, filename, endFolder);
}

vector<int> get_frequencies(string text, vector<char> &arr) {
    vector<char> chars;
    for (int i = 0; i < text.size(); i++) {
        bool found = false;
        for (int j = 0; j < chars.size(); j++) {
            if (text[i] == chars[j]) {
                found = true;
            }
        }
        if (!found) {
            chars.push_back(text[i]);
        }
    }

    vector<int> frequencies(chars.size(),0);

    for (int i = 0; i < text.size(); i++) {
        for (int j = 0; j < chars.size(); j++) {
            if (text[i] == chars[j]) {
                frequencies[j]++;
            }
        }
    }

    // cout << "UNSORTED FREQS:";
    // for (auto z : frequencies) {
    //     cout << z << ",";
    // }
    // cout << "\nUNSORTED CHARS:";
    // for (auto z : chars) {
    //     cout << z << ",";
    // }
    arr = chars;
    return frequencies;
}

int main () {
 
    vector<int> freqs;
    vector<char> arr;
    //vector<int> freqs = {5, 9, 12, 13, 16, 45};
    //vector<int> freqs = get_frequencies("abcdef", arr);
    // cout << "\n\n CHARS: ";
    // for (auto z : arr) {
    //     cout << z << ",";
    // }
    // cout << "\n FREQS: ";
    // for (auto z : freqs) {
    //     cout << z << ",";
    // }
    RESTART_INPUT:
    cout << "Do you want compress or decompress a text file [C/D]:\n";
    string str_c_or_d;
    bool wants_compress;
    cin >> str_c_or_d;
    if (tolower(str_c_or_d[0]) == 'c') {
        wants_compress = true;
    } else if(tolower(str_c_or_d[0]) == 'd') {
        wants_compress = false;
    } else {
        goto RESTART_INPUT;
    }

    //if (wants_compress) {
    if (wants_compress) {
        cout << "Enter the name of the file that you would like to compress. A compressed copy of it will be created\nFilename(must be .txt): ";
        string filename;
        cin >> filename;
        filename = filename;
        string text;
        ifstream in_file ("Text_Documents/" + filename); // reads file character by character (byte by byte)
        while (in_file && in_file.is_open()) {
            char temp_char;
            temp_char = in_file.get();
            text.push_back(temp_char);
            arr.push_back(temp_char);
        }
        text.pop_back();
        freqs = get_frequencies(text, arr);
        // for testing
        //text = "abcdef";
        //arr = {'a','b','c','d','e','f'};
        //freqs = {5,9,12,13,16,45};
        //freqs = {1,1,1,1,1,1};
        cout << "TEXT: " << text;
        huffman_compression(text, arr, freqs, filename, "Compressed_Documents/");

    } else {
        cout << "Enter the name of the file that you would like to decompress. A decompressed copy of it will be created.\nFilename (must be .huff): ";
        string filename;
        cin >> filename;
        string decompressed_text = decompress_huffman("Compressed_Documents/" + filename);
        cout << "\nTEXT: " << decompressed_text << "\n";
    }
}