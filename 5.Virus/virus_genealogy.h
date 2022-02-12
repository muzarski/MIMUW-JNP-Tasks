#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H

#include <map>
#include <set>
#include <vector>
#include <memory>
#include <iostream>
#include <iterator>
#include <iostream>

class VirusNotFound : public std::runtime_error {
public:
    VirusNotFound() : std::runtime_error("VirusNotFound") {};
};

class VirusAlreadyCreated : public std::logic_error {
public:
    VirusAlreadyCreated() : std::logic_error("VirusAlreadyCreated") {};
};

class TriedToRemoveStemVirus : public std::logic_error {
public:
    TriedToRemoveStemVirus() : std::logic_error("TriedToRemoveStemVirus") {};
};

template<typename Virus>
class VirusGenealogy {
private:
    using id_type = typename Virus::id_type;

    struct Node {
        using set_t = std::set<std::shared_ptr<Node>>;

        std::shared_ptr<Virus> virus;
        set_t parents, children;

        Node(id_type id) {
            virus = std::make_shared<Virus>(id);
        };
    };

    using node_ptr = std::shared_ptr<Node>;
    using map_t = std::map<id_type, node_ptr>;

    id_type stem_id;
    map_t viruses;

    class ChildrenIterator {
    private:
        typename std::set<node_ptr>::const_iterator it;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = Virus;
        using pointer = std::shared_ptr<value_type>;
        using reference = const value_type &;

        ChildrenIterator() = default;
        ChildrenIterator(const ChildrenIterator &other) = default;
        ChildrenIterator(ChildrenIterator &&other) noexcept = default;
        ChildrenIterator(decltype(it) &&iter) : it(iter) {}

        ChildrenIterator &operator=(const ChildrenIterator &other) = default;
        ChildrenIterator &operator=(ChildrenIterator &&other) noexcept = default;

        reference operator*() const {
            return *((*it)->virus);
        }

        pointer operator->() const {
            return (*it)->virus;
        }

        ChildrenIterator &operator++() {
            it++;
            return *this;
        }

        ChildrenIterator operator++(int) {
            ChildrenIterator result(*this);
            operator++();
            return result;
        }

        ChildrenIterator &operator--() {
            it--;
            return *this;
        }

        ChildrenIterator operator--(int) {
            ChildrenIterator result(*this);
            operator--();
            return result;
        }

        bool operator==(const ChildrenIterator &other) const { return it == other.it; };
        bool operator!=(const ChildrenIterator &other) const { return it != other.it; };
    };

    class InsertGuard {
    public:
        InsertGuard(typename Node::set_t* ptrSet, node_ptr& v) : set_ptr(ptrSet), rollback(false) {
            it = set_ptr->emplace(v).first;
            rollback = true;
        }
        InsertGuard(InsertGuard const &) = delete;
        InsertGuard & operator=(InsertGuard const &) = delete;

        ~InsertGuard() noexcept {
            if (rollback)
                set_ptr->erase(it);
        }

        void dropRollback() noexcept {
            rollback = false;
        }

    private:
        typename Node::set_t *set_ptr;
        typename Node::set_t::iterator it;
        bool rollback;
    };


    using to_erase_t = std::vector<std::pair<typename Node::set_t *, typename Node::set_t::iterator>>;

    void remove_rec(id_type const &id, node_ptr &parent, map_t &viruses_copy,
                    to_erase_t &to_erase, std::map<node_ptr, int> &parents_count) {

        auto pair_iter = viruses_copy.find(id);
        node_ptr curr_node = pair_iter->second;
        to_erase.emplace_back(&curr_node->parents, curr_node->parents.find(parent));

        if (parents_count.contains(curr_node))
            parents_count[curr_node]--;
        else
            parents_count[curr_node] = curr_node->parents.size() - 1;

        if (parents_count[curr_node] == 0) {
            viruses_copy.erase(pair_iter);

            for (auto &par : curr_node->parents)
                to_erase.emplace_back(&par->children, par->children.find(curr_node));

            for (auto &child : curr_node->children)
                remove_rec(child->virus->get_id(), curr_node, viruses_copy, to_erase, parents_count);
        }
    }

    const node_ptr& get_node_ptr(id_type const &id) const {
        auto it = viruses.find(id);

        if (it == viruses.end())
            throw VirusNotFound();

        return it->second;
    }

public:
    using children_iterator = ChildrenIterator;

    VirusGenealogy(id_type const &stem_id) : stem_id(stem_id) {
        node_ptr node = std::make_shared<Node>(stem_id);
        viruses.emplace(stem_id, node);
    }

    VirusGenealogy(const VirusGenealogy &other) = delete;

    ~VirusGenealogy() {
        for (auto &node : viruses) {
            node.second->parents.clear();
            node.second->children.clear();
        }

        viruses.clear();
    }

    VirusGenealogy operator=(const VirusGenealogy &other) = delete;

    id_type get_stem_id() const {
        return stem_id;
    }

    bool exists(id_type const &id) const {
        return (viruses.contains(id));
    }

    VirusGenealogy<Virus>::children_iterator get_children_begin(id_type const &id) const {
        return ChildrenIterator(get_node_ptr(id)->children.begin());
    }

    VirusGenealogy<Virus>::children_iterator get_children_end(id_type const &id) const {
        return ChildrenIterator(get_node_ptr(id)->children.end());
    }

    std::vector<id_type> get_parents(id_type const &id) const {
        typename Node::set_t node_parents = get_node_ptr(id)->parents;
        std::vector<id_type> res;

        for (auto &parent : node_parents) {
            id_type parent_id;
            parent_id = parent->virus->get_id();
            res.push_back(parent_id);
        }

        return res;
    }

    const Virus &operator[](id_type const &id) const {
        return *(get_node_ptr(id)->virus);
    };

    void create(id_type const &id, id_type const &parent_id) {
        if (exists(id))
            throw VirusAlreadyCreated();
        if (!exists(parent_id))
            throw VirusNotFound();

        node_ptr new_node_ptr = std::make_shared<Node>(id);
        node_ptr parent_ptr = get_node_ptr(parent_id);

        new_node_ptr->parents.emplace(parent_ptr);
        InsertGuard guard(&(parent_ptr->children), new_node_ptr);
        viruses.emplace(id, new_node_ptr).first;

        guard.dropRollback();
    }

    void create(id_type const &id, std::vector<id_type> const &parent_ids) {
        if (exists(id))
            throw VirusAlreadyCreated();

        for (auto &parent : parent_ids)
            if (!exists(parent))
                throw VirusNotFound();

        if (parent_ids.size() == 0)
            return;

        std::set<id_type> parent_ids_set(parent_ids.begin(), parent_ids.end());
        std::vector<std::shared_ptr<InsertGuard>> insert_guards;
        node_ptr new_node_ptr = std::make_shared<Node>(id);

        for (auto &parent_id: parent_ids_set) {
            node_ptr parent_ptr = get_node_ptr(parent_id);
            new_node_ptr->parents.emplace(parent_ptr);
            std::shared_ptr<InsertGuard> temp_guard = std::make_shared<InsertGuard>(&(parent_ptr->children), new_node_ptr);
            insert_guards.emplace_back(temp_guard);
        }

        viruses.emplace(id, new_node_ptr);

        for (auto &guard: insert_guards) {
            guard->dropRollback();
        }
    }

    void connect(id_type const &child_id, id_type const &parent_id) {
        node_ptr parent = get_node_ptr(parent_id);
        node_ptr child = get_node_ptr(child_id);

        if (parent->children.contains(child)) return;

        InsertGuard guard(&(parent->children), child);
        child->parents.emplace(parent);

        guard.dropRollback();
    }

    void remove(id_type const &id) {
        if (!exists(id))
            throw VirusNotFound();
        if (id == stem_id)
            throw TriedToRemoveStemVirus();

        map_t viruses_copy(viruses);
        to_erase_t to_erase;
        std::map<node_ptr, int> parents_count;

        auto pair_iter = viruses_copy.find(id);
        node_ptr curr_node = pair_iter->second;
        viruses_copy.erase(pair_iter);

        for (auto &par : curr_node->parents)
            to_erase.emplace_back(&par->children, par->children.find(curr_node));

        for (auto &child : curr_node->children)
            remove_rec(child->virus->get_id(), curr_node, viruses_copy, to_erase, parents_count);

        for (auto &erase_pair: to_erase)
            erase_pair.first->erase(erase_pair.second);

        viruses.swap(viruses_copy);
    }
};

#endif // VIRUS_GENEALOGY_H