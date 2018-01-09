

all: ../inst

HDRS = I2Cdev.h MPU6050.h MotionInst.h
CMN_OBJS = I2Cdev.o MPU6050.o MotionInst.o INIReader.o ini.o
INST_OBJS = main.o

CXXFLAGS = -Wall -g -O2 -std=c++11 `pkg-config gtkmm-3.0 --cflags --libs` -DDEBUG=1

$(CMN_OBJS) $(INST_OBJS) : $(HDRS)

../inst: $(CMN_OBJS) $(INST_OBJS)
	$(CXX) -o $@ $^ -lm -lpthread -lrt

clean:
	rm -f $(CMN_OBJS) $(INST_OBJS) ../inst
