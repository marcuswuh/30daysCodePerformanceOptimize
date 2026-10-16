#include <iostream>
#include <string>

int main() {
  for (std::size_t len : {7, 15, 22, 32, 64}) {
    const std::string s(len, 'z');
    std::cout << "len=" << len << " capacity=" << s.capacity()
              << " data=" << static_cast<const void*>(s.data()) << '\n';
  }
  std::cout << "Use heaptrack on bench_strings to see alloc differences (long lines).\n";
  return 0;
}
