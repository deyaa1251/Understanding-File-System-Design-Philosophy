#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <chrono>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <random>        // ✅ ADD THIS LINE
#include <thread> 

// Metrics structure
struct OperationMetrics {
    long long insert_time_us;      // Insert time in microseconds
    long long search_time_us;      // Search time in microseconds
    long long range_query_time_us; // Range query time
    int tree_height;               // Height of tree
    size_t memory_nodes;           // Number of nodes
    size_t memory_bytes;           // Estimated memory usage
    int disk_reads;                // Simulated disk reads
    int disk_writes;               // Simulated disk writes
    double avg_keys_per_node;      // Average keys per node
};

// Test scenario types
enum class TestScenario {
    SEQUENTIAL,      // 1, 2, 3, 4, 5...
    RANDOM,          // Random order
    REVERSE,         // n, n-1, n-2...
    DUPLICATE_HEAVY, // Many duplicates
    SKEWED          // 90% small values, 10% large
};

// CSV Writer for results
class CSVWriter {
private:
    std::ofstream file;
    
public:
    CSVWriter(const std::string& filename) {
        file.open(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
    }
    
    ~CSVWriter() {
        if (file.is_open()) {
            file.close();
        }
    }
    
    void writeHeader(const std::vector<std::string>& headers) {
        for (size_t i = 0; i < headers.size(); i++) {
            file << headers[i];
            if (i < headers.size() - 1) file << ",";
        }
        file << "\n";
    }
    
    template<typename... Args>
    void writeRow(Args... args) {
        writeRowHelper(args...);
        file << "\n";
    }
    
private:
    template<typename T>
    void writeRowHelper(T value) {
        file << value;
    }
    
    template<typename T, typename... Args>
    void writeRowHelper(T first, Args... rest) {
        file << first << ",";
        writeRowHelper(rest...);
    }
};

// Statistics calculator
class Statistics {
public:
    static double mean(const std::vector<long long>& data) {
        if (data.empty()) return 0;
        long long sum = 0;
        for (auto val : data) sum += val;
        return static_cast<double>(sum) / data.size();
    }
    
    static double stddev(const std::vector<long long>& data) {
        if (data.size() < 2) return 0;
        double avg = mean(data);
        double sum_sq_diff = 0;
        for (auto val : data) {
            double diff = val - avg;
            sum_sq_diff += diff * diff;
        }
        return std::sqrt(sum_sq_diff / (data.size() - 1));
    }
    
    static long long min(const std::vector<long long>& data) {
        if (data.empty()) return 0;
        return *std::min_element(data.begin(), data.end());
    }
    
    static long long max(const std::vector<long long>& data) {
        if (data.empty()) return 0;
        return *std::max_element(data.begin(), data.end());
    }
    
    static double median(std::vector<long long> data) {
        if (data.empty()) return 0;
        std::sort(data.begin(), data.end());
        size_t n = data.size();
        if (n % 2 == 0) {
            return (data[n/2 - 1] + data[n/2]) / 2.0;
        } else {
            return data[n/2];
        }
    }
};

// Data generator
class DataGenerator {
public:
    static std::vector<int> sequential(int count) {
        std::vector<int> data;
        for (int i = 1; i <= count; i++) {
            data.push_back(i);
        }
        return data;
    }
    
    static std::vector<int> random(int count, int seed = 42) {
        std::vector<int> data;
        std::mt19937 gen(seed);
        std::uniform_int_distribution<> dis(1, count * 10);
        
        for (int i = 0; i < count; i++) {
            data.push_back(dis(gen));
        }
        return data;
    }
    
    static std::vector<int> reverse(int count) {
        std::vector<int> data;
        for (int i = count; i >= 1; i--) {
            data.push_back(i);
        }
        return data;
    }
    
    static std::vector<int> duplicateHeavy(int count, int seed = 42) {
        std::vector<int> data;
        std::mt19937 gen(seed);
        std::uniform_int_distribution<> dis(1, count / 10); // Only count/10 unique values
        
        for (int i = 0; i < count; i++) {
            data.push_back(dis(gen));
        }
        return data;
    }
    
    static std::vector<int> skewed(int count, int seed = 42) {
        std::vector<int> data;
        std::mt19937 gen(seed);
        std::uniform_real_distribution<> prob(0.0, 1.0);
        std::uniform_int_distribution<> small_dis(1, 100);
        std::uniform_int_distribution<> large_dis(1000, 10000);
        
        for (int i = 0; i < count; i++) {
            if (prob(gen) < 0.9) {
                data.push_back(small_dis(gen));  // 90% small values
            } else {
                data.push_back(large_dis(gen));  // 10% large values
            }
        }
        return data;
    }
};

// Disk I/O simulator
class DiskSimulator {
private:
    static constexpr int DISK_READ_TIME_US = 10000;  // 10ms per read
    static constexpr int DISK_WRITE_TIME_US = 10000; // 10ms per write
    
public:
    static void simulateRead() {
        std::this_thread::sleep_for(std::chrono::microseconds(DISK_READ_TIME_US));
    }
    
    static void simulateWrite() {
        std::this_thread::sleep_for(std::chrono::microseconds(DISK_WRITE_TIME_US));
    }
    
    static int getReadTime() { return DISK_READ_TIME_US; }
    static int getWriteTime() { return DISK_WRITE_TIME_US; }
};

#endif
