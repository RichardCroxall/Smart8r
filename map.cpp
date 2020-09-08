#include "include/runtime.h"

map::map()
{
    intToDeviceCode[0] = UNIT1;
    intToDeviceCode[1] = UNIT2;
    intToDeviceCode[2] = UNIT3;
    intToDeviceCode[3] = UNIT4;
    intToDeviceCode[4] = UNIT5;
    intToDeviceCode[5] = UNIT6;
    intToDeviceCode[6] = UNIT7;
    intToDeviceCode[7] = UNIT8;
    intToDeviceCode[8] = UNIT9;
    intToDeviceCode[9] = UNIT10;
    intToDeviceCode[10] = UNIT11;
    intToDeviceCode[11] = UNIT12;
    intToDeviceCode[12] = UNIT13;
    intToDeviceCode[13] = UNIT14;
    intToDeviceCode[14] = UNIT15;
    intToDeviceCode[15] = UNIT16;

    deviceCodeToInt[UNIT1] = 0;
    deviceCodeToInt[UNIT2] = 1;
    deviceCodeToInt[UNIT3] = 2;
    deviceCodeToInt[UNIT4] = 3;
    deviceCodeToInt[UNIT5] = 4;
    deviceCodeToInt[UNIT6] = 5;
    deviceCodeToInt[UNIT7] = 6;
    deviceCodeToInt[UNIT8] = 7;
    deviceCodeToInt[UNIT9] = 8;
    deviceCodeToInt[UNIT10] = 9;
    deviceCodeToInt[UNIT11] = 10;
    deviceCodeToInt[UNIT12] = 11;
    deviceCodeToInt[UNIT13] = 12;
    deviceCodeToInt[UNIT14] = 13;
    deviceCodeToInt[UNIT15] = 14;
    deviceCodeToInt[UNIT16] = 15;
    
    houseCodeToInt[HouseA] = 0;
    houseCodeToInt[HouseB] = 1;
    houseCodeToInt[HouseC] = 2;
    houseCodeToInt[HouseD] = 3;
    houseCodeToInt[HouseE] = 4;
    houseCodeToInt[HouseF] = 5;
    houseCodeToInt[HouseG] = 6;
    houseCodeToInt[HouseH] = 7;
    houseCodeToInt[HouseI] = 8;
    houseCodeToInt[HouseJ] = 9;
    houseCodeToInt[HouseK] = 10;
    houseCodeToInt[HouseL] = 11;
    houseCodeToInt[HouseM] = 12;
    houseCodeToInt[HouseN] = 13;
    houseCodeToInt[HouseO] = 14;
    houseCodeToInt[HouseP] = 15;

    intToHouseCode[ 0] = HouseA;
    intToHouseCode[ 1] = HouseB;
    intToHouseCode[ 2] = HouseC;
    intToHouseCode[ 3] = HouseD;
    intToHouseCode[ 4] = HouseE;
    intToHouseCode[ 5] = HouseF;
    intToHouseCode[ 6] = HouseG;
    intToHouseCode[ 7] = HouseH;
    intToHouseCode[ 8] = HouseI;
    intToHouseCode[ 9] = HouseJ;
    intToHouseCode[10] = HouseK;
    intToHouseCode[11] = HouseL;
    intToHouseCode[12] = HouseM;
    intToHouseCode[13] = HouseN;
    intToHouseCode[14] = HouseO;
    intToHouseCode[15] = HouseP;
}