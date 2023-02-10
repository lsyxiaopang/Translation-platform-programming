import serial
from time import sleep
import re

class MoveControlCallbacksBase:
    def status_refresh_call(self,newstatus:str):
        return
    
    def crash_call(self,crash_info):
        return
    
    def coordinate_refresh_call(self,newcoordinate:tuple):
        return

class MoveControl:
    def __init__(self,port="COM6",callback=MoveControlCallbacksBase()) -> None:
        self.ser=serial.Serial(port,baudrate=115200)
        if not self.ser.closed:#如果串口已经打开,那么重启
            self.ser.close()
        self.ser.open()   
        self._stat=""
        self.ser.write("\r\n\r\n".encode("ascii"))
        self._wait_check()#*将初始化信息读入
        self.feed_rate=100
        self.refreshtime=0.5#*状态更新时间
        self.callback=callback
        self._coordinate=(0,0,0)
        self._get_status()   

    def run(self,cmds):#*Controller会调用的一个函数,控制移动
        if len(cmds)==3:
            self._send_cmd(cmds)
        else:
            self._send_cmd_sync(cmds.movelist)
        self._get_status()
        while(self.stat=='Run'):#阻塞至运行结束
            sleep(self.refreshtime)
            self._get_status()
        return    
    ###################################################
    def _send_cmd(self,cmd) -> None:
        #这里的cmd是用来指示的三元组
        cmd="G1 X{0} Y{1} Z{2} F{3}\r\n".format(*cmd,self.feed_rate)
        self.ser.write(cmd.encode("ascii"))
        self._wait_check()
    #*这里要添加一个sync方法是为了以后可能使用蚁群算法导致路线过度曲折
    def _send_cmd_sync(self,cmdlist) ->None:
        #! 这里的cmdlist可能会溢出,但是这是小概率事件,暂且先不考虑
        cmds=["G1 X{0} Y{1} Z{2} F{3}\r\n".format(*i,self.feed_rate) for i in cmdlist]
        for cmd in cmds:
            self.ser.write(cmd.encode('ascii'))
            self._wait_check()
    
    @property
    def stat(self):
        return self._stat
    @stat.setter
    def stat(self,value):
        if(self._stat!=value):
            self._stat=value
            self.callback.status_refresh_call(self._stat)
    @property
    def coordinate(self):
        return self._coordinate
    @coordinate.setter
    def coordinate(self,value):
        diff=False
        for i in range(3):
            if self._coordinate[i]!=value[i]:
                self._coordinate=value
                diff=True
        if diff:
            self.callback.coordinate_refresh_call(self._coordinate)
        return

    def _get_status(self)->None:
        cmd="?\r\n"
        self.ser.write(cmd.encode("ascii"))
        pos_info=self._wait_check()[0]
        self.ser.readline()
        reline="<(.*),MPos:(.*),(.*),(.*)>"
        matchobj=re.match(reline,pos_info)
        stat=matchobj.group(1)
        x=float(matchobj.group(2))
        y=float(matchobj.group(3))
        z=float(matchobj.group(4))
        self.stat=stat
        self.coordinate=(x,y,z)
        return

    def _wait_check(self)->None:
        bak1=self.ser.readline().decode("utf-8")[:-2]
        bak2=self.ser.readline().decode("utf-8")[:-2]#*去掉\r\n
        #print(bak1,bak2)
        return bak1,bak2

    def _check_status(self):#*用于上级调用
        return self.stat,self._coordinate


if __name__=="__main__":
    try:
        test=MoveControl()
        test._send_cmd((10,20,30))
        sleep(1)
        test._get_status()
        test._send_cmd((30,20,10))
    finally:
        test.ser.close()
    
