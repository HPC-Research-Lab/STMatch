import sys
from itertools import product
from sympy.combinatorics import Permutation
from sympy.combinatorics.generators import symmetric
from copy import copy
from utils import list_of_set
Permutation.print_cyclic = False
def is_automorph(graph, perm):
    n = len(graph)
    for i in range(0, n):
        for j in graph[i]:
            if j^perm not in graph[i^perm]:
                return False
    return True

def list_automorphisms(graph):
    # return the group of automorphisms of graph
    n = len(graph)
    result = []
    for perm in symmetric(n):
        if is_automorph(graph, perm):
            result.append(perm)
    return result
    
# graph format, consists of lists a_i={b_1,b_2,etc} where i,b_1 and i,b_2, etc are edges. could be optimized with bitsets 
# Order is a list of pairs a,b 
def test_order_on_permutation(perm, order):
    for great,smalls in enumerate(order):
        for z in smalls:
            # great under perm has to be larger than z under perms, otherwise it doesn't match
            if z!=great and (great^perm) <= (z^perm):
                return False
    return True
    
# each pair has one of two directions of edge, or no edge
def generate_all_dags(n):
    # O( 2^(nc2))
    result = []
    nc2 = (n*(n-1))//2
    # generate all possible states 
    for ord in product(range(3), repeat=nc2):
        count = 0
        gek = list_of_set(n)
        for i in range(0, n):
            for j in range(0, i):
                if ord[count] is 0:
                    gek[j].add(i)
                elif ord[count] is 1:
                    gek[i].add(j)
                count += 1
        yield gek

# each pair either has edge or no edge
def generate_all_graphs(n):
    result = []
    nc2 = (n*(n-1))//2
    for ord in product(range(2), repeat=nc2):
        count = 0
        gek = list_of_set(n)
        for i in range(0, n):
            for j in range(0, i):
                if ord[count] is 0:
                    gek[j].add(i)
                    gek[i].add(j)
                count += 1
        yield gek

# This is a greedy algorithm to assign an ordering. I'm looking into proving it now.
# Basic idea:
#   For each vertex, examine all remaining automorphisms
#     Examine all the automorphisms, determine which vertices it could also be
#     Force this node to have the smallest (or largest) ID of those nodes (give < to all)
#     Ignore all automorphisms that move this vertex
#     Iterate on another node
#   repeat until there is only the trivial automorphism
#
#   This algorithm can likely be performed much faster using Strongly generating sets, and a generator, but this quick and dirty is meant to be fast.
def lazy_order(autos,n):
    parents = {}
    order = list_of_set(n)
    olds = copy(autos)
    for i in range(0,n):
        # find all places i could be moved to
        # force i to be biggest, remove all permutations that move i
        news = []
        locs = set()
        for x in olds:
            if i^x is i:
                # i doesn't move under x, hold onto x
                news.append(x)
            else:
                # add the position i gets moved to under x
                locs.add(i^x)
        order[i] = locs
        olds=news
    return order

def lazy_parent(autos,n):
    parents = [ [x] for x in range(0,n)]
    olds = copy(autos)
    for i in range(0,n):
        # find all places i could be moved to
        # force i to be biggest, remove all permutations that move i
        news = []
        for x in olds:
            if i^x is i:
                # i doesn't move under x, hold onto x
                news.append(x)
            else:
                # add the position i gets moved to under x
                parents[i^x][0] = i
        olds=news
    return parents
from time import process_time
from SGS_order import lazy_SGS, lazy_SGS_parent, jerrums
def test_lazy_on_graph(g,checking=False):
    n = len(g)
    autos = list_automorphisms(g)
    fy4x = False
    # Generate a list of cosets, and a map to which coset each element corresponds to
    coset_map = {}
    uniq = {}
    for z in symmetric(n):
        if z not in coset_map:
            uniq[z] = 0
            # add all the coset members in
            for autod in autos:
                coset_map[autod*z] = z
    #time this
    jerr = jerrums(autos,n)
    tsta = process_time()
    lazy = lazy_SGS_parent(jerr,n) # lazy_order(autos,n)
    time = process_time()-tsta
    # We check ordering y
    # counts from each coset
    counts = {}
    for a in uniq:
        # We have seen none from the coset
        counts[a]=0
    if checking:
        for z in symmetric(n):
            if test_order_on_permutation(z,lazy):
                # increase number of this coset that it accepts
                counts[coset_map[z]]+=1
        works = True
        for x in counts:
            if counts[x] is not 1:
                works = False
                print(f"Fails, counted {x} {counts[x]} times")
                print( list( x*autod for autod in autos))
                print( g)
                print(lazy)
                break
        if works:
            print("Success")
    return time,lazy

from regra import readgraph    
def main():
    for n in range(2,10):
        count = 0
        f = open(f"graph{n}c.g6","r")
        t = 0.0
        # read the graph from the file
        for line in f:
            count+=1
            time,lazy = test_lazy_on_graph(readgraph(line),False)
            t+= time
            #print(count)
        f.close()
        print(f"Took {t} seconds to generate orders for all {count} connected graphs of size {n}")
        sys.stdout.flush()
main()
