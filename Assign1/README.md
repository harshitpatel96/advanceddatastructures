# In memory and On disk search

The program named assn_1 will run from the command line. Three command line arguments will be specified: a search mode, the name of the key file, and the name of the seek file.

assn_1 search-mode keyfile-name seekfile-name

It supports four different search modes.

--mem-lin    Read the key file into memory. Perform a linear search for each seek element si in the seek file.
--mem-bin    Read the key file into memory. Perform a binary search for each seek element si in the seek file.
--disk-lin   Read each ki from the key file as it is needed. Perform a linear search for each seek element si in the seek file.
--disk-bin   Read each ki from the key file as it is needed. Perform a binary search for each seek element si in the seek file.

For example, executing your program as follows

assn_1 --disk-lin key.db seek.db

would search for each element in seek.db using an on-disk linear search within key.db.

On execution, it would print all the search result and how much time it took to perform the operation.
