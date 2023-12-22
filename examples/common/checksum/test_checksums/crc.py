# -*- coding: utf-8 -*-
"""
CRC8, CRC16 and CRC32 calculation w/o table

For common polynoms, initals etc. see https://crccalc.com/
Results have been checked checked vs. https://crccalc.com/

Note: is very slow, for demonstration only! For faster version see crc_lut.py

@author: ICK2BUE
"""


def calculate_crc8(Data:bytearray=None, Poly:int=0x07, Init:int=0x00, RefIn:bool=False, RefOut:bool=False, XorOut:int=0x00) -> int:
    """
    Calculate CRC8 checksum for the given byte array.
    Default parameters are for CRC8 in https://crccalc.com/

    Args:
        Data (bytearray): byte array to calculate checksum over
        Poly (int): used polynom (0x00-0xFF)
        Init (int): inital value (0x00-0xFF)
        RefIn (bool): reverse input bits
        RefOut (bool): reverse output bits
        XorOut (int): final xor value (0x00-0xFF)

    Returns:
        int: calculated CRC8 checksum.
    """

    # parameter type check
    if not isinstance(Data, (bytes, bytearray)):
        raise TypeError("Data must be of type bytes or bytearray.")
    if not isinstance(Poly, int) or not (0 <= Poly <= 0xFF):
        raise TypeError("Polynomial must be an 8-bit integer (0x00-0xFF).")
    if not isinstance(Init, int) or not (0 <= Init <= 0xFF):
        raise TypeError("Initial value must be an 8-bit integer (0x00-0xFF).")
    if not isinstance(RefIn, bool):
        raise TypeError("Input reversal must be boolean (True/False).")
    if not isinstance(RefOut, bool):
        raise TypeError("Output reversal must be boolean (True/False).")
    if not isinstance(XorOut, int) or not (0 <= XorOut <= 0xFF):
        raise TypeError("Final XOR value must be an 8-bit integer (0x00-0xFF).")

    # init checksum
    crc = Init
    
    # calculate checksum over data array
    for byte in Data:
        
        # reverse input bits if specified
        if RefIn == True:
            byte = int('{:08b}'.format(byte)[::-1], 2)

        # XOR with next data byte
        crc ^= byte
        
        # shift left until MSB is 1, then XOR with polynom
        for _ in range(8):
            if crc & 0x80:
                crc = (crc << 1) ^ Poly
            else:
                crc <<= 1
        
        # clip to 8-bit
        crc &= 0xFF
        
    # finalize checksum
    crc = crc ^ XorOut
    
    # reverse output bits if specified
    if RefOut:
        crc = int('{:08b}'.format(crc)[::-1], 2)
        
    
    # return checksum
    return crc & 0xFF


def calculate_crc16(Data:bytearray=None, Poly:int=0x1021, Init:int=0xFFFF, RefIn:bool=False, RefOut:bool=False, XorOut:int=0x0000) -> int:
    """
    Calculate CRC16 checksum for the given byte array.
    Default parameters are for CRC16-CCITT-FALSE in https://crccalc.com/

    Args:
        Data (bytearray): byte array to calculate checksum over
        Poly (int): used polynom (0x00-0xFFFF)
        Init (int): inital value (0x00-0xFFFF)
        RefIn (bool): reverse input bits
        RefOut (bool): reverse output bits
        XorOut (int): final xor value (0x00-0xFFFF)

    Returns:
        int: calculated CRC16 checksum.
    """
    
    # parameter type check
    if not isinstance(Data, (bytes, bytearray)):
        raise TypeError("Data must be of type bytes or bytearray.")
    if not isinstance(Poly, int) or not (0 <= Poly <= 0xFFFF):
        raise TypeError("Polynomial must be an 16-bit integer (0x00-0xFFFF).")
    if not isinstance(Init, int) or not (0 <= Init <= 0xFFFF):
        raise TypeError("Initial value must be an 16-bit integer (0x00-0xFFFF).")
    if not isinstance(RefIn, bool):
        raise TypeError("Input reversal must be boolean (True/False).")
    if not isinstance(RefOut, bool):
        raise TypeError("Output reversal must be boolean (True/False).")
    if not isinstance(XorOut, int) or not (0 <= XorOut <= 0xFFFF):
        raise TypeError("Final XOR value must be an 16-bit integer (0x00-0xFFFF).")

    # determine the endianness of the system
    little_endian = True if int.from_bytes(b'\x01\x00', byteorder='little') == 1 else False

    # init checksum
    crc = Init
    
    # calculate checksum over data array
    for byte in Data:
        
        # reverse input bits if specified
        if RefIn == True:
            byte = int('{:08b}'.format(byte)[::-1], 2)

        # XOR with next data byte. Account for endianness
        if little_endian:
            crc ^= (byte << 8)
        else:
            crc ^= byte
            
        # shift left until MSB is 1, then XOR with polynom
        for _ in range(8):
            if crc & 0x8000:
                crc = (crc << 1) ^ Poly
            else:
                crc <<= 1
                        
        # clip to 16-bit
        crc &= 0xFFFF

    # finalize checksum
    crc = crc ^ XorOut
    
    # reverse output bits if specified
    if RefOut:
        crc = int('{:016b}'.format(crc)[::-1], 2)
    
    # return checksum
    return crc & 0xFFFF


def calculate_crc32(Data:bytearray=None, Poly:int=0x04C11DB7, Init:int=0xFFFFFFFF, RefIn:bool=True, RefOut:bool=True, XorOut:int=0xFFFFFFFF) -> int:
    """
    Calculate CRC32 checksum for the given byte array.
    Default parameters are for CRC32 in https://crccalc.com/

    Args:
        Data (bytearray): byte array to calculate checksum over
        Poly (int): used polynom (0x0-0xFFFFFFFF)
        Init (int): inital value (0x0-0xFFFFFFFF)
        RefIn (bool): reverse input bits
        RefOut (bool): reverse output bits
        XorOut (int): final xor value (0x0-0xFFFFFFFF)

    Returns:
        int: calculated CRC32 checksum.
    """

    # parameter type check
    if not isinstance(Data, (bytes, bytearray)):
        raise TypeError("Data must be of type bytes or bytearray.")
    if not isinstance(Poly, int) or not (0 <= Poly <= 0xFFFFFFFF):
        raise TypeError("Polynomial must be an 32-bit integer (0x0-0xFFFFFFFF).")
    if not isinstance(Init, int) or not (0 <= Init <= 0xFFFFFFFF):
        raise TypeError("Initial value must be an 32-bit integer (0x0-0xFFFFFFFF).")
    if not isinstance(RefIn, bool):
        raise TypeError("Input reversal must be boolean (True/False).")
    if not isinstance(RefOut, bool):
        raise TypeError("Output reversal must be boolean (True/False).")
    if not isinstance(XorOut, int) or not (0 <= XorOut <= 0xFFFFFFFF):
        raise TypeError("Final XOR value must be an 32-bit integer (0x0-0xFFFFFFFF).")

    # determine the endianness of the system
    little_endian = True if int.from_bytes(b'\x01\x00', byteorder='little') == 1 else False

    # init checksum
    crc = Init
    
    # calculate checksum data array
    for byte in Data:
        
        # reverse input bits if specified
        if RefIn == True:
            byte = int('{:08b}'.format(byte)[::-1], 2)
        
        # XOR with next data byte. Account for endianness
        if little_endian:
            crc ^= (byte << 24)
        else:
            crc ^= byte
            
        # shift left until MSB is 1, then XOR with polynom
        for _ in range(8):
            if crc & 0x80000000:
                crc = (crc << 1) ^ Poly
            else:
                crc <<= 1
                        
        # clip to 32-bit
        crc &= 0xFFFFFFFF
                
    # finalize checksum
    crc = crc ^ XorOut

    # reverse output bits if specified
    if RefOut:
        crc = int('{:032b}'.format(crc)[::-1], 2)
   
    # return checksum
    return crc



if __name__ == "__main__":

    import random
    
    # Example usage
    data = bytearray([0x01, 0x02, 0x03, 0x04])
    random.seed(42); data = bytearray([random.randint(0, 255) for _ in range(100)])
    
    # calculate checksums
    crc8  = calculate_crc8(Data=data)
    crc16 = calculate_crc16(Data=data)
    crc32 = calculate_crc32(Data=data)
    
    # print results
    print("input:", end=" ")
    #print([f"0x{byte:02X}" for byte in data], end=" ")
    print(''.join(['{:02X}'.format(byte) for byte in data]))
    print(f"CRC8: 0x{crc8:02X}")
    print(f"CRC16/CCITT-FALSE: 0x{crc16:04X}")
    print(f"CRC32: 0x{crc32:08X}")
    