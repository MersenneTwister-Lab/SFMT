	.section __TEXT,__text,regular,pure_instructions
	.section __TEXT,__picsymbolstub1,symbol_stubs,pure_instructions,32
	.machine ppc7400
	.text
	.align 2
	.globl _get_rnd_maxdegree
_get_rnd_maxdegree:
	li r3,19968
	blr
	.align 2
	.globl _get_rnd_mexp
_get_rnd_mexp:
	li r3,19937
	blr
	.cstring
	.align 2
LC0:
	.ascii "POS1 = %u\12\0"
	.align 2
LC1:
	.ascii "SL1 = %u\12\0"
	.align 2
LC2:
	.ascii "SL2 = %u\12\0"
	.align 2
LC3:
	.ascii "SR1 = %u\12\0"
	.text
	.align 2
	.globl _print_param
_print_param:
	mflr r0
	bcl 20,31,"L00000000001$pb"
"L00000000001$pb":
	stmw r29,-12(r1)
	li r5,71
	mflr r31
	mr r29,r3
	stw r0,8(r1)
	stwu r1,-80(r1)
	addis r4,r31,ha16(LC0-"L00000000001$pb")
	la r4,lo16(LC0-"L00000000001$pb")(r4)
	bl L_fprintf$LDBLStub$stub
	li r5,11
	addis r4,r31,ha16(LC1-"L00000000001$pb")
	mr r3,r29
	la r4,lo16(LC1-"L00000000001$pb")(r4)
	bl L_fprintf$LDBLStub$stub
	li r5,7
	addis r4,r31,ha16(LC2-"L00000000001$pb")
	mr r3,r29
	la r4,lo16(LC2-"L00000000001$pb")(r4)
	bl L_fprintf$LDBLStub$stub
	mr r3,r29
	addis r4,r31,ha16(LC3-"L00000000001$pb")
	li r5,17
	la r4,lo16(LC3-"L00000000001$pb")(r4)
	bl L_fprintf$LDBLStub$stub
	addi r1,r1,80
	lwz r0,8(r1)
	mr r3,r29
	lmw r29,-12(r1)
	mtlr r0
	b L_fflush$stub
	.cstring
	.align 2
LC4:
	.ascii "[POS1, SL1, SL2, SR1] = [%u,%u,%u,%u]\12\0"
	.text
	.align 2
	.globl _print_param2
_print_param2:
	mflr r0
	bcl 20,31,"L00000000002$pb"
"L00000000002$pb":
	stmw r29,-12(r1)
	li r5,71
	mflr r31
	li r6,11
	stw r0,8(r1)
	li r7,7
	stwu r1,-80(r1)
	li r8,17
	addis r4,r31,ha16(LC4-"L00000000002$pb")
	mr r29,r3
	la r4,lo16(LC4-"L00000000002$pb")(r4)
	bl L_fprintf$LDBLStub$stub
	addi r1,r1,80
	lwz r0,8(r1)
	mr r3,r29
	lmw r29,-12(r1)
	mtlr r0
	b L_fflush$stub
	.const
	.align 4
LC5:
	.long	17
	.long	17
	.long	17
	.long	17
	.align 4
LC6:
	.long	1077952576
	.long	1077952576
	.long	1077952576
	.long	1077952576
	.text
	.align 2
_gen_rand_all:
	mfspr r0,256
	stw r0,-8(r1)
	oris r0,r0,0xc3fc
	mtspr 256,r0
	vspltisw v7,11
	mflr r0
	bcl 20,31,"L00000000003$pb"
"L00000000003$pb":
	vspltisw v8,7
	mflr r10
	mtlr r0
	li r0,1136
	addis r2,r10,ha16(LC6-"L00000000003$pb")
	addis r11,r10,ha16(_sfmt-"L00000000003$pb")
	la r2,lo16(LC6-"L00000000003$pb")(r2)
	la r9,lo16(_sfmt-"L00000000003$pb")(r11)
	lvx v9,0,r2
	addis r2,r10,ha16(LC5-"L00000000003$pb")
	lvx v13,r9,r0
	la r2,lo16(LC5-"L00000000003$pb")(r2)
	li r0,2480
	lvx v12,0,r9
	lvx v1,r9,r0
	vsro v11,v13,v9
	lvx v6,0,r2
	vslw v0,v12,v7
	vslo v10,v1,v9
	li r2,84
	vsrw v13,v13,v6
	li r0,1136
	vxor v0,v0,v12
	mtctr r2
	vxor v13,v13,v11
	vslw v1,v1,v8
	vxor v0,v0,v13
	vxor v1,v1,v10
	vxor v10,v0,v1
	stvx v10,0,r9
	addi r9,r9,16
L10:
	lvx v13,r9,r0
	vslo v11,v10,v9
	lvx v0,0,r9
	vsro v12,v13,v9
	vslw v1,v0,v7
	vsrw v13,v13,v6
	vxor v1,v1,v0
	vxor v13,v13,v12
	vslw v0,v10,v8
	vxor v1,v1,v13
	vxor v0,v0,v11
	vxor v10,v1,v0
	stvx v10,0,r9
	addi r9,r9,16
	bdz L17
	b L10
L12:
	li r2,-1360
	lvx v0,0,r9
	lvx v13,r9,r2
	vslo v11,v10,v9
	vslw v1,v0,v7
	vsro v12,v13,v9
	vsrw v13,v13,v6
	vxor v1,v1,v0
	vxor v13,v13,v12
	vslw v0,v10,v8
	vxor v1,v1,v13
	vxor v0,v0,v11
	vxor v10,v1,v0
	stvx v10,0,r9
	addi r9,r9,16
	bdz L18
	b L12
L17:
	li r0,71
	la r2,lo16(_sfmt-"L00000000003$pb")(r11)
	mtctr r0
	addi r9,r2,1360
	b L12
L18:
	lwz r12,-8(r1)
	mtspr 256,r12
	blr
	.align 2
	.globl _gen_rand
_gen_rand:
	mflr r0
	bcl 20,31,"L00000000004$pb"
"L00000000004$pb":
	stmw r30,-8(r1)
	mflr r31
	stw r0,8(r1)
	stwu r1,-80(r1)
	addis r30,r31,ha16(_idx-"L00000000004$pb")
	lwz r11,lo16(_idx-"L00000000004$pb")(r30)
	cmplwi cr7,r11,623
	ble- cr7,L21
	bl _gen_rand_all
	li r11,0
	stw r11,lo16(_idx-"L00000000004$pb")(r30)
L21:
	addi r0,r11,1
	addi r1,r1,80
	addis r2,r31,ha16(_sfmt-"L00000000004$pb")
	stw r0,lo16(_idx-"L00000000004$pb")(r30)
	lwz r0,8(r1)
	slwi r9,r11,2
	la r2,lo16(_sfmt-"L00000000004$pb")(r2)
	lmw r30,-8(r1)
	lwzx r3,r9,r2
	mtlr r0
	blr
	.align 2
	.globl _init_gen_rand
_init_gen_rand:
	mflr r0
	bcl 20,31,"L00000000005$pb"
"L00000000005$pb":
	lis r10,0x6c07
	li r9,1
	mflr r8
	ori r10,r10,35173
	mtlr r0
	li r0,623
	addis r2,r8,ha16(_sfmt-"L00000000005$pb")
	stw r3,lo16(_sfmt-"L00000000005$pb")(r2)
	la r11,lo16(_sfmt-"L00000000005$pb")(r2)
	mtctr r0
L25:
	lwz r0,0(r11)
	srwi r2,r0,30
	xor r0,r0,r2
	mullw r0,r0,r10
	add r0,r0,r9
	addi r9,r9,1
	stwu r0,4(r11)
	bdnz L25
	li r0,624
	addis r2,r8,ha16(_idx-"L00000000005$pb")
	stw r0,lo16(_idx-"L00000000005$pb")(r2)
	blr
.lcomm _idx,4,2
.lcomm _sfmt,2496,4
	.section __TEXT,__picsymbolstub1,symbol_stubs,pure_instructions,32
	.align 5
L_fflush$stub:
	.indirect_symbol _fflush
	mflr r0
	bcl 20,31,"L00000000001$spb"
"L00000000001$spb":
	mflr r11
	addis r11,r11,ha16(L_fflush$lazy_ptr-"L00000000001$spb")
	mtlr r0
	lwzu r12,lo16(L_fflush$lazy_ptr-"L00000000001$spb")(r11)
	mtctr r12
	bctr
	.lazy_symbol_pointer
L_fflush$lazy_ptr:
	.indirect_symbol _fflush
	.long	dyld_stub_binding_helper
	.section __TEXT,__picsymbolstub1,symbol_stubs,pure_instructions,32
	.align 5
L_fprintf$LDBLStub$stub:
	.indirect_symbol _fprintf$LDBLStub
	mflr r0
	bcl 20,31,"L00000000002$spb"
"L00000000002$spb":
	mflr r11
	addis r11,r11,ha16(L_fprintf$LDBLStub$lazy_ptr-"L00000000002$spb")
	mtlr r0
	lwzu r12,lo16(L_fprintf$LDBLStub$lazy_ptr-"L00000000002$spb")(r11)
	mtctr r12
	bctr
	.lazy_symbol_pointer
L_fprintf$LDBLStub$lazy_ptr:
	.indirect_symbol _fprintf$LDBLStub
	.long	dyld_stub_binding_helper
	.subsections_via_symbols
