#!/usr/bin/python3
import serial
import time
import crcmod

def calculate_CRC32(Buffer):
    Buffer_Length = len(Buffer)
    CRC_Value = 0xFFFFFFFF
    for DataElem in Buffer[0:Buffer_Length]:
        CRC_Value = CRC_Value ^ DataElem
        for _ in range(32):
            if(CRC_Value & 0x80000000):
                CRC_Value = (CRC_Value << 1) ^ 0x04C11DB7
            else:
                CRC_Value = (CRC_Value << 1)
    return CRC_Value & 0xFFFFFFFF


page_size = 1024

Commands_Names = [
    "BL_GET_VER_CMD",
	"BL_GET_HELP_CMD",
	"BL_GET_CID_CMD",
	"BL_GET_RDP_STATUS_CMD",
	"BL_GO_TO_ADDR_CMD",
	"BL_FLASH_ERASE_CMD",
	"BL_MEM_WRITE_CMD",
	"BL_MEM_READ_CMD",
    "BL_JUMP_TO_MAIN",
	"BL_CHANGE_ROP_Level_CMD",
]
        
def printMenu():
    print("choose one of the booloader commands")
    print("------------------------------------")
    print("1- Bootloader get version")
    print("2- Bootloader get help")
    print("3- Bootloader get Chip Id")
    print("4- Bootloader get Read Protection Status")
    print("5- Bootloader go to Address")
    print("6- Bootloader Erase Flash")
    print("7- Bootloader Write Memory")
    print("8- Bootloader Read Memory")
    print("9- Jump To The App")
    print("10- Bootloader Change Read Out Protection Level")
    print("11- quit")
    try:
        choice = int(input())
    except:
        print("wrong input format")
        return 0 
    return choice

def sendToTarget(ser, command):
    send_success = False

    ser.write((len(command) + 4).to_bytes(2,'little'))
    ser.write(command)

    crc =  calculate_CRC32((len(command) + 4).to_bytes(2,'little') + command)
    ser.write(crc.to_bytes(4, 'little'))

    bootloader_response = int(ser.read(1)[0])
    
    data = []
    if bootloader_response == 1:
        send_success = True
        data_length = ser.read(1)
        data = ser.read(int(data_length[0]))

    return (send_success, data)

def sendBootloader(choice, ser):
    if choice == 1:
        print("Get Version Request")
        print("--------------------")

        command = bytearray()
        command.extend(bytes.fromhex("10"))

        success, data = sendToTarget(ser, command)    
        if success == True:    
            data = [str(int(x)) for x in data]
            print("Bootloader Vendor Id: " + data[0]) 
            print("Bootloader Version: " + data[1] + " . " + data[2] + " . " + data[3]) 
        else:
            print("bootloader sent nack")


    elif choice == 2:
        print("Get Help Request")
        print("--------------------")
        
        command = bytearray()
        command.extend(bytes.fromhex("11"))
        
        success, data = sendToTarget(ser, command)    
        if success == True:
            data = [hex(x) for x in data]
            print("Available Commands: ")
            for i in range(len(data)):
                print(f"     {Commands_Names[i]: <30}    ->     {data[i]}")
        else:
            print("bootloader sent nack")

    elif choice == 3:
        print("Get Chip ID")
        print("--------------------")

        command = bytearray()
        command.extend(bytes.fromhex("12"))

        success, data = sendToTarget(ser, command)    
        if success == True:
            data = [int(x) for x in data]
            data[1] = data[1] << 8
            data = hex(data[0] + data[1])
            print("Chip ID: " + str(data))
        else:
            print("bootloader sent nack")

    elif choice == 4:
        print("Get Read Protection LeveL")
        print("--------------------")

        command = bytearray()
        command.extend(bytes.fromhex("13"))
        
        success, data = sendToTarget(ser, command)    
        if success == True:
            if(int(data[0]) == 0):
                RPD = 1
            else:
                RPD = 0
            print("RDP LeveL: " + str(RPD))
        else:
            print("bootloader sent nack")

    elif choice == 5:
        print("Go To Address")
        print("--------------------")

        address = input("Enter the address to go to: ")
        
        command = bytearray()
        command.extend(bytes.fromhex("14"))
        
        if len(bytearray.fromhex(address)) != 4:
            print("Address must be 4 bytes long")
        else:
            address_arr = bytearray.fromhex(address)
            address_arr.reverse()

            command.extend(address_arr)

            success, data = sendToTarget(ser, command) 
            if success == True:
                print("Gone to Address ->", "0x" + address)
                print(ser.read_until())  
            else:
                print("bootloader sent nack")

    elif choice == 6:
        print("Erase Flash")
        print("------------")

        start_page = int(input("Enter the start page (0 - 127): "))
        number_of_pages = int(input("Enter the number of pages to erase: "))

        command = bytearray()
        command.extend(bytes.fromhex("15"))
        command.extend(start_page.to_bytes(1, 'little'))
        command.extend(number_of_pages.to_bytes(1, 'little'))

        success, data = sendToTarget(ser, command)    
        if success == True:
            print("Pages",str([i for i in range(start_page, start_page+number_of_pages)]), "erased")
        else:
            print("bootloader sent nack")
    
    elif choice == 7:
        print("Write To Memory")
        print("------------")
        file_name = input("Enter the file name: ")
        page_number = int(input("Enter the page number (0-127): "))
         
        binary_data = []
        try:
            with open(file_name, 'rb') as file:
                binary_data = file.read()
                binary_data = [binary_data[i*1024:i*1024+1024] for i in range((len(binary_data) + 1023) // 1024)]
        except:
            print("File not exists in current directory")

        for data in binary_data:
            command = bytearray()
            command.extend(bytes.fromhex("16"))
            command.extend((page_number).to_bytes(1, 'little'))
            command.extend((len(data)).to_bytes(2, 'little'))
            command.extend(data)

            success, _ = sendToTarget(ser, command)
            if success == True:
                print(f"page {page_number} is written")
                page_number += 1
            else:
                print("bootloader sent nack on writing page", page_number)
                page_number += 1
        
    elif choice == 8:
        print("Memory Read")
        print("--------------------")

        address = input("Enter the address to read: ")
         
        command = bytearray()
        command.extend(bytes.fromhex("17"))
        
        if len(bytearray.fromhex(address)) != 4:
            print("Address must be 4 bytes long")
        else:
            noOfBytes = input("Enter the number of bytes: ")
            address = bytearray.fromhex(address)
            address.reverse()

            command.extend(address)
            command.extend(int(noOfBytes).to_bytes(4, 'little'))

            success, data = sendToTarget(ser, command) 

            if success == True: 
                data = bytearray(data)
                data.reverse()          
                print("Data: " + data.hex())
            else:
                print("bootloader sent nack")


    elif choice == 9:
        print("jump to app")
        print("--------------------")
        page_number = int(input("Enter the page number to jump to (0-127): "))
         
        command = bytearray()
        command.extend(bytes.fromhex("18"))
        command.extend((page_number).to_bytes(1, 'little'))

        success, _ = sendToTarget(ser, command)
        if success == True:
            print("jumped") 
        else:
            print("bootloader sent nack")

    elif choice == 10:
        print("Change Read Protection Level")
        print("--------------------")
        print("Enter the Level (0 | 1): ", end="")
        level = int(input())
        if level not in [0,1]:
            print("Protection Level must be 0 | 1")
        
        else:
            command = bytearray()
            command.extend(bytes.fromhex("19"))
            if level == 0:
                command.extend(bytes.fromhex("A5"))
            else:
                command.extend(bytes.fromhex("00"))

            success, _ = sendToTarget(ser, command)    
            if success == True:
                print(f"Read Protection Level Changed to {level}")
            else:
                print("bootloader sent nack")
    
    else:
        print("Command is not supported")

ser = True

while True:
    try:
        ser = serial.Serial("/dev/ttyUSB0", baudrate=115200)
        break
    except: 
        print("Error: cannot open the serial");
        time.sleep(2)
        continue

while True: 
    choice = printMenu()
    if choice == 11:
        break
    sendBootloader(choice, ser)
    print()




