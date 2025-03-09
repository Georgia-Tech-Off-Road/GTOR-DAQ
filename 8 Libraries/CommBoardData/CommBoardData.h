#include <stdint.h>
#include <vector>
#include "SensorInfo.h"
#include "PacketInstructions.h"
#include <bitset>
namespace cmb {
    //Stores information about the packet that was created by getPacketData(uint8_t)
    struct CreatedPacket {
        //Points to the location in memory where the packet is stored
        void* packetPointer;
        //Size of the packet in bits
        size_t packet_size_bits; 
    };
    typedef std::bitset<64> data_return_type;

    struct SensorDataInfo {
        Data::getDataFunction getData;
        //Size of the data in bits
        int data_size_bits;

        SensorDataInfo(Data::getDataFunction getDataFunc, int size) {
            getData = getDataFunc;
            //Size of the data (in bits)
            data_size_bits = size;
        } 
    };
    class Data {
        public:
            typedef data_return_type(Data::*getDataFunction)(void);

            std::vector<PacketInstructions> packets; 

            //Stores info about which function in Data retrieves data for each sensor, and the size of that data (in bits)
            //The indice of the information for each sensor should be the same as its SensorIndex::SensorName value.
            static std::vector<SensorDataInfo> sensorDataInfo;

            Data();

        public:
        //Time since data aquisition began
        uint16_t millis;
        //Is clutch engaged or not?
        unsigned int clutchEngaged : 1;
        //Pit stop requested?
        unsigned int pitRequested : 1;
    
        //Speed X
        float speed_x;
        //Speed Y
        float speed_y;
        //Speed Z
        float speed_z;
    
        //Position X
        float position_x;
        //Position Y
        float position_y;
        //Position Z
        float position_z;
    
        unsigned int brake_pressure_front : 12;
        unsigned int brake_pressure_back : 12; 
    
        unsigned int RPM : 14;
    
        //Linear Accelerations
        float linear__acceleration_x;
        float linear_acceleration_y;
        float linear_acceleartion_z;
    
        //Continuously Variable Transmission Temp
        unsigned int cvt_temp : 12;
    
        //Brake Temperature (PLEASE RENAME THESE!!!!!!)
        //TODO: Rename brake temperatures
        unsigned int break_temp_front_left : 12;
        unsigned int break_temp_front_right : 12;
        unsigned int break_temp_back : 12;
    
        //Steering wheel rotation rotation
        unsigned int steering_rotation : 12;

        public:
            data_return_type getMilliSec();
            data_return_type getClutchEngaged();
            data_return_type getPitRequested();
            data_return_type getPositionX();
            data_return_type getPositionY();
            data_return_type getPositionZ();


        
    CreatedPacket getPacketData(uint8_t);
    }
}