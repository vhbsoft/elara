# Armen Arslanian
# Read R1 and R2
# "R3.txt" has the ratio R2/R1
# "R4.txt" has ratio of intervals
# ****NOTE: The values at the first times of R1 and R2 HAVE to be 0****

n = input("Enter n: ") #Ask user for n
n = int(n) #Convert it to an int
R1 = open("R1.txt", "r") #First input file
R2 = open("R2.txt", "r") #Second input file
R3 = open("R3.txt", "w") #This file will have the basic R2/R1 calculation
R1_total = [] #Holds the totals for R1 (time*value)
R2_total = [] #Holds the totals for R2 (time*value)
time = [] #Holds the times in a list
print("Reading from R1 & R2 ...")
while(True):
	#Read both input files line by line
	lineR1 = R1.readline()
	lineR2 = R2.readline()
	if(not lineR1 or not lineR2): #Break if reached the end of the file
		break
	#Split to separate times from values in each file
	lineR1_parts = lineR1.split("\t")
	lineR2_parts = lineR2.split("\t")
	time.append(lineR1_parts[0]) #Hold all times
	R1_total.append(float(lineR1_parts[0])*float(lineR1_parts[1])) #Totals of R1
	R2_total.append(float(lineR2_parts[0])*float(lineR2_parts[1])) #Totals of R2
	if(float(lineR1_parts[1]) == 0): #Take care of divide by zero because 1st line of data is always 0
		ratioR2R1 = 0.0
	else:
		ratioR2R1 = round(float(lineR2_parts[1])/float(lineR1_parts[1]), 2) #Ratio R2/R1 rounded to the hundredths place
	R3.write(lineR1_parts[0] + "\t" + str(ratioR2R1) + "\n") #Write ratio R2/R1 to R3.txt as well as the times
print("Generated R3.txt")
print("Calculating values for R4.txt ...")
loop_length = int(len(R1_total)/(n+1)) #Based on n and length of file, know how many times to loop
j = 0 #Will always be the first index of the interval, which is why it starts with 0
R4 = open("R4.txt", "w") #File for ratio of intervals
for x in range(0,loop_length):
	ratio_intervals = round((R2_total[j+n]-R2_total[j])/(R1_total[j+n]-R1_total[j]), 2) #Ratio of intervals based on n
	R4.write(time[j+n] + "\t" + str(ratio_intervals) + "\n") #Write ratio of intervals to R4.txt as well as the times
	j = j + (n+1) #Update
print("Generated R4.txt")
#Close all files
R1.close()
R2.close()
R3.close()
R4.close()
print("\nDone.")