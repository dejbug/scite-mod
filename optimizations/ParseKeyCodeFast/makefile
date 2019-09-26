NAME = main
CXXFLAGS = -std=c++17
OBJS = main.o ParseKeyCodeEmpty.o ParseKeyCode.o ParseKeyCodeFast.o ParseKeyCodeRegex.o

ifdef PROFILE
CXXFLAGS += -pg -DTEST_SPEED
.PHONY : profile
profile : | run $(NAME).stats
endif

ifdef COVERAGE
CXXFLAGS += --coverage
.PHONY : profile coverage
profile : | run $(NAME).stats
coverage : | profile
	gcov -mb $(OBJS) 2>>$(NAME).log
endif

$(NAME).exe : $(OBJS)
%.exe : ; g++ -o $@ $^ $(CXXFLAGS)
%.o : %.cpp ; g++ -o $@ -c $< $(CXXFLAGS)
gmon.out gmon.sum $(NAME).stats: $(NAME).exe ; gprof $< >$(NAME).stats

.PHONY : clean run
clean : ; del $(NAME).exe *.o *.gcda *.gcno *.gcov gmon.out $(NAME).stats $(NAME).log 2>NUL
run : $(NAME).exe ; @.\$<
