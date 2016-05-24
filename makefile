

all:struct2json


objs = structjson_bison.o structjson_flex.o structjson_lib.o translate.o structjson_translate.o



struct2json: $(objs)
	g++ -o $@ $^ -lfl


structjson_flex.c: structjson.l
	flex -o $@ $^

structjson_bison.c: structjson.y
	bison -o $@ -d $^


$(objs):structjson_lib.h


.phony:clean



clean:
	rm -f *.o struct2json structjson_bison.c structjson_bison.h structjson_flex.c
