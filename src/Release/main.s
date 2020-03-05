	.section		.rodata
.LC0:
	.string  "%d"
.LC1:
	.string  "%d\n"
	.text
	.globl  vi
	.type  vi,@function
vi:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$28, %esp
	movl	$9,%eax
	jmp		.Lendvi
.Lendvi:
	leave
	ret
	.size	vi,.-vi
	.text
	.globl  CallFunction
	.type  CallFunction,@function
CallFunction:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	$2,%esi
	leal	12(%esp),%edi
	movl	%esi,(%edi)
	call	vi
	movl	%eax,%esi
	leal	20(%esp),%edi
	movl	%esi,(%edi)
	movl	8(%ebp),%edi
	movl	$0,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	%edx,24(%esp)
	movl	8(%ebp),%edi
	movl	$1,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	24(%esp),%ebx
	imull	%ebx,%edx
	movl	%edx,24(%esp)
	movl	8(%ebp),%edi
	movl	$2,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	%edx,28(%esp)
	movl	12(%esp),%ebx
	movl	$3,%edx
	imull	%ebx,%edx
	movl	28(%esp),%ebx
	addl	%ebx,%edx
	movl	24(%esp),%eax
	movl	%edx,%esi
	cltd
	divl	%esi
	movl	%eax,%edx
	movl	%edx,24(%esp)
	movl	8(%ebp),%edi
	movl	$0,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	%edx,%ebx
	movl	12(%esp),%edx
	subl	%edx,%ebx
	movl	%ebx,%edx
	movl	24(%esp),%ebx
	addl	%ebx,%edx
	movl	%edx,24(%esp)
	movl	20(%esp),%ebx
	movl	12(%ebp),%edx
	imull	%ebx,%edx
	movl	24(%esp),%ebx
	addl	%ebx,%edx
	movl	%edx,%esi
	leal	16(%esp),%edi
	movl	%esi,(%edi)
	movl	16(%esp),%eax
	movl	%eax,4(%esp)
	movl	$.LC1,%ebx
	movl	%ebx,0(%esp)
	call	printf
.LendCallFunction:
	leave
	ret
	.size	CallFunction,.-CallFunction
	.text
	.globl  main
	.type  main,@function
main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$44, %esp
	leal	12(%esp),%edi
	movl	$0,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	$5,%esi
	movl	%esi,(%edi)
	leal	12(%esp),%edi
	movl	$1,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	$4,%esi
	movl	%esi,(%edi)
	leal	12(%esp),%edi
	movl	$2,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	$1,%esi
	movl	%esi,(%edi)
	movl	$.LC0,%ebx
	movl	%ebx,0(%esp)
	leal	8(%esp),%eax
	movl	%eax,4(%esp)
	call	__isoc99_scanf
	movl	8(%esp),%eax
	movl	%eax,%esi
	leal	24(%esp),%edi
	movl	%esi,(%edi)
	movl	24(%esp),%eax
	movl	%eax,4(%esp)
	leal	12(%esp),%eax
	movl	%eax,0(%esp)
	call	CallFunction
.Lendmain:
	leave
	ret
	.size	main,.-main
