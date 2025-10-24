#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include "../include/benchmark.h"
#include "../include/b_tree.h"
#include "../include/bst.h"

using namespace std;
using namespace chrono;

struct BenchmarkResult {
    string tree_type;
    string scenario;
    int num_elements;
    long long insert_time_us;
    long long search_time_us;
    long long range_query_time_us;
    int tree_height;
    int simulated_disk_reads;
    double insert_per_op_us;
    double search_per_op_us;
};

class ComprehensiveExporter {
public:
    static void exportToCSV(const vector<BenchmarkResult>& results, const string& filename) {
        ofstream file(filename);
        
        if (!file.is_open()) {
            cerr << "Error: Cannot open file " << filename << endl;
            return;
        }
        
        // Write header
        file << "TreeType,Scenario,NumElements,InsertTime_us,SearchTime_us,"
             << "RangeQueryTime_us,TreeHeight,DiskReads,InsertPerOp_us,SearchPerOp_us\n";
        
        // Write data
        for (const auto& result : results) {
            file << result.tree_type << ","
                 << result.scenario << ","
                 << result.num_elements << ","
                 << result.insert_time_us << ","
                 << result.search_time_us << ","
                 << result.range_query_time_us << ","
                 << result.tree_height << ","
                 << result.simulated_disk_reads << ","
                 << result.insert_per_op_us << ","
                 << result.search_per_op_us << "\n";
        }
        
        file.close();
        cout << "✅ Exported results to: " << filename << endl;
    }
    
    static void exportHeightComparison(const string& filename) {
        ofstream file(filename);
        
        file << "NumElements,BTreeHeight,BSTHeight_Best,BSTHeight_Avg,BSTHeight_Worst\n";
        
        // Generate theoretical height comparisons
        vector<int> sizes = {100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000};
        
        for (int n : sizes) {
            // B-tree with degree 100: log_100(n)
            int btree_height = static_cast<int>(ceil(log(n) / log(100)));
            
            // BST best case (balanced): log2(n)
            int bst_best = static_cast<int>(ceil(log2(n)));
            
            // BST average case: ~1.39 * log2(n)
            int bst_avg = static_cast<int>(ceil(1.39 * log2(n)));
            
            // BST worst case (linear): n
            int bst_worst = n;
            
            file << n << "," << btree_height << "," << bst_best << "," 
                 << bst_avg << "," << bst_worst << "\n";
        }
        
        file.close();
        cout << "✅ Exported height comparison to: " << filename << endl;
    }
    
    static void exportDiskIOComparison(const string& filename) {
        ofstream file(filename);
        
        file << "NumElements,BTreeDiskTime_ms,BSTDiskTime_ms,Speedup\n";
        
        vector<int> sizes = {1000, 5000, 10000, 50000, 100000, 500000, 1000000};
        const int DISK_READ_MS = 10;
        
        for (int n : sizes) {
            int btree_height = static_cast<int>(ceil(log(n) / log(100)));
            int bst_height = static_cast<int>(ceil(1.39 * log2(n))); // Average case
            
            int btree_disk_time = btree_height * DISK_READ_MS;
            int bst_disk_time = bst_height * DISK_READ_MS;
            double speedup = static_cast<double>(bst_disk_time) / btree_disk_time;
            
            file << n << "," << btree_disk_time << "," << bst_disk_time << "," 
                 << speedup << "\n";
        }
        
        file.close();
        cout << "✅ Exported disk I/O comparison to: " << filename << endl;
    }
};

// Run benchmarks and collect results
vector<BenchmarkResult> runAllBenchmarks() {
    vector<BenchmarkResult> results;
    
    vector<int> sizes = {1000, 10000, 100000};
    vector<pair<TestScenario, string>> scenarios = {
        {TestScenario::SEQUENTIAL, "Sequential"},
        {TestScenario::RANDOM, "Random"},
        {TestScenario::REVERSE, "Reverse"},
        {TestScenario::DUPLICATE_HEAVY, "DuplicateHeavy"},
        {TestScenario::SKEWED, "Skewed"}
    };
    
    cout << "\n🔄 Running comprehensive benchmarks...\n" << endl;
    
    int total_tests = sizes.size() * scenarios.size() * 2; // *2 for both tree types
    int completed = 0;
    
    for (int size : sizes) {
	for (const auto& scenarioPair : scenarios) {
	    TestScenario scenario = scenarioPair.first;
	    string scenarioName = scenarioPair.second;
            // Generate data
            vector<int> data;
            switch (scenario) {
                case TestScenario::SEQUENTIAL:
                    data = DataGenerator::sequential(size);
                    break;
                case TestScenario::RANDOM:
                    data = DataGenerator::random(size);
                    break;
                case TestScenario::REVERSE:
                    data = DataGenerator::reverse(size);
                    break;
                case TestScenario::DUPLICATE_HEAVY:
                    data = DataGenerator::duplicateHeavy(size);
                    break;
                case TestScenario::SKEWED:
                    data = DataGenerator::skewed(size);
                    break;
            }
            
            // B-Tree benchmark
            {
                BTree<int> tree(100);
                
                auto start = high_resolution_clock::now();
                for (int key : data) {
                    tree.insert(key);
                }
                auto end = high_resolution_clock::now();
                long long insert_time = duration_cast<microseconds>(end - start).count();
                
                start = high_resolution_clock::now();
                for (int key : data) {
                    tree.search(key);
                }
                end = high_resolution_clock::now();
                long long search_time = duration_cast<microseconds>(end - start).count();
                
                // Range query (100 keys)
                start = high_resolution_clock::now();
                for (int i = 0; i < min(100, (int)data.size()); i++) {
                    tree.search(data[i]);
                }
                end = high_resolution_clock::now();
                long long range_time = duration_cast<microseconds>(end - start).count();
                
                int height = calculateBTreeHeight(tree.getRoot());
                
                BenchmarkResult result;
                result.tree_type = "BTree";
                result.scenario = scenarioName;
                result.num_elements = size;
                result.insert_time_us = insert_time;
                result.search_time_us = search_time;
                result.range_query_time_us = range_time;
                result.tree_height = height;
                result.simulated_disk_reads = height * size; // height per search * num searches
                result.insert_per_op_us = static_cast<double>(insert_time) / size;
                result.search_per_op_us = static_cast<double>(search_time) / size;
                
                results.push_back(result);
                completed++;
                cout << "  Progress: " << completed << "/" << total_tests 
                     << " (" << (completed * 100 / total_tests) << "%)" << "\r" << flush;
            }
            
            // BST benchmark
            {
                BST<int> tree;
                
                auto start = high_resolution_clock::now();
                for (int key : data) {
                    tree.insert(key);
                }
                auto end = high_resolution_clock::now();
                long long insert_time = duration_cast<microseconds>(end - start).count();
                
                start = high_resolution_clock::now();
                for (int key : data) {
                    tree.search(key);
                }
                end = high_resolution_clock::now();
                long long search_time = duration_cast<microseconds>(end - start).count();
                
                // Range query
                start = high_resolution_clock::now();
                for (int i = 0; i < min(100, (int)data.size()); i++) {
                    tree.search(data[i]);
                }
                end = high_resolution_clock::now();
                long long range_time = duration_cast<microseconds>(end - start).count();
                
                int height = calculateBSTHeight(tree.getRoot());
                
                BenchmarkResult result;
                result.tree_type = "BST";
                result.scenario = scenarioName;
                result.num_elements = size;
                result.insert_time_us = insert_time;
                result.search_time_us = search_time;
                result.range_query_time_us = range_time;
                result.tree_height = height;
                result.simulated_disk_reads = height * size;
                result.insert_per_op_us = static_cast<double>(insert_time) / size;
                result.search_per_op_us = static_cast<double>(search_time) / size;
                
                results.push_back(result);
                completed++;
                cout << "  Progress: " << completed << "/" << total_tests 
                     << " (" << (completed * 100 / total_tests) << "%)" << "\r" << flush;
            }
        }
    }
    
    cout << "\n✅ All benchmarks completed!\n" << endl;
    
    return results;
}

int main() {
    cout << "\n╔════════════════════════════════════════════════════════════╗" << endl;
    cout << "║                                                            ║" << endl;
    cout << "║           CSV EXPORT FOR BLOG POST GRAPHS                  ║" << endl;
    cout << "║                                                            ║" << endl;
    cout << "╚════════════════════════════════════════════════════════════╝\n" << endl;
    
    // Run all benchmarks
    auto results = runAllBenchmarks();
    
    // Export to CSV files
    cout << "\n📊 Exporting data to CSV files...\n" << endl;
    
    ComprehensiveExporter::exportToCSV(results, "results/benchmark_results.csv");
    ComprehensiveExporter::exportHeightComparison("results/height_comparison.csv");
    ComprehensiveExporter::exportDiskIOComparison("results/disk_io_comparison.csv");
    
    cout << "\n✅ All data exported successfully!" << endl;
    cout << "\n📁 Files created in 'results/' directory:" << endl;
    cout << "   - benchmark_results.csv (detailed benchmark data)" << endl;
    cout << "   - height_comparison.csv (tree height scaling)" << endl;
    cout << "   - disk_io_comparison.csv (disk I/O performance)" << endl;
    
    cout << "\n💡 Next steps:" << endl;
    cout << "   1. Use scripts/generate_graphs.py to create visualizations" << endl;
    cout << "   2. Or use scripts/plot.gnu with gnuplot" << endl;
    cout << "   3. Import CSVs into Excel/Google Sheets for custom charts\n" << endl;
    
    return 0;
}
