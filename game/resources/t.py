import Image
import sys

img = Image.open("fonthelper.png")


w = img.size[0]
h = img.size[1]

c = ord(' ')
b = -1

for i in range(3):
	for j in range(32):
		print(str(chr(c) + "\t" + str(5*j) + "\t" + str(7*i + 7) + "\t5\t7"))
		c+=1

c = ord('!')
for z in range(9):

	for i in range(w):
		for j in range(72):
			p = img.getpixel((i, j + 72*z))
			if p[0] == 255 and p[1] == 0 and p[2] == 255:
				if b == -1:
					b = i;
				else:
					print(str(chr(c) + "\t" + str(b) + "\t" + str(72*z) + "\t" + str(i-b) + "\t72"))
					c +=1
					b = -1;
			
	
	##out.save("out"+str(z)+".png")