from PIL import Image
import random as rnd
import math

rnd.seed(1)

size = 32
img = Image.new('RGBA', (size, size), color = (255,0,0))

l = []

def rand_col():
    x = rnd.random()
    y = rnd.random()
    if x+y > 1:
        x = 1-x
        y = 1-y
    z = 1-x-y
    return tuple(x*c1 + y*c2 + z*c3 for c1,c2,c3 in zip((0,128,0),(128,100,0),(128,192,96)))

for i in xrange(30):
    l.append(((rnd.randint(0,size-1),rnd.randint(0,size-1)),rand_col()))

def dist(p1,p2):
    return math.sqrt(sum((x1-x2)**2 for x1,x2 in zip(p1,p2)))

def rnd_dist(p1,p2):
    return min(dist(p1,(p2[0]+x*size, p2[1]+y*size)) for x in xrange(-1,2) for y in xrange(-1,2))

for x in xrange(size):
    for y in xrange(size):
        col = min(l, key = lambda a: rnd_dist(a[0],(x,y)))[1]
        col = tuple(int(c) for c in col)
        img.putpixel((x,y), col+(255,))

img.save('../textures/army.png')