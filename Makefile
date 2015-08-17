all:
	gcc main.c chain/chain.c -o ctest

clean:
	rm -rf ctest
