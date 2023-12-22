# -*- coding: utf-8 -*-
"""
Measure rate of checksum collisions (=same checksums for different inputs)

Results for data_size=1024. For CRC compare with https://quickbirdstudios.com/blog/validate-data-with-crc/
  - CRC8: 3900ppm / 1E6 cycles / 1m9s (expect 3900ppm)
  - CRC16: 14ppm / 1E7 cycles / 41m51s (expect 15ppm)
  - CRC32: 0ppm / 1E7 cycles / 3h22m17s (too long runtime, expect 0.0002ppm)
  - Fletcher-16: 12ppm / 1E6 cycles / 2m4s
  - Fletcher-32: 0ppm / 1E7 cycles / 31m13s (too long runtime)
  - Fletcher-64: 0ppm / 1E7 cycles / 33m31 (too long runtime)

Test-System: Intel Core i5 @ 2.6GHz; Windows 10 Enterprise

@author: gicking @ Github
"""

#import crc
import crc_lut
import fletcher
import os


# collision measurement
def measure_collision_rate(data_size, num_iterations, method=None):
    
    collisions = 0
    print_period = num_iterations/50
    print_space  = print_period*5
    
    # store pointer to checksum routine
    if method.upper() == "CRC8":
        fct_checksum = crc_lut.calculate_crc8
    elif method.upper() == "CRC16":
        fct_checksum = crc_lut.calculate_crc16
    elif method.upper() == "CRC32":
        fct_checksum = crc_lut.calculate_crc32
    elif method.upper() == "FLETCHER16":
        fct_checksum = fletcher.calculate_fletcher16
    elif method.upper() == "FLETCHER32":
        fct_checksum = fletcher.calculate_fletcher32
    elif method.upper() == "FLETCHER64":
        fct_checksum = fletcher.calculate_fletcher64
    else:
        raise TypeError("Illegal parameter method: '" + str(method) + "'")

    # initialize first data set    
    data1 = bytearray(os.urandom(data_size))
    checksum1 = fct_checksum(data1)

    # loop over cycles. Don't use range() due to large num_iterations
    i = 0
    while i < int(num_iterations):
        
        # get new random array and calculate checksum
        data2 = bytearray(os.urandom(data_size))
        checksum2 = fct_checksum(data2)
        
        # if checksums match, increase error counter
        if checksum2 == checksum1:
            collisions += 1

        # make new data to old for next loop
        data1[:] = data2
        checksum1 = checksum2
        
        # increase cycle counter
        i += 1
        
        # print progress
        if i % print_period == 0:
            print(".", end="", flush=True)
        if i % print_space == 0:
            print(" ", end="", flush=True)

    # calculate collision rate [ppm]
    collision_rate = collisions / num_iterations * 1000000

    # return result
    return collision_rate


if __name__ == '__main__':
    
    import time

    # set parameters
    data_size = 1024            # data size [B]
    num_iterations = 1E7        # number of arrays to check for collisions
    method = "fletcher64"            # checksum method: crc8, crc16, crc32, fletcher16, fletcher32, fletcher64
    
    # measure checksum collision rates
    print("Start %g cycles %s " % (num_iterations, method), end="", flush=True)
    timeStart = time.time()
    collision_rate = measure_collision_rate(data_size, num_iterations, method=method)
    timeEnd = time.time()
    print(" done after %ds\n" % (timeEnd-timeStart))
    
    # print result [ppm]
    print("Collision rate: %.2gppm" % (collision_rate))
