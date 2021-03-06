# 4-node-additional-delay
# Example Usage:

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
delay_args = {}
delay_args['compression'] = '0'
#new arguments for shaper/delayer
delay_args['isShaper'] = '0'
delay_args['processingDelay'] = '10000' #in nano seconds
delay_args['entropy'] = 'l'
delay_args['numPackets'] = '10'
delay_args['packetSize'] = '1000'

delay_args['interPacketTime'] = '0.001'
delay_args['s0p0Delay'] = '30ms'
delay_args['p0p1Delay'] = '30ms'

delay_args['p1r0Delay'] = '30ms'

# delay_args['p0p1DataRate'] = '100Mbps'
delay_args['p1r0DataRate'] = '100Mbps'
delay_args['s0p0DataRate'] = '100Mbps'

delay_args['queueMode'] = 'p'

delay_args['s0QueueSize'] = '655350000'
delay_args['p0QueueSize'] = '75' 		#75 if delayer/queue size of delayer
delay_args['p1QueueSize'] = '655350000' #75 if shaper/queue size of shaper


#change values here to alter the name of the first half outputfile
delay_args['outputFile'] = './delayer_100Mbps_'

delay = ['10000', '100000', '1000000', '10000000' ]
numPackets = 100
counter = 0

while True:
	if counter != 0:
		numPackets = 100
	if counter > 3:
		break
	for y in range(500, 5500, 100):
		if numPackets > 5000:
			break
		comp_cmds = './waf --run "'
		comp_cmds += 'delayer'
		comp_cmds += ' --outputFile=' + delay_args['outputFile'] + delay[counter]+ 'ns' + '_num_of_packets_' +  str(numPackets) + '.csv' 
		comp_cmds += ' --isShaper=' + delay_args['isShaper']
		comp_cmds += ' --processingDelay=' + delay[counter]
		comp_cmds += ' --entropy=' + delay_args['entropy']
		comp_cmds += ' --numPackets=' + str(numPackets)
		comp_cmds += ' --packetSize=' + delay_args['packetSize']
		comp_cmds += ' --interPacketTime=' + delay_args['interPacketTime']
		comp_cmds += ' --s0p0Delay=' + delay_args['s0p0Delay'] 
		comp_cmds += ' --p0p1Delay=' + delay_args['p0p1Delay'] 
		comp_cmds += ' --p1r0Delay=' + delay_args['p1r0Delay'] 
		comp_cmds += ' --s0p0DataRate=' + delay_args['s0p0DataRate']
		comp_cmds += ' --p1r0DataRate=' + delay_args['p1r0DataRate']
		comp_cmds += ' --compression=' + delay_args['compression']
		comp_cmds += ' --queueMode=' + delay_args['queueMode']
		comp_cmds += ' --s0QueueSize=' + delay_args['s0QueueSize']
		comp_cmds += ' --p0QueueSize=' + delay_args['p0QueueSize']
		comp_cmds += ' --p1QueueSize=' + delay_args['p1QueueSize']
		comp_cmds += '"'
		os.system(comp_cmds) 
		numPackets += 100
	counter += 1