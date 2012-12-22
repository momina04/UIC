all:
	@echo "Compiling..."
	gcc -g matrix_mult.c -O3 -o mul_o3
	gcc -g matrix_mult.c -O0 -o mul_o0

run: clean all
	@echo "Run started..."
#1
	@echo "Solution 1"
	echo "{"
	./mul_o0 1024 o
	./mul_o3 1024 o
	echo "}"

#2
	@echo "Solution 2"
	echo "{"
	@for i in  16 32 64 128 256 512 1024 2048 4096; \
	do \
		./mul_o3 $$i o ; \
	done
	echo "}"
#3
	@echo "Solution 3"
	echo "{"
	@for i in  16 32 64 128 256 512 1024 2048 4096; \
	do \
		./mul_o3 $$i b 8 ; \
	done
	echo "}"
#4
	@echo "Solution 4"
	echo "{"
	@for i in  4 8 16 32 64 128 256 512; \
	do \
		./mul_o3 2048 b $$i ; \
	done
	echo "}"
	@echo "...Run complete"

#5. What did you learn from this project?

#	./mul_o3 1024 o
#	./mul_o3 1024 b 16

clean:
	@echo "Cleaning files..."
	rm -f mul_o0
	rm -f mul_o3
