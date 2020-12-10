	.file	"main.c"
	.text
	.globl	hello
	.data
	.align 8
hello:
	.ascii "hello gcc\0"
	.def	__main;	.scl	2;	.type	32;	.endef
	.section .rdata,"dr"
.LC0:
	.ascii "str:%s,ptr:%p,size:%d\12\0"
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	call	__main
	movl	$10, %r9d
	leaq	hello(%rip), %r8
	leaq	hello(%rip), %rdx
	leaq	.LC0(%rip), %rcx
	call	printf
	movl	$0, %eax
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.ident	"GCC: (GNU) 7.4.0"
	.def	printf;	.scl	2;	.type	32;	.endef
