#include <iostream>
#include <unordered_set>
#include <chrono>
#include "CoverTree/CoverTree.h"
#include "CoverTree/CoverTreePoint.h"
#include "IslandsMap.h"

#include "SampleData.h"


template<typename T>
void array_print(size_t len, T arr, bool end = true) {
    std::cout << '[';
    for (int i = 0; i < len; ++i) {
        std::cout << arr[i];
        if (i != len - 1)
            std::cout << ", ";
    }
    std::cout << ']';

    if (end) std::cout << std::endl;
}

void test_all(CoverTreePoint coords[], float treasure[], int faction[], int island_ct, bool show_results = true, const std::string& test_name = "") {
    auto time0 = std::chrono::steady_clock::now();

    IslandsMap map =  IslandsMap(coords, treasure, faction, island_ct);
    auto time1 = std::chrono::steady_clock::now();

    // 1.
    int faction_ct = map.get_n_factions();
    if (show_results) std::cout << faction_ct << std::endl;
    auto time2 = std::chrono::steady_clock::now();

    // 2.
    int faction_with_most = map.get_faction_with_most_colonies();
    if (show_results) std::cout << faction_with_most << std::endl;
    auto time3 = std::chrono::steady_clock::now();

    // 3.
    std::vector<int> controlling_factions;
    map.get_controlling_faction_ids(controlling_factions);
    if (show_results) array_print(controlling_factions.size(), controlling_factions);
    auto time4 = std::chrono::steady_clock::now();

    // 4.
    std::vector<float> faction_treasury;
    map.get_faction_treasury(faction_treasury);
    if (show_results) array_print(faction_treasury.size(), faction_treasury);
    auto time5 = std::chrono::steady_clock::now();

    // 5.
    std::vector<int> factions_with_rivals;
    map.get_factions_with_rival_neighbors(factions_with_rivals);
    if (show_results) array_print(factions_with_rivals.size(), factions_with_rivals);
    auto time6 = std::chrono::steady_clock::now();

    // 6.
    std::vector<CoverTreePoint> big_treasure_coords;
    map.get_big_treasure_coords(big_treasure_coords);
    if (show_results) array_print(big_treasure_coords.size(), big_treasure_coords);
    auto time7 = std::chrono::steady_clock::now();

    // 6. second version
    std::vector<CoverTreePoint> big_treasure_coords2;
    map.get_big_treasure_coords2(big_treasure_coords2);
    if (show_results) array_print(big_treasure_coords2.size(), big_treasure_coords2);
    auto time8 = std::chrono::steady_clock::now();

    if (!test_name.empty())
        std::cout << std::endl << test_name << std::endl;
    std::cout << "Timing test successful!" << std::endl
              << "    ctor:                              " << (std::chrono::duration_cast<std::chrono::microseconds>(time1 - time0).count())/1000000.0 << "s" << std::endl
              << "    faction count:                     " << (std::chrono::duration_cast<std::chrono::microseconds>(time2 - time1).count())/1000000.0 << "s" << std::endl
              << "    most colonies:                     " << (std::chrono::duration_cast<std::chrono::microseconds>(time3 - time2).count())/1000000.0 << "s" << std::endl
              << "    controlling faction:               " << (std::chrono::duration_cast<std::chrono::microseconds>(time4 - time3).count())/1000000.0 << "s" << std::endl
              << "    faction treasury:                  " << (std::chrono::duration_cast<std::chrono::microseconds>(time5 - time4).count())/1000000.0 << "s" << std::endl
              << "    rival neighbors:                   " << (std::chrono::duration_cast<std::chrono::microseconds>(time6 - time5).count())/1000000.0 << "s" << std::endl
              << "    big treasure:                      " << (std::chrono::duration_cast<std::chrono::microseconds>(time7 - time6).count())/1000000.0 << "s" << std::endl
              << "    big treasure2:                     " << (std::chrono::duration_cast<std::chrono::microseconds>(time8 - time7).count())/1000000.0 << "s" << std::endl
              << "    ============================================" << std::endl
              << "    TOTAL TIME:                        " << (std::chrono::duration_cast<std::chrono::microseconds>(time8 - time0).count())/1000000.0 << "s" << std::endl
              << "    TOTAL TIME without big treasure2 : " << (std::chrono::duration_cast<std::chrono::microseconds>(time7 - time0).count())/1000000.0 << "s" << std::endl;


}

int main() {
    CoverTreePoint coords[] = {CoverTreePoint(0, 0),
                               CoverTreePoint(0, 3),
                               CoverTreePoint(0, 5),
                               CoverTreePoint(0, 6),
                               CoverTreePoint(1, 2),
                               CoverTreePoint(1, 4),
                               CoverTreePoint(1, 5),
                               CoverTreePoint(2, 0),
                               CoverTreePoint(2, 3),
                               CoverTreePoint(2, 5),
                               CoverTreePoint(2, 9),
                               CoverTreePoint(3, 3),
                               CoverTreePoint(3, 5),
                               CoverTreePoint(3, 6),
                               CoverTreePoint(4, 3)};
    float treasure[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
    int faction[] = {0, -1, -1, 1, -1, -1, 2, 1, -1, -1, 4, -1, -1, 2, 3};
    int island_ct = sizeof(coords) / sizeof(coords[0]);

    /* default exercise */
    // test_all(coords, treasure, faction, island_ct);

    /* benchmarking */
    test_all(coords_test_1, treasures_test_1, factions_test_1, island_ct_test_1, false, test_1_name);
    test_all(coords_test_2, treasures_test_2, factions_test_2, island_ct_test_2, false, test_2_name);
    test_all(coords_test_3, treasures_test_3, factions_test_3, island_ct_test_3, false, test_3_name);


    /* A levont következtetések:
     * A lényegesen jobb eredmények mögött elsősorban a hatékony adatszerkezetek álnak, míg a numpy-os megoldásomban
     * a feladatok többsége lineáris keresésen alapul (összehasonlítások...), itt használunk adatokra vonatkozó heurisztikát,
     * */

    return 0;
}
