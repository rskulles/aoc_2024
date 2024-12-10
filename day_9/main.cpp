#include <iostream>
#include<fstream>
#include <ostream>
#include<string>
#include<vector>
#define CHANGE_TO_REBUILD 2

typedef unsigned long long ULL;

enum class FileSystemItemType {
    FreeBlock,
    FileBlock
};

class FileSystemMap : public std::vector<unsigned long> {
public:
    explicit FileSystemMap(const std::string &s) {
        for (auto element: s) {
            push_back(std::strtoul(std::string{element}.c_str(), nullptr, 10));
        }
    }

    FileSystemMap(const FileSystemMap &type) : vector(type.begin(), type.end()) {
    }

    friend std::ostream &operator<<(std::ostream &os, const FileSystemMap &fs) {
        for (auto it = fs.begin(); it != fs.end(); it++) {
            os << *it;
        }
        os << std::endl;
        return os << std::flush;
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

    explicit FileSystemBlock(FileSystemItemType type, ULL id) : type(type), id(id) {
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
    ULL id;

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
        for (const auto &element: obj) {
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
    std::cout << "Result 2: " << check_sum_2 << std::endl;
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

FileSystemItemType get_file_system_item_type_from_index(const size_t &index) {
    if (index % 2 == 0) {
        return FileSystemItemType::FileBlock;
    }
    return FileSystemItemType::FreeBlock;
}

size_t get_free_block_index(size_t start, const FileSystemMap &map) {
    for (auto i = start; i < map.size(); ++i) {
        if (get_file_system_item_type_from_index(i) == FileSystemItemType::FreeBlock && map[i] != 0) {
            return i;
        }
    }
    return map.size();
}

size_t get_sized_free_block_index(size_t start, const FileSystemMap &map, const unsigned long min_size) {
    for (auto i = start; i < map.size(); ++i) {
        if (get_file_system_item_type_from_index(i) == FileSystemItemType::FreeBlock &&
            map[i] >= min_size) {
            return i;
        }
    }
    return map.size();
}

size_t get_file_block_index(size_t start, const FileSystemMap &map) {
    for (auto i = start; i > 0; --i) {
        if (get_file_system_item_type_from_index(i) == FileSystemItemType::FileBlock && map[i] != 0) {
            return i;
        }
    }
    return 0;
}

struct BlockBounds {
    size_t start;
    size_t end;

    [[nodiscard]] size_t size() const {
        return end - start + 1;
    }
};

BlockBounds get_free_block_bounds(size_t start, const FileSystemBlocks &disk_blocks) {
    size_t begin = 0;
    size_t end = 0;
    auto found_start = false;
    for (auto i = start; i < disk_blocks.size(); ++i) {
        if (disk_blocks[i].type == FileSystemItemType::FreeBlock && !found_start) {
            begin = i;
            found_start = true;
        } else if (found_start && disk_blocks[i].type == FileSystemItemType::FileBlock) {
            end = i - 1;
            break;
        }
    }
    return {begin, end};
}

BlockBounds get_file_block_bounds(size_t end, const FileSystemBlocks &disk_blocks) {
    size_t begin = 0;
    size_t last = 0;
    auto found_last = false;
    ULL id = 0;
    for (auto i = end; i > 0; --i) {
        if (disk_blocks[i].type == FileSystemItemType::FileBlock &&
            !found_last) {
            last = i;
            found_last = true;
            id = disk_blocks[i].id;
        } else if (found_last && (disk_blocks[i].type == FileSystemItemType::FreeBlock ||
            (disk_blocks[i].type == FileSystemItemType::FileBlock && disk_blocks[i].id !=id))) {
            begin = i + 1;
            break;
        }
    }
    return {begin, last};
}

void compact_disk_no_fragment(FileSystemBlocks &disk_blocks) {
    auto compacting = true;
    auto file_bounds = get_file_block_bounds(disk_blocks.size() - 1, disk_blocks);
    while (compacting) {

        if (file_bounds.start==0) {
           compacting = false;
            continue;
        }
        auto free_bounds = BlockBounds{0,0};
        do {
            free_bounds = get_free_block_bounds(free_bounds.start+1, disk_blocks);
        }while (free_bounds.size() <file_bounds.size());

        if (free_bounds.size()>= file_bounds.size()&& free_bounds.start< file_bounds.start) {

            for (auto i = free_bounds.start; i <= free_bounds.start+file_bounds.size()-1; ++i) {

                disk_blocks[i] = disk_blocks[file_bounds.end];
            }

            for (auto i = file_bounds.start; i <= file_bounds.end; ++i) {
                disk_blocks[i].id=0;
                disk_blocks[i].type = FileSystemItemType::FreeBlock;
            }
        }
        file_bounds = get_file_block_bounds(file_bounds.start-1, disk_blocks);
    }
}


ULL day_9_1(const std::string &file_system_description) {
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

ULL day_9_2(const std::string &file_system_description) {
    auto sum = 0ull;

    auto expanded = map_disk(file_system_description);
    const FileSystemMap map(file_system_description);

    std::cout << map << std::endl;
    std::cout << expanded;
    compact_disk_no_fragment(expanded);
    std::cout << expanded;

    for (auto i = 0; i < expanded.size(); i++) {
        if (expanded[i].type == FileSystemItemType::FreeBlock) {
            continue;
        }
        sum += i * expanded[i].id;
    }
    return sum;
}
