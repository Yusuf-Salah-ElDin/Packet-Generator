#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <cmath>
using namespace std;
class Ethernet{
    public:
        int ethLineRate;
        int ethCaptureSizeMs; //this is the time of generation, for example if it is 10 ms then 1 frame will be generated
        int ethMinNumOfIFGsPerPacket; //IFG is at the end of each packet after CRC, and e.g : if min number of ifgs is 3 then after each packet there should be 3 IFGs and you have to consider the alignment, so in this case if the packet is not 4 byte aligned we need to add extra IFGs 0x07
        string ethDestAddress;  // refactor it to Packet class
        string ethSourceAddress;    // refactor it to packet class
        int ethMaxPacketSize;    // Packet size in bytes, includes the preamble, SoP, SRC Add, Dest Add, ether type, payload, and CRC, if the packet size exceeds this number then fragmentation should happen. 
        int ethBurstSize;				// Number of ethernet packets in one burst
        int ethBurstPeriodicity_us;  // The periodicity of the burst in microseconds

};

class Packet {
    public:
    // structure of the packet:
        unsigned char preamble[8];
        unsigned char destAddress[6];
        unsigned char sourceAddress[6];
        unsigned short ethType; // 2 bytes
        unsigned char payload[1462];    // total size is 1500
        unsigned char fcs[4];
        unsigned char ifg[12];
        unsigned char fullPacket[1500];
        void generatePacket(){
            ofstream outputFile("fullPacketOutput.txt");

            if (!outputFile) {
                cerr << "Error opening file!" << std::endl;
                return;
            }
            int offset = 0;

            // Copy Preamble (8 bytes)
            memcpy(fullPacket + offset, preamble, sizeof(preamble));
            offset += sizeof(preamble);  // Move offset by 8 bytes
            // Copy Destination Address (6 bytes)
            memcpy(fullPacket + offset, destAddress, sizeof(destAddress));
            offset += sizeof(destAddress);  // Move offset by 6 bytes
            // Copy Source Address (6 bytes)
            memcpy(fullPacket + offset, sourceAddress, sizeof(sourceAddress));
            offset += sizeof(sourceAddress);  // Move offset by 6 bytes
            // Copy Ethernet Type (2 bytes)
            memcpy(fullPacket + offset, &ethType, sizeof(ethType));
            offset += sizeof(ethType);  // Move offset by 2 bytes
            // Copy Payload (1462 bytes)
            memcpy(fullPacket + offset, payload, sizeof(payload));
            offset += sizeof(payload);  // Move offset by 1462 bytes
            // Copy FCS (4 bytes)
            memcpy(fullPacket + offset, fcs, sizeof(fcs));
            offset += sizeof(fcs);  // Move offset by 4 bytes
            // Copy IFG (12 bytes)
            memcpy(fullPacket + offset, ifg, sizeof(ifg));
            offset += sizeof(ifg);  // Move offset by 12 bytes
            // Print out the fullPacket array as hex for verification
            //outputFile << "Full Packet: " << endl;
            for (int i = 0; i < offset; ++i) {
                char buffer[10];
                sprintf(buffer, "%04X\n", fullPacket[i]);  // Format each byte as two-digit hex
                outputFile << buffer;
            }
            cout << endl;
            outputFile.close();
            
        }      
};

int main(){
    
    Ethernet eth;
    ifstream in_file;
    string fileName;
    cout << "enter text file name" << endl;
    cin >> fileName;
    string line;
    vector <string> values;
    // open the file
    in_file.open(fileName);
    if(in_file.is_open()){
        cout << "file has opened successfully" << endl;
        while(getline(in_file,line)){
            stringstream ss(line);
            string varName;
            char equalSign;
            string value;
            ss >> varName >> equalSign >> value;
            values.push_back(value);
        }
        
        //Read the parameters from the text file
        eth.ethLineRate = stoi(values.at(0));
        eth.ethCaptureSizeMs = stoi(values.at(1));
        eth.ethMinNumOfIFGsPerPacket = stoi(values.at(2));
        eth.ethDestAddress = values.at(3);
        eth.ethSourceAddress = values.at(4);
        eth.ethMaxPacketSize = stoi(values.at(5));
        eth.ethBurstSize = stoi(values.at(6));
        eth.ethBurstPeriodicity_us = stoi(values.at(7));
    }
    else{
        cout << "couldn't open file";
    }
    Packet pkt;
    // filling preamble fixed:
    pkt.preamble[0] = 0xFB;
    for(int i = 1; i < 7; ++i){
        pkt.preamble[i] = 0x55;
    }
    pkt.preamble[7] = 0xD5;
    //-------------------
    // ethernet type assumed to be ipv4 fixed
    pkt.ethType = 0x0800;
    //--------------------------------------------
    // set payload to be all zeros variable
    memset(pkt.payload, 0x00, 1462);
    //--------------------------------------------
    // set destination and source addresses as provided from the text file:
    for (size_t i = 2; i < eth.ethDestAddress.length(); i += 2) { // Start from index 2 to skip "0x"
        std::string byteString = eth.ethDestAddress.substr(i, 2); // Get 2 characters
        unsigned char byteValue = static_cast<unsigned char>(std::stoi(byteString, nullptr, 16)); // Convert to unsigned char
        fill(pkt.destAddress, pkt.destAddress + 6, byteValue);  // store in array
        //pkt.destAddress.push_back(byteValue); // Store in vector
    }

    for (size_t i = 2; i < eth.ethSourceAddress.length(); i += 2) { // Start from index 2 to skip "0x"
        std::string byteString = eth.ethSourceAddress.substr(i, 2); // Get 2 characters
        unsigned char byteValue = static_cast<unsigned char>(std::stoi(byteString, nullptr, 16)); // Convert to unsigned char
        fill(pkt.sourceAddress, pkt.sourceAddress + 6, byteValue);  // store in array
        //pkt.destAddress.push_back(byteValue); // Store in vector
    }
    //---------------------------------------------------------------------
    // set fcs to all zeros variable
    memset(pkt.fcs, 0x00, 4);
    // set IFG fixed
    for(int i = 0; i < 12; ++i){
        pkt.ifg[i] = 0x07;
    }
    
    // generate the packet for each capture size , capture size represnts one run of this code
    // frame duration is 10 ms
    if(eth.ethCaptureSizeMs >= 10 && eth.ethCaptureSizeMs %10 == 0){
        int numOfBurstsPerCapture = (eth.ethCaptureSizeMs * pow(10,-3)) / (eth.ethBurstPeriodicity_us * pow(10,-6));
        int numOfPackets = numOfBurstsPerCapture * eth.ethBurstSize;
        cout << "number of bursts per capture: " << numOfBurstsPerCapture << endl;
        cout << "total number of generated packets: " << numOfPackets << endl;
        float lineRateBytes = (eth.ethLineRate / 8) * pow(10,9);
        float packetGenerationTime = (eth.ethMaxPacketSize / lineRateBytes) / pow(10,-6);
        cout << "each packet is generated in: " << packetGenerationTime << "microseconds" << endl;
         for(int i = 1; i <= numOfPackets; ++i){
            cout << "Packet: " << i << endl;
            pkt.generatePacket();
         }
    }
    else if (eth.ethCaptureSizeMs < 10){
         cout << "Capture size is less than one frame duration which is 10 ms";
     }
    return 0;
}