#include <iostream>
#include <vector>
#include <chrono>

// Example class representing a small object (e.g., a game particle).
struct Particle {
    float x, y, z;
};

// Custom memory pool for Particle objects.
template <size_t PoolSize>
class ParticlePool {
public:
    ParticlePool() {
    // Allocate a large block for PoolSize nodes.
    nodes = new Node[PoolSize];
    freeList = nullptr;
    // Initialize the free list by linking all nodes.
    for (size_t i = 0; i < PoolSize; ++i) {
      nodes[i].next = freeList;
      freeList = &nodes[i];
    }
  }

    ~ParticlePool() {
        delete[] nodes;
    }

    // Allocate a Particle either from pool or heap if pool is exhausted.
    Particle* allocate() {
        if (freeList == nullptr) {
            // Pool empty: allocate on heap.
            return new Particle();
        }
        Node* node = freeList;
        freeList = node->next;
        // Return the memory as a Particle pointer.
        return reinterpret_cast<Particle*>(node);
    }

    // Deallocate a Particle, returning it to pool or deleting if not from pool.
    void deallocate(Particle* p) {
        Node* node = reinterpret_cast<Node*>(p);
        // Check if pointer is in our pool range.
        if (node >= nodes && node < nodes + PoolSize) {
            node->next = freeList;
            freeList = node;
        } else {
            delete p;
        }
    }

private:
    // Union to store either a pointer to next free node or an actual Particle.
    union Node {
        Node* next;
        Particle particle;
    };
    Node* nodes;
    Node* freeList;
};

int main() {
    const int N = 10000000;
    std::vector<Particle*> list;
    list.reserve(N);

    // --- Standard allocation (new/delete) ---
    {
        // Allocate N Particles with new.
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < N; ++i) {
            Particle* p = new Particle();
            list.push_back(p);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Standard new alloc: "
                  << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
                  << " ns\n";
    }
    {
        // Deallocate all Particles with delete.
        auto start = std::chrono::high_resolution_clock::now();
        for (Particle* p : list) {
            delete p;
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Standard delete:    "
                  << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
                  << " ns\n";
    }

    // --- Custom memory pool allocation ---
    ParticlePool<N> pool;
    list.clear();
    list.reserve(N);
    {
        // Allocate N Particles from the pool.
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < N; ++i) {
            Particle* p = pool.allocate();
            list.push_back(p);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Pool alloc:         "
                  << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
                  << " ns\n";
    }
    {
        // Return all Particles to the pool.
        auto start = std::chrono::high_resolution_clock::now();
        for (Particle* p : list) {
            pool.deallocate(p);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Pool dealloc:       "
                  << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
                  << " ns\n";
    }

    return 0;
}

/* Example console output (timing varies by machine):
Standard new alloc:  31351953 ns
Standard delete:     14631760 ns
Pool alloc:          5280705 ns
Pool dealloc:        3461703 ns
*/
