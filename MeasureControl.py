import MPS010602 as MPS
import pandas as pd
import numpy as np

class MeasureControl:
    def __init__(self) -> None:
        self.adc_device=MPS.MPS010602_Base_Handler()
        #TODO 设置AD增益
        self.adc_device_settings=MPS.MPS010602_Para(SampleRate=5000
                                                    ,ADChannelNumber=4)
        self.adc_device.set_para(self.adc_device_settings)
        self._input_x="AD1"
        self._input_y="AD2"
        self._input_z="AD9"
        self._output="AD10"
        self.sample_number=128#一次性采样点数
        self.rough_measure_out=pd.DataFrame()

    def measure(self,cmd_input):
        x,y,z,_=cmd_input
        self.adc_device.data_in(256)#刷写掉可能有问题的缓存
        self.adc_device.data_in(10*self.sample_number)
        voltage1,voltage2,_,_=self.adc_device.voltage_data.read_voltage()
        voltage1=voltage1[::10]
        voltage2=voltage2[::10]
        out_data_dict={
                    "outmean":[voltage2[self._output].mean()],
                    "outstd":[voltage2[self._output].std()],
                    "xmean":[voltage1[self._input_x].mean()],
                    "xstd":[voltage1[self._input_x].std()],
                    "ymean":[voltage1[self._input_y].mean()],
                    "ystd":[voltage1[self._input_y].std()],
                    "zmean":[voltage2[self._input_z].mean()],
                    "zstd":[voltage2[self._input_z].std()],
                    "inx":[x],
                    "iny":[y],
                    "inz":[z]
                   }
        odf=pd.DataFrame(out_data_dict)
        self.rough_measure_out=pd.concat([self.rough_measure_out,odf])
