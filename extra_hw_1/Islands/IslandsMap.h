#ifndef ISLANDSMAP_H
#define ISLANDSMAP_H

#include <unordered_set>
#include "CoverTree/CoverTree.h"
#include "CoverTree/CoverTreePoint.h"

class IslandsMap {
public:
    IslandsMap(CoverTreePoint coords[], float treasure[], int faction[], int island_count){
        _coords = coords;
        _treasure = treasure;
        _faction = faction;

        _island_count =  island_count;
        _faction_count = get_n_factions();

        // build a cover tree from inhabited islands to speed up neighbour look-up
        // credits: https://github.com/DNCrane/Cover-Tree
        for (int i = 0; i < _island_count; ++i) {
            if (faction[i] != -1)
            {
                coords[i].set_faction_id(faction[i]);
                _inhabitedCTree.insert(coords[i]);
            }
        }

        // calculate some results to cache
        _get_controlling_faction_ids(_controlling_faction_ids);

        // build a cover tree from inhabited and uninhabited islands to speed up neighbour look-up
        for (int i = 0; i < _island_count; ++i) {
            if (faction[i] != -1)
            {
                coords[i].set_faction_id(faction[i]);
                _allCTree.insert(coords[i]);
            } else {
                coords[i].set_faction_id(_controlling_faction_ids[i]);
                _allCTree.insert(coords[i]);
            }
        }
    }

    int get_n_factions() {
        int faction_ct = -1;
        for (int i = 0; i < _island_count; ++i) {
            faction_ct = std::max(faction_ct, _faction[i]);
        }
        faction_ct += 1;  // to get the count, not the max id

        return faction_ct;
    }

    int get_faction_with_most_colonies() {
        int *faction_colony_counts = new int[_faction_count]();
        for (int i = 0; i < _island_count; ++i) {
            if (_faction[i] != -1)
                faction_colony_counts[_faction[i]]++;
        }

        int max = faction_colony_counts[0], max_ind = 0;
        for (int i = 0; i < _faction_count; ++i) {
            if (faction_colony_counts[i] > max) {
                max = faction_colony_counts[i];
                max_ind = i;
            }
        }

        delete[] faction_colony_counts;

        return max_ind;
    }

    void get_controlling_faction_ids(std::vector<int> &result) {
        result = _controlling_faction_ids;
    }

    void get_faction_treasury(std::vector<float> &result) {
        result.resize(_faction_count);
        std::fill(result.begin(), result.begin() + _faction_count, 0);


        for (int i = 0; i < _island_count; ++i) {
            result[_controlling_faction_ids[i]] += _treasure[i];
        }
    }

    void get_factions_with_rival_neighbors(std::vector<int> &result) {
        bool *factions_with_rival = new bool[_faction_count](); // bool vector that shows, whether a given faction has a rival

        for (int i = 0; i < _island_count; ++i) {
            int faction_id = _controlling_faction_ids[i];
            if (!factions_with_rival[faction_id]) {
                // get the 5 closest points
                // (5 because the tree contains the searched value, which obviously has minimal dist. + we have to check 4 neighbours)
                for (CoverTreePoint pt : _allCTree.kNearestNeighbors(_coords[i], 5)) {
                    if (pt.get_faction_id() != faction_id && pt.distance(_coords[i]) <= 1) {
                        factions_with_rival[faction_id] = true;
                        result.push_back(faction_id);
                    }
                }
            }
        }

        delete[] factions_with_rival;
    }

    void get_big_treasure_coords(std::vector<CoverTreePoint> &result) {
        // build an ordered set of island coords, so it can speed up lookup for inhabited islands, when checking neighbours
        std::set<CoverTreePoint, CoverTreePoint::compare> coord_set;
        std::multiset<CoverTreePoint, CoverTreePoint::compare> neighbour_set;

        // add possible treasure coords (neighbours of existing islands)
        for (int i = 0; i < _island_count; ++i) {
            coord_set.insert(_coords[i]);

            int x = _coords[i].get_x(), y = _coords[i].get_y();
            neighbour_set.insert(CoverTreePoint(x, y + 1));
            neighbour_set.insert(CoverTreePoint(x + 1, y));
            neighbour_set.insert(CoverTreePoint(x, y - 1));
            neighbour_set.insert(CoverTreePoint(x - 1, y));
        }

        auto coord_iter = coord_set.begin();
        auto neighbour_iter = neighbour_set.begin();

        // similar to merging 2 sorted arrays
        while (neighbour_iter != neighbour_set.end()) {
            if (coord_iter == coord_set.end() || *coord_iter > *neighbour_iter) {
                CoverTreePoint prev;
                int ct = 0;
                do {
                    ++ct;
                    prev = *neighbour_iter++;
                } while (neighbour_iter != neighbour_set.end() && prev == *neighbour_iter);

                if (ct >= 3)
                    result.push_back(prev);
            } else if (*coord_iter == *neighbour_iter) {
                coord_iter++;

                CoverTreePoint prev;
                do {
                    prev = *neighbour_iter++;
                } while (neighbour_iter != neighbour_set.end() && prev == *neighbour_iter);
            } else {
                ++coord_iter;
            }
        }
    }

    void get_big_treasure_coords2(std::vector<CoverTreePoint> &result) {
        // second version: use unordered_multiset.
        // For each coordinate, that is the neighbour of at least 3 island (the count can be determined in O(1) time, since max. neighbour count is 4. In worst case it can be determined in O(num_of_poss_coords))
        // find out whether it is inhabited (lookup in ordered set).
        std::set<CoverTreePoint, CoverTreePoint::compare> coord_set;
        std::unordered_multiset<CoverTreePoint, CoverTreePoint::hash, CoverTreePoint::equal> neighbour_set;

        // we need to store the results in a set, so duplicates are avoided
        std::set<CoverTreePoint, CoverTreePoint::compare> result_set;

        // add possible treasure coords (neighbours of existing islands)
        for (int i = 0; i < _island_count; ++i) {
            coord_set.insert(_coords[i]);

            int x = _coords[i].get_x(), y = _coords[i].get_y();
            neighbour_set.insert(CoverTreePoint(x, y + 1));
            neighbour_set.insert(CoverTreePoint(x + 1, y));
            neighbour_set.insert(CoverTreePoint(x, y - 1));
            neighbour_set.insert(CoverTreePoint(x - 1, y));
        }

        for (auto v : neighbour_set) {
            if (neighbour_set.count(v) >= 3){
                // if it is uninhabited
                if (coord_set.find(v) == coord_set.end()) {
                    result_set.insert(v);
                }
            }
        }

        // load result_et into vector
        std::copy(result_set.begin(), result_set.end(), std::back_inserter(result));
    }


private:
    CoverTreePoint *_coords;
    float *_treasure;
    int *_faction;

    int _island_count;
    int _faction_count;

    // helper data structures, to speed up some functions
    CoverTree<CoverTreePoint> _inhabitedCTree;
    CoverTree<CoverTreePoint> _allCTree;

    // some cached results
    std::vector<int> _controlling_faction_ids;

    // some functions on which the implementation also relies (in the ctor), so we cache their results, and return that the next call
    void _get_controlling_faction_ids(std::vector<int> &result) {
        result.resize(_island_count);
        for (int i = 0; i < _island_count; ++i) {
            if (_faction[i] != -1)
                result[i] = _faction[i];
            else {
                int minFactionId = _island_count;
                // it could potentially amortise the runtime to O(n^2)
                for (CoverTreePoint pt : _inhabitedCTree.kNearestNeighbors(_coords[i], 1)) {
                    if (pt.get_faction_id() < minFactionId) {
                        minFactionId = pt.get_faction_id();
                    }

                    result[i] = minFactionId;
                }
            }
        }
    }
};



#endif //ISLANDSMAP_H
