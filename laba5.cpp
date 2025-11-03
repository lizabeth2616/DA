// Суффиксные деревья 0 Упрощенный вариант
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

struct SuffixTreeNode {
    map<char, SuffixTreeNode*> children;
    SuffixTreeNode* suffix_link;
    int start;
    int* end;
    int suffix_index;

    SuffixTreeNode(int s, int* e)
        : suffix_link(nullptr), start(s), end(e), suffix_index(-1) {}
};

class SuffixTree {
private:
    string text;
    SuffixTreeNode* root;
    int *leaf_end;
    int *root_end;
    int size; // length of text (with $)
    SuffixTreeNode* last_new_node;
    SuffixTreeNode* active_node;
    int active_edge;   // index in text
    int active_length;
    int remaining_suffix_count;
    int leaf_end_value;

    SuffixTreeNode* new_node(int start, int* end) {
        SuffixTreeNode* node = new SuffixTreeNode(start, end);
        return node;
    }

    int edge_length(SuffixTreeNode* n) {
        if (n == root) return 0;
        return *(n->end) - n->start + 1;
    }

    bool walk_down(SuffixTreeNode* next) {
        // active_length >= edge_length(next) => move active point down
        if (active_length >= edge_length(next)) {
            active_edge += edge_length(next);
            active_length -= edge_length(next);
            active_node = next;
            return true;
        }
        return false;
    }

    void extend_suffix_tree(int pos) {
        leaf_end_value = pos;
        *leaf_end = leaf_end_value; // update global leaf_end
        remaining_suffix_count++;
        last_new_node = nullptr;

        while (remaining_suffix_count > 0) {
            if (active_length == 0)
                active_edge = pos; // active_edge is index into text

            char active_edge_char = text[active_edge];
            auto it = active_node->children.find(active_edge_char);

            if (it == active_node->children.end()) {
                // no outgoing edge starting with active_edge_char -> create leaf
                SuffixTreeNode* leaf = new_node(pos, leaf_end);
                leaf->suffix_index = pos - remaining_suffix_count + 1;
                active_node->children[active_edge_char] = leaf;

                // add suffix link from last_new_node to active_node if needed
                if (last_new_node != nullptr) {
                    last_new_node->suffix_link = active_node;
                    last_new_node = nullptr;
                }
            } else {
                SuffixTreeNode* next = it->second;
                if (walk_down(next)) {
                    // start next phase after walking down
                    continue;
                }

                // current character on edge to compare
                char ch = text[next->start + active_length];
                if (ch == text[pos]) {
                    // rule 3 (current character is already on the edge)
                    active_length++;
                    if (last_new_node != nullptr) {
                        last_new_node->suffix_link = active_node;
                        last_new_node = nullptr;
                    }
                    break;
                }

                // rule 2 (split edge)
                int* split_end = new int(next->start + active_length - 1);
                SuffixTreeNode* split = new_node(next->start, split_end);
                active_node->children[active_edge_char] = split;

                // new leaf from split
                SuffixTreeNode* leaf = new_node(pos, leaf_end);
                leaf->suffix_index = pos - remaining_suffix_count + 1;
                split->children[text[pos]] = leaf;

                // adjust next
                next->start = next->start + active_length;
                split->children[text[next->start]] = next;

                // suffix link from last newly created internal node
                if (last_new_node != nullptr) {
                    last_new_node->suffix_link = split;
                }
                last_new_node = split;
            }

            remaining_suffix_count--;

            if (active_node == root && active_length > 0) {
                active_length--;
                // set active_edge to the next suffix's first char index
                active_edge = pos - remaining_suffix_count + 1;
            } else if (active_node != root) {
                active_node = active_node->suffix_link ? active_node->suffix_link : root;
            }
        }
    }

    void set_suffix_index_by_dfs(SuffixTreeNode* n, int label_height) {
        if (!n) return;
        bool leaf = true;
        for (auto &p : n->children) {
            leaf = false;
            set_suffix_index_by_dfs(p.second, label_height + edge_length(p.second));
        }
        if (leaf) {
            n->suffix_index = size - label_height;
        }
    }

    void collect_suffix_indices(SuffixTreeNode* node, vector<int>& indices) {
        if (!node) return;
        if (node->suffix_index > -1) {
            indices.push_back(node->suffix_index);
            return;
        }
        for (auto &p : node->children)
            collect_suffix_indices(p.second, indices);
    }

    void search_pattern_from_node(SuffixTreeNode* node, const string& pattern, int pat_idx, vector<int>& positions) {
        if (!node) return;
        if (pat_idx == (int)pattern.size()) {
            // matched pattern completely => collect all suffixes under this node
            collect_suffix_indices(node, positions);
            return;
        }

        char c = pattern[pat_idx];
        auto it = node->children.find(c);
        if (it == node->children.end()) return;

        SuffixTreeNode* child = it->second;
        int edge_len = edge_length(child);
        int i = 0;
        // compare along edge
        while (i < edge_len && pat_idx < (int)pattern.size()) {
            if (text[child->start + i] != pattern[pat_idx]) return;
            i++; pat_idx++;
        }

        if (pat_idx == (int)pattern.size()) {
            // pattern exhausted in middle or end of edge
            collect_suffix_indices(child, positions);
            return;
        } else {
            search_pattern_from_node(child, pattern, pat_idx, positions);
            return;
        }
    }

    void free_tree(SuffixTreeNode* node) {
        if (!node) return;
        for (auto &p : node->children) free_tree(p.second);
        // free end pointer only if it was individually allocated (not leaf_end or root_end)
        if (node->end != leaf_end && node->end != root_end) {
            delete node->end;
        }
        delete node;
    }

public:
    explicit SuffixTree(const string& s) {
        text = s + '$';
        size = (int)text.size();
        leaf_end = new int(-1);
        root_end = new int(-1);
        root = new_node(-1, root_end);
        root->suffix_link = root;

        active_node = root;
        active_edge = -1;
        active_length = 0;
        remaining_suffix_count = 0;
        last_new_node = nullptr;
        leaf_end_value = -1;

        for (int i = 0; i < size; ++i) {
            extend_suffix_tree(i);
        }

        // set suffix indices by DFS so that internal nodes get proper indexes for leaves
        set_suffix_index_by_dfs(root, 0);
    }

    ~SuffixTree() {
        free_tree(root);
        delete leaf_end;
        delete root_end;
    }

    vector<int> search(const string& pattern) {
        vector<int> positions;
        if (pattern.empty()) return positions;
        search_pattern_from_node(root, pattern, 0, positions);
        sort(positions.begin(), positions.end());
        positions.erase(unique(positions.begin(), positions.end()), positions.end());
        // convert to 1-based positions as original program expected
        for (int &p : positions) ++p;
        return positions;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string text;
    if (!getline(cin, text)) return 0;

    SuffixTree st(text);

    string pattern;
    int pattern_number = 1;
    while (getline(cin, pattern)) {
        if (pattern.empty()) { pattern_number++; continue; }
        vector<int> pos = st.search(pattern);
        if (!pos.empty()) {
            cout << pattern_number << ": ";
            for (size_t i = 0; i < pos.size(); ++i) {
                if (i) cout << ",";
                cout << pos[i];
            }
            cout << "\n";
        }
        pattern_number++;
    }
    return 0;
}
