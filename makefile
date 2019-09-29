define DEL_EVERYTHING_EXCEPT_TXT
	FOR %%i IN ($1\*) DO @IF NOT %%~xi==.txt DEL %%i
endef

.PHONY : scintilla scite run clean reset
scite : | scintilla ; mingw32-make -C scite\win32
scintilla : ; mingw32-make -C scintilla-for-kronotope\scintilla\win32
run : | scite ; @scite\bin\Sc1.exe
clean :
	mingw32-make clean -C scite\win32
	mingw32-make clean -C scintilla-for-kronotope\scintilla\win32
reset : | clean
	$(call DEL_EVERYTHING_EXCEPT_TXT,scite\bin)
	$(call DEL_EVERYTHING_EXCEPT_TXT,scintilla-for-kronotope\scintilla\bin)
