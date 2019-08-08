#!/usr/bin/env python


##~     HomeAutomation.py
##~     This program provides a driveway monitoring system,
##~     water alarm monitoring and it can monitor other alarm inputs 
##~      
##~     Copyright (C) 2005  Fred Stelter
##~ 
##~     This program is free software; you can redistribute it and/or modify
##~     it under the terms of the GNU General Public License as published by
##~     the Free Software Foundation; either version 2 of the License, or
##~     (at your option) any later version.
##~ 
##~     This program is distributed in the hope that it will be useful,
##~     but WITHOUT ANY WARRANTY; without even the implied warranty of
##~     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##~     GNU General Public License for more details.
##~ 
##~     You should have received a copy of the GNU General Public License
##~     along with this program; if not, write to the Free Software
##~     Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
##~ 
##~     http://www.gnu.org/copyleft/gpl.html


#HomeAutomation.py
#Written by: Fred Stelter
#Start Date: 20031005
#Modified Date: 20050707
#

#Future Enhancements
    #Use voice modem vocp dial phone number and play a message.


#Required modules - 
        #python, python - pyserial, 
        #pyserial - http://pyserial.sourceforge.net/
#External commands executed by this module:  zip, sox, curl       
        #Curl - http://curl.haxx.se/
        #Zip - http://www.info-zip.org/pub/infozip/Zip.html
        #Sox - http://sox.sourceforge.net
        
#Other External programs used by system
      #Motion - http://www.lavrsen.dk/twiki/bin/view/Motion/WebHome  
      #See Linux Journal Issue 131 March 2005 for Motion article
#Hardware - Notes
        #--------------IO serial port controlled box
        #http://store.qkits.com/moreinfo.cfm/QK108 $50
        #Linux supported webcam that works with motion.
        #--------------Drive alert - Mier Products
        #http://www.absoluteautomation.com/drive_alert/ $199
        #--------------Water alarm plans and parts
        #http://www.mitedu.freeserve.co.uk/Circuits/Alarm/walarm.htm
        #The only part required for the computer detected alarm is the BC109C
        #and a 6-12v dc adapter.
        #---------------Fire alarm  
        #Purchase a fire alarm with light $15
        #Remove cover and attach input leads to light terminals.
        #NOTE: do not replace existing fire alarms with this modified version since it could fail
        #and is only intended to possibly warn you if no one is home 
        
#Known BUGS - bug fixes completed 2005-7-25
#Corrected    #1. motion's cp fails on lastsnap.jpg file if motion has just opened the file
            ##Reconfigure motion so lastsnap is a softlink to most current
            ##image.  This elminates copying of a blank file
            ##The default motion snapshot_filename value works fine
            ##but motion will create a snapshot file according to the
            ##snapshot_interval which should be set to 1 
            ##to catch all possible activity.
            ##
            ##Create MotionClean.py to clean up the mess left by the 
            ##snap_shotinterval = 1 setting.  I setup a cronjob which
            ##executes MotionClean.py every 2 seconds.  This keeps the
            ##jpg files down to a manageable level.
            ##Corrected    #2. Serial port connection is lost when a short power outage occurs
            ##Reconfigured program so that it check for expect reads.
            ##This allows it to flush the serial connection until a
            ##non '' read is obtained.   
#Corrected    #3. No built in handling of RemoteSSH threads.  Multiple copies could be started
          #    on the same remote host.  I'll probably add a stopp check to these threads using
          #   a global variable.
        
import ConfigParser
from getopt import getopt    
import MailAttachment as MA
import os 
import popen2
import serial, shutil, string, sys
import threading, time

MAX_IMAGETHREAD_RUNTIME=6
DEFAULT_NUMBER_CAMIMAGES = 10

#Parse config file
configp=ConfigParser.ConfigParser()
configp.read('homeautomation.conf')

MonitorInputLst = []
tmpInputList = configp.get('serialinputrelaybox', 'monitorinputs').split(',')
for m in tmpInputList:
    MonitorInputLst.append(m.replace('\n','')) 
InputAssignment = eval(configp.get('serialinputrelaybox','inputassignment'))
#Do all monitor inputs have an input assignment
for mi in MonitorInputLst:
    if InputAssignment.has_key(mi) == 0:
        print 'ERROR: monitorinputs %s contains an input that does not have an inputassignment' %mi
        
MailHost        = configp.get('email','mailhost')
MailList        = configp.get('email','maillist').split() 
FromEmail       = configp.get('email', 'fromemail')

WavCmd          = configp.get('wave','wavcmd')
WavRepititions  = configp.getint('wave','wavrepititions')

ZipCmd          = configp.get('zip','zipcmd') 

AlarmSounds = {}
for ao in configp.options('alarmwav'):
    AlarmSounds[ao] = configp.get('alarmwav', ao)
    
camDirs = {}
for cd in configp.options('camdir'):
    camDirs[cd] = configp.get('camdir', cd)
    
CamCOMMANDS = {}
for cam in configp.options('camcmd'):
    CamCOMMANDS[cam] = eval(configp.get('camcmd', cam)) #Remove outer quotes
    
NumCamImages = {}
for nci in configp.options('camimages'):
    NumCamImages[nci] = configp.getint('camimages', nci)

#Verify that there is a camDir for every cam in the camCOMMANDS dict
for ccmd in CamCOMMANDS.keys():
    if camDirs.has_key(ccmd) == 0:
        print 'ERROR: configuration file cmd %s does not have an output directory' %ccmd
    else:
        tstr = camDirs[ccmd] +'%s'
        CamCOMMANDS[ccmd] = CamCOMMANDS[ccmd]%tstr
        
    if NumCamImages.has_key(ccmd) == 0: #Num cam images not present set to default
        NumCamImages[ccmd] = DEFAULT_NUMBER_CAMIMAGES
        print 'WARNING: Setting number of cam images for %s to %d' %(ccmd, DEFAULT_NUMBER_CAMIMAGES)
        print '         Add %s to homeautomation.conf ' %ccmd

#Remote machine commands
RemoteCMDS = {}
for rc in configp.options('remotemachinecmd'):
    RemoteCMDS[rc] = eval(configp.get('remotemachinecmd', rc))

IMAGE_DELAY = configp.getint('camimagedelay','image_delay')
INPUTTIMEOUT = configp.getfloat('inputtimeout', 'input_timeout')
ALARMSLEEP  = configp.getint('sleeptimes','alarmsleeptime') 
QUIET = configp.getint('debug','quiet')


#--------------------------------------------------------------------------------------
def PrintConfigs():
    """
        Print config file info to stdout 
    """
    print '\n\n-----IO Box Inputs---------------------------------'
    print 'Monitor Inputs: %s' %MonitorInputLst 
    print 'Assign Inputs to Alarm Type'
    for ik in InputAssignment.keys():
        print '     %s: %s' %(ik, InputAssignment[ik])
    print '\n\n-----Emails----------------------------------------'
    print 'MailHost(smtp) server: %s' %MailHost
    print 'Send notification email to: '
    cnt = 0
    pstr = '     '
    for m in MailList:
        pstr += m
        if cnt == 1:
            pstr += '\n     '
            cnt = 0
        else:
            cnt += 1
    print pstr
    print 'Emails sent from: %s' %FromEmail      
    print '\n\n-----Wave files------------------------------------'
    print 'Play wave files with: %s' %WavCmd  
    print 'Play wave files %s times' %WavRepititions
    print 'Wave file sounds for defined alarm types\n' 
    for ak in AlarmSounds.keys():
        print '     %s: %s' %(ak,AlarmSounds[ak])
    print '\n\n-----Zip files-------------------------------------'
    print 'Zip files with command %s' %ZipCmd        
    print '\n\n-----Camera information----------------------------'
    print 'Directories'
    for ck in camDirs.keys():
        print '     %s: %s' %(ck, camDirs[ck])
    print 'Images per cam'
    for nk in NumCamImages.keys():
        print '     %s: %s' %(nk, NumCamImages[nk])
    print 'Camera image delay: %s' %IMAGE_DELAY
    print '\n\n-----Remote commands-------------------------------'
    for rk in RemoteCMDS.keys():
        print '     %s: %s' %(rk, RemoteCMDS[rk])
    print '\n\n-----Other-----------------------------------------'
    print 'Sleep time between alarm/drive executions: %s' %ALARMSLEEP
    print 'Quiet: %s' %QUIET
    print '\n\n'
    print 'Usage: HomeAutomation: -d = run  -c = print configuration\n\n'
    
#-------------------------------------------------------------------------------------- 
def RemoveImages():
    tmstmp = time.strftime("%Y%m%d_%H_%M_%S", time.localtime(time.time()))
    for cdir in camDirs.keys():
        os.system('rm ' + camDirs[cdir] + '*.jpg')
        try:
            shutil.move(camDirs[cdir] + '%s.zip' % cdir, camDirs[cdir] + '%s%s.zip' %(cdir, tmstmp))
        except:
            if QUIET == 0:
                print 'Could not backup %s.zip file' %cdir
    
#-------------------------------------------------------------------------------------------------------------------------        
class PipeReader(threading.Thread):
    """
        Written by: Bil Richter
    """
    def __init__(self, srcFileObj, buffsize=1024):
        self.infp = srcFileObj
        self.buffsize = buffsize
        threading.Thread.__init__(self)
        self.setDaemon(1)

    def run(self):
        self.dataBlocks = []
        while 1:
            block = self.infp.read(self.buffsize)
            if not block: break
            self.dataBlocks.append(block)
        self.infp.close()

    def getdata(self):
        return string.join(self.dataBlocks, '')
    
#-------------------------------------------------------------------------------------------------------------------------        
class ThreadMonitor(threading.Thread):
    def __init__(self, threads = []):
        self.threads = threads
        threading.Thread.__init__(self)
    def run(self):
        """
        """
        #Create thread monitor list
        ThreadsStopped = 0
        ThreadList = []
        if QUIET == 0:
            print 'self.threads: %s' %`self.threads` 
        while ThreadsStopped == 0:
               if QUIET == 0:
                   print '---------------MONITOR THREAD----------------'
               time.sleep(10)
               ThreadsStopped = 1
               ThrdCnt = 0
               for thrd in self.threads:
                  if thrd._Thread__stopped == 0:
                    ThreadsStopped = 0
                    CurrTime = int(time.strftime("%H%M%S",time.localtime(time.time())))
                    if QUIET == 0:
                        print '%s Thread CurrTime %d    Begin Time %s' %(`thrd`,CurrTime,`thrd.JobBegin`)
                    if ((CurrTime - thrd.JobBegin) > MAX_IMAGETHREAD_RUNTIME):
                         if QUIET == 0:
                            print 'Killing PID %s ' %thrd.p.pid
                         try: 
                            os.kill(thrd.p.pid, 9) 
                         except:
                            if QUIET == 0:
                                print 'ERROR: Process id %s does not exist' %(str(thrd.p.pid))
                  ThrdCnt += 1
        if QUIET == 0:
            print '   Monitor thread exiting'
            
#-------------------------------------------------------------------------------------------------------------------------
class MonitorInputs(threading.Thread):
    """
       
    """
    def __init__(self, Inputs = []):
        self.Inputs = Inputs
        self.InputLastActivity = {}
        self.InputThreads = {}
        for inp in Inputs: #Create input activity dict
            self.InputLastActivity[inp] = 0.00
            self.InputThreads[inp] = None
        self.serdevice = serial.device(int(configp.get('serialport','serial')))
        self.ser = serial.Serial(self.serdevice) 
        self.ser.timeout = 1 #Time out after 4 seconds
        threading.Thread.__init__(self)
        self.setDaemon(True)
        self.InputActivity = '' #Input id activity detected on
    def run(self):
        s= '\n\n=========================================='
        s += '============================================'
        if QUIET == 0:
            print s
            print 'Monitoring Inputs %s' %`self.Inputs`
        istat = 0
        while 1:
            for inp in self.Inputs:
                istat = self.GetInputStatus(inp)
                if istat == -1: #Serial port error - reconnect
                    if QUIET == 0:
                        print 'Serial Port Read Error: reconnecting'
                    time.sleep(2)
                    self.ser.flush()
                    while self.ser.read() != '':
                        print '---->flushing'
                elif istat == 1: #Do something if 
                    CurrTime = float(time.strftime("%Y%m%d%H%M%S",time.localtime(time.time())))
                    if QUIET == 0:
                        print '________________Activity Detected %f_____________________' %(CurrTime)
                    if (CurrTime - self.InputLastActivity[inp]) > INPUTTIMEOUT or\
                        self.InputLastActivity[inp] == 0:
                        if QUIET == 0:
                            print 'Activity detected on %s assignment %s' %(inp,InputAssignment[inp][0])
                        self.InputLastActivity[inp] = CurrTime
                        if InputAssignment[inp][0] == 'drive': #Start drive alert thread
                            if QUIET == 0:
                                print '--------------Activity is the DriveAlert------------'
                            if self.InputThreads[inp] == None:
                                Drv = DriveAlert()
                                Drv.start()
                                self.InputThreads[inp] = Drv
                            elif self.InputThreads[inp]._Thread__stopped == 1:
                                Drv = DriveAlert()
                                Drv.start()
                                self.InputThreads[inp] = Drv
                        elif InputAssignment[inp][0] == 'alarm': #Start alarm thread
                            if QUIET == 0:
                                print '--------------Activity is the Alarm-------------------'
                            if self.InputThreads[inp] == None:
                                Alm = Alarm(InputAssignment[inp][1])    
                                Alm.start()
                                self.InputThreads[inp] = Alm
                            elif self.InputThreads[inp]._Thread__stopped == 1:
                                Alm = Alarm(InputAssignment[inp][1])    
                                Alm.start()
                                self.InputThreads[inp] = Alm
    
    def GetInputStatus(self, Input = None):
            self.ser.write(Input + '\r\n')
            #print '\n\n'
            Expect = [Input[0], Input[1], '\r', '\n']
            cnt = 0
            while cnt <= 3:
                a = self.ser.read()
                #print '     a1: %s' %`a`
                if a == '' or a != Expect[cnt]:
                    return -1
                cnt += 1
            val= self.ser.read()
            #print '     Value: %s' %`val`
            Expect = ['\r', '\n', '#']
            cnt = 0
            while cnt <= 2:
                a = self.ser.read()
                #print '     a2: %s' %`a`
                if a == '' or a != Expect[cnt]:
                    return -1
                cnt += 1
            if val == '1':
                return 1
            else:
                return 0
            

    def RelaySwitch(self, Relay = None):
        """
            This method is intended for the serial IO input/relay module
            relay control.  If the inputs are routed through the relays this
            code could control the voltage to the inputs.  This could keep 
            it from sending a load of emails of the input voltage remains on
            for an extended period.
        """
        if Relay != None:
            ser.write('R' + str(Relay) + '\r\n')
            pass


    def RelayStatus(self, Relay = None):
        if Relay != None:
            pass
    

#-------------------------------------------------------------------------------------------------------------------------
class ZipIt(threading.Thread):
    def __init__(self, cam = None):
        self.cam = str(cam)
        if camDirs.has_key(self.cam):
            self.camZipFile = camDirs[self.cam] + self.cam + ".zip"
            self.camDir = camDirs[self.cam]
        else:
            self.camZipFile = None
        threading.Thread.__init__(self)
    def run(self):
        if self.camZipFile != None:
            try:
                os.remove(self.camZipFile)
            except:
                estr= "ERROR:  " + self.camZipFile
                if QUIET == 0:
                    print estr
            cmd = ZipCmd + " " + self.camZipFile + " " + self.camDir + "/*.jpg"
            if QUIET == 0:
                print 'ZIP Cmd: %s' %`cmd`
            self.p = popen2.Popen3("exec " + cmd, 1024)
            self.errReader = PipeReader(self.p.childerr); self.errReader.start()
            self.outReader = PipeReader(self.p.fromchild); self.outReader.start()
            try:
                self.p.wait()
            except OSError, (errno, errnostr):
                if QUIET == 0:
                    print 'ERROR: ZipIt self.p.wait Errno %s: %s' %(`errno`, `errnostr`)
            except:
                if QUIET == 0:
                    print 'ERROR: ZipIt unknown error.' 
            #Check of file
            if os.path.isfile(self.camZipFile) == 0:
                self.camZipFile = None 
                if QUIET == 0:
                    print 'ERROR: failed to create zip file for %s images' %self.cam
            
#-------------------------------------------------------------------------------------------------------------------------
class GetImage(threading.Thread): 
    def __init__(self, cam = None, numImages = 1):
        self.cam       = cam
        self.JobBegin  = -1
        self.camCmd    = CamCOMMANDS[cam]
        self.numImages = NumCamImages[cam]
        self.Zip = None
        threading.Thread.__init__(self)
        
    def run(self):
        for i in range(self.numImages):
            self.JobBegin = int(time.strftime("%H%M%S",time.localtime(time.time())))
            if QUIET == 0:
                print 'Getting %s image' %self.cam
            filename = time.strftime("%H%M%S", time.localtime(time.time())) + '.jpg'
            execcmd = self.camCmd %filename
            #if QUIET == 0:
            #    print 'popening %s' %execcmd
            self.p = popen2.Popen3("exec " + execcmd, 1024)
            self.errReader = PipeReader(self.p.childerr); self.errReader.start()
            self.outReader = PipeReader(self.p.fromchild); self.outReader.start()
            try:
                self.p.wait()
            except OSError, (errno, errnostr):
                if QUIET == 0:
                    print 'ERROR: GetImage self.p.wait Errno %s: %s' %(`errno`, `errnostr`)
            except:
                if QUIET == 0:
                    print 'ERROR: self.p.wait Unknown error'
            #try:
            #    if QUIET == 0:
            #        print 'cam popen pipereader  %s %s' %(self.errReader.getdata(),self.outReader.getdata())
            #except OSError, (errno, errnostr):
            #    if QUIET == 0:
            #        print 'ERROR: GetImage popen output pipereader Errno %s: %s' %(`errno`, `errnostr`)
            #except: pass
            time.sleep(IMAGE_DELAY)
        #Popen complete - create zipfiles
        self.Zip = ZipIt(self.cam)
        self.Zip.start()
        self.Zip.join() #Wait on zip file creation
    
        
#--------------------------------------------------------------------------------------------- 
class DriveAlert(threading.Thread):
    def __init__(self):
        self.threads = []
        threading.Thread.__init__(self)
    def run(self):
        #Start and wait on cam image threads
        if QUIET == 0:
            print 'DriveAlert Starting Alarm'
        alarmThread = Alarm('drivealert')
        alarmThread.start()
        for rcmdkey in RemoteCMDS.keys():
            if remotethreads.has_key(rcmdkey):
                if remotethreads[rcmdkey]._Thread__stopped == 1:
                    if QUIET == 0:
                        print 'Starting remote %s' %`RemoteCMDS[rcmdkey]`
                    sshrem = SSHRemote(RemoteCMDS[rcmdkey])
                    remotethreads[rcmdkey] = sshrem
                    remotethreads[rcmdkey].start()
                elif QUIET == 0:
                    print 'SSH remote already running %s' %`RemoteCMDS[rcmdkey]` 
            else:
                if QUIET == 0:
                    print 'Starting remote %s' %`RemoteCMDS[rcmdkey]`
                sshrem = SSHRemote(RemoteCMDS[rcmdkey])
                remotethreads[rcmdkey] = sshrem
                remotethreads[rcmdkey].start()
        if QUIET == 0:
            print 'CamCOMMANDS: %s' %`CamCOMMANDS`
        for cam in CamCOMMANDS.keys():
            if QUIET == 0:
                print '=========Creating %s Image Thread=========' %cam
            thrd = GetImage(cam)
            thrd.start()
            self.threads.append(thrd)
            
        #Start monitor thread to kill off any curl hangups
        TMon = ThreadMonitor(self.threads)
        TMon.start()
        
        #Wait on threads
        for thrd in self.threads:
            thrd.join()
            
        #Threads are now complete - build zipfile list
        AttachFiles = []
        if QUIET == 0:
            print `threads`
        for thrd in self.threads:
            if QUIET == 0:
                print thrd.Zip.camZipFile
            if thrd.Zip.camZipFile != None:
                AttachFiles.append(thrd.Zip.camZipFile)
                
        tstr = '------------------Threads Complete----------------------'
        if QUIET == 0:
            print tstr
        tmstmp = time.strftime("%Y%m%d_%H_%M_%S", time.localtime(time.time()))
        MA.MAIL(SendTo = MailList, AttchFiles = AttachFiles,\
                From = FromEmail, Content = 'Driveway Capture',\
                Subject = 'Driveway Capture ' + tmstmp)

        tstr = 'Images Mailed ' + tmstmp
        if QUIET == 0: 
            print tstr
        RemoveImages()
    
#-------------------------------------------------------------------------------------------------------------------------            
class Alarm(threading.Thread):
    def __init__(self, AlarmType = ''):
        self.AlarmType = AlarmType
        threading.Thread.__init__(self)
        
    def run(self):
        if QUIET == 0:
            print 'Alarm Starting PlayWav %s' %self.AlarmType
        Pwav = PlayWav(self.AlarmType)
        Pwav.start()
        
        tmstmp = time.strftime("%Y%m%d_%H_%M_%S", time.localtime(time.time()))
        if QUIET == 0:
            print '%s Alarm Detected %s' %(self.AlarmType, `tmstmp`)
     
        MA.MAIL(SendTo = MailList, AttchFiles = [],\
                From = FromEmail, Content = '%s Alarm Detected' %self.AlarmType,\
                Subject = '%s Alarm Detected %s' %(self.AlarmType.upper(), tmstmp))
        Pwav.join()
        time.sleep(ALARMSLEEP)

#-------------------------------------------------------------------------------------------------------------------------        
class PlayWav(threading.Thread):
    def __init__(self, alarmtype = ''):
        if AlarmSounds.has_key(alarmtype):
            alarmwav = AlarmSounds[alarmtype]
        else:
            alarmwav = ''
        self.wavcmd = "exec " + WavCmd + ' ' + alarmwav
        threading.Thread.__init__(self)
        
    def run(self): 
        for i in range(WavRepititions):
            if QUIET == 0:
                print 'Playing %s' %`self.wavcmd`
            os.system(self.wavcmd)
                  
#-------------------------------------------------------------------------------------------------------------------------      
class SSHRemote(threading.Thread):
    def __init__(self, remotecmd = ''):
        self.cmd = remotecmd
        threading.Thread.__init__(self)
    def run(self):
        if QUIET == 0:
            print 'SSHRemote executing %s' %self.cmd
        os.system(self.cmd)
            
#-------------------------------------------------------------------------------------------------------------------------      
def main():
    global threads, remotethreads
    remotethreads = {}
    threads = []
    MonThread = MonitorInputs(MonitorInputLst)
    MonThread.start()
    MonThread.join()
            
#-------------------------------------------------------------------------------------------------------------------------      
if __name__ == '__main__':
    optlist, args = getopt(sys.argv[1:], 'cd')
    if '-c' in optlist[0]:
        PrintConfigs()
    elif '-d' in optlist[0]:
        try:
            RemoveImages()
        except: 
            pass
        main()
    else:
        print 'Usage: HomeAutomation: -d = run  -c = print configuration' 
