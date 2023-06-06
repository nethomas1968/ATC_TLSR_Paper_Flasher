GCC=g++
CFLAGS=-g -Wall -Wunused-variable -Wshadow -Wconversion

TARGET=HanshowFlasher
OBJS=HanshowFlasher.o Serial.o MCUTools.o TelinkFile.o FileUtils.o Logger.o

INCDIR=-I./
LIBDIR=-L./

all: $(TARGET)

$(TARGET): $(OBJS)
	$(GCC) $(CFLAGS) $(LIBDIR) -o $(TARGET) $(OBJS) -lm
	
HanshowFlasher.o: HanshowFlasher.cpp HanshowFlasher.h
	$(GCC) $(CFLAGS) $(INCDIR) -c HanshowFlasher.cpp

Serial.o: Serial.cpp Serial.h
	$(GCC) $(CFLAGS) $(INCDIR) -c Serial.cpp

MCUTools.o: MCUTools.cpp MCUTools.h
	$(GCC) $(CFLAGS) $(INCDIR) -c MCUTools.cpp

TelinkFile.o: TelinkFile.cpp TelinkFile.h
	$(GCC) $(CFLAGS) $(INCDIR) -c TelinkFile.cpp

FileUtils.o: FileUtils.cpp FileUtils.h
	$(GCC) $(CFLAGS) $(INCDIR) -c FileUtils.cpp

Logger.o: Logger.cpp Logger.h
	$(GCC) $(CFLAGS) $(INCDIR) -c Logger.cpp

.phony clean:

clean:
	@echo "Cleaning..."
	@rm -f *.o
	@rm -f $(TARGET)
	@echo "Done"
