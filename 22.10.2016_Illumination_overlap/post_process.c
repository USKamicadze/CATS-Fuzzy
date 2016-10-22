#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

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
const unsigned char* afl_postprocess(const unsigned char* in_buf, unsigned int* len) {

  if (post_log == NULL) {
  	post_log = fopen("postlog.txt", "w");
  	fprintf(post_log, "sizeof(int64_t)=%d sizeof(char)%d\n", sizeof(int64_t), sizeof(char));
  }
  //postlog(NULL, "++++++++++++++++++++afl_postprocess_start+++++++++++++++++++++++");
  static unsigned char* saved_buf;
  unsigned char* new_buf;
  int64_t N;
  int64_t M;
  /*if (sscanf(in_buf, "%"PRId64" %"PRId64, &N, &M) == 2) {
    if (validate_ints(N, M)) {
      goto NEW_BUFF;
    }
    postlogData(in_buf, *len, N, M);
    return postlog_and_ret("not valid string", NULL);
  } */
  
  //try to parse as binary
  if (*len < 16) {
  	//postlogData(in_buf, *len, 0, 0);
    return postlog_and_ret("not enough 'len'", NULL);
  }
  N = *in_buf;
  M = *(in_buf + 8);
  if (!validate_ints(N, M)) {
    //try swap ints
    //int64_t tmp = M;
    //M = N;
    //N = tmp;
    //if (!validate_ints(N, M)) {
    	//postlogData(in_buf, *len, N, M);
    	return postlog_and_ret("not valid binary", NULL);
    //}
  }

  NEW_BUFF:
  new_buf = realloc(saved_buf, 30);
  if (!new_buf) return postlog_and_ret("failed to realloc buffer", NULL);
  saved_buf = new_buf;
  *len = sprintf(new_buf, "%"PRId64" %"PRId64, N, M);
  //postlogData(in_buf, *len, N, M);
  return postlog_and_ret("return new_buf", new_buf);
}
