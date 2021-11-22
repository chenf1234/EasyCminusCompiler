	.section		.rodata
.LC0:
	.string  "%d"
.LC1:
	.string  "%d\n"
	.comm  as,4,4
	.text
	.globl  fuction
	.type  fuction,@function
fuction:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$36, %esp
	movl	$1,%esi
	leal	12(%esp),%edi
	movl	%esi,(%edi)
	movl	12(%esp),%esi
	leal	16(%esp),%edi
	movl	%esi,(%edi)
	movl	16(%esp),%ecx
	cmpl	$0,%ecx
	jne		.L0
	jmp		.L1
.L0:
	movl	16(%esp),%eax
	jmp		.Lendfuction
	jmp		.L2
.L1:
	movl	as,%eax
	jmp		.Lendfuction
.L2:
.Lendfuction:
	leave
	ret
	.size	fuction,.-fuction
	.text
	.globl  mul
	.type  mul,@function
mul:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$28, %esp
	movl	20(%ebp),%edi
	movl	$2,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	%edx,12(%esp)
	movl	20(%ebp),%edi
	movl	$3,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	12(%esp),%ebx
	imull	%ebx,%edx
	movl	%edx,4(%esp)
	movl	$.LC1,%ebx
	movl	%ebx,0(%esp)
	call	printf
	movl	8(%ebp),%ebx
	movl	12(%ebp),%edx
	imull	%ebx,%edx
	movl	%edx,%ebx
	movl	16(%ebp),%edx
	imull	%ebx,%edx
	movl	%edx,12(%esp)
	movl	20(%ebp),%edi
	movl	$3,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	12(%esp),%ebx
	imull	%ebx,%edx
	movl %edx,%eax
	jmp		.Lendmul
.Lendmul:
	leave
	ret
	.size	mul,.-mul
	.text
	.globl  v
	.type  v,@function
v:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$28, %esp
	movl	8(%ebp),%edi
	movl	$0,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl %edx,%eax
	jmp		.Lendv
.Lendv:
	leave
	ret
	.size	v,.-v
	.comm  a,48,32
	.text
	.globl  vi
	.type  vi,@function
vi:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$28, %esp
	movl	8(%ebp),%eax
	movl	%eax,0(%esp)
	call	v
	movl	%eax,%eax
	jmp		.Lendvi
.Lendvi:
	leave
	ret
	.size	vi,.-vi
	.comm  c,4,4
	.comm  d,4,4
	.comm  ss,4,4
	.text
	.globl  main
	.type  main,@function
main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$608, %esp
	movl	$5,%esi
	leal	ss,%edi
	movl	%esi,(%edi)
	movl	$3,%esi
	leal	c,%edi
	movl	%esi,(%edi)
	leal	16(%esp),%edi
	movl	$1,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	$2,%esi
	movl	%esi,(%edi)
	movl	$.LC0,%ebx
	movl	%ebx,0(%esp)
	leal	8(%esp),%eax
	movl	%eax,4(%esp)
	call	__isoc99_scanf
	movl	8(%esp),%eax
	movl	%eax,%esi
	leal	508(%esp),%edi
	movl	%esi,(%edi)
	movl	508(%esp),%eax
	movl	%eax,4(%esp)
	movl	$.LC1,%ebx
	movl	%ebx,0(%esp)
	call	printf
	movl	$4,%esi
	leal	512(%esp),%edi
	movl	%esi,(%edi)
	leal	16(%esp),%eax
	movl	%eax,0(%esp)
	call	v
	movl	%eax,%esi
	leal	as,%edi
	movl	%esi,(%edi)
	movl	as,%esi
	movl	512(%esp),%eax
	cltd
	divl	%esi
	movl	%eax,%edx
	movl	%edx,%esi
	leal	512(%esp),%edi
	movl	%esi,(%edi)
	movl	512(%esp),%edx
	shr		$1,%edx
	movl	%edx,%esi
	leal	512(%esp),%edi
	movl	%esi,(%edi)
	movl	512(%esp),%edx
	sal		$2,%edx
	movl	%edx,%esi
	leal	512(%esp),%edi
	movl	%esi,(%edi)
	leal	16(%esp),%eax
	movl	%eax,0(%esp)
	call	v
	movl	512(%esp),%eax
	movl	%eax,4(%esp)
	movl	$.LC1,%ebx
	movl	%ebx,0(%esp)
	call	printf
.L3:
	movl	c,%ecx
	cmpl	$0,%ecx
	jne		.L4
	jmp		.L5
.L4:
	movl	$3,%esi
	leal	520(%esp),%edi
	movl	%esi,(%edi)
	movl	$4,%esi
	leal	532(%esp),%edi
	movl	%esi,(%edi)
	movl	$2,%esi
	leal	516(%esp),%edi
	movl	%esi,(%edi)
	movl	516(%esp),%esi
	leal	528(%esp),%edi
	movl	%esi,(%edi)
	movl	$1,4(%esp)
	movl	$.LC1,%ebx
	movl	%ebx,0(%esp)
	call	printf
.L6:
	movl	516(%esp),%ebx
	movl	$0,%edx
	cmpl	%edx,%ebx
	setge	%al
	movzbl	%al,%eax
	movl	%eax,%edx
	movl	%edx,%ecx
	cmpl	$0,%ecx
	jne		.L7
	jmp		.L8
.L7:
	movl	516(%esp),%eax
	movl	%eax,4(%esp)
	movl	$.LC1,%ebx
	movl	%ebx,0(%esp)
	call	printf
	movl	516(%esp),%ebx
	movl	$1,%edx
	subl	%edx,%ebx
	movl	%ebx,%edx
	movl	%edx,%esi
	leal	516(%esp),%edi
	movl	%esi,(%edi)
	jmp		.L6
.L8:
	movl	$2,4(%esp)
	movl	$.LC1,%ebx
	movl	%ebx,0(%esp)
	call	printf
	movl	520(%esp),%ebx
	movl	532(%esp),%edx
	imull	%ebx,%edx
	movl	%edx,%ebx
	movl	512(%esp),%edx
	imull	%ebx,%edx
	movl	%edx,%esi
	leal	528(%esp),%edi
	movl	%esi,(%edi)
	movl	528(%esp),%eax
	movl	%eax,4(%esp)
	movl	$.LC1,%ebx
	movl	%ebx,0(%esp)
	call	printf
	movl	520(%esp),%ebx
	movl	532(%esp),%edx
	addl	%ebx,%edx
	movl	%edx,%ebx
	movl	c,%edx
	imull	%ebx,%edx
	movl	512(%esp),%esi
	movl	%edx,%eax
	cltd
	divl	%esi
	movl	%eax,%edx
	movl	%edx,%ebx
	movl	c,%edx
	subl	%edx,%ebx
	movl	%ebx,%edx
	movl	%edx,%esi
	leal	524(%esp),%edi
	movl	%esi,(%edi)
	movl	524(%esp),%eax
	movl	%eax,4(%esp)
	movl	$.LC1,%ebx
	movl	%ebx,0(%esp)
	call	printf
	movl	$0,%esi
	leal	c,%edi
	movl	%esi,(%edi)
	movl	532(%esp),%ebx
	movl	$0,%edx
	cmpl	%edx,%ebx
	setl	%al
	movzbl	%al,%eax
	movl	%eax,%edx
	movl	%edx,%ecx
	cmpl	$0,%ecx
	jne		.L9
	jmp		.L10
.L9:
	movl	$1,%eax
	jmp		.Lendmain
.L10:
	jmp		.L3
.L5:
	movl	508(%esp),%eax
	movl	%eax,4(%esp)
	movl	$.LC1,%ebx
	movl	%ebx,0(%esp)
	call	printf
	movl	508(%esp),%ecx
	cmpl	$0,%ecx
	jne		.L11
	jmp		.L12
.L11:
	leal	544(%esp),%edi
	movl	$1,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	$0,%esi
	movl	%esi,(%edi)
	leal	544(%esp),%edi
	movl	$1,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	%edx,4(%esp)
	movl	$.LC1,%ebx
	movl	%ebx,0(%esp)
	call	printf
.L12:
	leal	16(%esp),%edi
	movl	$1,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	508(%esp),%esi
	movl	%esi,(%edi)
	leal	16(%esp),%edi
	movl	$2,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	ss,%esi
	movl	%esi,(%edi)
	leal	16(%esp),%edi
	movl	$2,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	%edx,4(%esp)
	movl	$.LC1,%ebx
	movl	%ebx,0(%esp)
	call	printf
	leal	16(%esp),%edi
	movl	$2,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	%edx,12(%esp)
	movl	$1,8(%esp)
	movl	c,%ebx
	movl	%ebx,4(%esp)
	leal	16(%esp),%eax
	movl	%eax,0(%esp)
	call	vi
	movl	$3,12(%esp)
	movl	$2,8(%esp)
	movl	$1,4(%esp)
	leal	16(%esp),%eax
	movl	%eax,0(%esp)
	call	vi
	leal	16(%esp),%edi
	movl	$3,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	%eax,%esi
	movl	%esi,(%edi)
	leal	16(%esp),%edi
	movl	$3,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	%edx,4(%esp)
	movl	$.LC1,%ebx
	movl	%ebx,0(%esp)
	call	printf
	movl	$1,0(%esp)
	call	fuction
	movl	%eax,%esi
	leal	as,%edi
	movl	%esi,(%edi)
	movl	as,%ebx
	movl	%ebx,4(%esp)
	movl	$.LC1,%ebx
	movl	%ebx,0(%esp)
	call	printf
	leal	16(%esp),%eax
	movl	%eax,12(%esp)
	movl	as,%ebx
	movl	%ebx,8(%esp)
	movl	512(%esp),%eax
	movl	%eax,4(%esp)
	movl	ss,%ebx
	movl	%ebx,0(%esp)
	call	mul
	movl	%eax,%esi
	leal	as,%edi
	movl	%esi,(%edi)
	movl	as,%ebx
	movl	%ebx,4(%esp)
	movl	$.LC1,%ebx
	movl	%ebx,0(%esp)
	call	printf
	leal	16(%esp),%edi
	movl	$1,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	%edx,592(%esp)
	movl	508(%esp),%edx
	sal		$2,%edx
	movl	592(%esp),%ebx
	addl	%ebx,%edx
	movl	%edx,592(%esp)
	leal	16(%esp),%edi
	movl	$1,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	%edx,%ebx
	movl	$4,%edx
	addl	%ebx,%edx
	movl	%edx,596(%esp)
	leal	16(%esp),%edi
	movl	$2,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	%edx,600(%esp)
	leal	16(%esp),%edi
	movl	$3,%ebx
	leal	0(,%ebx,4),%edx
	addl	%edi,%edx
	movl	%edx,%edi
	movl	(%edx),%edx
	movl	600(%esp),%ebx
	imull	%ebx,%edx
	movl	596(%esp),%eax
	movl	%edx,%esi
	cltd
	divl	%esi
	movl	%eax,%edx
	movl	592(%esp),%ebx
	addl	%ebx,%edx
	movl	%edx,4(%esp)
	movl	$.LC1,%ebx
	movl	%ebx,0(%esp)
	call	printf
	movl	$0,%eax
	jmp		.Lendmain
.Lendmain:
	leave
	ret
	.size	main,.-main
