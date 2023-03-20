import command_exchange as ce
from time import sleep
import re
import loguru

class MoveControlCallbacksBase:
    def status_refresh_call(self,newstatus:str):
        return
    
    def crash_call(self,crash_info):
        return
    
    def coordinate_refresh_call(self,newcoordinate:tuple):
        return

class MotorInterface:
    def __init__(self,port="COM6",log=None,call_backs=MoveControlCallbacksBase(),
                 get_stat_freq=10) -> None:
        self.log=log
        self.motor=ce.Motor(port,3,log)
        self._status=""
        self._call_backs=call_backs
        self._coordinate=(0,0,0)
        self._unlock()
        self._home()
        self.refresh_time=1/get_stat_freq
     
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
            self._call_backs.coordinate_refresh_call(self._coordinate)
        return
        
    def _unlock(self):
        self.motor.send_cmd("$X")
        self.logger("The motor is unlocked!")
        
    def _home(self):
        self.logger("Start backing home!")
        self.motor.send_cmd("$H")
        self.logger("Back home!")
        
    def _get_coordinate(self):#!注意,这个函数还有刷新状态的功能
        stat=self.motor.send_cmd("?")[0]
        s=re.findall("<(.+?),",stat)[0]
        if self._status!=s:
            self._status=s
            self._call_backs.status_refresh_call(s)
        cstrlist=re.findall("MPos:(.+?),(.+?),(.+?),",stat)[0]
        coordinate=[float(i) for i in cstrlist]
        self.coordinate=coordinate
        return
    
    def _wait_for_complete(self):
        self._get_coordinate()
        while(self._status=="Run"):
            sleep(self.refresh_time)
            self._get_coordinate()
        
    def logger(self,str,type="info"):
        if self.log:
            if type=="info":
                self.log.info(str)
            elif type=="debug":
                self.log.debug(str)
            else:
                self.log.warning(str)
                
    def _send_cmd(self,cmd) -> None:
        #这里的cmd是用来指示的三元组
        #TODO 这里使用G0控制移动,因为G1还要专门指定速度,不需要
        cmd="G0 X{0} Y{1} Z{2}".format(*cmd)
        self.motor.send_cmd(cmd)
        
    #*这里要添加一个sync方法是为了以后可能使用蚁群算法导致路线过度曲折
    def _send_cmd_sync(self,cmdlist) ->None:
        #! 这里的cmdlist可能会溢出,但是这是小概率事件,暂且先不考虑
        cmds=["G0 X{0} Y{1} Z{2} F{3}".format(*i) for i in cmdlist]
        for cmd in cmds:
            self.motor.send_cmd(cmd)
            
    def run(self,cmds):#*Controller会调用的一个函数,控制移动
        if len(cmds)==3:
            self._send_cmd(cmds)
        else:
            self._send_cmd_sync(cmds.movelist)
        self._wait_for_complete()
        return 
    
if __name__=="__main__":
    m=MotorInterface("COM5",loguru.logger)
    m._send_cmd((-10,-20,-20))