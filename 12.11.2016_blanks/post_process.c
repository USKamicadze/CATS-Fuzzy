#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "bitstream.h"
#include "log.h"

int BitsForSymbolInMap;
int BitsForN;

#define BitStream_Check(bs, err) if (bs->Error) {Log("BitStreamError!!!");err;}
#define ClearAndReturn(bs, res) BitStream_Destroy(bs); return res
#define BitStream_Checkd(bs) BitStream_Check(bs, ClearAndReturn(bs, NULL))

void InitializeMap() {
  static int initialized = 0;
  if (!initialized) {
    Log("Initializing MAP");
    AddToMap('\n');
    AddToMap(' ');
    AddToMap('_');
    EnumerateCharacters('a', 'z');
    EnumerateCharacters('A', 'Z');
    initialized = 1;
    BitsForSymbolInMap = NeededBits(CurrentPositionInMap);
    BitsForN = NeededBits(100);
  }
}

#define MAXLEN 1000

typedef struct Caption {
    char Name[MAXLEN];
    char Value[MAXLEN];
} Caption;

#define Add(arr, length, value) arr[length++] = value
#define AddAndRet(arr, length, value, max, ret) Add(arr, length, value); if (length >= max) ret


void ReadString(BitStream* stream, char* dest, int allowSpace) {
    int length = 0;
    while(1) {
        int idx = BitStream_ReadInt(stream, BitsForSymbolInMap);
        BitStream_Check(stream, return);
        char symbol = Map[idx];
        if (symbol == '\n') {
            if (length == 0) {
                continue;
            }
            break;
        }
        if (symbol == '_') {
          continue;
        }
        if (!allowSpace && symbol == ' ') {
            if (length == 0) {
                continue;
            }
            break;
        }

        AddAndRet(dest, length, symbol, MAXLEN, return);
    }
}

static int count = 0;

const unsigned char* afl_postprocess(const unsigned char* in_buf, unsigned int* len) {
  InitializeMap();
  LogT("START POSTPROCESS #%d:\nLength:%d\nInput:%s", ++count, *len, in_buf);
  BitStream* bitStream = BitStream_Create(in_buf, 0, *len);
  if (bitStream == NULL) {
    return NULL;
  }
  int N = BitStream_ReadInt(bitStream, BitsForN) % 101;
  BitStream_Checkd(bitStream);
  LogT("N:%d", N);
  int BitsForCaptionIndex = NeededBits(N);
  LogT("BitsForCaptionIndex:%d", BitsForCaptionIndex);
  int readingUnderscore = 0;
  int fields[333];
  int captionsLength = 0;
  char blank[1000];
  int blankLength = 0;
  Caption captions[100];
  Log("start for reading blank");
  while(1) {
      int idx = BitStream_ReadInt(bitStream, BitsForSymbolInMap);
      BitStream_Checkd(bitStream);
      char symbol = Map[idx];
      if (symbol == '_') {
          if (readingUnderscore) {
              AddAndRet(blank, blankLength, symbol, MAXLEN, break);
          } else {
              Log("start reading underscores: add 3");
              //"All fields consist of at least 3 underscores"
              
              AddAndRet(blank, blankLength, symbol, MAXLEN, break);
              AddAndRet(blank, blankLength, symbol, MAXLEN, break);
              AddAndRet(blank, blankLength, symbol, MAXLEN, break);
          }
          readingUnderscore = 1;
      } else if (readingUnderscore && symbol != '_') {
          Log("stop underscores, start caption index");
          readingUnderscore = 0;
          int captionIndex = BitStream_ReadInt(bitStream, BitsForCaptionIndex);
          BitStream_Checkd(bitStream);
          if (N > 0) {
            captionIndex = captionIndex % N;
          }
          AddAndRet(fields, captionsLength, captionIndex, 333, break);
      } else if (symbol == '\n') {
          break;
      } else {
        AddAndRet(blank, blankLength, symbol, MAXLEN, break);
      }
  }
  int i;
  LogT("Generated Blank:%s", blank);
  for(i = 0; i < N; ++i) {
      ReadString(bitStream, captions[i].Name, 0);
      BitStream_Checkd(bitStream);
      LogT("Readed Caption:%s", captions[i].Name);
      ReadString(bitStream, captions[i].Value, 1);
      LogT("Readed Value: %s", captions[i].Value);
      BitStream_Checkd(bitStream);
      int j;
      for(j = 0; j < i; ++j) {
          if (strcmp(captions[j].Name, captions[i].Name) == 0) {
              LogT("non unique caption: %s", captions[i].Name);
              ClearAndReturn(bitStream, NULL);
          }
      }
  }
  static unsigned char* buffer;
  Log("trying to create buffer");
  unsigned char* newBuffer = realloc(buffer,
      1000 * 200/*afterN*/ +
      1000/*firstline*/ +
      1000 * 333/*secondline*/ +
      3/*N*/ +
      202/*\n*/
  );
  if (!newBuffer) {
      Log("Failed to allocate mem for result buffer");
      ClearAndReturn(bitStream, NULL);
  }
  buffer = newBuffer;
  Log("buffer created, now fill it");
  Log("adding blank");
  sprintf(buffer, "%s\n", blank);
  LogT("current Buffer: %s, captionsLength: %d", buffer, captionsLength);
  for (i = 0; i < captionsLength; ++i) {
      LogT("adding caption: i: %d", i);
      LogT("adding caption: field: %d", fields[i]);
      LogT("adding caption: caption Name: %s", captions[fields[i]].Name);
      sprintf(buffer, " %s", captions[fields[i]].Name);
  }

  LogT("secondline finished, adding N:%d", N);
  sprintf(buffer, "\n%d\n", N);
  Log("N ready, adding caption pairs");
  for (i = 0; i < N;++i) {
      LogT("adding i:%d name:%s value:%s", i, captions[i].Name, captions[i].Value);
      sprintf(buffer, "%s\n%s\n", captions[i].Name, captions[i].Value);
  }
  LogT("buffer: %s", buffer);
  ClearAndReturn(bitStream, buffer);
}


/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "BitStream.c"

int BitsForSymbolInMap;
int BitsForN;

void InitializeMap() {
  static int initialized = 0;
  if (!initialized) {
    AddToMap('\n');
    AddToMap(' ');
    AddToMap('_');
    EnumerateCharacters('a', 'z');
    EnumerateCharacters('A', 'Z');
    initialized = 1;
  }
  BitsForSymbolInMap = NeededBits(CurrentPositionInMap);
  BitsForN = NeededBits(100);
}

int validate_ints(int64_t N, int64_t M) {
  return 1 <= M && M <= N && N <= 10 * 10 * 10 * 10 * 10 * 10 * 10 * 10 * 10;
}

static FILE* post_log = NULL;
static int log_enabled = 1;

static void postlog(char* tag, char* str) {
    if (tag != NULL && log_enabled && post_log != NULL) {
        fprintf(post_log, "%s:%s\n", tag, str);
    }
    if (tag == NULL && log_enabled && post_log != NULL) {
        fprintf(post_log, "%s\n", str);
    }
}

static void postlogData(char* buf, int len, int N, int M){
    if (log_enabled && post_log != NULL) {
        fprintf(post_log, "data: %s\nlen:%d N:%d M:%d\n", buf, len, N, M);
    }
}

static const unsigned char* postlog_and_ret(char* tag,const unsigned char* ret) {
    if (ret == NULL) {
        //postlog(tag, "NULL");
    } else {
        postlog(tag, ret);
    }
    //postlog(NULL, "++++++++++++++++++++afl_postprocess_end+++++++++++++++++++++++++");
    return ret;

}

int contains(char** captions, char* caption, int i) {
  int j = 0;
  for (j = 0; j < i; ++j) {
    if (strcmp(captions[j], caption) == 0) {
      return 1;
    }
  }
  return 0;
}

static char* lineCaptions[100];
static int captionsInLine = 0;

void getcaptionsFromLine(char* line) {
  char* curr;
  int inWord = 0;
  captionsInLine = 0;
  for(curr = line; *curr != '\0'; ++curr) {
    if (isspace(*curr) && inWord) {
        *curr = '\0';
        inWord = 0;
    } else if (!isspace(*curr) && !inWord) {
      inWord = 1;
      lineCaptions[captionsInLine] = curr;
      captionsInLine++;
    }
  }
}

const unsigned char* afl_postprocess(const unsigned char* in_buf, unsigned int* len) {

  if (post_log == NULL) {
    post_log = fopen("postlog.txt", "a");
  }
  InitializeMap();

  //postlog(NULL, "++++++++++++++++++++afl_postprocess_start+++++++++++++++++++++++");
  static unsigned char* saved_buf;
  static unsigned char** captions;
  static unsigned char** values;
  unsigned char* new_buf;
  char* form;
  char* names;
  int N;

  postlog(in_buf, "");

  new_buf = realloc(saved_buf, *len + 1);
  if (!new_buf) return postlog_and_ret("failed to realloc buffer", NULL);
  saved_buf = new_buf;
  postlog(NULL, "2");

  memcpy(new_buf, in_buf, *len);
  unsigned char* f;
  for(f = new_buf; (f - new_buf) < *len; ++f) {
      if (48 <= *f && *f <= 57) {
          continue;
      }
      if (65 <= *f && *f <= 90) {
          continue;
      }
      if (97 <= *f && *f <= 122) {
          continue;
      }
      if (*f == '\n') {
          continue;
      }
      *f = 65;
  }
  postlog(NULL, "3");
  char* begin = new_buf;
  char* curLine = new_buf;
  curLine[*len] = '\0';
  form = curLine;
  curLine = strchr(curLine, '\n');
  postlog(NULL, "4");
  if (curLine == NULL) {
    return NULL;
  }
  *curLine = '\0';
  curLine = curLine + 1;
  names = curLine;

  curLine = strchr(curLine, '\n');
  postlog(NULL, "5");
  if (curLine == NULL) {
    return NULL;
  }
  *curLine = '\0';
  curLine = curLine + 1;
  getcaptionsFromLine(names);
  char* NLine = curLine;
  postlog(NULL, "6");
  if (sscanf(NLine, "%d", &N) != 1) {
    return NULL;
  }
  captions = realloc(captions, sizeof(char*) * N);
  values = realloc(values, sizeof(char) * N);
  postlog(NULL, "1");
  curLine = strchr(curLine, '\n');
  int i;
  for (i = 0; i < N; ++i) {
    if (curLine == NULL) {
      return NULL;
    }
    *curLine = '\0';
    curLine = curLine + 1;
    captions[i] = curLine;
    postlog(NULL, "7");

    curLine = strchr(curLine, '\n');
    if (curLine == NULL) {
      return NULL;
    }
    *curLine = '\0';
    if (contains(captions, captions[i], i)) {
      return NULL;
    }
    if (!contains(lineCaptions, captions[i],captionsInLine)) {
      return NULL;
    }
    curLine = curLine + 1;
    values[i] = curLine;
    curLine = strchr(curLine, '\n');
    postlog(NULL, "9");
  }

  postlog(NULL, "a");
  return postlog_and_ret("return new_buf", in_buf);
}

*/