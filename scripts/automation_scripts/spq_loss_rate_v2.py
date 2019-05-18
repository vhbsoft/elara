import csv
import os
import math
import loss_rate_functions as func
#This will be to calculate the loss rate in the effects of M

#Need to write the following to a csv file
#					Loss of L   			Diff. H - L 
#	500						  0 - 1 				0 - 1						0 - 1


#Directory where results are stored
pathTo = "/home/mario/Documents/research/Sim/SPQDataRun/TheEffectsOfN/"
#This command changes the directory to where the waf commands are enabled
os.chdir(pathTo)
#The only real variable changing in the filename
sep_packet_train = ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '20', '50', '100']
#File name arguments needed
index_of_sep_packet_train = 0
test_args ={}
test_args['sep_packet_train'] = sep_packet_train[index_of_sep_packet_train]
test_args['aptPriority'] = 'H'
test_args['initialPacketTrainLength'] = 150

#helper array to loop through bandwidths
outcome = []

for y in sep_packet_train:

	test_args['sep_packet_train'] = sep_packet_train[index_of_sep_packet_train]
	#high apt priority
	test_args['aptPriority'] = 'H'
	#filename manip
	fpath = "spq_num_of_sep_packet_train_"
	fpath += test_args['sep_packet_train'] 
	fpath += "_"
	fpath += test_args['aptPriority']
	fpath += "_"
	fpath += ".csv-H"

	spq_high_loss_rate = func.calc_loss_rate_switch(fpath, "SPQ-H", 150)

	#Low Apt Priority run
	test_args['aptPriority'] = 'L'
	#filename manip
	fpath = "spq_num_of_sep_packet_train_"
	fpath += test_args['sep_packet_train'] 
	fpath += "_"
	fpath += test_args['aptPriority']
	fpath += "_"
	fpath += ".csv-L"
	#read file and to a list for further processing
	spq_low_loss_rate = func.calc_loss_rate_switch(fpath, "SPQ-L", 0)


	diff = 0.0
	diff =abs(spq_high_loss_rate - spq_low_loss_rate)

	outcome.append({'Number_of_Sep_Packet_Train':sep_packet_train[index_of_sep_packet_train], 'High_Loss_Rate':str(spq_high_loss_rate), 'Low_Loss_Rate':str(spq_low_loss_rate), 'delta':str(diff)})

	index_of_sep_packet_train += 1

print outcome

#keys = outcome[0].keys()
with open('spq_loss_rate_n.csv', 'wb') as output_file:
	dict_writer = csv.DictWriter(output_file, fieldnames=["Number_of_Sep_Packet_Train", "High_Loss_Rate", "Low_Loss_Rate", "delta"], delimiter=';')
	dict_writer.writeheader()
	dict_writer.writerows(outcome)