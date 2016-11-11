#include <stdio.h>
#include <stdlib.h>
#include "bitstream.h"
#include "log.h"

#define PRINT_ERROR(bs, msg) bs->Error = 1; Log(msg)


BitStream* BitStream_Create(char* data, size_t bitPosition, size_t length) {
    BitStream* result = malloc(sizeof(BitStream));
    if (result == NULL) {
        Log("failed to allocate memory for BitStream");
    }
    result->CurrentBytePosition = data;
    result->CurrentBitPosition = bitPosition;
    result->Length = length;
    result->Error = 0;
    return result;
}

void BitStream_Destroy(BitStream* bitStream) {
    if (bitStream == NULL) {
        return;
    }
    free(bitStream);
}

char* BitStream_ToString(BitStream* bitStream) {
    int bit;
    int i;
    int length = bitStream->Length;
    char* currentPosition = bitStream->CurrentBytePosition;
    size_t resultLength = CHAR_SIZE * BITS * length;
    char* result = malloc(resultLength + 1);
    if (result == NULL) {
        PRINT_ERROR(bitStream, "failed to allocate memory for string representation");
        return NULL;
    }
    result[resultLength] = '\0';
    if (length > 0) {
        for (bit = 0; bit < bitStream->CurrentBitPosition; ++bit) {
            result[bit] = '-';
        }
        for (bit = bitStream->CurrentBitPosition; bit < BITS; ++bit) {
            result[bit] = (currentPosition[0] >> BITS - 1 - bit) & 1 ? '1' : '0';
        }
    }
    for (i = 1; i < length; ++i) {
        for (bit = 0; bit < BITS; ++bit) {
            result[i * BITS + bit] = (currentPosition[i] >> BITS - 1 - bit) & 1 ? '1' : '0';
        }
    }
    return result;
}

void BitStream_Print(BitStream* bitStream) {
    char* representation = BitStream_ToString(bitStream);
    printf("%s\n", representation);
    free(representation);
}


size_t BitStream_AvaliableBits(BitStream* bitStream) {
    char *curr = bitStream->CurrentBytePosition;
    size_t currBitPos = bitStream->CurrentBitPosition;
    return BITS - currBitPos + (bitStream->Length - 1) * BITS;
}

long long int BitStream_ReadInt(BitStream* bitStream, size_t bitCount) {
    if (bitCount > INT_BITS) {
        PRINT_ERROR(bitStream,"unable to read more than INT_BITS");
        return 0;
    }
    size_t avaliable = BitStream_AvaliableBits(bitStream);
    if (bitCount > avaliable) {
        LogT("failed to read bits: %d / %d", bitCount, avaliable);
        PRINT_ERROR(bitStream,"not enough bits");
        return 0;
    }


    
    char* curr = bitStream->CurrentBytePosition;
    size_t currBitPos = bitStream->CurrentBitPosition;
    long long int result = 0;
    size_t bitsReaded;
    #ifdef DEBUG
        LogT("trying to read int:");
    #endif
    for (bitsReaded = 0; bitsReaded < bitCount; ++bitsReaded) {
        #ifdef DEBUG
        LogT("reading bit: %d / %d", bitsReaded, bitCount);
        #endif
        size_t bitOffset = bitCount - 1 - bitsReaded + currBitPos;
        int bytePos = bitOffset / BITS;
        size_t bitPos = bitOffset % BITS;
        result |= ((long long int)((curr[bytePos] >> BITS - 1 - bitPos) & 1) << bitsReaded);
    }
    size_t lengthChange = (bitCount + currBitPos) / BITS;
    bitStream->CurrentBytePosition = bitStream->CurrentBytePosition + lengthChange;
    bitStream->CurrentBitPosition = (bitCount + currBitPos) % BITS;
    bitStream->Length -= lengthChange;
    #ifdef DEBUG
        LogT("int readed:%d", result);
    #endif 
    return result;
}


#define SYMBOLS_COUNT 256
char Map[SYMBOLS_COUNT];
int CurrentPositionInMap = 0;

void AddToMap(char c) {
    Map[CurrentPositionInMap++] = c;
}

void EnumerateCharacters(char from, char to) {
    char i;
    for (i = from; i <= to; ++i) {
        AddToMap(i);
    }
}

int NeededBits(int value) {
    int res;
    for (res = 0; res < 64; ++res) {
        if (value <= (1 << res)) {
            return res;
        }
    }
    return 64;
}

void ReadAndPrint(BitStream* bs, size_t bits) {
    char t = BitStream_ReadInt(bs, bits);
    printf("%c\n", Map[t]);
    BitStream_Print(bs);
}

/*
int main() {

    char* data = "Abbabaca";
    int len = strlen(data);
    printf("%s %d\n", data, len);
    BitStream* bitStream = BitStream_Create(data, 0, len);
    BitStream_Print(bitStream);
    ReadAndPrint(bitStream, BitsForSymbolInMap);
    ReadAndPrint(bitStream, BitsForSymbolInMap);
    ReadAndPrint(bitStream, BitsForSymbolInMap);
    ReadAndPrint(bitStream, BitsForSymbolInMap);
    ReadAndPrint(bitStream, BitsForSymbolInMap);
    ReadAndPrint(bitStream, BitsForSymbolInMap);
    ReadAndPrint(bitStream, BitsForSymbolInMap);
    ReadAndPrint(bitStream, BitsForSymbolInMap);
    BitStream_Destroy(bitStream);
    return 0;
}*/
