# -*- coding: utf-8 -*-
"""
Fletcher-16, Fletcher-32 and Fletcher-64 checksum calculation.

Results have been checked checked vs. https://en.wikipedia.org/wiki/Fletcher%27s_checksum

@author: gicking @ Github
"""


def calculate_fletcher16(Data:bytearray=None) -> int:
    """
    Calculate Fletcher-16 checksum for the given byte array.

    Args:
        Data (bytearray): byte array to calculate checksum over

    Returns:
        int: calculated Fletcher-16 checksum.
    """

    # parameter type check
    if not isinstance(Data, (bytes, bytearray)):
        raise TypeError("Data must be of type bytes or bytearray.")

    # init checksum variables
    sum1 = 0
    sum2 = 0
    
    # for big endian system reverse byte order in array
    if int.from_bytes(b'\x01\x00', byteorder='big') == 1:
        Data = bytearray(list(data)[::-1])
    
    # calculate checksum over bytes in data array
    for i in range(0, len(Data)):
        
        # get next 8-bit value from array
        val = Data[i] 

        # update sums
        sum1 = (sum1 + val)  % 0xFF
        sum2 = (sum2 + sum1) % 0xFF
                
    # concat checksum and clip to 16-bit
    chk = ((sum2 << 8) | sum1) & 0xFFFF
    
    # return checksum
    return chk


def calculate_fletcher32(Data:bytearray=None) -> int:
    """
    Calculates the Fletcher-32 checksum for the given byte array.

    Args:
        Data (bytearray): byte array to calculate checksum over

    Returns:
        int: calculated Fletcher-32 checksum.
    """

    # parameter type check
    if not isinstance(Data, (bytes, bytearray)):
        raise TypeError("Data must be of type bytes or bytearray.")

    # determine the endianness of the system
    little_endian = True if int.from_bytes(b'\x01\x00', byteorder='little') == 1 else False

    # pad data array with 0 if length is not dividable by 2
    while (len(Data) % 2) != 0:
        Data.append(0x00)
        
    # init checksum variables
    sum1 = 0
    sum2 = 0
    
    # calculate checksum over words in data array
    for i in range(0, len(Data), 2):
        
        # get next 16-bit value from array. Account for endianness
        if little_endian:
            val = (Data[i+1] << 8) | Data[i]
        else:
            val = (Data[i] << 8) | Data[i+1]
            
        sum1 = (sum1 + val)  % 0xFFFF
        sum2 = (sum2 + sum1) % 0xFFFF
            
    # concat checksum and clip to 16-bit
    chk = ((sum2 << 16) | sum1) & 0xFFFFFFFF
    
    # return checksum
    return chk


def calculate_fletcher64(Data: bytearray = None) -> int:
    """
    Calculates the Fletcher-64 checksum for the given byte array.

    Args:
        Data (bytearray): byte array to calculate checksum over

    Returns:
        int: calculated Fletcher-64 checksum.
    """

    # parameter type check
    if not isinstance(Data, (bytes, bytearray)):
        raise TypeError("Data must be of type bytes or bytearray.")

    # determine the endianness of the system
    little_endian = True if int.from_bytes(b'\x01\x00', byteorder='little') == 1 else False

    # pad data array with 0 if length is not dividable by 4
    while (len(Data) % 4) != 0:
        Data.append(0x00)

    # init checksum variables
    sum1 = 0
    sum2 = 0

    # calculate checksum over words in data array
    for i in range(0, len(Data), 4):

        # get next 32-bit value from array. Account for endianness
        if little_endian:
            val = (Data[i+3] << 24) | (Data[i+2] << 16) | (Data[i+1] << 8) | Data[i]
        else:
            val = (Data[i] << 24) | (Data[i+1] << 16) | (Data[i+2] << 8) | Data[i+3]

        sum1 = (sum1 + val)  % 0xFFFFFFFF
        sum2 = (sum2 + sum1) % 0xFFFFFFFF

    # concat checksum and clip to 32-bit
    chk = ((sum2 << 32) | sum1) & 0xFFFFFFFFFFFFFFFF

    # return checksum
    return chk



if __name__ == "__main__":

    #import random
    
    # Example usage
    #data = bytearray([0x01, 0x02])
    #data = bytearray([random.randint(0, 255) for _ in range(100)])
    data = bytearray("abcdefgh", 'utf-8')
    
    # calculate checksums
    chk16 = calculate_fletcher16(Data=data)
    chk32 = calculate_fletcher32(Data=data)
    chk64 = calculate_fletcher64(Data=data)
    
    # print results
    print("input:", end=" ")
    #print([f"0x{byte:02X}" for byte in data], end=" ")
    print(''.join(['{:02X}'.format(byte) for byte in data]))
    print(f"Fletcher-16: 0x{chk16:04X}")
    print(f"Fletcher-32: 0x{chk32:08X}")
    print(f"Fletcher-64: 0x{chk64:016X}")
    