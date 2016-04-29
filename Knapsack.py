'''
Implementation of 0-1 Knapsack problem

Dillon Irish
'''

import sys

'''
size - the size of the Knapsack

We add 1 to size as list indices begin at 0
'''

def main(size):
    entries = [ ]

    '''
    file contents is
    [item] [size] [value]
    '''
    
    file = open('goodies.txt')
    for line in file:
        entries.append(line.split())
    
    table = [[0 for x in range(size+1)] for x in range(len(entries)+1)]
    
    for i in range(len(entries)+1):
        for j in range(size+1):
            if i == 0 or j == 0:
                table[i][j] = 0
            elif int(entries[i-1][1]) <= j:
                table[i][j] = max(int(entries[i-1][2]) + table[i-1][j-int(entries[i-1][1])], table[i-1][j])
            else:
                table[i][j] = table[i-1][j]       
    return table[len(entries)][size]

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print 'Usage: python Knapsack [size]'
        quit()
    
    print main(int (sys.argv[1]))
