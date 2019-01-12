# HybridLargeDataStorage

This repo contains code of a hashmap-alike data structure optimized for genome sequences storage.

The "sequence" is an array of (A, T, G, C) symbols which were faced in genome code.

The point of this structire is to simplify search of repeating sequences.

Selection and Insertion complexity is about log(n).


![alt text](https://i.imgur.com/ZAdAjtm.png)
High-level scheme of the HLDS structure.

The top level is fixed-size **head** pointers -- an array of pointers which contain all possible **A T G C** combinations up to length N (4^N pointers in total).

Middle level is tree structure which defines a sequence **tail** -- a tree, root of one is pointed by according **head** pointer, and with branches which define a **tail**.

Bottom level is a counter value which is pointed by last symbol of every **tail**. It contains a number of sequence entries in a source gene.
