.text
.align 4
.global ARM_MoveImage
.global ARM_ClearImage
.global ARM_ClearImage_Fast
.global convert_frame_buffer_bgr555_to_rgb555

@arguments:
@ u8* src               r0
@ u8* dst               r1
@ u32 src_advance       r2
@ u32 w0                r3
@ u32 h1                sp+12
ARM_MoveImage:
	stmfd sp!,{r4-r6}

    ldr r4, [sp, #12]

    VertLoopMoveImage:
            mov r5, r3
            HorzLoopMoveImage:
                    ldrh r6, [r0], #2
					strh r6, [r1], #2
					subs r5, r5, #1
            bne HorzLoopMoveImage
            add r0, r0, r2
			add r1, r1, r2
            subs r4, r4, #1
    bne VertLoopMoveImage

	ldmfd sp!,{r4-r6}
	bx lr

@arguments:
@ u8* dst               r0
@ u32 src_advance       r1
@ u32 rgb				r2
@ u32 w0                r3
@ u32 h1                sp+8
ARM_ClearImage:
	stmfd sp!,{r4-r5}

    ldr r4, [sp, #8]

    VertLoopClearImage:
            mov r5, r3
            HorzLoopClearImage:
					strh r2, [r0], #2
					subs r5, r5, #1
            bne HorzLoopClearImage
            add r0, r0, r1
            subs r4, r4, #1
    bne VertLoopClearImage

	ldmfd sp!,{r4-r5}
	bx lr


@arguments:
@ u8* dst               r0
@ u32 src_advance       r1
@ u32 rgb				r2 -> r4-r11
@ u32 w0                r3 -> r2
@ u32 h1                sp+36 -> r12
ARM_ClearImage_Fast:
    stmfd sp!,{r4-r8,r10-r12,lr}
    ldr r12, [sp, #36]

	mov r4, r2
	mov r5, r2
	mov r6, r2
	mov r7, r2
	mov r8, r2
	mov r10, r2
	mov r11, r2
	mov lr, r2

    VertLoopClearImageFast:
            mov r2, r3
			bic  r0,r0,#3 @ stupid alignment
            HorzLoopClearImageFast:
					stmia r0!,{r4-r8,r10-r11,lr} @ 16 pixels
					subs r2, r2, #16
            bne HorzLoopClearImageFast
            add r0, r0, r1
            subs r12, r12, #1
    bne VertLoopClearImageFast

	ldmfd sp!,{r4-r8,r10-r12,lr}
	bx lr

@ 555 555
@ ABC DEF
@
@ CBA FED
@
@ 0B0 0E0  <- and mask
@ A00 D00  <- and mask + sh
@ 0BA 0ED  <- or + sh
@ 00C 00F  <- and mask + sh
@ CBA FED  <- or + sh
 
@ r0 is source image
 
#define convert_pixel_quad(pair_a, pair_b)                                    \
  and r8, pair_a, r1, lsl #5;                                                 \
  and r9, pair_b, r1, lsl #5;                                                 \
                                                                              \
  mov r8, r8, lsl #1;                                                         \
  mov r9, r9, lsl #1;                                                         \
                                                                              \
  and r12, pair_a, r1, lsl #10;                                               \
  and r14, pair_b, r1, lsl #10;                                               \
                                                                              \
  orr r8, r12, lsr #10;                                                       \
  orr r9, r14, lsr #10;                                                       \
 																																							\
  and pair_a, pair_a, r1;                                                     \
  and pair_b, pair_b, r1;                                                     \
                                                                              \
  orr pair_a, r8, pair_a, lsl #11;                                            \
  orr pair_b, r9, pair_b, lsl #11                                             \
 
 
convert_frame_buffer_bgr555_to_rgb555:
  stmdb sp!, { r4 - r9, lr }
 
  mov r1, #0x1F
  orr r1, r1, lsl #16
  mov r2, #60
  mov r3, #640
 
 l:
  ldmia r0, { r4 - r7 }
  convert_pixel_quad(r4, r5)
  convert_pixel_quad(r6, r7)
  stmia r0!, { r4 - r7 }
 
  subs r2, r2, #1
  bne l
 
  mov r2, #60
  subs r3, r3, #1
  bne l
 
  ldmfd sp!, { r4 - r9, pc }

