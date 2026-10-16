#pragma once

#include <cstddef>
#include <memory>
#include <vector>

namespace perf12 {

struct Node {
  int value;
  Node* next;
};

// 每帧 new/delete 链表节点
std::size_t BuildListHeap(std::size_t n);

// 对象池复用节点
class NodePool {
 public:
  Node* Acquire();
  void Release(Node* node);
  void Reset();

 private:
  std::vector<std::unique_ptr<Node[]>> blocks_;
  std::vector<Node*> free_;
  static constexpr std::size_t kBlockSize = 4096;
};

std::size_t BuildListPool(NodePool& pool, std::size_t n);

// std::pmr 单调缓冲区：批量小 vector 分配
std::size_t SumPmrVectors(std::size_t count, std::size_t elems_each);

std::size_t SumDefaultVectors(std::size_t count, std::size_t elems_each);

}  // namespace perf12
