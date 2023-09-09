#include <iostream>
#include <vector>
#include <string>
#include <bits/stdc++.h>

using namespace std;

/*class compressed_txt_file() {

)*/

struct node {
    
    char data;
    
    unsigned freq;

    struct node *left, *right;

};

class compressed_txt_file {

};

// first few bytes of the file should be the letters and their respective frequencies and codes | i.e. (W, 45, 01011) aka Letter, Freq, Code
// string decompress_txt (string txt) {

// }

// writes huffman bits to file
// codes go first, seperated by a space and from the main text with a newline
// void write_to_file(vector<int> codes, string txt) {

// }
// i.e. W11010
// If it value of the right is greater write a '1', otherwise right a '0'.
// recursive function
// perhaps I could generate the codes while I am generating the huffman tree?

void insert_sorted(vector<node> &tree, node temp_node) {
    tree.push_back(temp_node);

    for (int i = 0; i < tree.size(); i++) {
        for (int j = 0; j < tree.size(); j++) {
            if (tree[i].freq < tree[j].freq) {
                int temp_int = tree[i].freq;
                tree[i].freq = tree[j].freq;
                tree[j].freq = temp_int;
                char temp_char = tree[i].data;
                tree[i].data = tree[j].data;
                tree[j].data = temp_char;
            }
        }
    }
}

node min_heap(vector<node> &vec, int min_int) {
    cout << "SIZE:" << vec.size() << "\n";
    node min = vec[0];
    int min_index = 0;
    for (int i = 0; i < vec.size(); i++) {
        if (vec[i].freq < min.freq && vec[i].freq > min_int) {
            min = vec[i];
            min_index = i;
        }
    }
    return min;
}

bool is_leaf (node temp_node) {
    return !(temp_node.left) && !(temp_node.right);
}

vector<string> get_codes(vector<node> tree, vector<char> chars) {
    //vector<node> missed_left_turns;
    vector<string> codes;
    // find which node points to letter, then find which node points that one and repeat
    for (int i = 0; i < chars.size(); i++) {
        string code = "";
        code.push_back(chars[i]);

        node letter_node;
        // finds which node points to letter
        for (int j = 0; j < tree.size(); j++) {
            if (tree[j].data == chars[i]) {
                letter_node = tree[j];
                break;
            }
        }

        node last_point = letter_node;
        for (int j = 0; j < tree.size(); j++) {
            if (tree[j].right) {
                cout << "R:" << tree[j].freq << "," << tree[j].right->freq << "," << last_point.freq << "\n";
                if (tree[j].right->freq == last_point.freq) {
                    cout << "A\n";
                    code.insert(code.begin() + 1, '1');
                    last_point = tree[j];
                }
            }
            if (tree[j].left) {
                cout << "L:" << tree[j].freq << "," << tree[j].left->freq << "," << last_point.freq << "\n";
                if (tree[j].left->freq == last_point.freq) {
                    cout << "B\n";
                    code.insert(code.begin() + 1, '0');
                    last_point = tree[j];
                }
            }
        }
        cout << "\n---------\n";
        codes.push_back(code);
    }

    return codes;
}

void huffman_compression(vector<char> chars, vector<int> freqs) {

    vector<string> codes;
    vector<char> sorted_chars = chars;
    vector<int> sorted_freqs = freqs;

    // sorts frequencies
    for (int i = 0; i < sorted_freqs.size(); i++) {
        for (int j = 0; j < sorted_freqs.size(); j++) {
            if (sorted_freqs[i] < sorted_freqs[j]) {
                int temp_int = sorted_freqs[i];
                sorted_freqs[i] = sorted_freqs[j];
                sorted_freqs[j] = temp_int;
                char temp_char = sorted_chars[i];
                sorted_chars[i] = sorted_chars[j];
                sorted_chars[j] = temp_char;
            }
        }
    }

    cout << "\n SORTED: ";
    for (auto z : sorted_freqs) {
        cout << z << ",";
    }

    vector<node> heap_tree(freqs.size());

    // init heap tree
    for (int i = 0; i < freqs.size(); i++) {
        heap_tree[i].freq = sorted_freqs[i];
        heap_tree[i].data = sorted_chars[i];
    }

    // once max frequency is reached, break
    //bool reached_max;

    int min_int = 0;
    int merge = heap_tree.size();
    int i = 0;
    while (true) {
        node* node_left = new node;
        //node min = min_heap(heap_tree, min_int);
       // min_int = min.freq;
        node_left->freq = heap_tree[i].freq;
        node_left->data = heap_tree[i].data;
        cout << "\nI:" << i << "\n";
        i++;

        node* node_right = new node;
        //min = min_heap(heap_tree, min_int);
        //min_int = min.freq;
        node_right->freq = heap_tree[i].freq;
        node_right->data = heap_tree[i].data;
        i++;

        if (merge == 1) {
            break;
        }

        node* sum_node = new node;
        sum_node->freq = node_right->freq + node_left->freq;
        sum_node->data = 0;
        sum_node->left = node_left;
        sum_node->right = node_right;
        //heap_tree.push_back(sum_node);
        insert_sorted(heap_tree, *sum_node);
        cout << "\nSIZE:" << heap_tree.size();
        cout << "\nLEFT: " << sum_node->left->data << "," << sum_node->left->freq;
        cout << "\nRIGHT: " << sum_node->right->data << "," << sum_node->right->freq;
        cout << "\nSUM: " << sum_node->data << "," << sum_node->freq;
        
        merge--;
    }

    cout << "\nSIZE: " << heap_tree.size() << "\n";
    cout << "\n F: ";
    for (int i = 0; i < heap_tree.size(); i++) {
        cout << heap_tree[i].freq << ",";
    }
    /*cout << "\n RF: ";
    for (int i = 0; i < heap_tree.size(); i++) {
        if (heap_tree[i].right) {
            cout << heap_tree[i].right->freq << ",";
        }
    }*/
    cout << "\n";

    codes = get_codes(heap_tree, chars);

    cout << "\n CODES: ";
    for (auto z : codes) {
        cout << z << ",";
    }
}

vector<int> get_frequencies(string text, vector<char> &arr) {
    vector<char> chars;
    for (int i = 0; i < text.size(); i++) {
        bool found = false;
        for (int j = 0; j < chars.size(); j++) {
            if (chars[j] == text[i]) {
                found = true;
                break;
            }
        }
        if (found == false) {
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

    arr = chars;
    return frequencies;
}

int main () {
    //string str = "Wild YY Wild West";
    //vector<char> arr;
    //vector<int> freqs = get_frequencies(str, arr);
    vector<char> arr = {'a', 'b', 'c', 'd', 'e', 'f'};
    vector<int> freqs = {5, 9, 12, 13, 16, 45};
    cout << "\n\n CHARS: ";
    for (auto z : arr) {
        cout << z << ",";
    }
    cout << "\n\n FREQS: ";
    for (auto z : freqs) {
        cout << z << ",";
    }
    huffman_compression(arr, freqs);
}