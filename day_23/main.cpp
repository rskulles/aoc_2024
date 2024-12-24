#include <iostream>
#include<fstream>
#include <functional>
#include <sstream>
#include<string>
#include<vector>
#include<map>
#include<memory>
#define CHANGE_TO_REBUILD 124

struct Connection {
    std::string from;
    std::string to;
};

class BST_Node {
public:

    explicit BST_Node(const std::string& d): BST_Node() {
        data = d;
    }

    std::string data;
    std::shared_ptr<BST_Node> left{nullptr};
    std::shared_ptr<BST_Node> right{nullptr};
    std::vector<BST_Node *> network_connections{};
    [[nodiscard]] std::vector<std::string>get_largest_number_of_connections() const {
        std::vector<std::string> result;
        result.reserve(network_connections.size()+1);
        result.push_back(data);
        std::map<std::string,int> m;
        std::vector<BST_Node *>all_connections;
        if (data=="yn") {
            std::cout<<""<<std::endl;
        }
        m[data]=0;
        for (const auto& connection : network_connections) {
            m[connection->data]=1;
            for (const auto& node : connection->network_connections) {
                all_connections.push_back(node);
            }
        }
        for (const auto& connection : all_connections) {
            m[connection->data]+=1;
        }
        for (const auto&[fst, snd] : m) {
            if (snd==m[data]-1) {
                result.push_back(fst);
            }
        }
        return result;
    }

private:
    [[nodiscard]]bool connected_to(const std::string & string)const {
        auto found =false;
        for (const auto &c : network_connections) {
            if (string == c->data) {
                found = true;
                break;
            }
        }
        return found;
    }

    BST_Node()=default;
    static void delete_node(const BST_Node *node) {
        if (nullptr == node) {
            return;
        }
        delete node;
    }
};

class BST {
public:
    BST() = default;

    ~BST() = default;

    std::shared_ptr<BST_Node> insert_and_return(const std::string& data) {
        if (this->root == nullptr) {
            this->root =std::make_shared<BST_Node>(data);
            return this->root;
        }
        auto node = root;
        while (node != nullptr) {
            const auto v = node->data.compare(data);
            if (v == 0) {
                return node;
            }
            if (v > 0) {
                if (node->right == nullptr) {
                    node->right = std::make_shared<BST_Node>(data);
                    return node->right;
                }
                node = node->right;
            } else {
                if (node->left == nullptr) {
                    node->left = std::make_shared<BST_Node>(data);
                    return node->left;
                }
                node = node->left;
            }
        }
        return nullptr;
    }

   [[nodiscard]]std::shared_ptr<BST_Node> find(const std::string &data)const {
        return find_recursive(data, root);
    }

    std::vector<BST_Node *> collect(const std::function<bool(const std::shared_ptr<BST_Node>&)> &func) const {
        std::vector<BST_Node *> result;
        collect_recursive(func, root, result);
        return result;
    }

    [[nodiscard]]unsigned long long count() const{
        return count_recursive(root);
    }

    void print_connection_counts() const {
        print_connection_counts_recursive(root);
    }

private:
    static void print_connection_counts_recursive(const std::shared_ptr<BST_Node>& node){
        if (node == nullptr) {
            return;
        }
        std::cout<<node->data<<": "<<node->network_connections.size()<<"-> ";
        for (const auto& connection : node->network_connections) {
            std::cout<<connection->data<<" ";
        }
        std::cout<<std::endl<<std::flush;
        print_connection_counts_recursive(node->left);
        print_connection_counts_recursive(node->right);
    }
    static unsigned long long count_recursive(const std::shared_ptr<BST_Node>& node){
        if (node == nullptr) {
            return 0;
        }
        return 1 + count_recursive(node->left) + count_recursive(node->right);
    }

    static void collect_recursive(const std::function<bool(const std::shared_ptr<BST_Node> )> &func, const std::shared_ptr<BST_Node> &node,std::vector<BST_Node *> &nodes) {
        if (node == nullptr) {
            return;
        }

        if (func(node)) {
            nodes.push_back(node.get());
        }
        collect_recursive(func, node->left, nodes);
        collect_recursive(func, node->right, nodes);
    }

    static std::shared_ptr<BST_Node> find_recursive(const std::string &data,const std::shared_ptr<BST_Node>&node) {
        if (node == nullptr) {
            return nullptr;
        }
        if (node->data == data) {
            return node;
        }
        if (const auto &n = find_recursive(data, node->left); n != nullptr) {
            return n;
        }
        if (const auto &n = find_recursive(data, node->right); n != nullptr) {
            return n;
        }
        return nullptr;
    }

    std::shared_ptr<BST_Node> root{nullptr};
};

typedef BST set;

class Puzzle {
public:
    Puzzle() = delete;

    explicit Puzzle(const std::vector<Connection> &connections) {
        for (const auto &connection: connections) {
            auto f = network.insert_and_return(connection.from);
            auto t = network.insert_and_return(connection.to);
            t->network_connections.push_back(f.get());
            f->network_connections.push_back(t.get());
        }
    }

    [[nodiscard]] unsigned long long get_network_count() const {
        const auto t_nodes = network.collect([](const std::shared_ptr<BST_Node>&node) { return node->data[0] == 't'; });
        std::vector<std::vector<std::string> > network_strings;
        for (const auto &node_layer_1: t_nodes) {
            std::vector<BST_Node *> good_layer_2_nodes;
            for (const auto &node_layer_2: node_layer_1->network_connections) {
                if (node_layer_2->data != node_layer_1->data) {
                    good_layer_2_nodes.push_back(node_layer_2);
                }
            }

            for (const auto &node_layer_2: good_layer_2_nodes) {
                std::vector<BST_Node *> good_layer_3_nodes;
                for (const auto &node_layer_3: node_layer_2->network_connections) {
                    if (node_layer_3->data != node_layer_1->data) {
                        auto good = false;
                        for (const auto &node: node_layer_3->network_connections) {
                            if (node->data == node_layer_1->data) {
                                good = true;
                            }
                        }
                        if (good) {
                            good_layer_3_nodes.push_back(node_layer_3);
                            network_strings.push_back({node_layer_1->data, node_layer_2->data, node_layer_3->data});
                        }
                    }
                }
            }
        }
        set unique_nodes;
        for (auto &n: network_strings) {
            std::sort(n.begin(), n.end());
            std::stringstream ss;
            for (const auto &s: n) {
                ss << s;
            }

             unique_nodes.insert_and_return(ss.str());
        }

        return unique_nodes.count();
    }

    [[nodiscard]]std::string get_network_password() const {
        const auto nodes = network.collect([](const std::shared_ptr<BST_Node> &node) {return true;});
        std::vector<std::string> largest_network;

        for (const auto &node: nodes) {
            const auto network = node->get_largest_number_of_connections();
            if (network.size()> largest_network.size()) {
                largest_network = network;
            }
        }
        std::sort(largest_network.begin(), largest_network.end());
        std::stringstream ss;
        for (auto it = largest_network.begin(); it != largest_network.end(); ++it) {
            if (it == largest_network.end()-1) {
                ss<<*it;
            }else {
                ss<<*it<<',';
            }
        }
        return ss.str();
    }

private:
    set network;
};

unsigned long long day_23_1(const Puzzle &puzzle);

std::string day_23_2(const Puzzle &puzzle);

Puzzle parse_puzzle(const std::vector<std::string> &puzzle);

int main(const int argc, const char *argv[]) {
    std::ifstream in_file("input.txt");
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(in_file, line)) {
        lines.push_back(line);
    }
    in_file.close();
    const auto puzzle = parse_puzzle(lines);

    const auto result_1 = day_23_1(puzzle);
    const auto result_2 = day_23_2(puzzle);
    std::cout << "Result 1: " << result_1 << "\n";
    std::cout << "Result 2: " << result_2 << "\n";
}

Puzzle parse_puzzle(const std::vector<std::string> &puzzle) {
    std::vector<Connection> connections;
    for (const auto &line: puzzle) {
        const auto it = line.find("-");
        const auto first = line.substr(0, it);
        const auto second = line.substr(it + 1);
        connections.push_back({first, second});
    }
    return Puzzle(connections);
}

unsigned long long day_23_1(const Puzzle &puzzle) {
    return puzzle.get_network_count();
}

std::string day_23_2(const Puzzle &puzzle) {
    return puzzle.get_network_password();
}
