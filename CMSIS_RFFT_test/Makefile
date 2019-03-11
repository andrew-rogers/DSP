
INCS=-I./CMSIS/DSP/Include -I./CMSIS/Core/Include

# By defining __ARM_ARCH_6M__ we avoid the SIMD stuff.
DEFS=-D__ARM_ARCH_6M__

test:	test.o arm_rfft_init_q15.o arm_rfft_init_q31.o arm_const_structs.o arm_common_tables.o arm_rfft_q15.o arm_rfft_inplace_q15.o arm_cfft_q15.o arm_cfft_radix4_q15.o arm_bitreversal.o arm_bitreversal2.o
	g++ -m32 $^ -o $@

test.o:	test.cpp
	g++ $(INCS) -m32 $(DEFS) -c $<

arm_rfft_init_q15.o:	CMSIS/DSP/Source/TransformFunctions/arm_rfft_init_q15.c
	gcc $(INCS) -m32 $(DEFS) -c $< -o $@

arm_rfft_init_q31.o:	./CMSIS/DSP/Source/TransformFunctions/arm_rfft_init_q31.c
	gcc $(INCS) -m32 $(DEFS) -c $< -o $@

arm_cfft_q15.o:	CMSIS/DSP/Source/TransformFunctions/arm_cfft_q15.c
	gcc $(INCS) -m32 $(DEFS) -c $< -o $@

arm_cfft_radix4_q15.o:	./CMSIS/DSP/Source/TransformFunctions/arm_cfft_radix4_q15.c
	gcc $(INCS) -m32 $(DEFS) -c $< -o $@

arm_bitreversal.o:	./CMSIS/DSP/Source/TransformFunctions/arm_bitreversal.c
	gcc $(INCS) -m32 $(DEFS) -c $< -o $@

arm_rfft_q15.o:	CMSIS/DSP/Source/TransformFunctions/arm_rfft_q15.c
	gcc $(INCS) -m32 $(DEFS) -c $< -o $@

arm_rfft_inplace_q15.o:	arm_rfft_inplace_q15.c
	gcc $(INCS) -m32 $(DEFS) -c $< -o $@

arm_const_structs.o:	./CMSIS/DSP/Source/CommonTables/arm_const_structs.c
	gcc $(INCS) -m32 $(DEFS) -c $< -o $@

arm_common_tables.o:	./CMSIS/DSP/Source/CommonTables/arm_common_tables.c
	gcc $(INCS) -m32 $(DEFS) -c $< -o $@

arm_bitreversal2.o:	arm_bitreversal2.c
	gcc $(INCS) -m32 $(DEFS) -c $< -o $@

clean:
	rm -f arm_rfft_init_q15.o arm_cfft_q15.o arm_rfft_init_q31.o arm_const_structs.o arm_common_tables.o arm_rfft_q15.o arm_rfft_inplace_q15.o arm_cfft_q15.o arm_cfft_radix4_q15.o arm_bitreversal.o arm_bitreversal2.o
	rm -f test.o
	rm -f test

