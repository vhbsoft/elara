#Shaper Simulation Run for NS-3

#when is a shaper pass a big number like 1 million to delayer arg
#when not a shaper pass a small number like 10 to the delayer
#shaper = set isShaper to True 
#delayer = set isShaper to Flase

#Needed to run command line arguments in python
import os

#Directory where waf commands will compile
pathTo = "/home/mario/repos/ns-allinone-3.14.1/ns-3.14.1"

#This command changes the directory to where the waf commands are enabled
os.chdir(pathTo)

#Experiment Settings Default Values
shaper_args = {}
shaper_args['compression'] = '0'
#new arguments for shaper/delayer
shaper_args['isShaper'] = '1' #set to one for true it is shaper or shaping is on
shaper_args['processingDelay'] = '10000' #in nano seconds does not matter for shaper because it never gets used in scratch file
shaper_args['entropy'] = 'l'
shaper_args['numPackets'] = '5000'
shaper_args['packetSize'] = '1024'

shaper_args['interPacketTime'] = '0.001'

shaper_args['s0p0Delay'] = '30ms'
shaper_args['p0p1Delay'] = '30ms'
shaper_args['p1r0Delay'] = '30ms'

# shaper_args['p0p1DataRate'] = '100Mbps'
shaper_args['p1r0DataRate'] = '2Mbps' #bottleneck for shaper
shaper_args['s0p0DataRate'] = '10Mbps' #sending rate

shaper_args['queueMode'] = 'p'

shaper_args['s0QueueSize'] = '655350000'
shaper_args['p0QueueSize'] = '655350000' 		#75 if delayer/queue size of delayer
shaper_args['p1QueueSize'] = '75'				#75 if shaper/queue size of shaper

num_of_packets = ['500', '1000', '1500', '2000', '2500', '3000', '3500', '4000', '4500', '5000', '5500', '6000']

bottleneck = ['500kbps', '1Mbps', '2Mbps', '3Mbps', '4Mbps', '5Mbps', '6Mbps', '7Mbps', '8Mbps', '9Mbps', '10Mbps']

#Outputfile name start here changes within the for loop depending on argument
#For number of packets args
#shaper_args['outputFile'] = './shaper' + '_num_of_packets_'
#For bottleneck args
shaper_args['outputFile'] = './shaper' + '_bottleneck_'

#automate for high and low 
#For High Apt Priority
index = 0
for x in bottleneck:
	comp_cmds = './waf --run "'
	comp_cmds += 'delayer'
	comp_cmds += ' --outputFile=' + shaper_args['outputFile'] + bottleneck[index] + '.csv' 
	comp_cmds += ' --isShaper=' + shaper_args['isShaper']
	comp_cmds += ' --entropy=' + shaper_args['entropy']
	comp_cmds += ' --numPackets=' + shaper_args['numPackets']								#num_of_packets[index]
	comp_cmds += ' --packetSize=' + shaper_args['packetSize']
	comp_cmds += ' --interPacketTime=' + shaper_args['interPacketTime']
	comp_cmds += ' --s0p0Delay=' + shaper_args['s0p0Delay'] 
	comp_cmds += ' --p0p1Delay=' + shaper_args['p0p1Delay'] 
	comp_cmds += ' --p1r0Delay=' + shaper_args['p1r0Delay'] 
	comp_cmds += ' --s0p0DataRate=' + shaper_args['s0p0DataRate']
	comp_cmds += ' --p1r0DataRate=' + 	bottleneck[index]									#shaper_args['p1r0DataRate'] 
	comp_cmds += ' --compression=' + shaper_args['compression']
	comp_cmds += ' --queueMode=' + shaper_args['queueMode']
	comp_cmds += ' --s0QueueSize=' + shaper_args['s0QueueSize']
	comp_cmds += ' --p0QueueSize=' + shaper_args['p0QueueSize']
	comp_cmds += ' --p1QueueSize=' + shaper_args['p1QueueSize']
	comp_cmds += '"'
	os.system(comp_cmds) 
	index +=1