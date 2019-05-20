import csv
import os
import math
import loss_rate_functions as func

#Need to write the following to a csv file
#Number of packets 			Loss of H 				Loss of L   			Diff. H - L 
#	500						  0 - 1 				0 - 1						0 - 1

#Directory where results are stored
pathTo = "/home/mario/Documents/research/Sim/DelayerDataRun/All100Mbps/"
#This command changes the directory to where the waf commands are enabled
os.chdir(pathTo)

num_of_packets = ['500', '1000', '1500', '2000', '2500', '3000', '3500', '4000', '4500', '5000']

index = 0

outcome = []
outcome_ni = []

# for y in num_of_packets:
# 	#filename manip
# 	fpath = "delayer_10_micro_x_z_100Mbps"
# 	fpath += "_num_of_packets_"
# 	fpath += num_of_packets[index]
# 	fpath += ".csv"

# 	delay_loss_rate_ni = func.calc_loss_rate_switch(fpath, "DELAYER-NI", 0)
# 	delay_loss_rate = func.calc_loss_rate_switch(fpath, "DELAYER", 0)

# 	outcome.append({'Number_of_Packets': num_of_packets[index], 'Loss_Rate' :str(delay_loss_rate) })
# 	outcome_ni.append({'Number_of_Packets': num_of_packets[index], 'Loss_Rate' :str(delay_loss_rate_ni) })

# 	index += 1

# print outcome
# print outcome_ni

# # #keys = outcome[0].keys()
# with open('delay_loss_rate_100Mbps_without_ni_final.csv', 'wb') as output_file:
# 	dict_writer = csv.DictWriter(output_file, fieldnames=["Number_of_Packets", "Loss_Rate"], delimiter=';')
# 	dict_writer.writeheader()
# 	dict_writer.writerows(outcome)
# with open('delay_loss_rate_100Mbps_with_ni_final.csv', 'wb') as output_file:
# 	dict_writer = csv.DictWriter(output_file, fieldnames=["Number_of_Packets", "Loss_Rate"], delimiter=';')
# 	dict_writer.writeheader()
# 	dict_writer.writerows(outcome_ni)

delay = ['10000', '100000', '1000000', '10000000' ]
numPackets = 100

for y in range(100, 5500, 100):
		if numPackets > 5000:
			break
		# 	#filename manip
		#	For 1microsecond
		fpath = "delayer_100Mbps_"
		fpath += delay[0]
		fpath += "ns_num_of_packets_"
		fpath += str(numPackets)
		fpath += ".csv"

		gpath = "delayer_100Mbps_"
		gpath += delay[1]
		gpath += "ns_num_of_packets_"
		gpath += str(numPackets)
		gpath += ".csv"

		hpath = "delayer_100Mbps_"
		hpath += delay[2]
		hpath += "ns_num_of_packets_"
		hpath += str(numPackets)
		hpath += ".csv"

		ipath = "delayer_100Mbps_"
		ipath += delay[3]
		ipath += "ns_num_of_packets_"
		ipath += str(numPackets)
		ipath += ".csv"

		delay_loss_rate_10microsec = func.calc_loss_rate_switch(fpath, "DELAYER", 0)
		delay_loss_rate_100microsec = func.calc_loss_rate_switch(gpath, "DELAYER", 0)
		delay_loss_rate_1ms = func.calc_loss_rate_switch(hpath, "DELAYER", 0)
		delay_loss_rate_10ms = func.calc_loss_rate_switch(ipath, "DELAYER", 0)

		outcome.append({'Number_of_Packets': str(numPackets), 'Loss_Rate_10_microsec' :str(delay_loss_rate_10microsec),'Loss_Rate_100_microsec' :str(delay_loss_rate_100microsec),'Loss_Rate_1ms' :str(delay_loss_rate_1ms),'Loss_Rate_10ms' :str(delay_loss_rate_10ms) })
		numPackets += 100

#for debugging 
#print outcome

with open('delay_loss_rate_all_100Mbps.csv', 'wb') as output_file:
	dict_writer = csv.DictWriter(output_file, fieldnames=["Number_of_Packets", "Loss_Rate_10_microsec", "Loss_Rate_100_microsec", "Loss_Rate_1ms", "Loss_Rate_10ms"], delimiter=';')
	dict_writer.writeheader()
	dict_writer.writerows(outcome)