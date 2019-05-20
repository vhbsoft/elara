import csv
import os
import math
import loss_rate_functions as func #import for functions to calc loss rate

#Need to write the following to a csv file
#Number of packets 			Loss Rate			
#	500						  0 - 1 	

#OR

#Bottleneck      			Loss Rate			
#	500						  0 - 1 


#Directory where results are stored
pathTo = "/home/mario/Documents/research/Current"
#This command changes the directory to where the waf commands are enabled
os.chdir(pathTo)

#collect data in here to be written out to .csv file
outcome = []
outcome_ni = []

#arg manip due to changes in filename for 2 scenarios ratio and number of packets
num_of_packets = ['500', '1000', '1500', '2000', '2500', '3000', '3500', '4000', '4500', '5000', '5500', '6000', '6500', '7000', '7500', '8000', '8500', '9000', '9500','10000']
#Total Number of packets = 5000 for this experiment
bottleneck = ['500kbps','1Mbps',  '1.5Mbps',  '2Mbps', '2.5Mbps' ,'3Mbps',  '3.5Mbps',  '4.5Mbps', '5Mbps','5.5Mbps',  '6Mbps',  '6.5Mbps', '7Mbps', '7.5Mbps','8Mbps',  '8.5Mbps',  '9Mbps', '9.5Mbps', '10Mbps']
#bottleneck = ['2Mbps', '5Mbps' ,'7Mbps',  '10Mbps']
base = 0
superpacket_size = 2
#loss rate for ratio simulation
# index = 0
# for y in bottleneck:

# 	#filename manip for number of packets 
# 	fpath = "shaper"
# 	fpath += "_bottleneck_"
# 	fpath += bottleneck[index]
# 	fpath += ".csv"

# 	#function call to calc loss rate
# 	# the third argument is usually for initial packet train but I used it for number of packets in this scenario
# 	loss_rate_ni = func.calc_loss_rate_spq_switch(fpath, "SHAPER-NI", 5000, superpacket_size) 
# 	loss_rate = func.calc_loss_rate_spq_switch(fpath, "SHAPER", 5000, superpacket_size)

# 	delta_ni = loss_rate_ni - base
# 	delta = loss_rate - base
# 	#Store Bottleneck and 
# 	outcome.append({'Bottleneck': bottleneck[index], 'Loss_Rate' :str(loss_rate), "Base": base , "delta": delta})
# 	outcome_ni.append({'Bottleneck': bottleneck[index], 'Loss_Rate' :str(loss_rate_ni), "Base": base, "delta": delta_ni})

# 	index += 1


index = 0
for y in num_of_packets:
	#filename manip for number of packets 
	fpath = "shaper_"
	fpath += "num_of_packets_"
	fpath += num_of_packets[index]
	fpath += ".csv"
	loss_rate_ni = func.calc_loss_rate_spq_switch(fpath, "SHAPER-NI", int(num_of_packets[index]), superpacket_size)
	loss_rate = func.calc_loss_rate_spq_switch(fpath, "SHAPER", int(num_of_packets[index]), superpacket_size)
	delta_ni = loss_rate_ni - base
 	delta = loss_rate - base
	outcome.append({'Number_of_Packets': num_of_packets[index], 'Loss_Rate' :str(loss_rate), 'Base': base, "delta": delta })
	outcome_ni.append({'Number_of_Packets': num_of_packets[index], 'Loss_Rate' :str(loss_rate_ni), 'Base': base, "delta": delta_ni})
	index += 1

#for debugging
#print outcome
#print outcome_ni

#For Number of Packets
# #keys = outcome[0].keys()
with open('shaper_num_of_packets_loss_rate_ratio_without_ni_sp_2.csv', 'wb') as output_file:
	dict_writer = csv.DictWriter(output_file, fieldnames=["Number_of_Packets", "Loss_Rate", "Base", "delta"], delimiter=';')
	dict_writer.writeheader()
	dict_writer.writerows(outcome)
with open('shaper_num_of_packets_loss_rate_ratio_with_ni_sp_2.csv', 'wb') as output_file:
	dict_writer = csv.DictWriter(output_file, fieldnames=["Number_of_Packets", "Loss_Rate", "Base", "delta"], delimiter=';')
	dict_writer.writeheader()
	dict_writer.writerows(outcome_ni)

# # #keys = outcome[0].keys()
# with open('shaper_bottleneck_loss_rate_ratio_without_ni_sp_6.csv', 'wb') as output_file:
# 	dict_writer = csv.DictWriter(output_file, fieldnames=["Bottleneck", "Loss_Rate", "Base", "delta"], delimiter=';')
# 	dict_writer.writeheader()
# 	dict_writer.writerows(outcome)
# with open('shaper__bottleneck_loss_rate_ratio_with_ni_sp_6.csv', 'wb') as output_file:
# 	dict_writer = csv.DictWriter(output_file, fieldnames=["Bottleneck", "Loss_Rate", "Base", "delta"], delimiter=';')
# 	dict_writer.writeheader()
# 	dict_writer.writerows(outcome_ni)
