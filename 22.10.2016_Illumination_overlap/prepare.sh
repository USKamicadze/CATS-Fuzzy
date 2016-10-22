#prepares all solutons for work
#usage example: bash prepare.sh -c checker.cpp -ps post_process.c -r run.cpp -t tests -a afl-g++

while [[ $# > 1 ]]
do
key="$1"

case $key in
    -c|--checker)
    checker="$2"
    shift
    ;;
    -ps|--post_lib_source)
    post_lib_source="$2"
    shift
    ;;
    -t|--tests)
    initial_tests_dir="$2"
    shift
    ;;
    -a|--afl)
	afl="$2"
	shift
	;;
	-r|--run)
	run="$2"
	shift
	;;
    *)
        # unknown option
    ;;
esac
shift # pdast argument or value
done

echo ${checker:?}
echo ${post_lib_source:?}
echo ${initial_tests_dir:?}
echo ${afl:?}
echo ${post_lib:?}
echo ${run:?}

gcc -shared -Wall -O3 $post_lib_source -o "post_process.so"

if [ ! -d "results" ]; then
	mkdir "results"
fi
if [ ! -d "works" ]; then
	mkdir "works"
fi

function prepare_single {
	local work_dir
	local result_dir
	local solution
	work_dir="works/$1"
	result_dir="results/$1"
	result_dir+=$(date -d today +"%Y.%M.%d-%H.%M.%S")
	solution="solutions/$1/solution.cpp"	
	

	if [ -d $work_dir ]; then
		cp -rf $work_dir $result_dir
		echo "previous results for $work_dir saved in $result_dir"
		rm -rf $work_dir
	fi
	mkdir $work_dir
	cp -rf $initial_tests_dir "$work_dir/in"
	mkdir "$work_dir/out"
	perl convert_solution.pl $solution > "$work_dir/solution.cpp"
	cp $checker "$work_dir/checker.cpp"
	cp "post_process.so" "$work_dir/post_lib.so"
	cp $run "$work_dir/run.cpp"
	cp "checker.h" "$work_dir/checker.h"
	cp "solution.h" "$work_dir/solution.h"
	cp "CMakeLists.txt" "$work_dir/CMakeLists.txt"
	$afl  -o "$work_dir/main" -Wno-unused-result "$work_dir/solution.cpp" "$work_dir/checker.cpp" "$work_dir/run.cpp" 
}
x=a/b/c
echo ${x#*/}
for d in solutions/*/; do
	p=${d#*/}
	prepare_single ${p%/}
done
