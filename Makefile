file_fw:file_fw.c
	-gcc -o $@ $^

file_kh:file_kh.c
	-gcc -o $@ $^
	
clear:
	-rm file_*
