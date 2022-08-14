# tobits adapted from https://stackoverflow.com/questions/10237926/convert-string-to-list-of-bits-and-viceversa

def tobits(s):
    result = []
    for c in s:
        bits = bin(ord(c)-63)[2:]
        bits = '00000000'[len(bits):] + bits
        result.extend([int(b) for b in bits[2:]])
    return result
def intify(bits):
    curr = 0
    for x in bits:
        curr*=2
        if x is 1:
            curr+=1
    return curr
# reads small graph6 format graphs
def readgraph(s):
    bits = tobits(s)
#    print(bits)
    n = intify(bits[0:6])
    curr = 6
    gra = [False]*n
    for x in range(0,n):
        gra[x] = set()
        for y in range(0,x):
            if bits[curr] is 1:
                gra[x].add(y)
            curr+=1
    for x in range(0,n):
        for y in gra[x]:
            if y < x:
                gra[y].add(x)
    return gra
