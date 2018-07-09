# mesh file
mesh = open("mesh.x", "w")

# number of blocks
mesh.write("5\n") 

# uniform grid space step
N = 16
h = 1/N

# block 1 range: 0-14, 0-4, 0-5
# block 2 range: 2-3, -1-0, 2-3
# block 3 range: 5-6, -2-0, 2-3
# block 4 range: 8-9, -3-0, 2-3
# block 5 range: 11-9, -4-0, 2-3
# block index range
mesh.write( repr(14*N+1) + ' ' + repr(4*N+1) + ' ' + repr(5*N+1) + ' ' 
          + repr(N+1)    + ' ' + repr(N+1)   + ' ' + repr(N+1)   + '\n')
mesh.write( repr(N+1) + ' ' + repr(2*N+1) + ' ' + repr(N+1) + ' '
          + repr(N+1) + ' ' + repr(3*N+1) + ' ' + repr(N+1) + '\n')
mesh.write( repr(N+1) + ' ' + repr(4*N+1) + ' ' + repr(N+1) + '\n')

# block 1 x
count = 0
for k in range(5*N+1):      
    for j in range(4*N+1):
        for i in range(14*N+1):
            mesh.write(repr(i*h) + ' ')
            count += 1
            if(count == 6):
                mesh.write("\n")
                count = 0
# block 1 y
count = 0
for k in range(5*N+1):      
    for j in range(4*N+1):
        for i in range(14*N+1):
            mesh.write(repr(j*h) + ' ')
            count += 1
            if(count == 6):
                mesh.write("\n")
                count = 0
# block 1 z
count = 0
for k in range(5*N+1):      
    for j in range(4*N+1):
        for i in range(14*N+1):
            mesh.write(repr(k*h) + ' ')
            count += 1
            if(count == 6):
                mesh.write("\n")
                count = 0

# block 2 x
count = 0
for k in range(N+1):      
    for j in range(N+1):
        for i in range(N+1):
            mesh.write(repr(i*h+2.0) + ' ')
            count += 1
            if(count == 6):
                mesh.write("\n")
                count = 0
# block 2 y
count = 0
for k in range(N+1):      
    for j in range(N+1):
        for i in range(N+1):
            mesh.write(repr(j*h-1.0) + ' ')
            count += 1
            if(count == 6):
                mesh.write("\n")
                count = 0
# block 2 z
count = 0
for k in range(N+1):      
    for j in range(N+1):
        for i in range(N+1):
            mesh.write(repr(k*h+2.0) + ' ')
            count += 1
            if(count == 6):
                mesh.write("\n")
                count = 0

# block 3 x
count = 0
for k in range(N+1):      
    for j in range(2*N+1):
        for i in range(N+1):
            mesh.write(repr(i*h+5.0) + ' ')
            count += 1
            if(count == 6):
                mesh.write("\n")
                count = 0
# block 3 y
count = 0
for k in range(N+1):      
    for j in range(2*N+1):
        for i in range(N+1):
            mesh.write(repr(j*h-2.0) + ' ')
            count += 1
            if(count == 6):
                mesh.write("\n")
                count = 0
# block 3 z
count = 0
for k in range(N+1):      
    for j in range(2*N+1):
        for i in range(N+1):
            mesh.write(repr(k*h+2.0) + ' ')
            count += 1
            if(count == 6):
                mesh.write("\n")
                count = 0

# block 4 x
count = 0
for k in range(N+1):      
    for j in range(3*N+1):
        for i in range(N+1):
            mesh.write(repr(i*h+8.0) + ' ')
            count += 1
            if(count == 6):
                mesh.write("\n")
                count = 0
# block 4 y
count = 0
for k in range(N+1):      
    for j in range(3*N+1):
        for i in range(N+1):
            mesh.write(repr(j*h-3.0) + ' ')
            count += 1
            if(count == 6):
                mesh.write("\n")
                count = 0
# block 4 z
count = 0
for k in range(N+1):      
    for j in range(3*N+1):
        for i in range(N+1):
            mesh.write(repr(k*h+2.0) + ' ')
            count += 1
            if(count == 6):
                mesh.write("\n")
                count = 0

# block 5 x
count = 0
for k in range(N+1):      
    for j in range(4*N+1):
        for i in range(N+1):
            mesh.write(repr(i*h+11.0) + ' ')
            count += 1
            if(count == 6):
                mesh.write("\n")
                count = 0
# block 5 y
count = 0
for k in range(N+1):      
    for j in range(4*N+1):
        for i in range(N+1):
            mesh.write(repr(j*h-4.0) + ' ')
            count += 1
            if(count == 6):
                mesh.write("\n")
                count = 0
# block 5 z
count = 0
for k in range(N+1):      
    for j in range(4*N+1):
        for i in range(N+1):
            mesh.write(repr(k*h+2.0) + ' ')
            count += 1
            if(count == 6):
                mesh.write("\n")
                count = 0

mesh.close()

# mapfile
bcMap = open("mapfile", "w")

# title line
bcMap.write("Type Block# Face# Edg1.Indx1 Edg1.Indx2 Edg2.Indx1 Edg2.Indx2 - Block# ")
bcMap.write("Face# Edg1.Indx1 Edg1.Indx2 Edg2.Indx1 Edg2.Indx2 Flip\n")

# block 1 face 1
bcMap.write("FarField 1 1\n")
# block 1 face 2
bcMap.write("EulerWall 1 2 1 " + repr(5*N+1) + " 1 " + repr(2*N+1) + "\n")
for i in range(4):
    bcMap.write("EulerWall 1 2 1 " + repr(2*N+1) + " " + repr((3*i+2)*N+1) + " " \
                                   + repr((i*3+3)*N+1) + "\n")
    bcMap.write("Block2Block 1 2 " + repr(2*N+1) + " " + repr(3*N+1) + " " \
                                   + repr((3*i+2)*N+1) + " " \
                                   + repr((3*i+3)*N+1) + " " \
                                   + repr(i+2) + " 5 1 " + repr(N+1) + " 1 " \
                                   + repr(N+1) + "\n")
    bcMap.write("EulerWall 1 2 "   + repr(3*N+1) + " " + repr(5*N+1) + " " \
                                   + repr((3*i+2)*N+1) + " " \
                                   + repr((3*i+3)*N+1) + "\n")
    bcMap.write("EulerWall 1 2 1 " + repr(5*N+1) + " " + repr((3*i+3)*N+1) + " " \
                                   + repr((3*i+5)*N+1) + "\n")
# block 1 face 3,4,5,6
bcMap.write("EulerWall 1 3\n")
bcMap.write("FarField 1 4\n")
bcMap.write("EulerWall 1 5\n")
bcMap.write("EulerWall 1 6\n")
# block block 2,3,4,5, face 1,2,3,4,6
for i in range(2,6):
    for j in [1,2,3,4,6]:
        bcMap.write("EulerWall " + repr(i) + " " + repr(j) + "\n")
# close file
bcMap.close()
