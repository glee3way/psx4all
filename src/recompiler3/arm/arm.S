.text
.global clear_insn_cache
.type	clear_insn_cache, %function
.align 4

clear_insn_cache:
	swi 0x9f0002
	bx lr
