
#include <gtest/gtest.h>

#include "Tools.h"

TEST(Utility,contSignFromLowBit) {

    uint64_t x;
    int index = -1;

    x = 0xFFFFFFFeFFF;
    index = contSignFromLowBit(x);
    EXPECT_EQ(index,12);

    x = 0xFFFFFFFFFFFFFFFe;
    index = contSignFromLowBit(x);
    EXPECT_EQ(index,0);

    x = 0xFFFFFFFFFFFFFFFF;
    index = contSignFromLowBit(x);
    EXPECT_EQ(index,64);

    x = 0x00000000FFF;
    index = contSignFromLowBit(x);
    EXPECT_EQ(index,12);

    x = 1;
    index = contSignFromLowBit(x);
    EXPECT_EQ(index,1);

    x = 0x0000000000000000;
    index = contSignFromLowBit(x);
    EXPECT_EQ(index,0);
}


TEST(Utility,GetRadixFloor) {

    uint64_t reval;

    reval = GetRadixFloor(11,5);
    EXPECT_EQ(reval,10);

    reval = GetRadixFloor(14,5);
    EXPECT_EQ(reval,10);

    reval = GetRadixCeil(11,5);
    EXPECT_EQ(reval,15);

    reval = GetRadixCeil(14,5);
    EXPECT_EQ(reval,15);

    reval = GetRadixFloor(10,5);
    EXPECT_EQ(reval,10);

    reval = GetRadixCeil(10,5);
    EXPECT_EQ(reval,10);
}

TEST(Utility,bitMapCheck) {

//High Bit                                                     Low Bit
//0x0000FCFFF7FF8000               |->27|-> 24      |->15 (27 - 15 = 12)
//0x0000  F    C    F    F    F    |7   | F    F    |8    0    0    0
// 0(16) 1111 1100 1111 1111 1111 0|111 |1111 1111 1|000 0000 0000 0000

    uint64_t bitmap[] = { 0x0000FCFFF7FF8000};
    size_t len;

    len = 12;
    bitMapCheck((uint8_t *)bitmap,15,&len);
    EXPECT_EQ(len,12);

    len = 30;
    bitMapCheck((uint8_t *)bitmap,15,&len);
    EXPECT_EQ(len,12);

    len = 30;
    bitMapCheck((uint8_t *)bitmap,20,&len);
    EXPECT_EQ(len,7);

    //                    Low byte  ---------------------------------------------------------------------> High Byte
    uint64_t bitmap2[] = { 0x0000000000000000,0xFFFFFFFFFFCFF800,0xFFFFFFFFFFFFFFFF,0x0000F7FFFFFFFFFF,0x0F0F0CAB0E14134C };
    //                    High Byte --------------------------------------------------------------------> Low Byte
    //                    0x0F0F0CAB0E14134C,00x0000 F         7 FFFFFFFFFF,|0xFF(8),|0xFFFFFFFFFF    C       |FF      8      00,       0x0000000000000000
    //                    randomVal(64)      0(16) 1111[F] 0|111[7] 1(40)   |1(64)   | 1(40)         11|00[C] |1(8) 1|000[8] 0(8)          0(64)
    //                                                      |->  235        |->192   |->128            |->86 B|->84  |-> 64 + 8 + 3 = 75

    //0xFFFFFFFFFFCFF800 : 11111111 11111111 11111111 11111111 11111111 1100 1111 1111 1000 00000000
    //0x0000F7FFFFFFFFFF : 00000000 00000000 11110111 11111111 11111111 11111111 11111111 11111111   43
    len = 149; //235 - 86
    bitMapCheck((uint8_t *)bitmap2,86,&len);
    EXPECT_EQ(len,149);

    len = 200;
    bitMapCheck((uint8_t *)bitmap2,86,&len);
    EXPECT_EQ(len,149);

    len = 100;
    bitMapCheck((uint8_t *)bitmap2,86,&len);
    EXPECT_EQ(len,100);

    len = 100;
    bitMapCheck((uint8_t *)bitmap2,200,&len);
    EXPECT_EQ(len,35);

    uint64_t bitmap3[] = { 0xFFFFFFFFFFFFFFFF };

    len = 0;
    bitMapCheck((uint8_t *)bitmap3,0,&len);
    EXPECT_EQ(len,0);

    len = 64;
    bitMapCheck((uint8_t *)bitmap3,0,&len);
    EXPECT_EQ(len,64);

    len = 15;
    bitMapCheck((uint8_t *)bitmap3,0,&len);
    EXPECT_EQ(len,15);

    len = 4;
    bitMapCheck((uint8_t *)bitmap3,60,&len);
    EXPECT_EQ(len,4);
}



TEST(Utility,bitMapSet) {
    uint64_t bitmap1[4] = { 0,0,0,0 };
    bitMapSet((uint8_t*)bitmap1,0,64);
    EXPECT_EQ(bitmap1[0],0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(bitmap1[1],0);
    EXPECT_EQ(bitmap1[2],0);
    EXPECT_EQ(bitmap1[3],0);


    uint64_t bitmap2[4] = { 0 };
    bitMapSet((uint8_t*)bitmap2,84,93);
    EXPECT_EQ(bitmap2[0],0);
    EXPECT_EQ(bitmap2[1],0xFFFFFFFFFFF00000);
    EXPECT_EQ(bitmap2[2],0x0001FFFFFFFFFFFF);
    EXPECT_EQ(bitmap2[3],0);

    uint64_t bitmap3[4] = { 0 };
    bitMapSet((uint8_t*)bitmap3,84,157);
    EXPECT_EQ(bitmap3[0],0);
    EXPECT_EQ(bitmap3[1],0xFFFFFFFFFFF00000);
    EXPECT_EQ(bitmap3[2],0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(bitmap3[3],0x0001FFFFFFFFFFFF);
}