import serial
import loguru
import re
class MotorError(Exception):
    def __init__(self, value,log=None) -> None:
        self.value = value
        if(log):
            log.error(value)
    def __str__(self) -> str:
        return repr(self.value)

class Motor:
    def __init__(self,port="COM6",init_len=3,log=None) -> None:
        """__init__ 与一个电机建立连接

        Parameters
        ----------
        port : str, optional
            连接用端口
        init_len : int, optional
            初始化信息行数,默认为3
        """    
        self.log=log
        self.logger("Connecting to the motor ...")
        try:
            self.ser=serial.Serial(port,baudrate=115200)
            if not self.ser.closed:#如果串口已经打开,那么重启
                self.ser.close()
            self.ser.open()  
        except Exception as e:
            raise MotorError(str(e),self.log)
        self.ser.write("\r\n\r\n".encode("ascii"))
        init_message=[self.ser.readline().decode("utf-8") for i in range(init_len)]
        self.version=self._check_version(init_message[1])
        self.logger("The version of the grbl is {}".format(self.version))
        self.logger("Connected!")
        
    def _check_version(self,version_str)->str:
        out=re.findall(r"Grbl\s(.+)\s\[",version_str)
        if len(out)>0:
            return out[0]
        else:
            raise MotorError("Cannot get version value!",self.log) 
    
    def logger(self,str,type="info"):
        if self.log:
            if type=="info":
                self.log.info(str)
            elif type=="debug":
                self.log.debug(str)
            else:
                self.log.warning(str)
                
    def _get_parse_back(self)->list:
        """get_parse_back 得到发送信息后的返回

        Returns
        -------
        list
            除了最后两行的其他信息

        Raises
        ------
        MotorError
            返回的命令中存在error
        """        
        back_data=[]
        nlen=0
        while(True):
            data=self.ser.readline()
            datastr=data.decode("utf-8")[:-2]
            if datastr=="ok":
                nlen+=1
                if(nlen==2):
                    for i in back_data:
                        self.logger(i,"debug")
                    return back_data
            if datastr[0:5]=="error" or datastr[0:5]=="ALARM":#发生错误
                raise MotorError(datastr,self.log)
            else:
                back_data.append(datastr)
            
    def send_cmd(self,cmd:str)->str:
        """send_cmd 发送一条命令

        Parameters
        ----------
        cmd : str
            要发送的命令内容(没有换行)

        Returns
        -------
        str
            该命令的返回
        """        
        ncmd=cmd+"\r\n"
        self.ser.write(ncmd.encode("ascii"))
        return self._get_parse_back()
            
            
if __name__=="__main__":  
    Motor("COM5",log=loguru.logger)  