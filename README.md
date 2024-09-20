This file is built with CPP and it generates dummy packets with parameters read from a text file and it writes the output to a new text file.
you need to enter the text file name and it generates a text file named "fullPacketOutput.txt"
The parameters that are read from the text file are as follows:
Eth.LineRate = 10
Eth.CaptureSizeMs = 10
Eth.MinNumOfIFGsPerPacket = 12
Eth.DestAddress = 0x010101010101
Eth.SourceAddress = 0x333333333333
Eth.MaxPacketSize = 1500
Eth.BurstSize = 3
Eth.BurstPeriodicity_us = 100
The program contains two classes Ethernet and Packet.
