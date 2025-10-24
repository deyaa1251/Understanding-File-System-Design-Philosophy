# Understanding File System Design Philosophy

This repository demonstrates the performance differences between B-Trees and Binary Search Trees (BSTs) in the context of file system design. It provides comprehensive benchmarks that illustrate why B-Trees are preferred for disk-based storage systems.

## Table of Contents

- [Overview](#overview)
- [Building the Project](#building-the-project)
- [Running Benchmarks](#running-benchmarks)
- [Benchmark Results](#benchmark-results)
- [Generating Graphs](#generating-graphs)
- [Key Takeaways](#key-takeaways)

## Overview

Modern file systems like ext4, Btrfs, and NTFS use B-Trees instead of Binary Search Trees. This project explores why through extensive benchmarking across different scenarios:

- **Sequential Insert**: Best case for BST (balanced)
- **Random Insert**: Typical real-world scenario
- **Reverse Insert**: Worst case for BST (degenerate tree)
- **Duplicate-Heavy**: 90% duplicate keys
- **Skewed Distribution**: 90-10 rule (Pareto distribution)

## Building the Project

```bash
make all
```

This will compile:
- `benchmark` - Main benchmark executable
- `csv_export` - CSV data exporter for graphs

## Running Benchmarks

### Run the comprehensive benchmark:

```bash
./benchmark
```

### Export CSV data for graphing:

```bash
./csv_export
```

This generates:
- `results/benchmark_results.csv` - Detailed benchmark data
- `results/height_comparison.csv` - Tree height scaling
- `results/disk_io_comparison.csv` - Disk I/O performance

## Benchmark Results

### Performance Summary

The benchmarks test both data structures with 1,000, 10,000, and 100,000 elements across five different insertion patterns.

#### 1. Tree Height Comparison

| Elements | B-Tree Height | BST (Best) | BST (Avg) | BST (Worst) |
|----------|---------------|------------|-----------|-------------|
| 1,000    | 2 levels      | 10 levels  | 14 levels | 1,000 levels|
| 10,000   | 2 levels      | 14 levels  | 19 levels | 10,000 levels|
| 100,000  | 3 levels      | 17 levels  | 24 levels | 100,000 levels|

**Key Insight**: B-Trees maintain logarithmic height with a much larger base (degree=100), resulting in dramatically shorter trees. For 100,000 elements, a B-Tree has only 3 levels while a BST can have up to 100,000 levels in the worst case.

#### 2. Disk I/O Performance

With simulated disk reads (10ms per operation):

| Elements | B-Tree Search Time | BST Search Time | Speedup |
|----------|-------------------|-----------------|---------|
| 1,000    | 20 ms             | 140 ms          | 7.0x    |
| 10,000   | 20 ms             | 190 ms          | 9.5x    |
| 100,000  | 30 ms             | 240 ms          | 8.0x    |

**Key Insight**: B-Trees require far fewer disk reads due to their shorter height. Each node contains multiple keys, so one disk read retrieves much more data.

### Detailed Results by Scenario

#### Sequential Insert (1,000 elements)

```
🌳 B-Tree (degree=100)
  Insert time:      32 μs
  Search time:      89 μs
  Tree height:      2 levels

🌲 Binary Search Tree
  Insert time:      8,994 μs
  Search time:      2,665 μs
  Tree height:      1,000 levels

📊 Performance: B-tree is 281.06x faster for inserts
💾 Disk I/O: B-tree is 500.0x faster for searches
```

#### Random Insert (10,000 elements)

```
🌳 B-Tree (degree=100)
  Insert time:      1,091 μs
  Search time:      1,378 μs
  Tree height:      2 levels

🌲 Binary Search Tree
  Insert time:      2,627 μs
  Search time:      983 μs
  Tree height:      29 levels

📊 Performance: B-tree is 2.41x faster for inserts
💾 Disk I/O: B-tree is 14.5x faster for searches
```

#### Reverse Insert (100,000 elements)

```
🌳 B-Tree (degree=100)
  Insert time:      7,691 μs
  Search time:      5,143 μs
  Tree height:      3 levels

🌲 Binary Search Tree
  Insert time:      11,998,561 μs
  Search time:      9,826,908 μs
  Tree height:      100,000 levels

📊 Performance: B-tree is 1,560.08x faster for inserts
💾 Disk I/O: B-tree is 33,333.3x faster for searches
```

### Memory vs Disk Performance

**In-Memory (RAM)**:
- BSTs can be faster for searches when data fits in memory
- Random access is essentially free (~nanoseconds)
- Cache-friendly for small datasets

**On-Disk (Storage)**:
- B-Trees are dramatically faster (7-500x speedup)
- Disk seeks are expensive (~10ms per operation)
- Fewer disk reads = massive performance gains
- B-Trees pack multiple keys per node = better locality

### CSV Data

#### Benchmark Results Sample

```csv
TreeType,Scenario,NumElements,InsertPerOp_us,SearchPerOp_us,TreeHeight
BTree,Sequential,1000,0.019,0.071,2
BST,Sequential,1000,2.643,2.168,1000
BTree,Random,10000,0.1337,0.1847,2
BST,Random,10000,0.3128,0.1302,29
BTree,Reverse,100000,0.07235,0.04704,3
BST,Reverse,100000,141.589,103.514,100000
```

## Generating Graphs

### Option 1: Python (Recommended)

Install dependencies:
```bash
pip install pandas matplotlib seaborn numpy
```

Generate graphs:
```bash
python3 scripts/generate_graphs.py
```

This creates:
- `results/graphs/height_comparison.png` - Tree height scaling
- `results/graphs/disk_io_performance.png` - Disk I/O comparison
- `results/graphs/insert_performance_scenarios.png` - Insert performance by scenario
- `results/graphs/search_performance_heatmap.png` - Search performance heatmap
- `results/summary_statistics.txt` - Statistical summary

### Option 2: Gnuplot

```bash
gnuplot scripts/plot.gnu
```

### Option 3: Excel/Google Sheets

Import the CSV files from `results/` directory and create custom visualizations.

## Key Takeaways

### Why File Systems Use B-Trees

1. **Height Matters on Disk**: Tree height directly correlates to the number of disk reads required. B-Trees maintain much lower height than BSTs.

2. **Fewer Disk Reads**: A B-Tree with degree 100 searching 100,000 elements requires only 3 disk reads, while a BST might require 17-100,000 reads.

3. **Bulk Data Transfer**: Disk I/O works best when reading larger chunks at once. B-Trees pack multiple keys per node, making each disk read more valuable.

4. **Worst-Case Protection**: BSTs degenerate into linked lists with sequential or reverse inserts (O(n) height). B-Trees self-balance and maintain O(log n) height.

5. **Cache-Friendly**: Modern SSDs and HDDs have prefetch buffers. Reading a B-Tree node brings many keys into cache at once.

### Real-World File System Examples

- **ext4**: Uses HTrees (hash-tree variant of B-Tree) for directory indexing
- **Btrfs**: Built entirely on B-Trees for all metadata
- **NTFS**: Uses B+ Trees for the Master File Table (MFT)
- **XFS**: B+ Trees for allocation groups and directory structures

### Performance Characteristics

| Scenario | Winner (Memory) | Winner (Disk) |
|----------|----------------|---------------|
| Sequential Insert | B-Tree (281x faster) | B-Tree (500x faster) |
| Random Insert | B-Tree (2-3x faster) | B-Tree (10-15x faster) |
| Reverse Insert | B-Tree (188-1560x faster) | B-Tree (5000-33333x faster) |
| Duplicate-Heavy | B-Tree (1.3-1.6x faster) | B-Tree (10x faster) |
| Skewed Distribution | Comparable | B-Tree (7-12x faster) |

## Project Structure

```
.
├── src/                    # Source files
│   ├── main.c             # Main benchmark program
│   ├── csv_exporter.c     # CSV export utility
│   ├── b_tree.c           # B-Tree implementation
│   └── bst.c              # Binary Search Tree implementation
├── include/               # Header files
├── results/               # Benchmark results and graphs
├── scripts/               # Graph generation scripts
│   ├── generate_graphs.py # Python visualization
│   └── plot.gnu           # Gnuplot script
├── Makefile              # Build configuration
└── README.md             # This file
```

## License

This project is for educational purposes to understand file system design philosophy.

## Contributing

Feel free to submit issues or pull requests to improve the benchmarks or add new test scenarios.

---

**Generated with comprehensive benchmarks comparing B-Trees vs Binary Search Trees**
