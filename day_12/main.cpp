#include <iostream>
#include<fstream>
#include <map>
#include <ostream>
#include<string>
#include<vector>
#include<sstream>
#define CHANGE_TO_REBUILD 211

struct Position {
    friend bool operator==(const Position &lhs, const Position &rhs) {
        return lhs.x == rhs.x
               && lhs.y == rhs.y;
    }

    friend bool operator!=(const Position &lhs, const Position &rhs) {
        return !(lhs == rhs);
    }
    std::string to_string() const {
        std::stringstream ss;
        ss << this->x;
        ss<<":"<<this->y;
        return ss.str();
    }
    long x;
    long y;
};


class GardenTile {
public:
    GardenTile(const GardenTile &other) = default;

    GardenTile &operator=(const GardenTile &other) {
        if (this == &other)
            return *this;
        _label = other._label;
        _position = other._position;
        _area = other._area;
        _perimeter = other._perimeter;
        return *this;
    }

    friend bool operator==(const GardenTile &lhs, const GardenTile &rhs) {
        return lhs._label == rhs._label &&
               lhs._position.x == rhs._position.x &&
               lhs._position.y == rhs._position.y;
    }

    friend bool operator!=(const GardenTile &lhs, const GardenTile &rhs) {
        return !(lhs == rhs);
    }

    bool next_to(const GardenTile &garden_tile) const {
        const auto up = Position{_position.x, _position.y - 1};
        const auto down = Position{_position.x, _position.y + 1};
        const auto left = Position{_position.x - 1, _position.y};
        const auto right = Position{_position.x + 1, _position.y};
        const auto test_position = garden_tile._position;
        return up == test_position ||
               down == test_position ||
               left == test_position ||
               right == test_position;
    }

    bool contains_vertex(int x, int y) const {
        const auto pos = Position{x,y};
        Position  positions[4];
        positions[0] = _position;
        positions[1] = Position{_position.x + 1,_position.y};
        positions[2] = Position{_position.x ,_position.y + 1};
        positions[3] = Position{_position.x + 1,_position.y + 1};
        return positions[0]==pos ||
        positions[1]==pos ||
        positions[2]==pos ||
        positions[3]==pos;
    }

    explicit GardenTile(const char label, const Position &position): _label(label), _position(position) {
    }

    const char &get_label() const {
        return _label;
    }

    const Position &get_position() const {
        return _position;
    }

    void set_perimeter(unsigned int perimeter) {
        _perimeter = perimeter;
    }

    unsigned int get_perimeter() const {
        return _perimeter;
    }

    [[nodiscard]] unsigned int get_area() const {
        return _area;
    }

private:
    char _label;
    Position _position;
    unsigned int _area = 1;
    unsigned int _perimeter;
};


class GardenRegion : public std::vector<GardenTile> {

    friend std::ostream & operator<<(std::ostream &os, const GardenRegion &region) {
        os<<"=========\n";
        os<<"Region: "<<region[0].get_label();
        os<<"\n=========\n";
        auto largest = Position{0,0};

        for (const auto &garden_tile : region) {
            const auto tile_position = garden_tile.get_position();
            if ( tile_position.x > largest.x ) {
                largest.x = tile_position.x;
            }

            if ( tile_position.y > largest.y ) {
                largest.y = tile_position.y;
            }
        }

        std::vector<std::vector<char>> tiles;
        tiles.resize(largest.y+1);
        for (auto & labels: tiles) {
            labels.resize(largest.x+1);
        }
        for (auto& chars: tiles) {
            for (auto & c: chars) {
                c='.';
            }
        }
        for (auto& t: region) {
                auto l = t.get_label();
                auto p = t.get_position();
                tiles[p.y][p.x]=l;
        }

        for (auto y: tiles) {
            for (const auto& x: y) {
                os<<x;
            }
            os<<'\n';
        }
        return os<<std::flush;
    }

public:
    GardenRegion(): _label('\0') {
    }

    explicit GardenRegion(const GardenTile &tile): _label(tile.get_label()) {
        push_back(tile);
    }

    void add_garden_tile(const GardenTile &tile) {
        if (contains(tile)) {
            return;
        }
        push_back(tile);
    }

    const char &get_label() const {
        return _label;
    }

    bool contains(const GardenTile &tile) const {
        return  std::any_of(std::begin(*this), std::end(*this),[&](const GardenTile &t){return t==tile;});
    }

    std::vector<GardenTile> get_tiles(const std::string& location)const {
        std::vector<GardenTile> tiles;
        const auto it = location.find(':');
        if (it == std::string::npos) {
            return tiles;
        }
        const auto x = std::stoi(location.substr(0,it));
        const auto y = std::stoi(location.substr(it+1));
        for (const auto &tile: *this) {
            if (tile.contains_vertex(x,y)) {
                tiles.push_back(tile);
            }
        }
        return tiles;
    }

private:
    const char _label;
};

class Garden : public std::vector<std::vector<GardenTile> > {
public:
    explicit Garden(const std::vector<std::string> &lines) {
        for (int i = 0; i < lines.size(); ++i) {
            std::vector<GardenTile> tiles;
            for (int j = 0; j < lines[i].size(); ++j) {
                GardenTile t(lines[i][j], Position{j, i});
                tiles.push_back(t);
            }
            add_row(tiles);
        }

        for (auto &tiles: *this) {
            for (auto &t: tiles) {
                t.set_perimeter(perimeter_of(t));
            }
        }
    }

    [[nodiscard]]GardenRegion get_region(const Position& position) const {
        GardenRegion region(tile_at(position));
        auto next_tiles= get_connected_tiles(position);
        while (!next_tiles.empty()) {
            auto tile = next_tiles.end()-1;
            region.add_garden_tile(*tile);
            auto connected_tiles = get_connected_tiles(tile->get_position());
            next_tiles.pop_back();
            for (auto & connected_tile: connected_tiles) {
                if (!region.contains(connected_tile)) {
                    next_tiles.push_back(connected_tile);
                }
            }
        }
        return region;
    }

    void add_row(const std::vector<GardenTile> &row) {
        this->push_back(row);
    }

    const GardenTile &tile_at(const int &x, const int &y) const {
        return this->at(y).at(x);
    }

    const GardenTile &tile_at(const Position &position) const {
        return tile_at(position.x, position.y);
    }

    friend std::ostream &operator<<(std::ostream &os, const Garden &garden) {
        for (const auto &row: garden) {
            for (const GardenTile &tile: row) {
                os << tile.get_label();
            }
            os << std::endl;
        }
        return os << std::flush;
    }

    bool on_garden(const Position &position) const {
        const auto &vec = *this;
        return position.x >= 0 && position.y >= 0 && position.y < vec.size() && position.x < vec[0].size();
    }

    unsigned int perimeter_of(const GardenTile &tile) const {
        Position check_positions[4];

        constexpr auto up = 0ul;
        constexpr auto down = 1ul;
        constexpr auto left = 2ul;
        constexpr auto right = 3ul;

        check_positions[up] = tile.get_position();
        check_positions[up].y -= 1;

        check_positions[down] = tile.get_position();
        check_positions[down].y += 1;

        check_positions[left] = tile.get_position();
        check_positions[left].x -= 1;

        check_positions[right] = tile.get_position();
        check_positions[right].x += 1;

        const auto label = tile.get_label();
        unsigned int perimeter = 0;
        for (int i = 0; i < 4; ++i) {
            if (auto position = check_positions[i]; on_garden(position)) {
                if (const auto &other_label = tile_at(position).get_label(); other_label != label) {
                    perimeter += 1;
                }
            } else {
                perimeter += 1;
            }
        }
        return perimeter;
    }
    private:
    [[nodiscard]]std::vector<GardenTile> get_connected_tiles(const Position& position) const {
        std::vector<GardenTile> tiles;
        const auto source_tile = tile_at(position);
        Position check_positions[4];
        check_positions[0] = Position{position.x, position.y - 1};
        check_positions[1] = Position{position.x, position.y + 1};
        check_positions[2]=  Position{position.x-1, position.y};
        check_positions[3]=  Position{position.x+1, position.y };
        for (auto check_position : check_positions) {
            if (on_garden(check_position)) {
                if (auto tile = tile_at(check_position); tile.get_label() == source_tile.get_label()) {
                    tiles.push_back(tile);
                }
            }
        }
        return tiles;
    }
};


class VertexMap:public std::map<std::string, long> {
public:
    static VertexMap from_region(const GardenRegion& region) {
        VertexMap map;
        for (const auto &tile: region) {
            const auto top_left = tile.get_position();

            if (map.find(top_left.to_string()) != map.end()) {
                map[top_left.to_string()] += 1;
            }else {
                map[top_left.to_string()] = 1;
            }

            const auto top_right = Position{top_left.x + 1, top_left.y};
            if (map.find(top_right.to_string()) != map.end()) {
                map[top_right.to_string()] += 1;
            }else {
                map[top_right.to_string()] = 1;
            }

            const auto bottom_left = Position{top_left.x , top_left.y+1};
            if (map.find(bottom_left.to_string()) != map.end()) {
                map[bottom_left.to_string()] += 1;

            }else {
                map[bottom_left.to_string()] = 1;
            }

            const auto bottom_right = Position{top_left.x + 1, top_left.y+1};
            if (map.find(bottom_right.to_string()) != map.end()) {
                map[bottom_right.to_string()] += 1;

            }else {
                map[bottom_right.to_string()] = 1;
            }
        }
        return map;
    }
};

class GardenRegions : public std::vector<GardenRegion> {
public:
    void add(const GardenTile &tile, const Garden &garden) {

        if (this->empty()) {
            push_back(garden.get_region(tile.get_position()));
            return;
        }

        if (tile.get_perimeter() == 4) {
            push_back(GardenRegion(tile));
            return;
        }

        for (auto & region: *this) {
            if (region.contains(tile)) {
                return;
            }
        }

        push_back(garden.get_region(tile.get_position()));
    }
};

unsigned long long day_12_1(const Garden &garden);
unsigned long long day_12_2(const Garden &garden);

int main(const int argc, const char *argv[]) {
    std::ifstream in_file("input.txt");
    std::string line;
    std::vector<std::string> lines;

    while (std::getline(in_file, line)) {
        lines.push_back(line);
    }

    const Garden garden(lines);
    std::cout << garden;
    const auto result_1 = day_12_1(garden);
    std::cout << "Result 1: " << result_1 << std::endl;
    const auto result_2 = day_12_2(garden);
    std::cout << "Result 2: " << result_2 << std::endl;
}

//#define debug_draw_1 1

unsigned long long day_12_1(const Garden &garden) {
    unsigned long long result = 0;
    GardenRegions regions;
    for (const auto &row: garden) {
        for (const GardenTile &tile: row) {
            regions.add(tile, garden);
        }
    }

#ifdef debug_draw_1
    for (const auto &region: regions) {
        std::cout << region.get_label() << "-> " << region.size() << std::endl;
    }
#endif

    for (const auto &region: regions) {
        unsigned long a=0;
        unsigned long p=0;
        for (const GardenTile &tile: region) {
            a+= tile.get_area();
            p+= tile.get_perimeter();
        }
            result += a*p;
    }
    return result;
}
#define debug_draw_2 1
unsigned long long day_12_2(const Garden &garden) {
    unsigned long long result = 0;
    GardenRegions regions;
    for (const auto &row: garden) {
        for (const GardenTile &tile: row) {
            regions.add(tile, garden);
        }
    }

#ifdef debug_draw_2
    for (const auto &region: regions) {
        std::cout << region;
    }
#endif
    for (const auto &region: regions) {
        auto side_count =0;
        auto map = VertexMap::from_region(region);
        for (const auto &kvp: map) {
            if (kvp.second ==1 || kvp.second==3) {
                ++side_count;
            }else if (kvp.second==2) {
               std::vector<GardenTile> tiles = region.get_tiles(kvp.first);
                auto tile_1 = tiles[0];
                auto tile_2 = tiles[1];
                if (!tile_1.next_to(tile_2)) {
                    side_count += 2;
                }
            }
        }
        result += side_count * region.size();
    }
    return result;
}
