CC      = gcc
CFLAGS  = -std=c99 -fstack-protector-all -Wall -Wextra -g
INC     = -I include -I unity

OBJS = src/shape.o src/bst.o src/list.o src/sort.o src/svg.o

# ------------------------------------------------------------------ #
# Executavel principal                                                  #
# ------------------------------------------------------------------ #

ted: $(OBJS) src/main.o
	$(CC) $(CFLAGS) -o ted $^ -lm

src/%.o: src/%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

# ------------------------------------------------------------------ #
# Framework Unity                                                      #
# ------------------------------------------------------------------ #

unity/unity.o: unity/unity.c
	$(CC) $(CFLAGS) -c $< -o $@

UOBJ = unity/unity.o

# ------------------------------------------------------------------ #
# Testes unitarios                                                     #
# ------------------------------------------------------------------ #

test/%.o: test/%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

test_shape: $(UOBJ) src/shape.o test/test_shape.o
	$(CC) $(CFLAGS) -o test_shape $^ -lm
	./test_shape

test_list: $(UOBJ) src/shape.o src/list.o test/test_list.o
	$(CC) $(CFLAGS) -o test_list $^ -lm
	./test_list

test_bst: $(UOBJ) src/shape.o src/bst.o test/test_bst.o
	$(CC) $(CFLAGS) -o test_bst $^ -lm
	./test_bst

test_sort: $(UOBJ) src/shape.o src/list.o src/sort.o test/test_sort.o
	$(CC) $(CFLAGS) -o test_sort $^ -lm
	./test_sort

tests: test_shape test_list test_bst test_sort

# ------------------------------------------------------------------ #
# Execucao com entradas de amostra                                     #
# ------------------------------------------------------------------ #

run: ted
	./ted -f input/sample.geo -o output

run_qry: ted
	./ted -f input/sample.geo -o output -q input/sample.qry

# ------------------------------------------------------------------ #
# Limpeza                                                              #
# ------------------------------------------------------------------ #

clean:
	-del /Q src\*.o test\*.o unity\*.o 2>NUL
	-del /Q ted.exe test_shape.exe test_list.exe test_bst.exe test_sort.exe 2>NUL

.PHONY: tests run run_qry clean