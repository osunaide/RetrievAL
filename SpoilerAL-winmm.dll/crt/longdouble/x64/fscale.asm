public _fscale

.code

_fscale proc
	fld     tbyte ptr [r8]
	fld     tbyte ptr [rdx]
	fscale
	fstp    tbyte ptr [rcx]
	fstp    st(0)
	ret
_fscale endp

end
