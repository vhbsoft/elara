import csv
import os
import math

#Functions used in: 
#spq_loss_rate_v1.py
#spq_loss_rate_v2.py
#spq_loss_rate_v3.py

def calc_loss_rate_switch(file_to_convert, experiment_type, init_packet_train_length):
	loss_rate = 0
	if experiment_type == "SPQ-H":
		loss_rate = calc_spq_high_apt_loss_rate(file_to_convert, init_packet_train_length)
	elif experiment_type == "SPQ-L":
		loss_rate = calc_spq_low_apt_loss_rate(file_to_convert)
	elif experiment_type == "COMP-NI":
		loss_rate = calc_compression_loss_rate_with_ni(file_to_convert)
	elif experiment_type == "COMP":
		loss_rate = calc_compression_loss_rate_wo_ni(file_to_convert)
	elif experiment_type == "DELAYER-NI":
		loss_rate = calc_delayer_loss_rate_with_ni(file_to_convert)
	elif experiment_type == "DELAYER":
		loss_rate = calc_delayer_loss_rate_wo_ni(file_to_convert)
	else:
		print("error")
	return loss_rate

def calc_delayer_loss_rate_with_ni(file_to_convert):
	list_before_modifying = []
	list_before_modifying = convert_file_to_list(file_to_convert)
	size_of_init_packet_train = get_initial_packet_train_length(list_before_modifying)
	list_after_removing_init_packet_train = create_new_list_with_n_i(list_before_modifying, size_of_init_packet_train)
	number_of_packets = len(list_after_removing_init_packet_train)
	number_of_packets_lost = get_number_of_dropped_packets(list_after_removing_init_packet_train)
	delayer_loss_rate_with_ni = calc_loss_rate(number_of_packets, number_of_packets_lost)
	return delayer_loss_rate_with_ni

def calc_delayer_loss_rate_wo_ni(file_to_convert):
	a_list = []
	a_list = convert_file_to_list(file_to_convert)
	number_of_packets = len(a_list)
	number_of_packets_lost = get_number_of_dropped_packets(a_list)
	delayer_loss_rate = calc_loss_rate(number_of_packets, number_of_packets_lost)
	return delayer_loss_rate

def calc_compression_loss_rate_with_ni(file_to_convert):
	list_before_modifying = []
	list_before_modifying = convert_file_to_list(file_to_convert)
	size_of_init_packet_train = get_initial_packet_train_length(list_before_modifying)
	list_after_removing_init_packet_train = create_new_list_with_n_i(list_before_modifying, size_of_init_packet_train)
	number_of_packets = len(list_after_removing_init_packet_train)
	number_of_packets_lost = get_number_of_dropped_packets(list_after_removing_init_packet_train)
	comp_loss_rate_with_ni = calc_loss_rate(number_of_packets, number_of_packets_lost)
	return comp_loss_rate_with_ni

def calc_compression_loss_rate_wo_ni(file_to_convert):
	a_list = []
	a_list = convert_file_to_list(file_to_convert)
	number_of_packets = len(a_list)
	number_of_packets_lost = get_number_of_dropped_packets(a_list)
	comp_loss_rate = calc_loss_rate(number_of_packets, number_of_packets_lost)
	return comp_loss_rate

def calc_spq_high_apt_loss_rate(file_to_convert, size_of_init_packet_train):
	list_before_modifying = []
	list_before_modifying = convert_file_to_list(file_to_convert)
	list_after_removing_init_packet_train = create_new_list_with_n_i(list_before_modifying, size_of_init_packet_train)
	number_of_packets = len(list_after_removing_init_packet_train)
	number_of_packets_lost = get_number_of_dropped_packets(list_after_removing_init_packet_train)
	spq_high_loss_rate = calc_loss_rate(number_of_packets, number_of_packets_lost)
	return spq_high_loss_rate

def calc_spq_low_apt_loss_rate(file_to_convert):
	a_list = []
	a_list = convert_file_to_list(file_to_convert)
	number_of_packets = len(a_list)
	number_of_packets_lost = get_number_of_dropped_packets(a_list)
	spq_low_loss_rate = calc_loss_rate(number_of_packets, number_of_packets_lost)
	return spq_low_loss_rate

#working
def create_new_list_with_n_i(a_list, n):
	new_list = []
	for i in range(len(a_list)):
		if i >= n:
			new_list.append(a_list[i])
	return new_list

#getting errors
def get_number_of_dropped_packets(a_list):
	num_of_dropped_packets = 0
	index = 0
	for i in range(len(a_list)):
		if a_list[index][1]=='-1':
			num_of_dropped_packets += 1
		index += 1
	return num_of_dropped_packets

def calc_loss_rate(number_of_packets, number_of_packets_lost):
	numerator = number_of_packets_lost
	denom = number_of_packets
	if denom == 0:
		raise MyError("error cannot divide by 0")
	loss_rate = (numerator *1.0)/(denom * 1.0) *100.0
	convert_loss_rate = float("{0:.2f}".format(loss_rate)) 
	loss_rate = convert_loss_rate
	return loss_rate 

#getting errors about list index out of range
def get_initial_packet_train_length(a_list):
	first_dropped_packet = 0
	for x in range(len(a_list)):
		if a_list[x][1]=='-1':
			first_dropped_packet = x
			break
	return first_dropped_packet


#working
def convert_file_to_list(file_to_convert):
	#read file and to a list for further processing
	ifile = open(file_to_convert)
	reader = csv.reader(ifile, delimiter="\t")
	rownum = 0
	a = []
	for row in reader:
		a.append(row)
		rownum +=1
	ifile.close()
	return a

#SHAPER SUPERPACKET FUNCTIONS --------------------------------------------------------------------------------
def get_num_dropped_packets_with_superpacket_2(a_list):
	num_of_dropped_packets = 0
	index = 0
	index2 = 1
	size_of_list = len(a_list)

	for i in range(len(a_list)):
		if (a_list[index][1]=='-1') or (a_list[index2][1] =='-1'):
			num_of_dropped_packets += 1

		index += 2
		index2 += 2

		if (index >= size_of_list) or (index2 >= size_of_list):
			break

	return num_of_dropped_packets

def get_num_dropped_packets_with_superpacket_3(a_list):
	num_of_dropped_packets = 0
	index = 0
	index2 = 1
	index3 = 2
	size_of_list = len(a_list)

	for i in range(len(a_list)):
		if (a_list[index][1]=='-1') or (a_list[index2][1] =='-1') or (a_list[index3][1] =='-1'):
			num_of_dropped_packets += 1

		index += 3
		index2 += 3
		index3 += 3

		if (index >= size_of_list) or (index2 >= size_of_list) or (index3 >= size_of_list):
			break

	return num_of_dropped_packets	

def get_num_dropped_packets_with_superpacket_4(a_list):
	num_of_dropped_packets = 0
	index = 0
	index2 = 1
	index3 = 2
	index4 = 3
	size_of_list = len(a_list)

	for i in range(len(a_list)):
		if (a_list[index][1]=='-1') or (a_list[index2][1] =='-1') or (a_list[index3][1]=='-1') or (a_list[index4][1] =='-1') :
			num_of_dropped_packets += 1

		index += 4
		index2 += 4
		index3 += 4
		index4 += 4

		if (index >= size_of_list) or (index2 >= size_of_list) or (index3 >= size_of_list) or (index4 >= size_of_list):
			break

	return num_of_dropped_packets

def get_num_dropped_packets_with_superpacket_5(a_list):
	num_of_dropped_packets = 0
	index = 0
	index2 = 1
	index3 = 2
	index4 = 3
	index5 = 4
	size_of_list = len(a_list)

	for i in range(len(a_list)):
		if (a_list[index][1]=='-1') or (a_list[index2][1] =='-1') or (a_list[index3][1]=='-1') or (a_list[index4][1] =='-1') or (a_list[index5][1] =='-1') :
			num_of_dropped_packets += 1

		index += 5
		index2 += 5
		index3 += 5
		index4 += 5
		index5 += 5
		if (index >= size_of_list) or (index2 >= size_of_list) or (index3 >= size_of_list) or (index4 >= size_of_list) or (index5 >= size_of_list):
			break

	return num_of_dropped_packets

def get_num_dropped_packets_with_superpacket_6(a_list):
	num_of_dropped_packets = 0
	index = 0
	index2 = 1
	index3 = 2
	index4 = 3
	index5 = 4
	index6 = 5
	size_of_list = len(a_list)

	for i in range(len(a_list)):
		if (a_list[index][1]=='-1') or (a_list[index2][1] =='-1') or (a_list[index3][1]=='-1') or (a_list[index4][1] =='-1') or (a_list[index5][1] =='-1')  or (a_list[index6][1] =='-1'):
			num_of_dropped_packets += 1

		index += 6
		index2 += 6
		index3 += 6
		index4 += 6
		index5 += 6
		index6 += 6
		if (index >= size_of_list) or (index2 >= size_of_list) or (index3 >= size_of_list) or (index4 >= size_of_list) or (index5 >= size_of_list) or (index6 >= size_of_list):
			break

	return num_of_dropped_packets

def shaper_calc_loss_rate_wo_ni(file_to_convert, number_of_packets, superpacket_size):
	number_of_packets_lost = 0
	num_of_actual_packets = 0
	a_list = []
	a_list = convert_file_to_list(file_to_convert)
	num_of_actual_packets = int(number_of_packets / superpacket_size)
	if superpacket_size == 1:
		number_of_packets_lost = get_number_of_dropped_packets(a_list)
	elif superpacket_size == 2:
		number_of_packets_lost = get_num_dropped_packets_with_superpacket_2(a_list)
	elif superpacket_size == 3:
		number_of_packets_lost = get_num_dropped_packets_with_superpacket_3(a_list)
	elif superpacket_size == 4:
		number_of_packets_lost = get_num_dropped_packets_with_superpacket_4(a_list)
	elif superpacket_size == 5:
		number_of_packets_lost = get_num_dropped_packets_with_superpacket_5(a_list)
	elif superpacket_size == 6:
		number_of_packets_lost = get_num_dropped_packets_with_superpacket_6(a_list)
	else:
		print("error")
	shaper_loss_rate = calc_loss_rate(num_of_actual_packets, number_of_packets_lost)
	return shaper_loss_rate

def shaper_calc_loss_rate_with_ni(file_to_convert, number_of_packets, superpacket_size):
	#initial arguments to be calculated within the function
	number_of_packets_lost = 0
	num_of_actual_packets = 0
	a_list = []
	a_list = convert_file_to_list(file_to_convert)
	n_i = get_initial_packet_train_length(a_list)
	a_list_with_n_i = []
	a_list_with_n_i = create_new_list_with_n_i(a_list, n_i)
	#need function to calc new number of packets after removing n_i
	num_of_actual_packets = int((number_of_packets - n_i) / superpacket_size)
	if superpacket_size == 1:
		number_of_packets_lost = get_number_of_dropped_packets(a_list_with_n_i)
	elif superpacket_size == 2:
		number_of_packets_lost = get_num_dropped_packets_with_superpacket_2(a_list_with_n_i)
	elif superpacket_size == 3:
		number_of_packets_lost = get_num_dropped_packets_with_superpacket_3(a_list_with_n_i)
	elif superpacket_size == 4:
		number_of_packets_lost = get_num_dropped_packets_with_superpacket_4(a_list_with_n_i)
	elif superpacket_size == 5:
		number_of_packets_lost = get_num_dropped_packets_with_superpacket_5(a_list_with_n_i)
	elif superpacket_size == 6:
		number_of_packets_lost = get_num_dropped_packets_with_superpacket_6(a_list_with_n_i)
	else:
		print("error")
	shaper_loss_rate = calc_loss_rate(num_of_actual_packets, number_of_packets_lost)
	return shaper_loss_rate	

def calc_loss_rate_spq_switch(file_to_convert, experiment_type, number_of_packets, superpacket_size):
	loss_rate = 0
	if experiment_type == "SHAPER":
		loss_rate = shaper_calc_loss_rate_wo_ni(file_to_convert, number_of_packets, superpacket_size)
	elif experiment_type == "SHAPER-NI":
		loss_rate = shaper_calc_loss_rate_with_ni(file_to_convert, number_of_packets, superpacket_size)
	else:
		print("error")
	return loss_rate