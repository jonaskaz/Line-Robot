'''
This file contains functions to collect and save 3D scan data as well as visualize it on a heatmap. 
'''

from os import read
import serial
from serial import Serial
import matplotlib.pyplot as plt
import numpy as np
import time
import pandas as pd 
import seaborn as sns

# set up serial communication, comment this out if not connected to arduino
# ser = serial.Serial('COM6',9600)
# ser.close()
# ser.open() 

def read_ser_data():
    ''' Reads data from serial and parses each line into the following format:
            [IR1_val, IR2_val, IR3_val, LW_speed, RW_speed]
    '''
    ser_data = ser.readline().decode()
    parsed_ser_data = [float(x) for x in ser_data.split()]
    if len(parsed_ser_data) == 5:
        return parsed_ser_data
    else:
        return 0

def collect_data():
    ''' Collects data of run
    '''
    with open("plot_data.csv", 'w') as fd:
        fd.write("index,IR1,IR2,IR3,LW_speed,RW_speed\n")

    # collect data
    collect = True # TODO: control this with a button later
    # t_end = time.time() + collection_time
    index = 0
    # while collect and (time.time() < t_end):
    while collect:

        # wait for a bit
        time.sleep(0.001)

        # fetch new serial data
        ser_data = read_ser_data()
        print(ser_data)

        if ser_data == 0:
            collect = False
            break

        # append to csv
        with open("plot_data.csv", 'a') as fd:
            fd.write(f"{index}, {ser_data[0]},{ser_data[1]},{ser_data[2]},{ser_data[3]},{ser_data[4]}\n")
        
        index += 1

def plot_existing_data():
    ''' Create heatmap from existing data.
    '''
    df = pd.read_csv('plot_data2 copy.csv',header=0)
    index = df['index'].tolist()
    IR1 = df['IR1'].tolist()
    IR2 = df['IR2'].tolist()
    IR3 = df['IR3'].tolist()
    LW_speed = df['LW_speed'].tolist()
    RW_speed = df['RW_speed'].tolist()

    # plot lines
    plt.plot(index, smooth(IR1), label = "IR_left")
    plt.plot(index, IR2, label = "IR_right")
    plt.plot(index, IR3, label = "IR_middle")
    plt.plot(index, LW_speed, label = "LW_speed")
    plt.plot(index, RW_speed, label = "RW_speed")
    plt.legend()
    plt.show()

if __name__ == "__main__":
    # collect_data()
    
    plot_existing_data()