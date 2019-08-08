print "Creating device device01"
go

disk init name="device01",
   physname="/opt/sybase/devices/device01.dat",
   vdevno=3,
   size=10240
go

print "Creating device device02"
go

disk init name="device02",
   physname="/opt/sybase/devices/device02.dat",
   vdevno=4,
   size=5120
go


