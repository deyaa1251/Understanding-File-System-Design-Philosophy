#include <iostream>
#include <iomanip>
#include <thread>
#include "../include/benchmark.h"
#include "../include/b_tree.h"
#include "../include/bst.h"

using namespace std;
using namespace chrono;

// Global counters for disk operations (we'll increment these in tree operations)
int g_disk_reads = 0;
int g_disk_writes = 0;

void printSectionHeader(const string& title) {
    cout << "\n╔══════════════════════════════════════════════════════════════════╗" << endl;
    cout << "║  " << left << setw(62) << title << "  ║" << endl;
    cout << "╚══════════════════════════════════════════════════════════════════╝\n" << endl;
}


void printSubHeader(const string& title) {
    cout << "\n" << title << endl;
    cout << string(title.length(), '-') << endl;  // ✅ Use regular dash
}




template<typename TreeType>
OperationMetrics benchmarkTree(
    TreeType& tree,
    const vector<int>& insertData,
    const vector<int>& searchData,
    const string& treeName,
    bool simulateDisk = false
) {
    OperationMetrics metrics = {0};
    
    // === INSERT BENCHMARK ===
    g_disk_reads = 0;
    g_disk_writes = 0;
    
    auto start = high_resolution_clock::now();
    for (int key : insertData) {
        tree.insert(key);
        if (simulateDisk) {
            g_disk_writes++;
        }
    }
    auto end = high_resolution_clock::now();
    metrics.insert_time_us = duration_cast<microseconds>(end - start).count();
    
    // === SEARCH BENCHMARK ===
    start = high_resolution_clock::now();
    for (int key : searchData) {
        tree.search(key);
        if (simulateDisk) {
            g_disk_reads++;
        }
    }
    end = high_resolution_clock::now();
    metrics.search_time_us = duration_cast<microseconds>(end - start).count();
    
    // === RANGE QUERY BENCHMARK (search consecutive keys) ===
    if (searchData.size() >= 100) {
        start = high_resolution_clock::now();
        for (size_t i = 0; i < 100; i++) {
            tree.search(searchData[i]);
        }
        end = high_resolution_clock::now();
        metrics.range_query_time_us = duration_cast<microseconds>(end - start).count();
    }
    
    metrics.disk_reads = g_disk_reads;
    metrics.disk_writes = g_disk_writes;
    
    return metrics;
}

void runComprehensiveBenchmark(int numElements, TestScenario scenario, const string& scenarioName) {
    printSectionHeader("Benchmark: " + to_string(numElements) + " elements - " + scenarioName);
    
    // Generate data based on scenario
    vector<int> data;
    switch (scenario) {
        case TestScenario::SEQUENTIAL:
            data = DataGenerator::sequential(numElements);
            break;
        case TestScenario::RANDOM:
            data = DataGenerator::random(numElements);
            break;
        case TestScenario::REVERSE:
            data = DataGenerator::reverse(numElements);
            break;
        case TestScenario::DUPLICATE_HEAVY:
            data = DataGenerator::duplicateHeavy(numElements);
            break;
        case TestScenario::SKEWED:
            data = DataGenerator::skewed(numElements);
            break;
    }
    
    vector<int> searchData = data; // Search for same keys we inserted
    
    // === B-TREE BENCHMARK ===
    printSubHeader("🌳 B-Tree (degree=100)");
    BTree<int> btree(100);
    auto btree_metrics = benchmarkTree(btree, data, searchData, "B-Tree", false);
    
    // Calculate B-tree specific metrics
    btree_metrics.tree_height = calculateBTreeHeight(btree.getRoot());
    
    cout << "  Insert time:      " << setw(10) << btree_metrics.insert_time_us << " μs" << endl;
    cout << "  Search time:      " << setw(10) << btree_metrics.search_time_us << " μs" << endl;
    cout << "  Range query:      " << setw(10) << btree_metrics.range_query_time_us << " μs (100 keys)" << endl;
    cout << "  Tree height:      " << setw(10) << btree_metrics.tree_height << " levels" << endl;
    
    // === BST BENCHMARK ===
    printSubHeader("🌲 Binary Search Tree");
    BST<int> bst;
    auto bst_metrics = benchmarkTree(bst, data, searchData, "BST", false);
    
    // Calculate BST specific metrics
    bst_metrics.tree_height = calculateBSTHeight(bst.getRoot());
    
    cout << "  Insert time:      " << setw(10) << bst_metrics.insert_time_us << " μs" << endl;
    cout << "  Search time:      " << setw(10) << bst_metrics.search_time_us << " μs" << endl;
    cout << "  Range query:      " << setw(10) << bst_metrics.range_query_time_us << " μs (100 keys)" << endl;
    cout << "  Tree height:      " << setw(10) << bst_metrics.tree_height << " levels" << endl;
    
    // === COMPARISON ===
    printSubHeader("📊 Performance Comparison");
    
    double insert_ratio = (double)bst_metrics.insert_time_us / btree_metrics.insert_time_us;
    double search_ratio = (double)bst_metrics.search_time_us / btree_metrics.search_time_us;
    double height_ratio = (double)bst_metrics.tree_height / btree_metrics.tree_height;
    
    cout << fixed << setprecision(2);
    cout << "  Insert:  B-tree is " << insert_ratio << "x "
         << (insert_ratio > 1.0 ? "faster ⚡" : "slower") << " than BST" << endl;
    cout << "  Search:  B-tree is " << search_ratio << "x "
         << (search_ratio > 1.0 ? "faster ⚡" : "slower") << " than BST" << endl;
    cout << "  Height:  B-tree is " << height_ratio << "x flatter 📏 than BST" << endl;
    
    // === DISK I/O SIMULATION ===
    printSubHeader("💾 Simulated Disk I/O (10ms per operation)");
    
    int btree_disk_time = btree_metrics.tree_height * 10;  // ms per search
    int bst_disk_time = bst_metrics.tree_height * 10;       // ms per search
    
    cout << "  B-tree avg search:  " << btree_disk_time << " ms "
         << "(" << btree_metrics.tree_height << " disk reads)" << endl;
    cout << "  BST avg search:     " << bst_disk_time << " ms "
         << "(" << bst_metrics.tree_height << " disk reads)" << endl;
    cout << "  Speedup on disk:    " << fixed << setprecision(1)
         << (double)bst_disk_time / btree_disk_time << "x faster ⚡⚡⚡" << endl;
}
int main() {
    cout << "\n";
    cout << "╔════════════════════════════════════════════════════════════════════╗" << endl;
    cout << "║                                                                    ║" << endl;
    cout << "║          COMPREHENSIVE FILE SYSTEM DATA STRUCTURE BENCHMARK        ║" << endl;
    cout << "║                    B-Tree vs Binary Search Tree                    ║" << endl;
    cout << "║                                                                    ║" << endl;
    cout << "╚════════════════════════════════════════════════════════════════════╝" << endl;
    
    vector<int> sizes = {1000, 10000, 100000};
    vector<pair<TestScenario, string>> scenarios = {
        {TestScenario::SEQUENTIAL, "Sequential Insert (Best Case for BST)"},
        {TestScenario::RANDOM, "Random Insert (Typical Case)"},
        {TestScenario::REVERSE, "Reverse Insert (Worst Case for BST)"},
        {TestScenario::DUPLICATE_HEAVY, "Duplicate-Heavy (90% duplicates)"},
        {TestScenario::SKEWED, "Skewed Distribution (90-10 rule)"}
    };
    
    for (size_t i = 0; i < sizes.size(); i++) {
        int size = sizes[i];
        for (size_t j = 0; j < scenarios.size(); j++) {
            runComprehensiveBenchmark(size, scenarios[j].first, scenarios[j].second);
        }
    }
    
    printSectionHeader("Key Takeaways for File Systems");
    cout << "  1. B-trees maintain much lower height than BSTs" << endl;
    cout << "  2. On disk, height = # of disk reads (each ~10ms)" << endl;
    cout << "  3. B-trees can be 10-20x faster for disk-based operations" << endl;
    cout << "  4. BSTs may be faster in RAM, but file systems use disk" << endl;
    cout << "  5. B-trees pack multiple keys per node = fewer disk blocks" << endl;
    cout << "\n";
    
    return 0;
}
