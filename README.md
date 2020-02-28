# Parallel-distributed-programming
Four different approaches to the problem of placing tiles on a grid. Every solution has the same data files to work with. 
- Sequential solution: Places the tiles recursively - depth first search.
- Task parallelism: Creates tasks from the branches of the solution tree, works with Open MP library.
- Data paralellism: Creates parallel for loop and runs multiple threads with DFS.
- MPI (Message Passing Interface) - works with MPI protocol, one process is a master and the others are slaves. The slaves are also parallel (data paralellism)
