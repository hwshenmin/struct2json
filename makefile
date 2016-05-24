

all:structjson


objs = jcejson_bison.o jcejson_flex.o jcejson_lib.o translate.o jcejson_translate.o



structjson: $(objs)
	g++ -o $@ $^ -lfl


jcejson_flex.c: jcejson.l
	flex -o $@ $^

jcejson_bison.c: jcejson.y
	bison -o $@ -d $^


$(objs):jcejson_lib.h


.phony:clean



clean:
	rm -f *.o jcejson jcejson_bison.c jcejson_bison.h jcejson_flex.c
