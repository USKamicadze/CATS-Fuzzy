#ifndef BITSTREAM
#define BITSTREAM


#define CHAR_SIZE sizeof(char)
#define BITS (CHAR_SIZE * 8)
#define INT_BITS (sizeof(long long int) * 8)

typedef struct BitStream {
    char* CurrentBytePosition;
    size_t CurrentBitPosition;
    size_t Length;
	int Error;
} BitStream;

BitStream* BitStream_Create(char* data, size_t bitPosition, size_t length);
void BitStream_Destroy(BitStream* bitStream);

char* BitStream_ToString(BitStream* bitStream);
void BitStream_Print(BitStream* bitStream);
long long int BitStream_ReadInt(BitStream* bitStream, size_t bitCount);


#define SYMBOLS_COUNT 256
extern char Map[SYMBOLS_COUNT];
extern int CurrentPositionInMap;
extern int BitStreamError;

void AddToMap(char c);
void EnumerateCharacters(char from, char to);
int NeededBits(int value);

#endif // BITSTREAM

