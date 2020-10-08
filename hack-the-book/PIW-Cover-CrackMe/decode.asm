;
; Hack-the-book - PIW Cover CrackMe
; http://blog.malcom.pl/2020/hackowanie-ksiazki-piw-cover-crackme.html
;
; Original code from the book cover
;

	lea esi, [bk_cvr]		; esi = ptr to book cover data
	lea edi, [rwx_mem]		; edi = ptr to rwx memory page
	mov ecx, 89				; number of words

dcd:
	lodsw					; ax = *bk_cvr, bk_cvr += 2
	xor ax, 0x5245			; ax ^ 0x5245
	stosw					; *rwx_mem = ax, rwx_mem += 2
	loop dcd				; jmp to dcd, ecx -= 1

	jmp rwx_mem				; call decoded code
