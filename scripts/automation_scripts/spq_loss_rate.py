import csv
import os
import math
import loss_rate_functions as func
#This will be to calculate the loss rate in the effects of M

#Need to write the following to a csv file
#	Number 				Loss of L   			Diff. 							H - L 
#	500						  0 - 1 				0 - 1						0 - 1

#Directory where results are stored
pathTo = "/home/mario/Documents/research/NewSPQ/nprimeis3"
#This command changes the directory to where the waf commands are enabled
os.chdir(pathTo)
#The only real variable changing in the filename
apt_priority_packets = ['10', '20', '50', '100', '200', '300', '400', '500', '1000', '2000', '3000', '4000', '5000', '6000', '7000', '8000', '9000', '10000']
#File name arguments needed
index_of_apt = 0
test_args ={}
test_args['apt_priority_packets'] = apt_priority_packets[index_of_apt]
test_args['aptPriority'] = 'H'
test_args['initialPacketTrainLength'] = 150

#helper array to loop through bandwidths
outcome = []

for y in apt_priority_packets:

	test_args['apt_priority_packets'] = apt_priority_packets[index_of_apt]
	#high apt priority
	test_args['aptPriority'] = 'H'
	#filename manip
	fpath = "spq_num_of_apt_priority_packets_"
	fpath += test_args['apt_priority_packets'] 
	fpath += "_"
	fpath += test_args['aptPriority']
	fpath += "_"
	fpath += ".csv-H"
	#read file and to a list for further processing
	#set up to run through high apt priority array a
	spq_high_loss_rate = func.calc_loss_rate_switch(fpath, "SPQ-H", 150)

	#Low Apt Priority run
	test_args['aptPriority'] = 'L'
	#filename manip
	fpath = "spq_num_of_apt_priority_packets_"
	fpath += test_args['apt_priority_packets'] 
	fpath += "_"
	fpath += test_args['aptPriority']
	fpath += "_"
	fpath += ".csv-L"
	#read file and to a list for further processing

	#set up to run through high apt priority array a
	spq_low_loss_rate = func.calc_loss_rate_switch(fpath, "SPQ-L", 0)

	diff = 0.0
	diff =abs(spq_high_loss_rate - spq_low_loss_rate)

	outcome.append({'Number_of_Apt_Priority_Packets':apt_priority_packets[index_of_apt], 'High_Loss_Rate':str(spq_high_loss_rate), 'Low_Loss_Rate':str(spq_low_loss_rate), 'delta':str(diff)})

	index_of_apt += 1

print outcome

#keys = outcome[0].keys()
with open('spq_loss_rate_m_nprime_3.csv', 'wb') as output_file:
	dict_writer = csv.DictWriter(output_file, fieldnames=["Number_of_Apt_Priority_Packets", "High_Loss_Rate", "Low_Loss_Rate", "delta"], delimiter=';')
	dict_writer.writeheader()
	dict_writer.writerows(outcome)