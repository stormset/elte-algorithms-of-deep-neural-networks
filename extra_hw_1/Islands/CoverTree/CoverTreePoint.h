#ifndef COVER_TREE_POINT_H
#define COVER_TREE_POINT_H

#include <cstddef>
#include <vector>

/**
 * A simple point class containing a vector of doubles and a single char name.
 */
class CoverTreePoint
{
private:
    int _x;
    int _y;
    int _faction;
public:
    CoverTreePoint(int x = 0, int y = 0, int faction = -1) : _x(x), _y(y), _faction(faction) {}
    int get_x() const { return _x; }
    int get_y() const { return _y; }
    double distance(const CoverTreePoint& p) const { return abs(_x - p._x) + abs(_y - p._y); };
    bool operator==(const CoverTreePoint& p) const { return (_x == p._x && _y == p._y); };
    bool operator>(const CoverTreePoint& p) const { return (_x != p._x ? (_x > p._x) : (_y > p._y)); };
    int get_faction_id() const { return _faction; };
    void set_faction_id(int faction_id) { _faction = faction_id; };
    friend std::ostream& operator<<(std::ostream& os, const CoverTreePoint& dt);

    struct hash {
        inline size_t operator()(const CoverTreePoint &p) const {
            std::hash<int> int_hash;
            return int_hash(p._x) ^ int_hash(p._y);
        }
    };
    struct equal {
        inline size_t operator()(const CoverTreePoint &p1, const CoverTreePoint &p2) const {
            return p1._x == p2._x && p1._y == p2._y;
        }
    };
    struct compare {
        inline bool operator()(const CoverTreePoint &p1, const CoverTreePoint &p2) const {
            if (p1._x != p2._x)
            {
                return (p1._x < p2._x);
            }
            return (p1._y < p2._y);
        }
    };
};

std::ostream &operator<<(std::ostream &os, const CoverTreePoint &p) {
    os << '(' << p._x << ',' << p._y << ')';
    return os;
}

#endif // COVER_TREE_POINT_H

