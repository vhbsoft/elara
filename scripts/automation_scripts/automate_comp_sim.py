import os

#This script is used to automate the waf commands that run compression simulation in NS-3.

#OUTPUT
#This will generate a .csv file with the following format
#
#	 _______________________________________________	
#	|		Packet id 		|		Timestamp 		|
#	|_______________________|_______________________|
#	|		  XXXX			|			-1			|
#	|_______________________|_______________________|	
#


#Example run of compression sim using waf commands for reference
#This example has every parameter that is possible present.
#./waf --run "4node-compression --outputFile=./test.dat --entropy=h 
#--numPackets=1000  --packetSize=1024 --interPacketTime=.0000001 --s0p0Delay=30ms 
#--p0p1Delay=30ms --p1r0Delay=30ms --s0p0DataRate=5Mbps --p0p1DataRate=4Mbps 
#--p1r0DataRate=5Mbps --queueMode=b --s0QueueSize=655350000 --p0QueueSize=655350000 
#--p1QueueSize=655350000  --compression=1"

#Directory where NS-3 is located waf commands can be run here.
pathTo = "/home/mario/repos/ns-allinone-3.14.1/ns-3.14.1"

#This command changes the directory to where the waf commands are enabled
os.chdir(pathTo)

#The below command is a simple example waf command of how to execute the compression simulation.
#os.system('./waf --run "compression-sim --numPackets=50 --packetSize=500"')

compression_args = {}

#Experiment Settings Default Values
compression_args['compression'] = '1'
compression_args['queueMode'] = 'p'
compression_args['entropy'] = 'h'
compression_args['packetSize'] = '1100'
compression_args['numPackets'] = '500'
compression_args['interPacketTime'] = '0.00000001'
compression_args['s0QueueSize'] = '655350000'
compression_args['p0QueueSize'] = '75' #Compressor Transmission queue size
compression_args['p1QueueSize'] = '655350000'
compression_args['p0p1DataRate'] = '2Mbps' #Compression link
compression_args['s0p0DataRate'] = '5Mbps' 
compression_args['p1r0DataRate'] = '5Mbps'
compression_args['s0p0Delay']='30ms'
compression_args['p0p1Delay']='30ms'
compression_args['p1r0Delay']='30ms'

#change values here to alter the name of the first half outputfile
compression_args['outputFile'] = './loss_' 

#helper array to loop through bandwidths
bands = [2, 4 , 5, 6]
counter = 0						#for str(bands[counter]) and test_args['bandwidth'] 
outcome = []
numPackets = 500

#loop through arguments and outputs to a .csv file in the pathTo folder
#you can change the outputfile type if needed

while True:
	if counter != 0:
		numPackets = 500
	if counter > 0:
		break
	for y in range(500, 10500, 500):
		if numPackets > 10000:
			break

		#high entropy turn
		comp_cmds = './waf --run "'
		comp_cmds += 'compression-sim'
		comp_cmds += ' --outputFile=' + compression_args['outputFile'] + str(bands[counter]) +'Mbps' + '_num_of_packets_' + str(numPackets) + '_H_' + '.csv'
		comp_cmds += ' --packetSize=' + compression_args['packetSize']
		comp_cmds += ' --s0p0DataRate=' + compression_args['s0p0DataRate']
		comp_cmds += ' --p1r0DataRate=' + compression_args['p1r0DataRate']
		comp_cmds += ' --compression=' + compression_args['compression']
		comp_cmds += ' --queueMode=' + compression_args['queueMode']
		#altered params
		comp_cmds += ' --numPackets=' + str(numPackets)
		comp_cmds += ' --entropy=' + 'h'
		comp_cmds += ' --p0p1DataRate=' + str(bands[counter]) + 'Mbps'
		comp_cmds += ' --p0QueueSize=' + compression_args['p0QueueSize']
		comp_cmds += '"'
		os.system(comp_cmds) 
		#Low entropy turn
		comp_cmds = './waf --run "'
		comp_cmds += 'compression-sim'
		comp_cmds += ' --outputFile=' + compression_args['outputFile'] + str(bands[counter]) +'Mbps' + '_num_of_packets_' + str(numPackets) + '_L_' + '.csv'
		comp_cmds += ' --packetSize=' + compression_args['packetSize']
		comp_cmds += ' --s0p0DataRate=' + compression_args['s0p0DataRate']
		comp_cmds += ' --p1r0DataRate=' + compression_args['p1r0DataRate']
		comp_cmds += ' --compression=' + compression_args['compression']
		comp_cmds += ' --queueMode=' + compression_args['queueMode']
		#altered params
		comp_cmds += ' --numPackets=' + str(numPackets)
		comp_cmds += ' --entropy=' + 'l'
		comp_cmds += ' --p0p1DataRate=' + str(bands[counter]) + 'Mbps'
		comp_cmds += ' --p0QueueSize=' + compression_args['p0QueueSize']
		comp_cmds += '"'
		os.system(comp_cmds) 
		numPackets += 500

	counter = counter + 1
