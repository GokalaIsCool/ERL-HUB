#pragma once

#include <queue>
#include <vector>

namespace erl {

template <typename T, typename Compare>
class PriorityDispatchQueue {
public:
    void push(const T& value);
    bool empty() const noexcept;
    std::size_t size() const noexcept;
    T pop();

private:
    std::priority_queue<T, std::vector<T>, Compare> queue_;
};

} // namespace erl

