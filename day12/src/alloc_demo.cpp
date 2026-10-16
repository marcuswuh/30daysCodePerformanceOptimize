#include "alloc_demo.hpp"

#include <array>
#include <memory_resource>

namespace perf12 {

std::size_t BuildListHeap(std::size_t n) {
  Node* head = nullptr;
  for (std::size_t i = 0; i < n; ++i) {
    auto* node = new Node{static_cast<int>(i), head};
    head = node;
  }
  std::size_t sum = 0;
  for (Node* p = head; p; p = p->next) sum += static_cast<std::size_t>(p->value);
  while (head) {
    Node* next = head->next;
    delete head;
    head = next;
  }
  return sum;
}

Node* NodePool::Acquire() {
  if (free_.empty()) {
    auto block = std::make_unique<Node[]>(kBlockSize);
    Node* base = block.get();
    blocks_.push_back(std::move(block));
    for (std::size_t i = 0; i < kBlockSize; ++i) free_.push_back(base + i);
  }
  Node* node = free_.back();
  free_.pop_back();
  return node;
}

void NodePool::Release(Node* node) { free_.push_back(node); }

void NodePool::Reset() {
  free_.clear();
  blocks_.clear();
}

std::size_t BuildListPool(NodePool& pool, std::size_t n) {
  Node* head = nullptr;
  for (std::size_t i = 0; i < n; ++i) {
    Node* node = pool.Acquire();
    node->value = static_cast<int>(i);
    node->next = head;
    head = node;
  }
  std::size_t sum = 0;
  for (Node* p = head; p; p = p->next) sum += static_cast<std::size_t>(p->value);
  while (head) {
    Node* next = head->next;
    pool.Release(head);
    head = next;
  }
  return sum;
}

std::size_t SumDefaultVectors(std::size_t count, std::size_t elems_each) {
  std::size_t total = 0;
  for (std::size_t i = 0; i < count; ++i) {
    std::vector<int> v(elems_each, static_cast<int>(i));
    total += static_cast<std::size_t>(v.back());
  }
  return total;
}

std::size_t SumPmrVectors(std::size_t count, std::size_t elems_each) {
  std::array<std::byte, 1 << 20> buffer{};
  std::pmr::monotonic_buffer_resource pool{buffer.data(), buffer.size()};
  std::pmr::polymorphic_allocator<int> alloc{&pool};

  std::size_t total = 0;
  for (std::size_t i = 0; i < count; ++i) {
    std::pmr::vector<int> v(elems_each, static_cast<int>(i), alloc);
    total += static_cast<std::size_t>(v.back());
  }
  return total;
}

}  // namespace perf12
