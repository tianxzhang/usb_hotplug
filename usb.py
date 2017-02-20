#!/usr/bin/env python
#coding=utf8
#import glib
import gobject
#import mpdor
#apt-get install python-gudev
import gudev
import pynotify
import sys
import os
import time

def callback(client, action, device, user_data):
    device_vendor = device.get_property("ID_VENDOR_ENC")
    device_model = device.get_property("ID_MODEL_ENC")
    device_parent = device.get_parent()
    print "subsystem",device.get_subsystem()
    print "devtype",device.get_devtype()
    print "name",device.get_name()
    print "number", device.get_number()
    print "sysfs_path:", device.get_sysfs_path()
    print "driver:", device.get_driver()
    print "action:", device.get_action()
    print "seqnum:", device.get_seqnum()
    print "device type:", device.get_device_type()
    print "device number:", device.get_device_number()
    print "device file:",device.get_device_file()
    print "device file symlinks:", ", ".join(device.get_device_file_symlinks())
    print "device keys:", ", ".join(device.get_property_keys())
    type(device.get_property_keys())
    for device_key in device.get_property_keys():
	print "device property %s: %s" % (device_key, device.get_property(device_key))
    

    device_sysfs = device.get_sysfs_path()
    device_sysfs_net = device_sysfs +"/"+ device_sysfs.split('/')[-1] + ":1.0/" + "net/usb0/"
    device_sysfs_net1 = device_sysfs + "/" + device_sysfs.split('/')[-1] + ":1.0/" + "net/usb1/"
    isExists=os.path.exists(device_sysfs_net)
    if not isExists:
    	device_sysfs_net = device_sysfs_net1
    isExists=os.path.exists(device_sysfs_net)
    if not isExists:
	n = pynotify.Notification("USB Device Removed","It seems that it is not a usb device")
        
    #print device_sysfs
    #print device_sysfs_net
    if action == "add":
       	#pass
	#time.sleep(1)
  	#print device_sysfs
  	#print device_sysfs_net
	#n = os.system('./info.sh ' + device_sysfs + ' ' +  device_sysfs_net)
        print 'add'
        #n = os.system('./gw.sh')
	#n = pynotify.Notification("USB Device Added","%s %s is now connected to your system" %(device_vendor,device_model))
	#n.show()
    elif action == "remove":
	#pass
 	#os.system('python ...')
        print 'remove'
	#n = pynotify.Notification("USB Device Removed","%s %s has been disconnected from your system" %(device_vendor,device_model))
	#n.show()
if not pynotify.init("USB Device Notifier"):
    sys.exit("Couldn't connect to the notification daemon!")

client = gudev.Client(["usb/usb_device"])
client.connect("uevent", callback, None)
#client=mpdor.client.Client()
#client.connect("uevent",callback)

loop = gobject.MainLoop()
loop.run()
