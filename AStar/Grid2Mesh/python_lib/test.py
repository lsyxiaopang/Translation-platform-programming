#* 这个函数是用于测试是否能够正常工作
import AStarSearch as ast
import numpy as np
import matplotlib.pyplot as plt

mesh=ast.Mesh(0,10.0,0,10.0,100,100)
wall=np.array([[1,1],
               [1,5],
               [7,5],
               [7,3],
               [5,3],
               [5,1]],dtype=float)
mesh.vec2mesh(wall)
route=mesh.find_route(np.array([0.5,3]),np.array([6,2]))
print(route)

showall=np.hstack([wall.reshape(-1),wall[0,:]]).reshape(-1,2)
plt.plot(showall[:,0],showall[:,1],"-")
plt.plot(route[:,0],route[:,1],"r*-")
plt.show()