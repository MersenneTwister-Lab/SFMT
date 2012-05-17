#!/usr/bin/awk -f
BEGIN {
    mexp[0] = 607;
    mexp[1] = 1279;
    mexp[2] = 2281;
    mexp[3] = 4253;
    mexp[4] = 11213;
    mexp[5] = 19937;
    mexp[6] = 44497;
    mexp[7] = 86243;
    mexp[8] = 132049;
    mexp[9] = 216091;
    len = 10;
    stored_string = "";
}

/^# INSERT HERE/ {
    insert();
    next;
}
{
    print;
}
/^# START MEXP/,/^# END MEXP/ {
    if (! match($0,/^# START|^# END|^# ---/)) {
	store();
    }
}

function store() {
    stored_string = stored_string substr($0, 2) "\n";
}

function insert(	i) {
    for (i = 0; i < len; i++) {
	tmp = stored_string;
	gsub(/XXX/, mexp[i], tmp);
	print tmp;
    }
}