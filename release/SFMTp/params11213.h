#define POS1	30
#define SL1	12
#define SL2	3
#define SR1	10
#define SR2	3
#define MSK1	0x7effffffU
#define MSK2	0xf5ffebffU
#define MSK3	0xbdffcb7dU
#define MSK4	0xfefffe7fU
#define PARITY1	0x00000001U
#define PARITY2	0x00000000U
#define PARITY3	0xa9da0800U
#define PARITY4	0xe23bc5a1U
#define ALTI_SL2_PERM \
(vector unsigned char)(3,21,21,21,7,0,1,2,11,4,5,6,15,8,9,10)
#define ALTI_SL2_PERM64 \
(vector unsigned char)(3,4,5,6,7,29,29,29,11,12,13,14,15,0,1,2)
#define IDSTR	"SFMTp-11213:30-12-3-10-3:7effffff-f5ffebff-bdffcb7d-fefffe7f"