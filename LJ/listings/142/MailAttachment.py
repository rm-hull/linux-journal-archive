#!/usr/bin/env python

##~     MailAttachment.py
##~     This programs sends emails with attachments.
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

import smtplib, string, StringIO, sys
import threading
import MimeWriter, base64

MAILHOST = '127.0.0.1'
MAILHOST = 'smtp-server.hot.rr.com'

ExtensionDict = {
    'doc': 'application/word',
    'exe':'application/exe', 
    'EXE':'application/exe',
    'gif':'image/gif', 
    'gz':'application/gz',
    'jpeg':'image/jpg', 
    'JPEG':'image/jpg', 
    'jpg':'image/jpg', 
    'JPG':'image/jpg',
    'pdf':'application/pdf', 
    'py': 'application/python',
    'txt': 'application/text',
    'wps': 'application/wordperfect',
    'xls': 'application/excel',
    'zip':'application/zip', 
}

class MAIL:

  def __init__(self, SendTo = [], AttchFiles = [],  From = '',\
               Content = '', Subject = ''):
      self.SendTo = SendTo
      self.AttchFiles = AttchFiles
      self.From = From
      self.Content = Content
      self.Subject = Subject
      if len(self.SendTo) > 0 and From != '' :
        self.BuildMessage()
        
  def BuildMessage(self):
    message = StringIO.StringIO()
    mimeWriter = MimeWriter.MimeWriter(message)
    mimeWriter.addheader('Subject', self.Subject)
    mimeWriter.addheader('MIME-Version', '1.0')
    mimeWriter.startmultipartbody('mixed')

    if self.Content != '':
      part = mimeWriter.nextpart()
      body = part.startbody('text/plain')
      body.write(self.Content)

      #Add attachment files
      for af in self.AttchFiles:
         ext_lst = string.split(af, '.')
         ext = ext_lst[len(ext_lst)-1]
         if ExtensionDict.has_key(ext):
           app_type = ExtensionDict[ext]
         else:
           app_type = 'ext'
         fn_lst = string.split(af, '/')
         filename = fn_lst[len(fn_lst)-1]
         part = mimeWriter.nextpart()
         part.addheader('Content-Transfer-Encoding', 'base64')
         part.addheader('Content-Disposition','attachment;\n filename="%s"' %filename)
         body = part.startbody(app_type + "; name = " + filename)
         try:
           base64.encode(open(af, 'rb'), body)
         except:
           print 'IOError: No such file or dir ' + filename
    mimeWriter.lastpart()
    msg = message.getvalue()
    #Thread smtp send
    threads = []
    for sendto in self.SendTo:
        Snd = Send(MAILHOST, self.From, sendto, msg)
        Snd.start()
        threads.append(Snd)
    for t in threads: #wait on threads
        t.join()
    
class Send(threading.Thread):
    def __init__(self, MailHost='',fromem = '', sendto = '', sendmsg = ''):
        self.fromem = fromem
        self.sendto = sendto
        self.MailHost = MailHost
        self.sendmsg = sendmsg
        try:
            self.smtp = smtplib.SMTP(self.MailHost)
        except:
            self.smtp = None
        threading.Thread.__init__(self)
        
    def run(self):
        print 'Sending to: ' + `self.sendto`
        sendmsg = "From: " + self.fromem + "\r\nTo: " + self.sendto + "\n" + self.sendmsg
        good_send = 0
        if self.smtp != None:
            while good_send <= 10:
                try:
                    self.smtp.sendmail(self.fromem, self.sendto, sendmsg)
                    good_send = 100
                except:
                    print 'SendMail exception to %s' %self.sendto
                    good_send += 1
            self.smtp.quit()

