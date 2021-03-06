#Needed to run command line arguments in python
import os
#Example ./waf Command for SPQ Simulation
#./waf --run "priority-queue-sim --ReceiveQueueSizeR2=10000 --TXQueueSizeS=400000000 --TXQueueSizeR1=6000000 --TXQueueSizeR2=15000000 --SR1DataRate=10Mbps 
#--R1R2DataRate=10Mbps --R2RDataRate=10Mbps --SR1Delay=5ms --R1R2Delay=5ms --R2RDelay=5ms --packetSize=10 --outputFile=./output_files/temp.dat 
#--interPacketTime=0.00000001 --separationPacketTrainLength=1000 --initialPacketTrainLength=150 --numAptPriorityProbes=500 --aptPriority='H'"

#Directory where waf commands will compile
pathTo = "/home/mario/repos/ns-allinone-3.14.1/ns-3.14.1"

#This command changes the directory to where the waf commands are enabled
os.chdir(pathTo)

#Experiment Settings Default Values
spq_args = {}
spq_args['ReceiveQueueSizeR2'] = '10000'
spq_args['TXQueueSizeS'] = '400000000'
spq_args['TXQueueSizeR1'] = '6000000'
spq_args['TXQueueSizeR2'] = '15000000'
spq_args['SR1DataRate'] = '10Mbps' #Sending Rate
spq_args['R1R2DataRate'] = '2Mbps' #actual bottleneck
spq_args['R2RDataRate'] = '10Mbps' 
spq_args['SR1Delay'] = '5ms' 
spq_args['R1R2Delay'] = '5ms'
spq_args['R2RDelay'] = '5ms' 
spq_args['packetSize'] = '1024' 
spq_args['interPacketTime'] = '0.00000001'
spq_args['separationPacketTrainLength']='6' #N' changed to 4 for most recent run
spq_args['initialPacketTrainLength']='150'
spq_args['numAptPriorityProbes']='5000'
spq_args['aptPriority']='H'
spq_args['highPriorityQueueSize'] = '75'
spq_args['lowPriorityQueueSize'] = '75'

#change values here to alter the name of the first half outputfile
spq_args['outputFile'] = './spq' + '_n_prime_6_bottleneck_'

#automate for high and low 
#For High Apt Priority
bottleneck = ['1Mbps', '1.5Mbps', '2Mbps', '2.5Mbps', '3Mbps', '3.5Mbps', '4Mbps', '4.5Mbps', '5Mbps', '5.5Mbps', '6Mbps','6.5Mbps', '7Mbps', '7.5Mbps', '8Mbps', '8.5Mbps', '9Mbps', '9.5Mbps', '10Mbps']
index = 0
for x in bottleneck:
	comp_cmds = './waf --run "'
	comp_cmds += 'priority-queue-sim'
	comp_cmds += ' --outputFile=' + spq_args['outputFile'] + bottleneck[index] + '_H_' + '.csv' #rest of outputfile name
	comp_cmds += ' --packetSize=' + spq_args['packetSize']
	comp_cmds += ' --ReceiveQueueSizeR2=' + spq_args['ReceiveQueueSizeR2']
	comp_cmds += ' --TXQueueSizeS=' + spq_args['TXQueueSizeS']
	comp_cmds += ' --TXQueueSizeR1=' + spq_args['TXQueueSizeR1']
	comp_cmds += ' --TXQueueSizeR2=' + spq_args['TXQueueSizeR2']
	comp_cmds += ' --SR1DataRate=' + spq_args['SR1DataRate']
	comp_cmds += ' --R1R2DataRate=' +  bottleneck[index]
	comp_cmds += ' --R2RDataRate=' + spq_args['R2RDataRate']
	comp_cmds += ' --SR1Delay=' + spq_args['SR1Delay']
	comp_cmds += ' --R1R2Delay=' + spq_args['R1R2Delay']
	comp_cmds += ' --R2RDelay=' + spq_args['R2RDelay']
	comp_cmds += ' --interPacketTime=' + spq_args['interPacketTime']
	comp_cmds += ' --separationPacketTrainLength=' + spq_args['separationPacketTrainLength']
	comp_cmds += ' --initialPacketTrainLength=' + spq_args['initialPacketTrainLength']
	comp_cmds += ' --numAptPriorityProbes=' + spq_args['numAptPriorityProbes']
	comp_cmds += ' --aptPriority=' + 'H' 
	comp_cmds += ' --highPriorityQueueSize=' + spq_args['highPriorityQueueSize']
	comp_cmds += ' --lowPriorityQueueSize=' + spq_args['lowPriorityQueueSize']
	comp_cmds += '"'
	os.system(comp_cmds) 
	index +=1

#For Low Apt Priority
index = 0
for x in bottleneck:
	comp_cmds = './waf --run "'
	comp_cmds += 'priority-queue-sim'
	comp_cmds += ' --outputFile=' + spq_args['outputFile'] + bottleneck[index] + '_L_' + '.csv' #rest of outputfile name
	comp_cmds += ' --packetSize=' + spq_args['packetSize']
	comp_cmds += ' --ReceiveQueueSizeR2=' + spq_args['ReceiveQueueSizeR2']
	comp_cmds += ' --TXQueueSizeS=' + spq_args['TXQueueSizeS']
	comp_cmds += ' --TXQueueSizeR1=' + spq_args['TXQueueSizeR1']
	comp_cmds += ' --TXQueueSizeR2=' + spq_args['TXQueueSizeR2']
	comp_cmds += ' --SR1DataRate=' + spq_args['SR1DataRate']
	comp_cmds += ' --R1R2DataRate=' +  bottleneck[index]
	comp_cmds += ' --R2RDataRate=' + spq_args['R2RDataRate']
	comp_cmds += ' --SR1Delay=' + spq_args['SR1Delay']
	comp_cmds += ' --R1R2Delay=' + spq_args['R1R2Delay']
	comp_cmds += ' --R2RDelay=' + spq_args['R2RDelay']
	comp_cmds += ' --interPacketTime=' + spq_args['interPacketTime']
	comp_cmds += ' --separationPacketTrainLength=' + spq_args['separationPacketTrainLength']
	comp_cmds += ' --initialPacketTrainLength=' + spq_args['initialPacketTrainLength']
	comp_cmds += ' --numAptPriorityProbes=' + spq_args['numAptPriorityProbes']
	comp_cmds += ' --aptPriority=' + 'L' 
	comp_cmds += ' --highPriorityQueueSize=' + spq_args['highPriorityQueueSize']
	comp_cmds += ' --lowPriorityQueueSize=' + spq_args['lowPriorityQueueSize']
	comp_cmds += '"'
	os.system(comp_cmds) 
	index +=1