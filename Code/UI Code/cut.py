# Team Id: 6
# Author List: Prateesh Goyal
# Filename: cut.py
# Theme: Image Processing
# Functions: None
# Global Variables: None


from PIL import Image
import numpy as np
import sys
import cv2


# Open the Image
im = Image.open(sys.argv[1])
# get the pixel value in the variable pix. pix[i][j][0] gives the r value of the pixel i,j pix[i][j][1] gives the g value of the pixel i,j and pix[i][j][2] gives the r value of the pixel i,j
pix = im.load()
# X_val: Width of input image by 5. Equal to horizontal partitions we want to divide our image in
X_val = im.size[0]/5
# Y_val: Height of input image by 5. Equal to vertical partitions we want
Y_val = im.size[1]/5
# Green: 2-D Matrix of size X_Val * Y_Val. Each index corresponds to 25 pixels. The value is 1 if the corresponding region is green else 0
Green = [[0 for x in range(Y_val)] for x in range(X_val)]
# Iterate over each index of green and if the corresponding region is green set the value to 1
for i in range(X_val):
	for j in range(Y_val):
			r = 0
			b = 0
			g = 0
			for k in range(0,4):
				for l in range(0,4):
					if((i*5+k)>=X_val*5 or (j*5+l)>=Y_val*5):
						continue
					r+=pix[i*5+k,j*5+l][0]
					g+=pix[i*5+k,j*5+l][1]
					b+=pix[i*5+k,j*5+l][2]
					# Set the value to 1 if green is atleast 1.2 times red and blue
			if(g>1.2*r and g>1.2*b):
				Green[i][j]=1


# Now we find the green rectangle of maximum size in the image which corresponds to the green identifier. So in short we need to find the biggest square submatrix in the Green Matrix				
# S: 2-D Matrix where S[i][j]=k indicates that biggest square submatrix of green with its bottom right index as i,j is of size k 
S = [[0 for x in range(Y_val)] for x in range(X_val)]
# loop to get the first column
for i in range(X_val):
	S[i][0] = Green[i][0]
# loop to get the first row
for j in range(Y_val):
	S[0][j] = Green[0][j]
# loop to get rest of the values      
for i in range(X_val):
	for j in range(Y_val):
		if(Green[i][j] == 1):
			S[i][j] = min(S[i][j-1], S[i-1][j], S[i-1][j-1]) + 1
		else:
			S[i][j] = 0
# max_of_s: size of biggest square submatrix of green   
max_of_s = S[0][0]
# max_i: x co-ordinate where biggest square submatrix of green occur. Equivalently x coordinate of the right most pixel in marker 
max_i = 0
# max_j: y co-ordinate where biggest square submatrix of green occur. Equivalently y coordinate of the bottom most pixel in marker
max_j = 0
# loop to get these values
for i in range(X_val):
	for j in range(Y_val):
		if(max_of_s < S[i][j]):
			max_of_s = S[i][j];
			max_i = i; 
			max_j = j;       
   

print max_i
print max_j
print max_of_s
# w: width of the image of the numberplate region
# h: height of the image of the numberplate region
w, h = int(max_of_s*10.5), int(max_of_s*4.5)+10
# data: matrix to store pixel values of the numbrerplate region of the image. Since we know the co-ordinates of the marker we know the co-ordinates of the number plate in the original image
data = np.zeros((h, w, 3), dtype=np.uint8)
a=0
b=0
# loop to copy pixel value from pix to data
for i in range((max_i-max_of_s)*5-w-9,(max_i-max_of_s)*5-9):
	b=0
	for j in range ((max_j)*5-h,(max_j)*5):
		data[b][a]=pix[i,j]
		b=b+1
	a=a+1


img = Image.fromarray(data, 'RGB')
# save the image of the number palte portion only as my.jpg
img.save('my.jpg')


# read my.jpg using cv2
img = cv2.imread('my.jpg',0)
# blur the image
img = cv2.medianBlur(img,5)
# apply gaussian blurring
blur = cv2.GaussianBlur(img,(1,1),0)
# =binarize the image using otsu algorithm
ret3,th4 = cv2.threshold(blur,0,255,cv2.THRESH_BINARY+cv2.THRESH_OTSU)

# save the binarized image as my2.jpg
cv2.imwrite('my2.jpg',th4)
