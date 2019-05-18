import csv
import os
import math
import loss_rate_functions as func

#Need to write the following to a csv file
#					Loss of L   			Diff. H - L 
#	500						  0 - 1 				0 - 1						0 - 1

#Directory where results are stored
pathTo = "/home/mario/Documents/research/Current/"
#This command changes the directory to where the waf commands are enabled
os.chdir(pathTo)
#The only real variable changing in the filename
bottleneck = ['1Mbps', '1.5Mbps', '2Mbps', '2.5Mbps', '3Mbps', '3.5Mbps', '4Mbps', '4.5Mbps', '5Mbps', '5.5Mbps', '6Mbps','6.5Mbps', '7Mbps', '7.5Mbps', '8Mbps', '8.5Mbps', '9Mbps', '9.5Mbps', '10Mbps']
#File name arguments needed
index_of_bottleneck = 0
test_args ={}
test_args['bottleneck'] = bottleneck[index_of_bottleneck]
test_args['aptPriority'] = 'H'
test_args['initialPacketTrainLength'] = 150

#helper array to loop through bandwidths
outcome = []

for y in bottleneck:

	test_args['bottleneck'] = bottleneck[index_of_bottleneck]
	#high apt priority
	test_args['aptPriority'] = 'H'
	#filename manip
	fpath = "spq_n_prime_6_bottleneck_"
	fpath += test_args['bottleneck'] 
	fpath += "_"
	fpath += test_args['aptPriority']
	fpath += "_"
	fpath += ".csv-H"

	spq_high_loss_rate = func.calc_loss_rate_switch(fpath, "SPQ-H", 150)

	#Low Apt Priority run
	test_args['aptPriority'] = 'L'
	#filename manip
	fpath = "spq_n_prime_6_bottleneck_"
	fpath += test_args['bottleneck'] 
	fpath += "_"
	fpath += test_args['aptPriority']
	fpath += "_"
	fpath += ".csv-L"
	#read file and to a list for further processing
	spq_low_loss_rate = func.calc_loss_rate_switch(fpath, "SPQ-L", 0)

	diff = 0.0
	diff =abs(spq_high_loss_rate - spq_low_loss_rate)
	outcome.append({'Bottleneck':bottleneck[index_of_bottleneck], 'High_Loss_Rate':str(spq_high_loss_rate), 'Low_Loss_Rate':str(spq_low_loss_rate), 'delta':str(diff)})

	index_of_bottleneck += 1

#For debugging
#print outcome

#keys = outcome[0].keys()
with open('spq_loss_rate_ratio_n_prime_6.csv', 'wb') as output_file:
	dict_writer = csv.DictWriter(output_file, fieldnames=["Bottleneck", "High_Loss_Rate", "Low_Loss_Rate", "delta"], delimiter=';')
	dict_writer.writeheader()
	dict_writer.writerows(outcome)