# Gnuplot script for generating benchmark graphs

set terminal pngcairo enhanced font 'Arial,12' size 1200,800
set datafile separator ","

# ============================================
# Graph 1: Tree Height Comparison
# ============================================
set output 'results/height_comparison.png'
set title "Tree Height vs Number of Elements" font ",16"
set xlabel "Number of Elements"
set ylabel "Tree Height (levels)"
set logscale x
set grid
set key left top

plot 'results/height_comparison.csv' using 1:2 with linespoints title 'B-Tree' lw 2 pt 7 ps 1.5, \
     '' using 1:3 with linespoints title 'BST (Best Case)' lw 2 pt 5 ps 1.5, \
     '' using 1:4 with linespoints title 'BST (Average)' lw 2 pt 9 ps 1.5

# ============================================
# Graph 2: Disk I/O Time Comparison
# ============================================
set output 'results/disk_io_comparison.png'
set title "Disk I/O Time for Search Operations" font ",16"
set xlabel "Number of Elements"
set ylabel "Time per Search (milliseconds)"
set logscale x
set grid
set key left top

plot 'results/disk_io_comparison.csv' using 1:2 with linespoints title 'B-Tree' lw 2 pt 7 ps 1.5 lc rgb "green", \
     '' using 1:3 with linespoints title 'BST' lw 2 pt 5 ps 1.5 lc rgb "red"

# ============================================
# Graph 3: Speedup Factor
# ============================================
set output 'results/speedup.png'
set title "B-Tree Speedup Over BST (Disk Operations)" font ",16"
set xlabel "Number of Elements"
set ylabel "Speedup Factor (x times faster)"
set logscale x
set grid
set key left top

plot 'results/disk_io_comparison.csv' using 1:4 with linespoints title 'Speedup' lw 3 pt 7 ps 2 lc rgb "blue"

# ============================================
# Graph 4: Insert Time by Scenario
# ============================================
set output 'results/insert_time_comparison.png'
set title "Insert Time: B-Tree vs BST (Different Scenarios)" font ",16"
set xlabel "Number of Elements"
set ylabel "Insert Time (microseconds)"
set logscale x
set logscale y
set grid
set key left top

plot 'results/benchmark_results.csv' every ::1 using ($3==1000 && $1=="BTree" && $2=="Random" ? $3 : 1/0):4 title 'B-Tree Random' with points pt 7 ps 2, \
     '' every ::1 using ($3==1000 && $1=="BST" && $2=="Random" ? $3 : 1/0):4 title 'BST Random' with points pt 5 ps 2

print "Graphs generated successfully!"
print "Check results/ directory for PNG files"
