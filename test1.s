			# This code was produced by the CERI Compiler
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top
	push $6
	push $2
	pop %rbx
	pop %rax
	addq %rbx, %rax
	push %rax
	push $5
	push $5
	pop %rbx
	pop %rax
	addq %rbx, %rax
	push %rax
	pop %rbx
	pop %rax
	cmp %rbx, %rax
	setle %al
	movzbq %al, %rax
	push %rax
	movq %rbp, %rsp		# Restore the position of the stack's top
	ret			# Return from main function
