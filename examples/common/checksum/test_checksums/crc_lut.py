# -*- coding: utf-8 -*-
"""
CRC8, CRC16 and CRC32 using lookup tables

For common polynoms, initals etc. see https://crccalc.com/
Results have been checked checked vs. https://crccalc.com/


@author: gicking @ Github
"""

# global lookup tables
__CRC8_LUT  = None
__CRC16_LUT = None
__CRC32_LUT = None


def __generate_crc8_lut(Poly:int=0x07):
    """
    Calculates the CRC8 lookup table using the specified polynomial.

    Args:
        Poly (int): polynomial used for CRC8 calculation

    Returns:
        list: The CRC8 lookup table.

    """

    # initialize table 256*1B
    crc_table = [0] * 256

    # calculate LUT entries for given polynom
    for divident in range(256):
        current_byte = divident
        for _ in range(8):
            if current_byte & 0x80:
                current_byte = (current_byte << 1) ^ Poly
            else:
                current_byte <<= 1
        crc_table[divident] = current_byte & 0xFF

    #print([hex(num) for num in crc_table])
    return crc_table


def __generate_crc16_lut(Poly:int=0x1021):
    """
    Calculates the CRC16 lookup table using the specified polynomial.

    Args:
        Poly (int): polynomial used for CRC16 calculation

    Returns:
        list: The CRC16 lookup table.

    """
    
    # determine the endianness of the system
    little_endian = True if int.from_bytes(b'\x01\x00', byteorder='little') == 1 else False
    
    # initialize table 256*2B
    crc_table = [0] * 256

    # calculate LUT entries for given polynom. Account for system endianness
    for divident in range(256):
        current_value = divident << 8 if little_endian else divident
        for _ in range(8):
            if current_value & 0x8000:
                current_value = (current_value << 1) ^ Poly
            else:
                current_value <<= 1
        crc_table[divident] = current_value & 0xFFFF

    #print([hex(num) for num in crc_table])
    return crc_table


def __generate_crc32_lut(Poly: int = 0x04C11DB7):
    """
    Calculates the CRC32 lookup table using the specified polynomial.

    Args:
        Poly (int): polynomial used for CRC32 calculation

    Returns:
        list: The CRC32 lookup table.

    """

    # Determine the endianness of the system
    little_endian = True if int.from_bytes(b'\x01\x00', byteorder='little') == 1 else False

    # initialize table 256*4B
    crc_table = [0] * 256

    # calculate LUT entries for given polynom. Account for system endianness
    for divident in range(256):
        current_value = divident << 24 if little_endian else divident
        for _ in range(8):
            if current_value & 0x80000000:
                current_value = (current_value << 1) ^ Poly
            else:
                current_value <<= 1
        crc_table[divident] = current_value & 0xFFFFFFFF

    #print([hex(num) for num in crc_table])
    return crc_table



def calculate_crc8(Data: bytearray = None, Poly:int=0x07, Init: int = 0x00, RefIn: bool = False, RefOut: bool = False, XorOut: int = 0x00):
    """
    Calculate CRC8 checksum for the given byte array using lookup table.
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

    global __CRC8_LUT

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

    # compute CRC8 lookup table (global) on first call
    if __CRC8_LUT is None:
        __CRC8_LUT = __generate_crc8_lut(Poly)
    
    # init checksum
    crc = Init

    # calculate checksum over bytes in data array
    for byte in Data:
        
        # reverse input bits if specified
        if RefIn == True:
            byte = int('{:08b}'.format(byte)[::-1], 2)

        # update with correct LUT content
        crc = __CRC8_LUT[crc ^ byte]

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

    global __CRC16_LUT

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

    # compute CRC16 lookup table (global) on first call
    if __CRC16_LUT is None:
        __CRC16_LUT = __generate_crc16_lut(Poly)

    # determine the endianness of the system
    little_endian = True if int.from_bytes(b'\x01\x00', byteorder='little') == 1 else False

    # init checksum
    crc = Init
    
    # little endianness
    if little_endian:
        
        # calculate checksum over data array
        for byte in Data:
        
            # reverse input bits if specified
            if RefIn == True:
                byte = int('{:08b}'.format(byte)[::-1], 2)

            # XOR with next data byte. Account for endianness
            crc = (crc << 8) ^ __CRC16_LUT[((crc >> 8) ^ byte) & 0xFF]
            
            # clip to 16-bit
            crc &= 0xFFFF

    # big endian
    else:

        # calculate checksum over data array
        for byte in Data:
        
            # reverse input bits if specified
            if RefIn == True:
                byte = int('{:08b}'.format(byte)[::-1], 2)

            # XOR with next data byte. Account for endianness
            crc = (crc >> 8) ^ __CRC16_LUT[(crc ^ byte) & 0xFF]
            
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
    Default parameters are for CRC16-CCITT-FALSE in https://crccalc.com/

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

    global __CRC32_LUT

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

    # compute CRC16 lookup table (global) on first call
    if __CRC32_LUT is None:
        __CRC32_LUT = __generate_crc32_lut(Poly)

    # determine the endianness of the system
    little_endian = True if int.from_bytes(b'\x01\x00', byteorder='little') == 1 else False

    # init checksum
    crc = Init 
    
    # little endianness
    if little_endian:
        
        # calculate checksum over data array
        for byte in Data:
        
            # reverse input bits if specified
            if RefIn == True:
                byte = int('{:08b}'.format(byte)[::-1], 2)

            # XOR with next data byte. Account for endianness
            crc = (crc << 8) ^ __CRC32_LUT[((crc >> 24) ^ byte) & 0xFF]
            
            # clip to 32-bit
            crc &= 0xFFFFFFFF
            
    # big endian
    else:

        # calculate checksum over data array
        for byte in Data:
                
            # reverse input bits if specified
            if RefIn == True:
                byte = int('{:08b}'.format(byte)[::-1], 2)

            # XOR with next data byte. Account for endianness
            crc = (crc >> 8) ^ __CRC32_LUT[(crc ^ byte) & 0xFF]
                    
            # clip to 32-bit
            crc &= 0xFFFFFFFF

    # finalize checksum
    crc = crc ^ XorOut
    
    # reverse output bits if specified
    if RefOut:
        crc = int('{:032b}'.format(crc)[::-1], 2)
    
    # return checksum
    return crc & 0xFFFFFFFF



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
    