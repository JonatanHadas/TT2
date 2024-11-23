from PIL import Image

size = 20
hsize = 100
img = Image.new('RGBA', (size, hsize*2), color = (255,0,0))

off = lambda dx: 2*dx*dx * float(hsize) / (size*size)

for x in xrange(size):
    for y in xrange(hsize*2):
        dx = abs(x - (size/2))
        dy = (y - off(dx))%hsize

        a = abs(dy-(hsize/2))*2 - hsize/4

        c = min(255,max(0,int(a*255.0/(hsize/2))))
        
        img.putpixel((x,y), (255,c,c,255))


img.save('../tank/deathray.png')
