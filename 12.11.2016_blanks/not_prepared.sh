
for f in not_prepared/*; do
	echo "Processing $f file..."
	filename=$(basename "$f" | sed 's/#/__/g')
	mkdir solutions/$filename
	cp $f solutions/$filename/solution.cpp
done