; Mandelbrot set; x86-64 project; Piotr Zajac;

;rdi - pixel buffer
;rsi - width and height (in pixels)
;rdx - max iterations
;xmm0 - x left
;xmm1 - y top
;xmm2 - step

section .text
	global f

f:
	push rbp	
	mov rbp, rsp	
	
	mov r9, rsi		
	imul r9, 3		; store width*3

	mov rbx, rdx		; store max iterations
	movss xmm4, xmm0	; store left x coordinate (p.real)
	movss xmm5, xmm1	; store top y coordinate (p.imag)

	mov r8, 255
	cvtsi2ss xmm3, r8	; store max color value
	cvtsi2ss xmm7, rbx	; store max iterations (converted to float)
	inc rbx				; max iterations + 1

	mov r8, 2
	cvtsi2ss xmm8, r8	; store value 2 (float) (used in further calculations)
	mov r8, 4
	cvtsi2ss xmm15, r8	; store value 4	(float)	(used in further calculations)

	mov r12, rdi	; save address of pixel buffer in r12
	mov r13, 0		; set offset in current row to 0
	mov r14, 0		; set current row to 0	

prepare_next_pixel:
	mov r10, rbx		; set color value to max iterations + 1
	xorps xmm9, xmm9	; initial z.real = 0
	xorps xmm10, xmm10	; initial z.imag = 0

count_iter:
	dec r10		; decrement color value

	movss xmm11, xmm9	; z.real
	mulss xmm11, xmm11	; z.real^2
	movss xmm12, xmm10	; z.imag
	mulss xmm12, xmm12	; z.imag^2
	subss xmm11, xmm12 	; z.real^2 - z.imag^2
	addss xmm11, xmm4	; z.real^2 - z.imag^2+ p.real

	movss xmm13, xmm9	; z.real
	mulss xmm13, xmm10	; z.real * z.imag
	mulss xmm13, xmm8	; 2 * z.real * z.imag + p.imag
	addss xmm13, xmm5	; 2 * z.real * z.imag  + p.imag
	
	movss xmm9, xmm11	; store new z.real
	movss xmm10, xmm13	; store new z.imag

	mulss xmm11, xmm11	; z.real^2
	mulss xmm13, xmm13	; z.imag^2

	addss xmm11, xmm13	; |z|^2 = z.real^2 + z.imag^2

	cmp r10, 0	
	jbe put_pixel		; jump to put_pixel if color value is lower or equal to 0

	comiss xmm11, xmm15
	jb count_iter		; stay in loop if |z|^2 < 4

put_pixel:
	cvtsi2ss xmm6, r10	; convert color value to float
	divss xmm6, xmm7	; divide by max iterations
	mulss xmm6, xmm3	; multiply by max color value
	cvtss2si r10, xmm6	; convert to int

	mov byte[r12+r13], r10b			
	mov byte[r12+r13+1], r10b		
	mov byte[r12+r13+2], r10b		; set pixel color

	addss xmm4, xmm2	; update p.real (add step to x coordinate)

	add r13, 3			; add 3 to row offset
	cmp r13, r9
	jb prepare_next_pixel	; go to next pixel, unless all were visited

	subss xmm5, xmm2	; update p.imag (subtract step from y coordinate)
	movss xmm4, xmm0	; set p.real (x left)

	mov r13, 0			; set offset in current row to 0
	sub r12, r9			; subtract width*3 from r12
	inc r14				; increment row counter
	cmp r14, rsi		
	jb prepare_next_pixel	; go to next row, unless all were visited

exit:
	mov rsp, rbp	
	pop rbp		
	ret
