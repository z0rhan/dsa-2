# About
Programming tasks for the course Data Structure and Algorithms 2 at the university. These tasks require certain behaviour from the solution to test them deteministically againts the automated tests and hence the algorimthms can vary slightly from their origianl version.

# Usage
Inside each folder that belongs to a particular problem and a example *input.txt* file. Use the compiler of choice to compile the file given. I will only use *C* and *C++* for solutions.

## Minimum spanning tree
Inside the `mst/` directory, compile as
```bash
g++ mst.cpp
```
Run the program as
```bash
./a.out input.txt <start-node>
```

## Topological sort or cycle detection
Inside the `topological-order/` directory, complie as
```bash
gcc topcycle.cpp
```
Run the program as
```bash
./a.out input.txt
```
Lmao, not doing the rest with C at all. Will stick with C++ for mental well-being.

## Articulation-node and Bridge detection
Inside the `articulation-node-bridge/` directory, compile as
```bash
g++ artbridges.cpp
```

Run the program as,
```bash
./a.out input.txt
```

## 3-way-quicksort with MSD radix sort for sorting strings
Inside the `3-way-qs-msd-radix-strings/` directory, compile as
```bash
g++ strquick3.cpp
```

Run the program as,
```bash
./a.out input.txt
```

## Morris-Pratt string matching
Inside the `mp-string-matching/` directory, compile as
```bash
g++ mp.cpp
```

Run the program as,
```bash
./a.out input.txt
```

