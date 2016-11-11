#start fuzzing for single solution
#usage: bash run_work solution_name

 cd $1

AFL_POST_LIBRARY=./post_lib.so AFL_SKIP_CPUFREQ=1 afl-fuzz -i in -o out -f input.txt -t 1000 -T $1 ./main #|| \

#AFL_POST_LIBRARY=./post_lib.so AFL_SKIP_CPUFREQ=1 afl-fuzz -i in -o out -f input.txt -t 1000 -T $1 -C ./main
