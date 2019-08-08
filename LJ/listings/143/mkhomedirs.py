#!/usr/bin/python

import sys
import os
import ldap

if not len(sys.argv) > 1:
	print "USAGE: %s <User Netgroup 1> [[User Netgroup 2] ...]"%(sys.argv[0])
	sys.exit(0)

ldap.set_option(ldap.OPT_X_TLS,1)
ldap.set_option(ldap.OPT_X_TLS_TRY,1)

ciLDAP=ldap.open("ldap.example.com",636)
ciLDAP.simple_bind_s()

users=[]

doPrint=0

if sys.argv[1] == 'DUMMY-print':
	print "DEBUG ENABLED"
	if len(sys.argv) > 2:
		doPrint=1
		netgroupList=sys.argv[2:]
	else:
		netgroupList=sys.argv[1:]
else:
	netgroupList=sys.argv[1:]

for netgroup in netgroupList:
	userList=ciLDAP.search_s("ou=netgroups,o=ci,dc=example,dc=com",ldap.SCOPE_SUBTREE,"(cn=%s)"%(netgroup),('nisNetgroupTriple',))
	if userList:
		if userList[0][1].keys().__contains__('nisNetgroupTriple'):
			userList=userList[0][1]['nisNetgroupTriple'];

			for name in userList:
				parsedName=name.split(',')[1];
				if not users.__contains__(parsedName):
					users.append(parsedName)
		else:
			print "WARNING: No users found in netgroup %s"%(netgroup)
	else:
		print "WARNING: Netgroup %s does not exist"%(netgroup)

if not users:
	print "No users found!"
	sys.exit(1)

if doPrint:
	print "DEBUG: " + str(users)

for user in users:
	userInfo=ciLDAP.search_s("ou=people,o=ci,dc=example,dc=com",ldap.SCOPE_SUBTREE,"(uid=%s)"%(user),('gidNumber','sshPublicKey'))
	userInfo=userInfo[0][1];

	if doPrint:
		print "DEBUG:"
		print userInfo
		continue

	if os.system('[ -d /home/%s ] '%(user)):
		os.system('mkdir /home/%s'%(user))
		os.system('cp -R /etc/skel/* /etc/skel/.[^.]* /home/%s > /dev/null 2>&1'%(user))
		os.system('mkdir /home/%s/.ssh > /dev/null 2>&1'%(user))
		os.system('chmod 700 /home/%s/.ssh > /dev/null 2>&1'%(user))
		os.system('chown -R %s:%s /home/%s'%(user,userInfo['gidNumber'][0],user))
		print "Created home directory for %s"%(user)

	if userInfo.keys().__contains__('sshPublicKey'):
		if os.system('[ -d /home/%s/.ssh ]'%(user)):
			os.system('mkdir /home/%s/.ssh > /dev/null 2>&1'%(user))
			os.system('chmod 700 /home/%s/.ssh > /dev/null 2>&1'%(user))
			os.system('chown -R %s:%s /home/%s/.ssh > /dev/null 2>&1'%(user,userInfo['gidNumber'][0],user))

		if (os.system('[ -f /home/%s/.ssh/authorized_keys ]'%(user))) and (len(userInfo['sshPublicKey'])):
			authKeyFile=open('/home/%s/.ssh/authorized_keys'%(user),'w');
			for sslKey in userInfo['sshPublicKey']:
				authKeyFile.write(sslKey);
				authKeyFile.write('\n');
			authKeyFile.close();
			os.system('chown -R %s:%s /home/%s/.ssh > /dev/null 2>&1'%(user,userInfo['gidNumber'][0],user))
			print "* Added authorized_keys file for %s"%(user)

