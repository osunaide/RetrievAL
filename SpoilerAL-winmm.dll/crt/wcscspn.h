#ifdef UTF16MAP
#ifndef _M_IX86
size_t __cdecl wcscspn(const wchar_t *string, const wchar_t *control)
{
	const wchar_t *p;
	wchar_t       c;
	size_t        index;

	for (UTF16MAP[0] = 1, p = control; c = *(p++); )
		_bittestandset(UTF16MAP, c);
	index = -1;
	string++;
	do
		c = string[index++];
	while (!_bittest(UTF16MAP, c));
	for (UTF16MAP[0] = 0, p = control; c = *(p++); )
		UTF16MAP[c >> 5] = 0;
	return index;
}

wchar_t * __cdecl wcspbrk(const wchar_t *string, const wchar_t *control)
{
	const wchar_t *p;
	wchar_t       c;

	for (UTF16MAP[0] = 1, p = control; c = *(p++); )
		_bittestandset(UTF16MAP, c);
	do
		c = *(string++);
	while (!_bittest(map, c));
	string = c ? string - 1 : NULL;
	for (UTF16MAP[0] = 0, p = control; c = *(p++); )
		UTF16MAP[c >> 5] = 0;
	return (wchar_t *)string;
}
#else
static unsigned __int64 __fastcall internal_wcspbrk(const wchar_t *string, const wchar_t *control);

__declspec(naked) size_t __cdecl wcscspn(const wchar_t *string, const wchar_t *control)
{
	__asm
	{
		#define string  (esp + 4)
		#define control (esp + 8)

		mov     ecx, dword ptr [string]                     // ecx = string
		mov     edx, dword ptr [control]                    // edx = control
		call    internal_wcspbrk

		// Return code
		mov     ecx, dword ptr [string]                     // ecx = string
		mov     eax, edx
		sub     eax, ecx
		shr     eax, 1
		ret

		#undef string
		#undef control
	}
}

__declspec(naked) wchar_t * __cdecl wcspbrk(const wchar_t *string, const wchar_t *control)
{
	__asm
	{
		#define string  (esp + 4)
		#define control (esp + 8)

		mov     ecx, dword ptr [string]                     // ecx = string
		mov     edx, dword ptr [control]                    // edx = control
		call    internal_wcspbrk

		// Return code
		add     eax, -1
		sbb     eax, eax
		and     eax, edx
		ret

		#undef string
		#undef control
	}
}

__declspec(naked) static unsigned __int64 __fastcall internal_wcspbrk(const wchar_t *string, const wchar_t *control)
{
	__asm
	{
		#define string  ecx
		#define control edx

		push    edx
		xor     eax, eax
		mov     dword ptr [UTF16MAP], 1
		jmp     init

		// Set control char bits in map
		align   16
	initnext:
		bts     dword ptr [UTF16MAP], eax
	init:
		mov     ax, word ptr [edx]
		add     edx, 2
		test    ax, ax
		jnz     initnext

		mov     edx, ecx                                    // edx = string
		pop     ecx                                         // ecx = control

		// Loop through comparing source string with control bits
		align   16
	dstnext:
		mov     ax, word ptr [edx]
		add     edx, 2
		bt      dword ptr [UTF16MAP], eax
		jnc     dstnext                                     // did not find char, continue

		push    eax
		mov     dword ptr [UTF16MAP], 0
		jmp     clear

		// Clear control char bits in map
		align   16
	clearnext:
		shr     eax, 5
		mov     dword ptr [UTF16MAP + eax * 4], 0
		xor     eax, eax
	clear:
		mov     ax, word ptr [ecx]
		add     ecx, 2
		test    ax, ax
		jnz     clearnext
		pop     eax
		sub     edx, 2
		ret                                                 // __cdecl return

		#undef string
		#undef control
	}
}
#endif
#else
#ifndef _M_IX86
size_t __cdecl wcscspn(const wchar_t *string, const wchar_t *control)
{
	size_t        n;
	const wchar_t *p1, *p2;
	wchar_t       c1, c2;

	n = -1;
	for (p1 = string + 1; c1 = p1[n++]; )
		for (p2 = control; c2 = *(p2++); )
			if (c2 == c1)
				goto DONE;
DONE:
	return n;
}

wchar_t * __cdecl wcspbrk(const wchar_t *string, const wchar_t *control)
{
	const wchar_t *p1, *p2;
	wchar_t       c1, c2;

	for (p1 = string; c1 = *(p1++); )
		for (p2 = control; c2 = *(p2++); )
			if (c2 == c1)
				return (wchar_t *)p1 - 1;
	return NULL;
}
#else
__declspec(naked) size_t __cdecl wcscspn(const wchar_t *string, const wchar_t *control)
{
	__asm
	{
		#define string  (esp + 4)
		#define control (esp + 8)

		push    esi                                         // preserve esi
		push    edi                                         // preserve edi
		mov     eax, dword ptr [string + 8]
		mov     edi, dword ptr [control + 8]

		align   16
	outer_loop:
		mov     dx, word ptr [eax]
		add     eax, 2
		test    dx, dx
		jz      epilog
		mov     esi, edi

		align   16
	inner_loop:
		mov     cx, word ptr [esi]
		add     esi, 2
		test    cx, cx
		jz      outer_loop
		cmp     cx, dx
		jne     inner_loop
	epilog:
		sub     eax, 2
		mov     ecx, dword ptr [string + 8]
		sub     eax, ecx
		pop     edi                                         // restore edi
		shr     eax, 1
		pop     esi                                         // restore esi
		ret                                                 // __cdecl return

		#undef string
		#undef control
	}
}

__declspec(naked) wchar_t * __cdecl wcspbrk(const wchar_t *string, const wchar_t *control)
{
	__asm
	{
		#define string  (esp + 4)
		#define control (esp + 8)

		push    ebx                                         // preserve ebx
		push    esi                                         // preserve esi
		mov     ecx, dword ptr [string + 8]
		mov     esi, dword ptr [control + 8]
		xor     eax, eax

		align   16
	outer_loop:
		mov     ax, word ptr [ecx]
		add     ecx, 2
		test    ax, ax
		jz      epilog
		mov     edx, esi

		align   16
	inner_loop:
		mov     bx, word ptr [edx]
		add     edx, 2
		test    bx, bx
		jz      outer_loop
		cmp     bx, ax
		jne     inner_loop
		lea     eax, [ecx - 2]
	epilog:
		pop     esi                                         // restore esi
		pop     ebx                                         // restore ebx
		ret                                                 // __cdecl return

		#undef string
		#undef control
	}
}
#endif
#endif

