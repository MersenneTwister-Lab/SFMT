#if !defined(MEXP)
  #define MEXP 19937
#endif

#if MEXP == 521
  #define	N	(MEXP / 32)
  #define	GMM	9
  #define	GS2	4
  #define	GS3	9
  #define	GROT1	3
  #define	GROT2	15
  #define	INITIAL_LUNG	0x4d4e4850U
  #define	IDSTRING "PMT32-521:9-4-9-3-15:4d4e4850"
#elif MEXP == 1279
  #define	N	(MEXP / 32)
  #define	GMM	28
  #define	GS2	6
  #define	GS3	13
  #define	GROT1	3
  #define	GROT2	17
  #define	INITIAL_LUNG	0x4d4e4850U
  #define	IDSTRING "PMT32-1279:28-6-13-3-17:4d4e4850"
#elif MEXP == 2203
  #define	N	(MEXP / 32)
  #define	GMM	24
  #define	GS2	5
  #define	GS3	9
  #define	GROT1	15
  #define	GROT2	13
  #define	INITIAL_LUNG	0x4d4e5048U
  #define	IDSTRING "PMT32-2203:24-5-9-15-13:4d4e5048"
#elif MEXP == 4253
  #define	N	(MEXP / 32)
  #define	GMM	50
  #define	GS2	12
  #define	GS3	9
  #define	GROT1	9
  #define	GROT2	10
  #define	INITIAL_LUNG	0x4d4e5048U
  #define	IDSTRING "PMT32-4253:50-12-9-9-10:4d4e5048"
#elif MEXP == 11213
  #define	N	(MEXP / 32)
  #define	GMM	142
  #define	GS2	5
  #define	GS3	21
  #define	GROT1	15
  #define	GROT2	13
  #define	INITIAL_LUNG	0x4d4e5048U
  #define	IDSTRING "PMT32-11213:142-5-21-15-13:4d4e5048"
#elif MEXP == 19937
  #define	N	(MEXP / 32)
  #define	GMM	294
  #define	GS2	5
  #define	GS3	9
  #define	GROT1	17
  #define	GROT2	22
  #define	INITIAL_LUNG	0x4d4e4850U
  #define	IDSTRING "PMT32-19937:294-5-9-17-22:4d4e4850"
#elif MEXP == 44497
  #define	N	(MEXP / 32)
  #define	GMM	873
  #define	GS2	22
  #define	GS3	3
  #define	GROT1	5
  #define	GROT2	8
  #define	INITIAL_LUNG	0x4d4e5048U
  #define	IDSTRING "PMT32-44497:873-22-3-5-8:4d4e5048"
#elif MEXP == 86243
  #define	N	(MEXP / 32)
  #define	GMM	264
  #define	GS2	5
  #define	GS3	23
  #define	GROT1	16
  #define	GROT2	4
  #define	INITIAL_LUNG	0x4d4e5048U
  #define	IDSTRING "PMT32-86243:264-5-23-16-4:4d4e5048"
#elif MEXP == 110503
  #define	N	(MEXP / 32)
  #define	GMM	2550
  #define	GS2	5
  #define	GS3	3
  #define	GROT1	5
  #define	GROT2	21
  #define	INITIAL_LUNG	0x4d4e5048U
  #define	IDSTRING "PMT32-110503:2550-5-3-5-21:4d4e5048"
#elif MEXP == 132049
  #define	N	(MEXP / 32)
  #define	GMM	3518
  #define	GS2	4
  #define	GS3	9
  #define	GROT1	23
  #define	GROT2	16
  #define	INITIAL_LUNG	0x4d4e5048U
  #define	IDSTRING "PMT32-132049:3518-4-9-23-16:4d4e5048"
#elif MEXP == 216091
  #define	N	(MEXP / 32)
  #define	GMM	5463
  #define	GS2	6
  #define	GS3	19
  #define	GROT1	12
  #define	GROT2	4
  #define	INITIAL_LUNG	0x4d4e5048U
  #define	IDSTRING "PMT32-216091:5463-6-19-12-4:4d4e5048"
#endif
