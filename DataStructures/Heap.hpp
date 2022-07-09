#pragma once 

#include <map>
#include <vector>

#include <stdexcept>

namespace parsiss
{

template <typename key_t, typename value_t, typename compare>
class Heap
{
public:
    Heap() {}
    ~Heap() {}

    void push(const key_t &key, const value_t &value)
    {
        map[key] = data.size();
        data.push_back(std::make_pair(key, value));
        bubble_up(data.size() - 1);
    }

    void update(const key_t &key, const value_t &value)
    {
        auto current_value = data[map[key]].second;
        data[map[key]].second = value;
        if(compare()(current_value, value)) {                
            bubble_down(map[key]);
        } else {
            bubble_up(map[key]);
        }
    }

    std::pair<key_t, value_t> top() const
    {
        if(data.empty()) {
            throw std::runtime_error("Heap is empty");
        }
        return data[0];
    }

    std::pair<key_t, value_t> pop()
    {
        if(data.empty()) {
            throw std::runtime_error("Heap is empty");
        }

        std::pair<key_t, value_t> root = data[0];
        swap(0, data.size() - 1);
        data.pop_back();
        bubble_down(0);
        return root;
    }

    value_t &operator[](const key_t &key)
    {
        return data.empty();
    }

    
    bool empty() const
    {
        return data.empty();
    }

    int size() const
    {
        return data.size();
    }

private:
    void bubble_up(int index)
    {
        if(index == 0) {
            return;
        }
        int parent = (index - 1) / 2;
        if(compare()(data[index].second, data[parent].second)) {
            swap(index, parent);
            bubble_up(parent);
        }
    }

    void bubble_down(int index)
    {
        int left = index * 2 + 1;
        int right = index * 2 + 2;
        int largest = index;
        if(left < data.size() && compare()(data[left].second, data[largest].second)) {
            largest = left;
        }
        if(right < data.size() && compare()(data[right].second, data[largest].second)) {
            largest = right;
        }
        if(largest != index) {
            swap(index, largest);
            bubble_down(largest);
        }
    }

    void swap(int i, int j)
    {
        std::swap(data[i], data[j]);
        map[data[i].first] = i;
        map[data[j].first] = j;
    }

private:
    std::map<key_t, int> map;
    std::vector<std::pair<key_t, value_t>> data;
};

template <typename key_t, typename value_t>
using MinHeap = Heap<key_t, value_t, std::less<value_t>>;

template <typename key_t, typename value_t>
using MaxHeap = Heap<key_t, value_t, std::greater<value_t>>;

}

