#!/usr/bin/env python3

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import os

# Set style
sns.set_style("whitegrid")
plt.rcParams['figure.figsize'] = (12, 8)
plt.rcParams['font.size'] = 12

# Create output directory if it doesn't exist
os.makedirs('results/graphs', exist_ok=True)

print("📊 Generating comprehensive graphs for blog post...\n")

# ============================================
# Graph 1: Height Comparison
# ============================================
print("  Generating height comparison graph...")
df_height = pd.read_csv('results/height_comparison.csv')

plt.figure(figsize=(12, 7))
plt.plot(df_height['NumElements'], df_height['BTreeHeight'], 
         'o-', linewidth=3, markersize=10, label='B-Tree (degree=100)', color='green')
plt.plot(df_height['NumElements'], df_height['BSTHeight_Best'], 
         's-', linewidth=2, markersize=8, label='BST (Best Case - Balanced)', color='blue')
plt.plot(df_height['NumElements'], df_height['BSTHeight_Avg'], 
         '^-', linewidth=2, markersize=8, label='BST (Average Case)', color='orange')

plt.xscale('log')
plt.xlabel('Number of Elements', fontsize=14, fontweight='bold')
plt.ylabel('Tree Height (levels)', fontsize=14, fontweight='bold')
plt.title('Tree Height Scaling: B-Tree vs Binary Search Tree', fontsize=16, fontweight='bold')
plt.legend(fontsize=12, loc='upper left')
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig('results/graphs/height_comparison.png', dpi=300, bbox_inches='tight')
plt.close()

# ============================================
# Graph 2: Disk I/O Performance
# ============================================
print("  Generating disk I/O performance graph...")
df_disk = pd.read_csv('results/disk_io_comparison.csv')

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 7))

# Subplot 1: Absolute time
ax1.plot(df_disk['NumElements'], df_disk['BTreeDiskTime_ms'], 
         'o-', linewidth=3, markersize=10, label='B-Tree', color='green')
ax1.plot(df_disk['NumElements'], df_disk['BSTDiskTime_ms'], 
         's-', linewidth=3, markersize=10, label='BST', color='red')
ax1.set_xscale('log')
ax1.set_xlabel('Number of Elements', fontsize=14, fontweight='bold')
ax1.set_ylabel('Search Time (milliseconds)', fontsize=14, fontweight='bold')
ax1.set_title('Disk I/O Time per Search Operation', fontsize=14, fontweight='bold')
ax1.legend(fontsize=12)
ax1.grid(True, alpha=0.3)

# Subplot 2: Speedup factor
ax2.plot(df_disk['NumElements'], df_disk['Speedup'], 
         'D-', linewidth=3, markersize=10, color='purple')
ax2.set_xscale('log')
ax2.set_xlabel('Number of Elements', fontsize=14, fontweight='bold')
ax2.set_ylabel('Speedup Factor (× faster)', fontsize=14, fontweight='bold')
ax2.set_title('B-Tree Speedup Over BST', fontsize=14, fontweight='bold')
ax2.axhline(y=1, color='gray', linestyle='--', linewidth=2, alpha=0.5)
ax2.grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig('results/graphs/disk_io_performance.png', dpi=300, bbox_inches='tight')
plt.close()

# ============================================
# Graph 3: Insert Performance by Scenario
# ============================================
print("  Generating insert performance by scenario...")
df_bench = pd.read_csv('results/benchmark_results.csv')

scenarios = df_bench['Scenario'].unique()
fig, axes = plt.subplots(2, 3, figsize=(18, 12))
axes = axes.flatten()

for idx, scenario in enumerate(scenarios):
    if idx < len(axes):
        ax = axes[idx]
        
        df_scenario = df_bench[df_bench['Scenario'] == scenario]
        
        btree_data = df_scenario[df_scenario['TreeType'] == 'BTree']
        bst_data = df_scenario[df_scenario['TreeType'] == 'BST']
        
        ax.plot(btree_data['NumElements'], btree_data['InsertPerOp_us'], 
                'o-', linewidth=2, markersize=8, label='B-Tree', color='green')
        ax.plot(bst_data['NumElements'], bst_data['InsertPerOp_us'], 
                's-', linewidth=2, markersize=8, label='BST', color='red')
        
        ax.set_xscale('log')
        ax.set_xlabel('Number of Elements', fontsize=11)
        ax.set_ylabel('Time per Insert (μs)', fontsize=11)
        ax.set_title(f'{scenario} Insert Pattern', fontsize=12, fontweight='bold')
        ax.legend(fontsize=10)
        ax.grid(True, alpha=0.3)

# Remove extra subplot if odd number of scenarios
if len(scenarios) < len(axes):
    fig.delaxes(axes[-1])

plt.tight_layout()
plt.savefig('results/graphs/insert_performance_scenarios.png', dpi=300, bbox_inches='tight')
plt.close()

# ============================================
# Graph 4: Search Performance Heatmap
# ============================================
print("  Generating search performance heatmap...")

pivot_search = df_bench.pivot_table(
    values='SearchPerOp_us', 
    index='Scenario', 
    columns='NumElements', 
    aggfunc='mean'
)

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 6))

# B-Tree heatmap
btree_pivot = df_bench[df_bench['TreeType'] == 'BTree'].pivot_table(
    values='SearchPerOp_us', 
    index='Scenario', 
    columns='NumElements'
)
sns.heatmap(btree_pivot, annot=True, fmt='.2f', cmap='Greens', ax=ax1, cbar_kws={'label': 'μs per search'})
ax1.set_title('B-Tree Search Performance', fontsize=14, fontweight='bold')
ax1.set_xlabel('Number of Elements', fontsize=12)
ax1.set_ylabel('Scenario', fontsize=12)

# BST heatmap
bst_pivot = df_bench[df_bench['TreeType'] == 'BST'].pivot_table(
    values='SearchPerOp_us', 
    index='Scenario', 
    columns='NumElements'
)
sns.heatmap(bst_pivot, annot=True, fmt='.2f', cmap='Reds', ax=ax2, cbar_kws={'label': 'μs per search'})
ax2.set_title('BST Search Performance', fontsize=14, fontweight='bold')
ax2.set_xlabel('Number of Elements', fontsize=12)
ax2.set_ylabel('Scenario', fontsize=12)

plt.tight_layout()
plt.savefig('results/graphs/search_performance_heatmap.png', dpi=300, bbox_inches='tight')
plt.close()

# ============================================
# Summary Statistics
# ============================================
print("\n📈 Generating summary statistics...")

with open('results/summary_statistics.txt', 'w') as f:
    f.write("=" * 70 + "\n")
    f.write("BENCHMARK SUMMARY STATISTICS\n")
    f.write("=" * 70 + "\n\n")
    
    f.write("Average Insert Time per Operation (microseconds):\n")
    f.write("-" * 70 + "\n")
    summary = df_bench.groupby(['TreeType', 'Scenario'])['InsertPerOp_us'].mean()
    f.write(str(summary) + "\n\n")
    
    f.write("Average Search Time per Operation (microseconds):\n")
    f.write("-" * 70 + "\n")
    summary = df_bench.groupby(['TreeType', 'Scenario'])['SearchPerOp_us'].mean()
    f.write(str(summary) + "\n\n")
    
    f.write("Average Tree Height:\n")
    f.write("-" * 70 + "\n")
    summary = df_bench.groupby(['TreeType', 'NumElements'])['TreeHeight'].mean()
    f.write(str(summary) + "\n\n")

print("✅ All graphs generated successfully!")
print("\n📁 Output files:")
print("   - results/graphs/height_comparison.png")
print("   - results/graphs/disk_io_performance.png")
print("   - results/graphs/insert_performance_scenarios.png")
print("   - results/graphs/search_performance_heatmap.png")
print("   - results/summary_statistics.txt")
print("\n💡 These graphs are ready for your blog post!")
