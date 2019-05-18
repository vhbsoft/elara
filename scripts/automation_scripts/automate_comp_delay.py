import csv
import os

#.csv filename format example: "test_1Mbps__num_of_packets_500_H_"
#the above filname was produced using "automate_comp_sim.py"

#INPUT
#Reads a .csv file with format below
#	 _______________________________________________	
#	|		Packet id 		|		Timestamp 		|
#	|_______________________|_______________________|
#	|		  XXXX			|			-1			|
#	|_______________________|_______________________|

#OUTPUT
#Writes out the following to a csv file
#Number of Packets			ID of Last Packet Received 		Bandwitdh 		Entropy 			
#500 							499						   	 1Mbps			  H	

#Directory where my .csv files are located
pathTo = "/home/mario/Documents/research/CompressionDataRun/all/"
#This command changes the directory to where the waf commands are enabled
os.chdir(pathTo)


#Experiment details broken down 
test_args = {}
#I left this as just Mbps because the actual value changes inside the main for loop.
test_args['bandwidth'] = 'Mbps'			
test_args['number_of_packets'] = '500'
#The only possible values for entropy are H for high and L for low.
test_args['entropy'] = 'H'				
test_args['last_packet'] = '500'

#helper array to loop through bandwidths
bands = [1, 4 , 5, 6]
counter = 0
outcome = []
numPackets = 500

while True:
	if counter != 0:
		numPackets = 500
	if counter > 3:
		break
	for y in range(500, 6500, 500):
		if numPackets > 6000:
			break
		test_args['number_of_packets'] = str(numPackets)
		#high entropy
		test_args['entropy'] = 'H'
		#filename manipulation
		fpath = "test_"
		fpath += str(bands[counter])
		fpath += test_args['bandwidth'] 
		fpath += "__num_of_packets_"
		fpath += str(numPackets)
		fpath += "_"
		fpath += test_args['entropy']
		fpath += "_.csv"
		#read file and scan for items
		ifile = open(fpath)
		reader = csv.reader(ifile, delimiter="\t")
		rownum = 0
		a = []
		for row in reader:
			a.append(row)
			rownum +=1
		ifile.close()
		#set up to find last packet recevied 
		biggest = len(a) -1
		last_packet = '-1';
		#loop thru to find last packet 
		while last_packet == '-1':
			last_packet = a[biggest][1]
			biggest -= 1
		id_of_packet = biggest + 2
		outcome.append({'Bandwidth' : str(bands[counter]) + test_args['bandwidth'] , 
			'NP': str(numPackets), 'Entropy': test_args['entropy'], 'Last' : id_of_packet })
		#Low Entropy
		test_args['entropy'] = 'L'
		fpath = "test_"
		fpath += str(bands[counter])
		fpath += test_args['bandwidth'] 
		fpath += "__num_of_packets_"
		fpath += str(numPackets)
		fpath += "_"
		fpath += test_args['entropy']
		fpath += "_.csv"
		ifile = open(fpath)
		reader = csv.reader(ifile, delimiter="\t")
		rownum = 0
		a = []
		for row in reader:
			a.append(row)
			rownum +=1
		ifile.close()
		#set up to find last packet recevied 
		biggest = len(a) -1
		last_packet = '-1';
		#loop thru to find last packet 
		while last_packet == '-1':
			last_packet = a[biggest][1]
			biggest -= 1
		id_of_packet = biggest + 2

		outcome.append({'Bandwidth' : str(bands[counter]) + test_args['bandwidth'] , 
			'NP': str(numPackets), 'Entropy': test_args['entropy'], 'Last' : id_of_packet })
		numPackets += 500

	counter = counter + 1

#Write out otucome to final.csv
keys = outcome[0].keys()
with open('final.csv', 'wb') as output_file:
	dict_writer = csv.DictWriter(output_file, keys)
	dict_writer.writerows(outcome)
