This file is built with CPP and it generates dummy packets with parameters read from a text file and it writes the output to a new text file.
you need to enter the text file name and it generates a text file named "fullPacketOutput.txt"
The parameters that are read from the text file are as follows:
Eth.LineRate
Eth.CaptureSizeMs
Eth.MinNumOfIFGsPerPacket
Eth.DestAddress
Eth.SourceAddress
Eth.MaxPacketSize
Eth.BurstSize
Eth.BurstPeriodicity_us
The program contains two classes Ethernet and Packet.
