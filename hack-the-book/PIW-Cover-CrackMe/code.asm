;
; Hack-the-book - PIW Cover CrackMe
; http://blog.malcom.pl/2020/hackowanie-ksiazki-piw-cover-crackme.html
;
; Copyright (C) 2020 by Marcin 'Malcom' Malich <me@malcom.pl>
; Released under the MIT License
;
; Reverse enginered code from code.bin file
;

bits 32

	call $+5						; Position Independent Code
	pop ebp							; it's nice trick to determine
	sub ebp, 5						; the base address of this code

	xor ecx, ecx					; clear ecx / counter

loop:
	xor eax, eax					; clear eax / crc_val
	push ecx						; store the start pos

crc_calc:
	mov dl, [ebp+ecx+passwd]		; dl = passwd[ecx]
	test dl, dl						; if got '\0'
	jz short crc_cmp				; check crc sum

	crc32 eax, dl					; otherwise calc crc for dl
	inc ecx							; and goto to next
	jmp short crc_calc				; char processing

crc_cmp:
	pop ecx							; get the start pos
	cmp [ebp+ecx*4+crcsum], eax		; crcsum[ecx] == eax
	jnz short bad					; if not jmp to error

	inc ecx							; otherwise continue
	cmp cl, 17h						; checking next substring
	jnz short loop					; until all crcsum pass

	mov eax, 646F6F47h				; eax = "Good"
	int3							; dbg break

bad:
	mov eax, 45504F4Eh				; eax = "NOPE"
	int3							; dbg break

crcsum	dd 0x564D94CE, 0x56487985, 0xCD6966E6, 0x791B5538
		dd 0xBDC0BFB7, 0x8ECBF593, 0xC652C4A2, 0x94A3EBF7
		dd 0x2673B49E, 0x89F7731E, 0x32C1EB44, 0x7886F083
		dd 0x52E2BB44, 0xC7FDFFAA, 0x69F9005A, 0xE04743AC
		dd 0x44229524, 0xE8032961, 0x8CC30F1E, 0x084CDEA3
		dd 0xEB48D1AD, 0x90809740, 0xE4292D5A

passwd	db 'TutajWpiszTajneHaslo!!!',0

align 2, db 0
