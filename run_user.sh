
# while sleep 0.5; do clear; ./a.out; done
resize -s 22 100
while [ 1 ]; do gcc /home/tejas/Documents/gitRepos/mytop/top.c; ./a.out && break; done