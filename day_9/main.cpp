#include <iostream>
#include<fstream>
#include <ostream>
#include<string>
#include<vector>
#define CHANGE_TO_REBUILD 4

typedef unsigned long long ULONGLONG;

enum class FileSystemItemType {
    FreeBlock,
    FileBlock
};

class FileSystemMap : public std::vector<unsigned long> {
    public:
    explicit FileSystemMap(const std::string &s) {
        for (auto element: s) {
           push_back(std::strtoul(std::string{element}.c_str(),nullptr,10));
        }
    }
    friend std::ostream &operator<<(std::ostream &os, const FileSystemMap &fs) {
        for (auto it = fs.begin(); it != fs.end(); it++) {
            os << *it;
        }
        os << std::endl;
       return os<<std::flush;
    }

};

class FileSystemBlock {
public:
    friend bool operator==(const FileSystemBlock &lhs, const FileSystemBlock &rhs) {
        return lhs.type == rhs.type
               && lhs.id == rhs.id;
    }

    friend bool operator!=(const FileSystemBlock &lhs, const FileSystemBlock &rhs) {
        return !(lhs == rhs);
    }

    explicit FileSystemBlock(FileSystemItemType type, ULONGLONG id) : type(type), id(id) {
    }

    FileSystemBlock(const FileSystemBlock &other)
        : type(other.type),
          id(other.id) {
    }

    FileSystemBlock(FileSystemBlock &&other) noexcept
        : type(other.type),
          id(other.id) {
    }

    FileSystemBlock &operator=(const FileSystemBlock &other) {
        if (this == &other)
            return *this;
        type = other.type;
        id = other.id;
        return *this;
    }

    FileSystemBlock &operator=(FileSystemBlock &&other) noexcept {
        if (this == &other)
            return *this;
        type = other.type;
        id = other.id;
        return *this;
    }

    FileSystemItemType type;
    ULONGLONG id;

    [[nodiscard]] std::string to_string() const {
        if (type == FileSystemItemType::FileBlock) {
            return std::to_string(id);
        }
        return ".";
    }
};


class FileSystemBlocks : public std::vector<FileSystemBlock> {
public:
    friend std::ostream &operator<<(std::ostream &os, const FileSystemBlocks &obj) {
        for (const auto& element: obj) {
            os << element.to_string();
        }
        os << '\n';
        os << std::flush;
        return os;
    }
};


unsigned long long day_9_1(const std::string &file_system_description);

unsigned long long day_9_2(const std::string &file_system_description);

int main(const int argc, const char *argv[]) {
    std::ifstream in_file("input.txt");
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(in_file, line)) {
        lines.push_back(line);
    }
    const auto check_sum = day_9_1(lines[0]);
    std::cout << "Result 1: " << check_sum << std::endl;
    const auto check_sum_2 = day_9_2(lines[0]);
    std::cout << "Result 2: " << check_sum << std::endl;
}

FileSystemBlocks map_disk(const std::string &input) {
    FileSystemBlocks result;
    for (size_t i = 0; i < input.size(); i++) {
        if (i % 2 == 0) {
            const auto file_block_size = std::strtoul(input.substr(i, 1).c_str(), nullptr, 10);
            for (int j = 0; j < file_block_size; j++) {
                result.push_back(FileSystemBlock{FileSystemItemType::FileBlock, i / 2});
            }
        } else {
            const auto free_block_size = std::strtoul(input.substr(i, 1).c_str(), nullptr, 10);
            for (int j = 0; j < free_block_size; j++) {
                result.push_back(FileSystemBlock{FileSystemItemType::FreeBlock, 0});
            }
        }
    }
    return result;
}

void compact_disk(FileSystemBlocks &disk_map) {
    auto end = disk_map.size() - 1;
    do {
        if (const auto &end_item = disk_map[end]; end_item.type == FileSystemItemType::FileBlock) {
            break;
        }
        --end;
    } while (end > 0);
    auto begin = 0;
    do {
        if (disk_map[begin].type == FileSystemItemType::FreeBlock) {
            disk_map[begin] = disk_map[end];
            disk_map[end] = FileSystemBlock{FileSystemItemType::FreeBlock, 0};
            do {
                --end;
            } while (disk_map[end].type == FileSystemItemType::FreeBlock);
        } else {
            ++begin;
        }
    } while (begin < end);
}

struct SearchResult {
    size_t start;
    size_t end;
    size_t size;
};

FileSystemItemType get_file_system_item_type_from_index(const size_t& index) {
    if (index %2== 0) {
        return FileSystemItemType::FileBlock;
    }
    return FileSystemItemType::FreeBlock;
}

size_t get_free_block_index(size_t start, const FileSystemMap& map) {
    for (auto i = start; i < map.size(); ++i) {
        if ( get_file_system_item_type_from_index(i)==FileSystemItemType::FreeBlock && map[i]!=0) {
            return i;
        }
    }
    return map.size();
}

size_t get_file_block_index(size_t start, const FileSystemMap& map) {
    for (auto i = start; i >0; --i) {
        if ( get_file_system_item_type_from_index(i)==FileSystemItemType::FileBlock&& map[i]!=0) {
            return i;
        }
    }
    return 0;
}

void compact_disk_no_fragment(const FileSystemMap& map,FileSystemBlocks &disk_map) {
    // TODO Recalculate free space when a file is moved. While files are correctly checked once, free space always needs to be checked from the start every time.
    size_t free_block = get_free_block_index(0, map);
    size_t file_block = get_file_block_index(map.size()-1, map);

    do {
        const auto free_value = map[free_block];
        const auto file_value = map[file_block];
        if (free_value >=file_value ) {
            auto begin = disk_map.begin();
            auto end = disk_map.end()-1;

            for (int i=0;i<free_block;++i) {
                begin += map[i];
            }

            for (auto i=map.size()-1;i>file_block;--i) {
                end-=map[i];

            }

            for (auto i=0;i<file_value;++i) {
                *begin= *end;
                *end = FileSystemBlock{FileSystemItemType::FreeBlock, 0};
                ++begin;
                --end;
            }
            free_block = get_free_block_index(free_block+1, map);
        }
            file_block = get_file_block_index(file_block-1, map);
    }while (file_block>=1);
}


ULONGLONG day_9_1(const std::string &file_system_description) {
    auto sum = 0ull;
    auto expanded = map_disk(file_system_description);
    std::cout << file_system_description << std::endl;
    std::cout << expanded;
    compact_disk(expanded);
    std::cout << expanded;
    for (auto i = 0; i < expanded.size(); i++) {
        if (expanded[i].type == FileSystemItemType::FreeBlock) {
            continue;
        }
        sum += i * expanded[i].id;
    }
    return sum;
}

ULONGLONG day_9_2(const std::string &file_system_description) {
    auto sum = 0ull;

    auto expanded = map_disk(file_system_description);
    const FileSystemMap map (file_system_description);

    std::cout << map<< std::endl;
    std::cout << expanded;
    compact_disk_no_fragment(map,expanded);
    std::cout << expanded;

    for (auto i = 0; i < expanded.size(); i++) {
        if (expanded[i].type == FileSystemItemType::FreeBlock) {
            continue;
        }
        sum += i * expanded[i].id;
    }
    return sum;
}
