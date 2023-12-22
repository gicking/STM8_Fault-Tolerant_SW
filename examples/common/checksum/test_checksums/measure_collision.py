# -*- coding: utf-8 -*-
"""
Measure rate of checksum collisions (=same checksums for different inputs)

Results for size=1024, iterations=1E6. For CRC compare with https://quickbirdstudios.com/blog/validate-data-with-crc/
  - CRC8: 3900ppm (expect 3900ppm)
  - CRC16: 16ppm (expect 15ppm)
  - CRC32: 0ppm (iterations=1E7; too long runtime, expect 0.0002ppm)
  - Fletcher-16: 12ppm
  - Fletcher-32: 0ppm (iterations=1E7; too long runtime)
  - Fletcher-64: 0ppm (iterations=1E7; too long runtime)

@author: ICK2BUE
"""

import crc
import crc_lut
import fletcher
import os

# global variables
data1 = None
checksum1 = None


# for convenience change checksum method only here
def calculate_checksum(data):
    #checksum = crc_lut.calculate_crc8(data)
    #checksum = crc_lut.calculate_crc16(data)
    #checksum = crc_lut.calculate_crc32(data)
    #checksum = fletcher.calculate_fletcher16(data)
    checksum = fletcher.calculate_fletcher32(data)
    #checksum = fletcher.calculate_fletcher64(data)
    return checksum


# actual collision measurement
def measure_collision_rate(data_size, num_iterations):
    global data1
    global checksum1 
    collisions = 0
    print_period = num_iterations/20

    # loop over cycles. Don't use range() due to large num_iterations
    i = 0
    while i < int(num_iterations):
        
        # get new random array and calculate checksum
        data2 = bytearray(os.urandom(data_size))
        checksum2 = calculate_checksum(data2)
        
        # if checksums match, increase error counter
        if checksum2 == checksum1:
            collisions += 1

        # make new data to old for next loop
        data1[:] = data2
        checksum1 = checksum2
        
        # increase cycle counter
        i += 1
        
        # print message
        if i % print_period == 0:
            print(".", end="")

    # calculate collision rate [ppm]
    collision_rate = collisions / num_iterations * 1000000

    # return result
    return collision_rate


if __name__ == '__main__':
    
    # set parameters
    data_size = 1024            # data size [B]
    num_iterations = 1E7        # number of arrays to check for collisions
    
    # initialize globals
    data1 = bytearray(os.urandom(data_size))
    checksum1 = calculate_checksum(data1)
    
    # print message
    print("Start ", end="", flush=True)
    
    # measure collision rate (slooooow)
    collision_rate = measure_collision_rate(data_size, num_iterations)
    
    # print result [ppm]
    print(" done\n")
    print("Collision rate: %.2gppm" % (collision_rate))
