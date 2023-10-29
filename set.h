#pragma once

#include <cstdlib> 

#include <iostream>

using namespace std;

namespace main_savitch_11
{
    template <class Item>
    class set
    {
    public:
        // TYPEDEFS
        typedef Item value_type;
        // CONSTRUCTORS and DESTRUCTOR
        set();
        set(const set& source);
        ~set() { clear(); }
        // MODIFICATION MEMBER FUNCTIONS
        void operator =(const set& source);
        void clear();
        bool insert(const Item& entry);
        std::size_t erase(const Item& target);
        // CONSTANT MEMBER FUNCTIONS
        std::size_t count(const Item& target) const;
        bool empty() const { return (data_count == 0); }
        // SUGGESTED FUNCTION FOR DEBUGGING
        void print(int indent = 0) const;
    private:
        // MEMBER CONSTANTS
        static const std::size_t MINIMUM = 1;
        static const std::size_t MAXIMUM = 2 * MINIMUM;
        // MEMBER VARIABLES
        std::size_t data_count;
        Item data[MAXIMUM + 1];
        std::size_t child_count;
        set* subset[MAXIMUM + 2];
        // HELPER MEMBER FUNCTIONS
        bool is_leaf() const { return (child_count == 0); }
        bool loose_insert(const Item& entry);
        bool loose_erase(const Item& target);
        void remove_biggest(Item& removed_entry);
        void fix_excess(std::size_t i);
        void fix_shortage(std::size_t i);
        // NOTE: The implementor may want to have additional helper functions
    };

    template <class T>
    set<T>::set() {
        data_count = 0;
        child_count = 0;
    }

    template <class T>
    set<T>::set(const set<T>& other) {
        this->data_count = other.data_count;
        this->child_count = other.child_count;
        for (int i = 0; i < data_count; ++i) {
            this->data[i] = other.data[i];
        }

        if (!is_leaf()) {
            for (int i = 0; i < child_count; ++i) {
                this->subset[i] = new set<T>(*other.subset[i]);
            }
        }
    }

    template <class T>
    void set<T>::operator=(const set<T>& other) {
        clear();
        this->data_count = other.data_count;
        this->child_count = other.child_count;
        for (int i = 0; i < data_count; ++i) {
            this->data[i] = other.data[i];
        }

        if (!is_leaf()) {
            for (int i = 0; i < child_count; ++i) {
                this->subset[i] = new set<T>(*other.subset[i]);
            }
        }
    }

    template <class T>
    void set<T>::clear() {
        if (this == nullptr) return;
        

        for (int i = 0; i < child_count; ++i) {
            delete subset[i];
            subset[i] = nullptr;
        }
    }

    template <class Item>
    bool set<Item>::insert(const Item& entry) {
        if (!loose_insert(entry)) return false;

        if (data_count > MAXIMUM) {
            set<Item>* _subset = new set<Item>(*this);
            child_count = 1;
            data_count = 0;
            subset[0] = _subset;
            fix_excess(0);
        }
        
        return true;
    }

    template <class Item>
    std::size_t set<Item>::erase(const Item& entry) {
        if (!loose_erase(entry)) return false;

        if (data_count == 0 && child_count == 1) {
            auto ref = subset[0];
            child_count = 0;
            operator=(*ref);
            ref->child_count = 0;
            delete ref;
        }

        return true;
    }

    template <class Item>
    std::size_t set<Item>::count(const Item& entry) const {
        int count = 0;
        for (int i = 0; i < data_count; ++i) {
            if (data[i] == entry) ++count;
        }
        if (!is_leaf()) {
            for (int i = 0; i < data_count + 1; ++i)
                count += subset[i]->count(entry);
        }
        return count;
    }

    template <class Item>
    void set<Item>::print(int indent) const {
        for (int i = 0; i < indent; ++i)
            cout << "\t";

        for (int i = 0; i < data_count; ++i) {
            cout << data[i] << " ";
        }
        cout << endl;
        for (int i = 0; i < child_count; ++i) {
            subset[i]->print(indent + 1);
        }
    }

    template <class Item>
    bool set<Item>::loose_insert(const Item& entry) {
        if (!is_leaf()) {
            for (int i = 0; i < data_count; ++i) {
                if (entry == data[i]) return false;
                else if (entry < data[i]) {
                    bool inserted = subset[i]->loose_insert(entry);
                    if (inserted && subset[i]->data_count > MAXIMUM) fix_excess(i);
                    return inserted;
                }
            }

            bool inserted = subset[data_count]->loose_insert(entry);
            if (inserted && subset[data_count]->data_count > MAXIMUM) fix_excess(data_count);
            return inserted;
        }
        else {
            for (int i = 0; i < data_count; ++i) {
                if (entry == data[i]) return false;
                else if (entry < data[i]) {
                    for (int j = data_count; j > i; --j) {
                        data[j] = data[j - 1];
                    }
                    data[i] = entry;
                    data_count++;
                    return true;
                }
            }
            data[data_count] = entry;
            data_count++;
            return true;
        }
    }

    template <class Item>
    bool set<Item>::loose_erase(const Item& entry) {
        if (!is_leaf()) {
            for (int i = 0; i < data_count; ++i) {
                if (entry == data[i]) {
                    subset[i]->remove_biggest(data[i]);
                    if (subset[i]->data_count < MINIMUM) fix_shortage(i);
                    return true;
                }
                else if (entry < data[i]) {
                    bool erased = subset[i]->loose_erase(entry);
                    if (erased && subset[i]->data_count < MINIMUM) fix_shortage(i);
                    return erased;
                }
            }
            bool erased = subset[data_count]->loose_erase(entry);
            if (erased && subset[data_count]->data_count < MINIMUM) fix_shortage(data_count);
            return erased;
        }
        else {
            for (int i = 0; i < data_count; ++i) {
                if (entry == data[i]) {
                    for (int j = i; j < data_count - 1; ++j) {
                        data[j] = data[j + 1];
                    }
                    data_count--;
                    return true;
                }
            }
            return false;
        }
    }

    template <class Item>
    void set<Item>::remove_biggest(Item& removed_entry) {
        if (!is_leaf()) {
            subset[data_count]->remove_biggest(removed_entry); 
            if (subset[data_count]->data_count < MINIMUM) fix_shortage(data_count);
        }
        else {
            data_count--;
            removed_entry = data[data_count];
        }
    }

    template <class Item>
    void set<Item>::fix_excess(std::size_t i) {
        set<Item> *sub = subset[i];

        for (int j = data_count; j > i; --j) {
            data[j] = data[j - 1];
        }
        data[i] = sub->data[MINIMUM];
        data_count++;

        set<Item>* new_subset = new set<Item>();

        for (int i = 0, j = MINIMUM + 1; j < sub->data_count; ++j, ++i) {
            new_subset->data[i] = sub->data[j];
        }
        sub->data_count = MINIMUM;
        new_subset->data_count = MINIMUM;
        
        if (!sub->is_leaf()) {
            for (int i = 0, j = MINIMUM + 1; j < sub->child_count; ++j, ++i) {
                new_subset->subset[i] = sub->subset[j];
            }
            sub->child_count = MINIMUM + 1;
            new_subset->child_count = MINIMUM + 1;
           
        }

        

        for (int j = child_count; j > i + 1; --j) {
            subset[j] = subset[j - 1];
        }
        subset[i + 1] = new_subset;
        child_count++;
    }

    template <class Item>
    void set<Item>::fix_shortage(std::size_t i) {
        auto sub = subset[i];
        if (i > 0 && subset[i - 1]->data_count > MINIMUM) {
            auto lsub = subset[i - 1];
            for (int i = sub->data_count; i > 0; --i) {
                sub->data[i] = sub->data[i - 1];
            }
            sub->data_count++;
            sub->data[0] = data[i - 1];
            data[i - 1] = lsub->data[lsub->data_count - 1];
            lsub->data_count--;
            if (!lsub->is_leaf()) {
                for (int i = sub->child_count; i > 0; --i) {
                    sub->subset[i] = sub->subset[i - 1];
                }
                lsub->child_count--;
                sub->subset[0] = lsub->subset[lsub->child_count];
                sub->child_count++;
            }
        }
        else if (i + 1 < child_count && subset[i + 1]->data_count > MINIMUM){
            auto rsub = subset[i + 1];

            sub->data_count++;
            sub->data[sub->data_count - 1] = data[i];

            data[i] = rsub->data[0];
            rsub->data_count--;
            for (int i = 0; i < rsub->data_count; ++i) {
                rsub->data[i] = rsub->data[i + 1];
            }
            if (!rsub->is_leaf()) {
                sub->subset[sub->child_count] = rsub->subset[0];
                
                sub->child_count++;
                rsub->child_count--;

                for (int i = 0; i < rsub->child_count; ++i) {
                    rsub->subset[i] = rsub->subset[i + 1];
                }
            }
        }
        else {
            set<Item>* lsub;
            if (i > 0 && subset[i - 1]->data_count == MINIMUM) {
                lsub = subset[i - 1];
            }
            else {
                lsub = subset[i];
                sub = subset[i + 1];
                ++i;
            }
            lsub->data[lsub->data_count] = data[i - 1];
            lsub->data_count++;

            data_count--;
            for (int j = i - 1; j < data_count; ++j) {
                data[j] = data[j + 1];
            }

            child_count--;
            for (int j = i; j < child_count; ++j) {
                subset[j] = subset[j + 1];
            }
            for (size_t i = 0, &j = lsub->data_count; i < sub->data_count; ++i, ++j) {
                lsub->data[j] = sub->data[i];
            }
            for (size_t i = 0, &j = lsub->child_count; i < sub->child_count; ++i, ++j) {
                lsub->subset[j] = sub->subset[i];
            }

            sub->child_count = 0;
            delete sub;
            return;
            
        }
    }
}