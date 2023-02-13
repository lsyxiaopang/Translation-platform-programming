import AntScan
import numpy as np
import matplotlib.pyplot as plt

# rstat=np.random.RandomState(1)
# testpoint=rstat.random([50,2])*100

ctest=np.arange(7)
X,Y=np.meshgrid(ctest,ctest)
testpoint=np.dstack([X,Y]).reshape(-1,2)

odata=[[np.sqrt((i[0]-j[0])**2+(i[1]-j[1])**2) for i in testpoint]for j in testpoint]
odata=np.array(odata)
print(odata.shape)


test_scan=AntScan.MapSearcher(odata,3,0.99,5,1,1)
test_list=[]
l_list=[]
for i in range(2000):
    l_list.append(test_scan.one_iter())
    test_list.append(test_scan.best_now)

print(np.array(test_scan.tmat))

plt.plot(range(2000),l_list,"b--",label="the best")
plt.plot(range(2000),test_list,"r--",label="each test")
plt.show()

bestpath=test_scan.best_route
plt.plot(testpoint[bestpath,0],testpoint[bestpath,1],".-")
plt.show()

tmat=test_scan.tmat
plt.contourf(np.array(tmat))
plt.show()

