#!/usr/bin/awk -f
BEGIN {
    FS=",";
}

/^[0-9]/ {
    printf("./calc-characteristic ");
    printf("%s ", $1);
    for (i = 3; i <= 15; i++) {
	printf("%s ", $i);
    }
    printf(">> characteristic.%s.txt\n", $1);
}