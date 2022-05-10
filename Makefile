.PHONY: test

OFLEX=../oflex/build/src/oflex.exe
#-d
#-t 
obison1: obison.l obison.y
	${OFLEX} -i obison.l -o lex_header.h -t otoken.h -p oflex_sample -k  OToken
	bison -d -v -t obison.y
	#mv lex.yy.c lex.yy.cpp -DEOF_20220422_EOF=0
	mv obison.tab.c obison.tab.cpp
	g++ -g  *.cpp -lfl -DEOF_20220422_EOF=0 -o obison1
	
test:
	rm -rf obison1.exe
	make obison1
dotest:	
	cd test & ../obison1.exe ../obison.y
	cd ..
	