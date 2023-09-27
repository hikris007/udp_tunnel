#include <iostream>
#include <unordered_set>

int main() {
    std::unordered_set<int> s;

    // 向集合中添加元素
    s.insert(3);
    s.insert(1);
    s.insert(2);
    s.insert(3); // 重复元素，不会被添加到集合中

    // 输出集合中的元素
    for(int elem : s) {
        std::cout << elem << " ";
    }
    // 输出顺序可能会变化，因为 unordered_set 是无序的

    return 0;
}