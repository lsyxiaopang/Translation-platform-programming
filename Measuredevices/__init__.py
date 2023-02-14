'''
这个文件是用来挑选作为控制器
下面的函数从前向后分别有三个选项:测量类型,测量设备,测量设备版本号(如果有)

对于模块的需求
模块需要有一个MeasureControl,其中支持一个measure方法,输入的是命令元组
'''
measure_type="Voltage"
measure_device="MPS010602"
measure_ver="1"
#######################################################
import os,sys
sys.path.append(os.path.dirname(__file__))
###################电压测量设备#########################
if measure_type=="Voltage":
    if measure_device=="ADS1263":
        from ADS1263 import *#2.14修改,可以在树莓派上使用
    elif measure_device=="MPS010602":
        from mps import *#基于Windows的AD卡,需要相应的包
    else:
        raise KeyError("Wrong measure_device {}".format(measure_device))
####################错误###############################
else:
    raise KeyError("Wrong measure_type {}".format(measure_type))

sys.path.pop()