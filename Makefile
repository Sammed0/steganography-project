OBJ:= $(patsubst %.c, %.o, $(wildcard*.c))
test.exe: $(OBJ)
	gcc -o $@ $^

clean:
	rm  *.exe *.o
	
# OBJ := $(patsubst %.c, %.o, $(wildcard *.c))

# test.exe: $(OBJ)
# 	gcc -o $@ $^

# clean:
# 	rm -f *.exe *.o