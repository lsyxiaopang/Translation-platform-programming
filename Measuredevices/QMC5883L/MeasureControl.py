import serial
import struct
import pandas as pd
import numpy as np
from loguru import logger

class MeasureControl:
    def __init__(self,port_name="COM9") -> None:
        logger.info("Trying to connect to the measure device!")
        sercom=serial.Serial(port_name,115200,timeout=0.1)
        sercom.readall()
        sercom.write("TES\n".encode("ascii"))
        bkstr=sercom.readline().decode("ascii")
        # if(bkstr=="Ready!\n"):
        #     logger.info("Measure device connected!")
        # else:
        #     logger.error("Cannot connect!")
        #     raise
        sercom.readall()
        self.com=sercom
        self.sample_number=100
        self.rough_measure_out=pd.DataFrame()
        self.this_rough_data=None
    
    def measure(self,cmd_input):
        x,y,z,_=cmd_input
        datalist=[]
        for t in range(self.sample_number):
            self.com.write("MEA\n".encode("ascii"))
            l=self.com.readline()
            l=self.com.readline()
            l=self.com.readline()
            data=self.com.read(12)
            unpacked_data=struct.unpack("fff",data)
            #检查存在错误数据的情况
            if(max(unpacked_data)>10 or min(unpacked_data)< -10):
                continue#发现数据错误立刻跳过这一组数据,继续测
            datalist.append(unpacked_data)
            self.com.read_all()
        outdata=np.array(datalist)
        self.this_rough_data=outdata
        out_data_dict={
                    "xmean":[outdata[:,0].mean()],
                    "xstd":[outdata[:,0].std()],
                    "ymean":[outdata[:,1].mean()],
                    "ystd":[outdata[:,1].std()],
                    "zmean":[outdata[:,2].mean()],
                    "zstd":[outdata[:,2].std()],
                    "inx":[x],
                    "iny":[y],
                    "inz":[z]
                   }
        odf=pd.DataFrame(out_data_dict)
        self.rough_measure_out=pd.concat([self.rough_measure_out,odf])
        logger.info("One measure complete")
        

if __name__ == "__main__":
    test=MeasureControl()
    test.measure((1,2,3,True))
    print(test.rough_measure_out)
    import matplotlib.pyplot as plt
    import seaborn as sns
    from scipy import stats
    rdata=test.this_rough_data
    print(stats.kstest(rdata[:,0],"norm"))
    sns.distplot(rdata[:,0],hist=False)
    plt.show()
