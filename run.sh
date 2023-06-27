
# while sleep 0.5; do clear; ./a.out; done
resize -s 22 100
while [ 1 ]; do gcc top.c; ./a.out && break; done