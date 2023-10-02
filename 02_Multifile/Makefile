SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
OUT=prog
EXECUTABLES=$(OUT) $(OUT)-a $(OUT)-so
DYNOBJECTS=$(SOURCES:.c=.d.o)
TEST_GENERATES=$(wildcard test_*)
GENERATES=$(OBJECTS) $(DYNOBJECTS) $(EXECUTABLES) $(TEST_GENERATES) $(wildcard *.a) $(wildcard *.so)

all: $(OUT)

$(OUT):   $(OBJECTS)
$(OUT)-a: $(OUT).o liboutput_static.a
		cc -L. $< -loutput_static -o $@
$(OUT)-so: $(OUT).d.o liboutput.so
		cc -L. $< -loutput -o $@
liboutput.so: $(filter-out $(OUT).d.o,$(DYNOBJECTS))
		cc -shared $^ -o $@
liboutput_static.a: $(filter-out $(OUT).o,$(OBJECTS))
		ar -rcs $@ $^

%.o: outlib.h
%.d.o: %.c outlib.h
		cc -fPIC -c $< -o $@
clean:
		rm -rf $(GENERATES)

test: $(EXECUTABLES)
		for EXEC in $(EXECUTABLES); do \
			LD_LIBRARY_PATH=`pwd` ./$$EXEC 2> test_1_$$EXEC; \
			sed -i -r "s/\.\/$$EXEC//g" test_1_$$EXEC; \
		done
		for EXEC in $(EXECUTABLES); do \
			LD_LIBRARY_PATH=`pwd` ./$$EXEC gibberish > test_2_$$EXEC; \
		done
		for EXEC in $(EXECUTABLES); do \
			LD_LIBRARY_PATH=`pwd` ./$$EXEC gibberish nonsense babble > test_3_$$EXEC; \
		done
		for n in 1 2 3; do \
			cmp test_$${n}_$(OUT) test_$${n}_$(OUT)-a; \
			cmp test_$${n}_$(OUT)-a test_$${n}_$(OUT)-so; \
		done


		 