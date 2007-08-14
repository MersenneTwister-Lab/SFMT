#!/usr/bin/awk -f
#
#
#
#
#
BEGIN {
    if (length(ARGV) <= 2) {
	usage();
	exit;
    } else {
	FS=",";
	line = ARGV[1] + 1;
	delete ARGV[1];
    }
}

NR == line { print_variable();}

function usage() {
    printf("usage:\n");
    printf("csv2param.awk line-no csv-filename\n");
}

function print_variable() {
  #printf("#ifndef SFMT_PARAMS%s_H\n", $1);
  #printf("#define SFMT_PARAMS%s_H\n\n", $1);
  printf("#define SFMT_POS1\t%s\n", $3);
  printf("#define SFMT_SL1\t%s\n", $4);
  sl1 = $4 % 8;
  printf("#define SFMT_SL2\t%s\n", $5);
  printf("#define SFMT_SR1\t%s\n", $6);
  printf("#define SFMT_SR2\t%s\n", $7);
  printf("#define SFMT_MSK1\tUINT64_C(0x%s)\n", $8);
  printf("#define SFMT_MSK2\tUINT64_C(0x%s)\n", $9);
  printf("#define SFMT_MSK32_1\t0x%sU\n", substr($8,1,8));
  printf("#define SFMT_MSK32_2\t0x%sU\n", substr($8,9,8));
  printf("#define SFMT_MSK32_3\t0x%sU\n", substr($9,1,8));
  printf("#define SFMT_MSK32_4\t0x%sU\n", substr($9,9,8));
  printf("#define SFMT_PCV1\tUINT64_C(0x%s)\n", substr($10, 3));
  printf("#define SFMT_PCV2\tUINT64_C(0x%s)\n", substr($11, 3));
  printf("#define SFMT_IDSTR \\\n\t\"dSFMT-%s:%s-%s-%s-%s-%s:%s-%s\"\n",
         $1, $3, $4, $5, $6, $7, $8, $9);

  printf("\n\n/* PARAMETERS FOR ALTIVEC */\n");
  printf("#if defined(__APPLE__)\t/* For OSX */\n");
  cnt = 12;
  printf("    #define ALTI_SL1 \t(vector unsigned int)(%d, %d, %d, %d)\n",
	 sl1, sl1, sl1, sl1);
  s = make_chars();
  printf("    #define ALTI_SL1_PERM \\\n\t(vector unsigned char)(%s)\n", s);
  s = make_ints()
  printf("    #define ALTI_SL1_MSK \\\n\t(vector unsigned int)(%s)\n", s);
  s = make_chars();
  printf("    #define ALTI_SL2_PERM \\\n\t(vector unsigned char)(%s)\n", s);
  printf("    #define ALTI_SR1 \\\n");
  printf("\t(vector unsigned int)(SFMT_SR1, SFMT_SR1, SFMT_SR1, SFMT_SR1)\n");
  s = make_ints();
  printf("    #define ALTI_SR1_MSK \\\n\t(vector unsigned int)(%s)\n", s);
  s = make_chars();
  printf("    #define ALTI_SR2_PERM \\\n\t(vector unsigned char)(%s)\n", s);
  printf("    #define ALTI_PERM \\\n");
  printf("\t(vector unsigned char)");
  printf("(8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7)\n");
  printf("    #define ALTI_LOW_MSK \\\n\t(vector unsigned int)");
  printf("(SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2, \\\n");
  printf("\t\tSFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2)\n");
  printf("    #define ALTI_HIGH_CONST \\\n");
  printf("\t(vector unsigned int)")
  printf("(SFMT_HIGH_CONST32, 0, SFMT_HIGH_CONST32, 0)\n");
  printf("#else\t/* For OTHER OSs(Linux?) */\n");
  cnt = 12;
  printf("    #define ALTI_SL1 \t{%d, %d, %d, %d}\n", sl1, sl1, sl1, sl1);
  s = make_chars();
  printf("    #define ALTI_SL1_PERM \\\n\t{%s}\n", s);
  s = make_ints();
  printf("    #define ALTI_SL1_MSK \\\n\t{%s}\n", s);
  s = make_chars();
  printf("    #define ALTI_SL2_PERM \\\n\t{%s}\n", s);
  printf("    #define ALTI_SR1 \\\n");
  printf("\t{SFMT_SR1, SFMT_SR1, SFMT_SR1, SFMT_SR1}\n");
  s = make_ints();
  printf("    #define ALTI_SR1_MSK \\\n\t{%s}\n", s);
  s = make_chars();
  printf("    #define ALTI_SR2_PERM \\\n\t{%s}\n", s);
  printf("    #define ALTI_PERM \\\n");
  printf("\t{8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7}\n");
  printf("    #define ALTI_LOW_MSK \\\n\t");
  printf("{SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2,");
  printf(" SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2}\n");
  printf("    #define ALTI_HIGH_CONST \\\n");
  printf("\t{SFMT_HIGH_CONST32, 0, SFMT_HIGH_CONST32, 0}\n");
  printf("#endif\t/* For OSX */\n");
  #printf("\n#endif /* SFMT_PARAMS%s_H */\n", $1);
}

function make_chars(s) {
  s = $(cnt++);
  for (i = 1; i < 16; i++) {
      s = s "," $(cnt++);
  }
  return s;
}

function make_ints(s) {
  s = $(cnt++);
  for (i = 1; i < 4; i++) {
      s = s "," $(cnt++);
  }
  return s;
}
