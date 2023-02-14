import py_cui
import Controller
import pandas as pd

def get_measure_out(rough_measure_out)->list:
        rmo:pd.DataFrame=rough_measure_out
        outdatalist=[]
        for ind,data in rmo.iterrows():
            s="x={} xdata={} y={} ydata={} z={} zdata={}".format(
                data["inx"],data["xmean"],data["iny"],
                data["ymean"],data["inz"],data["zmean"]
            )
            outdatalist.append(s)
        return outdatalist

class ApplicationControllerCallback(Controller.ControllerCallbackBase):
    def __init__(self,cui) -> None:
        self.cui=cui
        super().__init__()

    def measure_refresh_call(self, datalist):
        measureout=get_measure_out(datalist)
        if measureout:
            self.cui.data_out.clear()
            self.cui.data_out.add_item_list(measureout)
        return super().measure_refresh_call(datalist)

    def status_refresh_call(self, newstatus: str):
        self.cui.check_flag(newstatus)
        return super().status_refresh_call(newstatus)
    
    def cmd_refresh_call(self, new_cmd):
        self.cui.refresh_cmd(new_cmd)
        return super().cmd_refresh_call(new_cmd)

    def no_command_call(self):
        self.cui.no_cmd()
        return super().no_command_call()

    def coordinate_refresh_call(self, newcoordinate: tuple):
        self.cui.refresh_coordinate(newcoordinate)
        return super().coordinate_refresh_call(newcoordinate)

class MoveControllerCUI:
    def __init__(self,master:py_cui.PyCUI) -> None:
        self.master=master
        self.data_out=self.master.add_scroll_menu("CMD Processed",0,0,5,5)
        self.in_process=self.master.add_text_box("In Process",5,0,1,3)
        self.status_now=self.master.add_text_box("Status now",5,3,1,2)
        self.place_now=self.master.add_text_box("Coordinate now",5,5,1,3)
        self.new_command=self.master.add_text_box("New CMD",7,0,1,4)
        self.change_moving_stat_button=self.master.add_button("Start",7,4,1,4,
                                                command=self.change_moving_stat)
        self.command_to_process=self.master.add_scroll_menu("CMD To Process",0,5,5,3)

        self.controller=Controller.Controller(None,"COM5",ApplicationControllerCallback(self))

        self.new_command.add_key_command(py_cui.keys.KEY_ENTER,self.add_cmd)

    def refresh_coordinate(self,coordinate):
        self.place_now.set_text("x:{}\ty:{}\tz:{}".format(coordinate[0],coordinate[1],coordinate[2]))
        return

    def refresh_cmd(self,ncmd):#TODO 修改为合理的形式
        cmd_now=ncmd
        if cmd_now:
            cmdstr="To x:{}\ty:{}\tz:{}".format(cmd_now[0],cmd_now[1],cmd_now[2])
            self.in_process.set_text(cmdstr)

    def refresh_todo(self):
        cmd_to_run=self.controller.cmd_list
        outlist=[]
        for cmd in cmd_to_run:
            cmdstr="To x:{}\ty:{}\tz:{}".format(cmd[0],cmd[1],cmd[2])
            outlist.append(cmdstr)
        self.command_to_process.clear()
        self.command_to_process.add_item_list(outlist)

    def refresh_status(self,newstatus):
        self.status_now.set_text(newstatus)

    def no_cmd(self):
        self.change_moving_stat_button.set_title("Start")
        self.master.show_warning_popup("No more command!","Please add more command to execute!")

    def check_flag(self,new_status):
        # self.refresh_cmd()
        self.refresh_todo()
        self.refresh_status(new_status)

    def add_cmd(self):
        cmdstr=self.new_command.get()
        data=cmdstr.split(',')
        self.controller.add_cmd(data,True)
        self.new_command.clear()
        self.refresh_todo()

    def change_moving_stat(self):
        if self.controller.move_start:
            self.controller.move_start=False
            self.change_moving_stat_button.set_title("Start")
        else:
            self.controller.move_start=True
            self.change_moving_stat_button.set_title("Stop")
        

    

root =py_cui.PyCUI(8,8)
root.toggle_unicode_borders()
root.set_title("3D Move Measure Controller")
root.set_refresh_timeout(0.1)
s =MoveControllerCUI(root)
root.start()