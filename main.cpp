#include <iostream>
#include "include/queue.hpp"

int main() {
    Queue<int> queue(4);
    for (int i = 0; i < 3; ++i) {
        std::cout << queue.Full() << " ";
        queue.EnQueue(i);
        std::cout << queue.GetHead() << '\n';
    }
    std::cout << queue.Full() << "\n";
    queue.Modify(3, 1);
    for (int i = 0; i < 3; ++i) {
        std::cout << queue.GetHead() << '\n';
        queue.DeQueue();
    }
}