#Aren Mark Boghoziann
# July 12, 2018

import numpy as np
import pandas as pd
import os
import glob
import json
import csv
import pandas as pd
import time
from datetime import datetime
import random

import plotly
plotly.offline.init_notebook_mode(connected=True)
import plotly.offline as py
import plotly.graph_objs as go



# Converts the text file information into a json file
class jasonParser:

	def __init__(self):
		self.packetId = ''
		self.timeStamp = ''
		self.source = ''
		self.destination = ''
		self.destinationPort = ''
		self.size = ''
		self.fileName = ''

	def generateJason(self,experiments):
		
		# Will loop through all the experiments
		for i in range(len(experiments)):

			#Creates new directory called JsonInfo
			os.mkdir('TestResults/'+experiments[i]+'/dataAnalysis/JsonInfo')

			# Reads though each individual text file and ignores any folders	
			for f in os.listdir('TestResults/'+experiments[i]+'/dataAnalysis'):
				if f == 'JsonInfo':
					continue
					
				f2 = open('TestResults/'+experiments[i]+'/dataAnalysis/'+f,'r')
				isComplete = False
				fileName = f

				# Reads through each ine of text and if given headers are encountered there inforamtion is saved
				for line in f2:
					if 'Data:' in line:
						packetIdHex = line[10:14]
						packetId = int(str(packetIdHex),16)
					elif 'Arrival Time:' in line:
						timeStamp = line[18:-1]
					elif 'Source' in line and ('Source Port:' not in line and 'Source: Alcatel-_8d:ac:f1 (e8:e7:32:8d:ac:f1)' not in line):
						source = line[12:-1]
					elif 'Destination:' in line and 'Port:' not in line:
						destination = line[17:-1]
					elif 'Destination Port:' in line:
						destinationPort = line[22:-1]
					elif '[Length:' in line:
						size = line[13:17]
						isComplete = True

					# length is the last information of the text file, we save information and we reset to parse other frames
					if isComplete:
						r = {packetId:{'timeStamp':timeStamp,'Source IP':source,'Destination IP':destination,'Destination Port':destinationPort,'Size':size,'File Name':fileName}}
						with open('TestResults/'+experiments[i]+'/dataAnalysis/JsonInfo/'+f[:-4] +'.json', 'a') as feedsjson:
							json.dump(r, feedsjson)
							feedsjson.write(os.linesep)
						isComplete = False



# Converts the Json files into csv files
class structureData:

	def __init__(self):
		self.arr = []
	
	def restructure(self, experiments):
		
		# Will loop through all the experiments	
		for i in range(len(experiments)):
			#makes a new folder called structuredData
			os.mkdir('TestResults/'+experiments[i]+'/dataAnalysis/JsonInfo/structuredData')
			
			#goes through all the json files
			for f in os.listdir('TestResults/'+experiments[i]+'/dataAnalysis/JsonInfo'):

				#ifnore the folder
				if f == 'structuredData':
					continue

				arr = []
				tempDict = {}
				finalDict = {}
				source_ip = ""

				#Append all the json information into an array
				for line in open('TestResults/'+experiments[i]+'/dataAnalysis/JsonInfo/'+f, 'r'):
					arr.append(json.loads(line))
				#iterate through all the jason information inside the array 
				for x in range(len(arr)):
					for key in arr[x].keys():
						datetime_object = datetime.strptime(str(arr[x][key]['timeStamp'][:-7]), '%b %d, %Y %H:%M:%S.%f')
						tempDict[key] = time.mktime( (datetime_object.year, datetime_object.month, datetime_object.day, datetime_object.hour, datetime_object.minute, datetime_object.second, 0, 0, 1) )
						source_ip = arr[x][key]['Source IP']

				# If packet is recived we insert info dict with time stamp other wise we give it -1
				for x in range(1,5001):
					if str(x) not in tempDict:
						finalDict[str(x)] = -1
					else:
						finalDict[str(x)] = tempDict[str(x)]

				w = csv.writer(open('TestResults/'+experiments[i]+'/dataAnalysis/JsonInfo/structuredData/'+f[:-5]+'( '+source_ip+' )'+'.csv', 'w'))
				w.writerow(['ID','Loss vs No Loss'])
				for key, val in finalDict.items():
					w.writerow([key, val])



# Combines all the experiments into one csv file for data analysis
class CombineExperiments():
	
	def __init__(self):
		self.ingore = 0


	def getFirstPacketDropped(self,df):

		count = 0
		for index, row in df.iterrows():

			if row["Loss vs No Loss"] != -1:
				count = count + 1

			else:
				if count == 4999:
					return 0
				else:
					return count


	def getNumberOfPacketsDroped(self,df,experiment,ignore_Number_of_packets):
		
		count = 0
		time = ""
		shapingRow1 = ""
		shapingRow2 = ""
		shapingCounter = 0
		ignore_count = 0

		if experiment == 'Compression':
			for index, row in df.iterrows():

				if ignore_count < ignore_Number_of_packets:
					ignore_count = ignore_count + 1
					continue

				if row["Loss vs No Loss"] == -1:
					count = count + 1
				else:
					time = datetime.fromtimestamp(row["Loss vs No Loss"])


		# Counting pair packets, if one of them is droped, we consider all dropped
		elif experiment == 'ShapingFinal':
							
			for index, row in df.iterrows():
				
				if ignore_count < ignore_Number_of_packets:
					ignore_count = ignore_count + 1
					continue

				shapingCounter = shapingCounter + 1
								
				if shapingCounter == 1:
					if row["Loss vs No Loss"] == -1:
						shapingRow1 = -1
					else:
						shapingRow1 = datetime.fromtimestamp(row["Loss vs No Loss"])
						time = datetime.fromtimestamp(row["Loss vs No Loss"])
				else:
					if row["Loss vs No Loss"] == -1:
						shapingRow2 = -1
					else:
						shapingRow2 = datetime.fromtimestamp(row["Loss vs No Loss"])

					if shapingRow1 == -1 or shapingRow2 == -1:
						count = count + 2

					shapingCounter = 0


		return count,time


	# Comines information from all teh relevent text files into one giant csv file for further anaysis
	def combine(self, ignore_Num, experiments,sourceIp, shoud_Ignore):
		ignore = ignore_Num
		CompressionBadFiles = ['8601(','8602(','8535(','8706(','8603(']

		for i in range(len(experiments)):
			w = csv.writer(open( experiments[i]+'.csv', 'w'))
			w.writerow(['test_date','Country','host_ip','num_packets', 'packet_size', 'base_losses_str', 'id_pcap_data', 'discrimination_losses_str', 'id_pcap_data'])

			for country in sourceIp:
				pairTracker = 0
				file_array = []
				pcap1 = ""
				pcap2 = ""
				BaseLoss = 0
				discLoss = 0

				for f in os.listdir('TestResults/'+experiments[i]+'/dataAnalysis/JsonInfo/structuredData'):
					file_array.append(f)
				file_array.sort()
	

				for file in file_array:
					if country in file and file[:5] not in CompressionBadFiles:

						df = pd.read_csv('TestResults/'+experiments[i]+'/dataAnalysis/JsonInfo/structuredData/'+file)
						count = 0
						time = ""

						if shoud_Ignore:
							ignore = self.getFirstPacketDropped(df)

						if experiments[i] == 'Compression':

							pairTracker = pairTracker + 1
							count,time = self.getNumberOfPacketsDroped(df,experiments[i],ignore)


							if pairTracker == 1:
								BaseLoss = count
								pcap1 = file[:-4]
							else:
								discLoss = count
								pcap2 = file[:-4]


						elif experiments[i] == 'SPQ':
							print('SPQ')	


						else:
							pairTracker = pairTracker + 1
							count,time = self.getNumberOfPacketsDroped(df,experiments[i],ignore)

							if pairTracker == 2:
								discLoss = count
								pcap1 = file[:-4]
							else:
								BaseLoss = count
								pcap2 = file[:-4]															


						if pairTracker == 2:
							pairTracker = 0
							w.writerow([str(time),sourceIp[country],country,'5000','1024', str(BaseLoss),pcap1, str(discLoss), pcap2])
							BaseLoss = 0
							discLoss = 0


class graph():

	def parse(self,ip, data):
		x = np.array(data[data['host_ip'] == ip]['test_date'])
		y = np.array(data[data['host_ip'] == ip]['discrimination_losses_str'])
		z = np.array(data[data['host_ip'] == ip]['base_losses_str'])

		x_ax = []
		y_ax = []
		z_ax = []
		for i in range(0, len(x), 2):
			x_ax.append(str(x[i][:][6:16]))
			y_ax.append(str((y[i]/5000) * 100))
			z_ax.append(str((z[i]/5000) * 100))

		return x_ax, y_ax, z_ax


	def draw(self,x_axis, y_axis, z_axis, t, isSecondDay):

		if isSecondDay:
			start = 14.5
			end = 38.5
		else:
			start = 16.5
			end = 40.5

		trace1 = {"x":x_axis, "y":y_axis, "marker": {"color": "red", "size": 12},
				"mode": "markers",
				"name": "Discriminated",
				"type": "scatter"}


		trace2 = {"x":x_axis, "y":z_axis, "marker": {"color": "green", "size": 12},
				"mode": "markers",
				"name": "Base",
				"type": "scatter"}



		data = [trace1, trace2]

		layout = {"title": "Packet Loss " + t,
				"xaxis": {"title": "Time", },
				"yaxis": {"title": "Percent of Loss"}}

		layout = go.Layout(
			width = 2500,
			height = 2500 ,
			xaxis=dict(
			title = "Time",
			#range=[start, end]
		),
			yaxis=dict(title="Percent of Loss"),

			title = 'Packet Loss ' + t,
		)

		fig = go.Figure(data=data, layout=layout)

		py.iplot(fig, filename = t ,image='png')    	





def main():

	sourceIp = {'131.179.150.70':'planetlab1.cs.ucla.edu','131.179.150.72':'planetlab2.cs.ucla.edu', '192.16.125.12':'planetlab-2.ssvl.kth.se', '165.242.90.129':'pl2.sos.info.hiroshima-cu.ac.jp', '129.63.159.102':'planetlab2.cs.uml.edu', '192.91.235.230':'pluto.cs.brown.edu', '142.103.2.2':'planetlab2.cs.ubc.ca'}
	experiments = ['Compression','ShapingFinal']
	
	# print('Start of the Program')
	# parse = jasonParser()
	# parse.generateJason(experiments)

	# struct = structureData()
	# struct.restructure(experiments)

	c = CombineExperiments()
	c.combine(1000, experiments,sourceIp, False) # if false then the first parameter = the n_I 
											  # if true then auto N_I is discovered

'''
	g = graph()
	for i in range(len(experiments)):
		data = pd.read_csv(experiments[i] + '.csv')
		for key in sourceIp.keys():
			x ,y, z = g.parse(key, data)
			g.draw(x,y,z,sourceIp[key], isSecondDay = False)

	print('Program End')
'''


main()
