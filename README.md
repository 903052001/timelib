# timelib
private time lib include mktime gmtime .etc
remove <time.h> and <stdio.h>, open .h #if 0

实现time库中的api，删除<time.h> and <stdio.h>，并打开头文件中的宏，将代码整合到自己的工程中。

linux:
gcc private_time.c -o a
./a

win:
gcc private_time.c -o a.out
./a.out
