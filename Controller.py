import threading
import MoveControl
import Measuredevices
from time import sleep
import pandas as pd
from collections import namedtuple
###

class Controller_config:
    """ 这个用来储存移动控制的设置信息,留着备用
    """    
    def __init__(self) -> None:
        pass


class ControllerCallbackBase:
    def status_refresh_call(self,newstatus: str):
        pass

    def coordinate_refresh_call(self,newcoordinate:tuple):
        pass
    
    def cmd_refresh_call(self,new_cmd):
        pass

    def measure_refresh_call(self,datalist):
        pass

    def no_command_call(self):
        pass
    

    
class _ControllerMoveCallback(MoveControl.MoveControlCallbacksBase):
    def __init__(self,controller) -> None:
        self.controller=controller
        super().__init__()

    def status_refresh_call(self, newstatus: str):
        self.controller.motor_status=newstatus
        return super().status_refresh_call(newstatus)

    def coordinate_refresh_call(self, newcoordinate: tuple):
        self.controller.coordinate=newcoordinate
        self.controller.recall_class.coordinate_refresh_call(newcoordinate)
        return super().coordinate_refresh_call(newcoordinate)

class Controller:
    def __init__(self,_config:Controller_config,
                 motor_control_portname,
                 recall_class:ControllerCallbackBase) -> None:
        self._config=_config
        #*初始化命令列表
        self.cmd_list=[]#用来记录移动目的地(绝对坐标)
        self.cmd_now=None#现在正在执行的命令
        self._processing=False#是否正在移动
        self._status="STOP"#记录状态
        self._move_start=False#是否移动下一步
        self.motor_control=MoveControl.MoveControl(motor_control_portname,
                                                _ControllerMoveCallback(self))
        self.measure_control=Measuredevices.MeasureControl()
        # self.measure_control_callback= #这个关于测量器暂时不需要
        self.recall_class=recall_class
        self.motor_status=""#目前我们不单独管这个motor_status,作为一个未来的开发点
        self.coordinate=(0,0,0)

    def add_cmd(self,new_coordinate,measure:bool)->None:#*这个命令可能会经常发生改变
        nl=list(new_coordinate)
        nl.append(measure)
        cmd=tuple(nl)
        self.cmd_list.append(cmd)

    def change_motor_settings(self,newrefreshtime,newfeed_rate):
        self.motor_control.refreshtime=newrefreshtime
        self.motor_control.feed_rate=newfeed_rate

    @property
    def move_start(self):
        return self._move_start
    
    @move_start.setter
    def move_start(self,value):
        self._move_start=value
        if value:
           self._start_move_thread()
################################################################################
    @property
    def status(self):
        return self._status
    
    @status.setter
    def status(self,value):
        if self._status!=value:
            self._status=value
            self.recall_class.status_refresh_call(value)
            
    def _cmd_parser(self,rough_cmd):
        #*这个命令会分别返回三个内容:移动控制命令,测量控制命令,显示命令
        SingleMove=namedtuple("SingleMove",["x","y","z"])
        MultiMove=namedtuple("Multimove",["len","movelist"])
        SingleMeasure=namedtuple("SingleMeasure",["x","y","z","measure_flag"])
        ShowInfo=namedtuple("ShowInfo",["endx","endy","endz","measure_flag"])
        #首先是基本命令(由三个位移以及一个测量符号位构成,几乎不需要任何操作)
        if len(rough_cmd)==4:
            movecontrolcmd=SingleMove(*rough_cmd[0:3])
            Measuredevicescmd=SingleMeasure(*rough_cmd)
            infocmd=ShowInfo(*rough_cmd)
        else:#多个移动直接无中断进行
            xl=rough_cmd[0:-1:3]
            yl=rough_cmd[1:-1:3]
            zl=rough_cmd[2:-1:3]
            ml=[(x,y,z) for x,y,z in zip(xl,yl,zl)]
            movecontrolcmd=MultiMove(len(ml),ml)
            Measuredevicescmd=SingleMeasure(*ml[-1],rough_cmd[-1])
            infocmd=ShowInfo(*Measuredevicescmd)
        return movecontrolcmd,Measuredevicescmd,infocmd

    def _move(self)->None:
        while(1):#循环执行这个指令,直到没有命令或者停止
            if self.move_start:
                if self.cmd_list:
                    self.cmd_now=self.cmd_list.pop(0)
                    self._processing=True
                    self.status="MOVING"
                    #print("Running!")#DEBUG
                    moc,mec,infc=self._cmd_parser(self.cmd_now)
                    self.recall_class.cmd_refresh_call(infc)
                    try:#执行命令
                        self.motor_control.run(moc)
                    except:#应对各种可能的异常(例如撞车,掉线之类)
                        raise
                    #判断是否需要测量
                    if self._check_measure():
                        self.status="MEASURING"
                        try:
                            self.measure_control.measure(mec)
                            self.recall_class.measure_refresh_call(self.measure_control.rough_measure_out)
                        except:#应对测量时的各种可能异常
                            raise                 
                else:#没有命令了
                    #raise ValueError("There is no more command!")
                    self._processing=False
                    self.status="NO CMD"
                    self.move_start=False#关闭自动前进
                    self.recall_class.no_command_call()
                    return
            else:#命令停止
                self._processing=False
                self.status="STOP"
                return
    def _start_move_thread(self):
        if not self._processing:#避免重复添加
            th=threading.Thread(target=self._move)
            th.start()

    def _check_measure(self)->bool:
        return self.cmd_now[3]
    
if __name__=="__main__":
    pass