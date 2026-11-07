#pragma once
#include <functional>
#include <queue>
#include <utility>
#include <vector>
namespace erl { template <typename T, typename Compare = std::less<T>> class PriorityDispatchQueue { public: void push(const T& v) { queue_.push(v); } void push(T&& v) { queue_.push(std::move(v)); } bool empty() const noexcept { return queue_.empty(); } std::size_t size() const noexcept { return queue_.size(); } T pop() { T v = queue_.top(); queue_.pop(); return v; } const T& top() const { return queue_.top(); } private: std::priority_queue<T, std::vector<T>, Compare> queue_; }; }

