# Disk Merge Sort

The goals of this assignment are two-fold:

 - To introduce you to sorting data on disk using mergesort.
 - To compare the performance of different algorithms for creating and merging runs during mergesort.
 

This program will be named assn_3 and it will run from the command line. Three command line arguments will be specified: a mergesort method, the name of the input index file, and the name of the sorted output index file.


assn_3 mergesort-method index-file sorted-index-file
This program supports three different mergesort methods.

1.	`--basic`	Split the index file into sorted runs stored on disk, then merge the runs to produce a sorted index file.
2.	`--multistep`	Split the index file into sorted runs. Merge subsets of runs to create super-runs, then merge the super-runs to produce a sorted index file.
3.	`--replacement`	Split the index file into sorted runs created using replacement selection, then merge the runs to produce a sorted index file.

For example,

    assn_3 --multistep input.bin sort.bin

This would apply a multistep mergesort to input.bin to sort it ascending by key value. The result would be stored in sort.bin.
