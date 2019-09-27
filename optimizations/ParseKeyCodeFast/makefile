NAME = main
CXXFLAGS = -std=c++17
OBJS = main.o ParseKeyCodeEmpty.o ParseKeyCode.o ParseKeyCodeFast.o
# OBJS += ParseKeyCodeRegex.o

ifeq ($(SPEEDTEST),1)
CXXFLAGS += -O0 -DSPEEDTEST
else ifeq ($(SPEEDTEST),2)
CXXFLAGS += -O3 -DSPEEDTEST
else ifeq ($(SPEEDTEST),3)
CXXFLAGS += -Os -DSPEEDTEST
endif

ifneq (,$(findstring profile,$(MAKECMDGOALS)))
CXXFLAGS += -pg -DSPEEDTEST
endif

ifneq (,$(findstring coverage,$(MAKECMDGOALS)))
CXXFLAGS += -pg --coverage -DSPEEDTEST
endif

$(NAME).exe : $(OBJS)
%.exe : ; g++ -o $@ $^ $(CXXFLAGS)
%.o : %.cpp ; g++ -o $@ -c $< $(CXXFLAGS)
gmon.out gmon.sum $(NAME).stats : $(NAME).exe ; gprof $< >$(NAME).stats

.PHONY : clean run
clean : ; del $(NAME).exe *.o *.gcda *.gcno *.gcov gmon.out $(NAME).stats $(NAME).log 2>NUL
run : $(NAME).exe ; @.\$<

.PHONY : coverage profile
coverage : | profile ; gcov -m $(OBJS) 2>>$(NAME).log
profile : | run $(NAME).stats
