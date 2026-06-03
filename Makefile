all: file_fw file_kh

file_fw:file_fw.c
	-gcc -o $@ $^

file_kh:file_kh.c
	-gcc -o $@ $^
	
.PHONG:clean
clean:
	-rm file_fw file_kh
