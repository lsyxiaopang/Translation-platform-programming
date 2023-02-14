import ADS1263
import pandas as pd
import numpy as np

class MeasureControl:#TODO 现在数据缺乏管理,如果要修改数据或者再处理数据需要提供合适的接口
    def __init__(self) -> None:
        self.adc=ADS1263.ADS1263()
        self.adc.ADS1263_init_ADC1('ADS1263_100SPS')
        self.adc.ADS1263_SetMode(1)
        self.current_output=0#0-1
        self.input_x=1#2-3
        self.input_y=2#4-5
        self.input_z=3#6-7
        self.rough_measure_out=pd.DataFrame()
        self.sample_number=20#每一次测量的取样点数

    def get_real_voltage(voltage_inputs,ref=2.5):#TODO 实际上这么做效率非常低下,有机会就改掉
        output=[]
        for voltage_input in voltage_inputs:
            if(voltage_input>>31==1):
                #print("负")
                output.append(-(2*ref-voltage_input*ref/0x80000000)) 
            else:
                #print("正")
                output.append(voltage_input*ref/ 0x7fffffff)
        return output

    def measure(self,cmd_input):
        x,y,z,_=cmd_input
        rough_data=self.one_full_measure([self.current_output,
                                          self.input_x,
                                          self.input_y,
                                          self.input_z])
        df=pd.DataFrame(rough_data,columns=["inp","x","y","z"])
        means=df.mean()
        stds=df.std()
        out_data_dict={
                    "inpmean":[means["inp"]],
                    "inpstd":[stds["inp"]],
                    "inx":[x],
                    "xmean":[means["x"]],                  
                    "xstd":[stds["x"]],
                    "iny":[y],
                    "ymean":[means["y"]],
                    "ystd":[stds["y"]],
                    "inz":[z],
                    "zmean":[means["z"]],
                    "zstd":[stds["z"]]
                   }
        odf=pd.DataFrame(out_data_dict)
        self.rough_measure_out=pd.concat([self.rough_measure_out,odf])

    def one_full_measure(self,channel_numbers):#定义一次完整的测量
        samp=self.sample_number
        data=[]
        for i in range(samp):
            channel_data=self.adc.ADS1263_GetAll(channel_numbers)
            #print(channel_data)
            newdatas=MeasureControl.get_real_voltage(channel_data)
            data.append(newdatas)
        #print(data)
        return np.array(data)

if __name__=="__main__":
    test=MeasureControl()
    print(test.one_full_measure([0,1,2,3]))
    test.measure((1,2,3,True))
    print(test.rough_measure_out)

        