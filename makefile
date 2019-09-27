define DEL_EVERYTHING_EXCEPT_TXT
	FOR %%i IN ($1\*) DO @IF NOT %%~xi==.txt DEL %%i
endef

.PHONY : scintilla scite run clean
scite : | scintilla ; make -C scite/win32
scintilla : ; make -C scintilla-for-kronotope/scintilla/win32
run : | scite ; @scite/bin/Sc1.exe
clean :
	make clean -C scite/win32
	make clean -C scintilla-for-kronotope/scintilla/win32
	FOR %%i IN (scite\bin\*) DO @IF NOT %%~xi==.txt DEL %%i
	$(call DEL_EVERYTHING_EXCEPT_TXT,scintilla-for-kronotope\scintilla\bin)
