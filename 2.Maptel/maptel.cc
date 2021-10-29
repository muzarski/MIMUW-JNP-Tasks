#include <iostream>
#include <unordered_map>
#include <cstring>
#include <unordered_set>
#include <cassert>
#include "maptel.h"

using namespace std;

namespace {
#ifdef NDEBUG
    const bool debug = false;
#else
    const bool debug = true;
#endif

    using dictionary = unordered_map<string, string>;
    using map_of_dictionaries = unordered_map<size_t, dictionary>;

    // Size of the map that holds the dictionaries.
    size_t dicts_size = 0;

    // Preventing the static initialization order fiasco.
    map_of_dictionaries& dictionaries() {
        static map_of_dictionaries* dictionaries = new map_of_dictionaries();
        return *dictionaries;
    }

    // Follows the path of the consecutive number changes (basically DFS on the directed graph).
    // Returns whether the changes form a cycle.
    bool traverse(dictionary &current_dict, string &tel_src, string &tel_cur, char *tel_dst,
                  size_t len, unordered_set<string> &visited) {

        visited.insert(tel_cur);
        auto found = current_dict.find(tel_cur);

        // No number change for tel_cur
        if (found == current_dict.end()) {
            // '<' since \0 at the end
            assert(tel_cur.length() < len);
            strcpy(tel_dst, tel_cur.c_str());
            return false;
        }
        // A cycle found
        else if (visited.count(found->second)) {
            assert(tel_src.length() < len);
            strcpy(tel_dst, tel_src.c_str());
            return true;
        }
        // Keep traversing
        else {
            return traverse(current_dict, tel_src, found->second, tel_dst, len, visited);
        }
    }

    void check_number(char const *num) {
        if (debug) {
            size_t counter = 0;
            while(num[counter] != '\0') {
                assert(num[counter] >= 48 && num[counter] <= 57);
                counter++;
            }
            assert(counter > 0 && counter <= jnp1::TEL_NUM_MAX_LEN);
        }
    }

    /* ----- LOGGING FUNCTIONS ----- */

    void log(const string& function) {
        if (debug) {
            cerr << "maptel: " << function << "()" << '\n';
        }
    }

    void log(const string& function, const string& additional_info) {
        if (debug) {
            cerr << "maptel: " << function << additional_info << '\n';
        }
    }

    void log(const string& function, unsigned long arg0) {
        if (debug) {
            cerr << "maptel: " << function << "(" << arg0 << ")\n";
        }
    }

    void log(const string& function, unsigned long arg0, char const *arg1) {
        if (debug) {
            cerr << "maptel: " << function << "(" << arg0 << ", " << arg1 << ")\n";
        }
    }

    void log(const string& function, unsigned long arg0, char const *arg1, char const *arg2) {
        if (debug) {
            cerr << "maptel: " << function << "(" << arg0 << ", " << arg1 << ", " << arg2 << ")\n";
        }
    }

    void log(const string& function, unsigned long arg0, char const *arg1, char *arg2, size_t arg3) {
        if (debug) {
            cerr << "maptel: " << function << "(" << arg0 << ", " << arg1 << ", "
                 << static_cast<const void*>(arg2) << ", " << arg3 << ")\n";
        }
    }
}

unsigned long jnp1::maptel_create(void) {
    log(__FUNCTION__);
    dictionaries().emplace(dicts_size, dictionary());
    log(__FUNCTION__, ": new map id = " + to_string(dicts_size));

    return dicts_size++;
}

void jnp1::maptel_delete(unsigned long id) {
    log(__FUNCTION__, id);
    assert(dictionaries().count(id) != 0);

    dictionaries().erase(id);
    log(__FUNCTION__, ": map " + to_string(id) + " deleted");
}

void jnp1::maptel_insert(unsigned long id, char const *tel_src, char const *tel_dst) {
    assert(tel_src != NULL && tel_dst != NULL);
    log(__FUNCTION__, id, tel_src, tel_dst);
    assert(dictionaries().count(id) != 0);
    check_number(tel_src);
    check_number(tel_dst);

    dictionaries()[id][string(tel_src)] = string(tel_dst);
    log(__FUNCTION__, ": inserted");
}

void jnp1::maptel_erase(unsigned long id, char const *tel_src) {
    assert(tel_src != NULL);
    log(__FUNCTION__, id, tel_src);
    assert(dictionaries().count(id) != 0);
    check_number(tel_src);

    dictionary& dic = dictionaries()[id];
    auto el = dic.find(tel_src);

    if (el == dic.end()) {
        log(__FUNCTION__, ": nothing to erase");
    }
    else {
        dic.erase(tel_src);
        log(__FUNCTION__, ": erased");
    }
}

void jnp1::maptel_transform(unsigned long id, char const *tel_src, char *tel_dst, size_t len) {
    assert(tel_src != NULL && tel_dst != NULL);
    log(__FUNCTION__, id, tel_src, tel_dst, len);
    assert(dictionaries().count(id) != 0);
    check_number(tel_src);

    string src = string(tel_src);
    unordered_set<string> visited;
    bool hasCycle = traverse(dictionaries()[id], src, src, tel_dst, len, visited);
    if (hasCycle) log(__FUNCTION__, ": cycle detected");
    log(__FUNCTION__, ": " + src + " -> " + + tel_dst);
}
