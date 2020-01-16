# Repertoires du projet

BIN_DIR = bin
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
OBJPROF_DIR = obj-prof

# Options de compilation/édition des liens

CC = clang
LD = clang
INC = -I$(INC_DIR)

CFLAGS += $(INC) -Wall -std=c99 -O0 -g  -Wextra
LDFLAGS = -lm

# Liste des fichiers objet

# et les votres!
OBJ_FILES = $(OBJ_DIR)/outils.o $(OBJ_DIR)/jpeg_reader.o $(OBJ_DIR)/huffman.o $(OBJ_DIR)/bitstream.o $(OBJ_DIR)/2a_reconstruction_bloc.o $(OBJ_DIR)/2c_conversion_RGB.o $(OBJ_DIR)/3_ecriture_PPM.o $(OBJ_DIR)/2_decodage_MCU.o $(OBJ_DIR)/2_decodage_progressif.o $(OBJ_DIR)/jpeg2ppm.o

# cible par défaut

TARGET = $(BIN_DIR)/jpeg2ppm

all: $(TARGET)

$(TARGET): $(OBJPROF_FILES) $(OBJ_FILES)
	$(LD) $(LDFLAGS) $(OBJ_FILES) -o $(TARGET)

# compilation outils
$(OBJ_DIR)/outils.o: $(SRC_DIR)/outils.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/outils.c -o $(OBJ_DIR)/outils.o

# compilation bistream
$(OBJ_DIR)/bitstream.o: $(SRC_DIR)/bitstream.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/bitstream.c -o $(OBJ_DIR)/bitstream.o

# compilation huffman
$(OBJ_DIR)/huffman.o: $(SRC_DIR)/huffman.c $(INC_DIR)/outils.h $(INC_DIR)/bitstream.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/huffman.c -o $(OBJ_DIR)/huffman.o

#compilation jpeg_reader
$(OBJ_DIR)/jpeg_reader.o: $(SRC_DIR)/jpeg_reader.c $(INC_DIR)/outils.h $(INC_DIR)/bitstream.h $(INC_DIR)/huffman.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/jpeg_reader.c -o $(OBJ_DIR)/jpeg_reader.o

# compilation 2a
$(OBJ_DIR)/2a_reconstruction_bloc.o: $(SRC_DIR)/2a_reconstruction_bloc.c $(INC_DIR)/outils.h $(INC_DIR)/bitstream.h $(INC_DIR)/jpeg_reader.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/2a_reconstruction_bloc.c -o $(OBJ_DIR)/2a_reconstruction_bloc.o

# compilation 2c
$(OBJ_DIR)/2c_conversion_RGB.o: $(SRC_DIR)/2c_conversion_RGB.c $(INC_DIR)/outils.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/2c_conversion_RGB.c -o $(OBJ_DIR)/2c_conversion_RGB.o

# compilation 3
$(OBJ_DIR)/3_ecriture_PPM.o: $(SRC_DIR)/3_ecriture_PPM.c $(INC_DIR)/2c_conversion_RGB.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/3_ecriture_PPM.c -o $(OBJ_DIR)/3_ecriture_PPM.o

# compilation 2
$(OBJ_DIR)/2_decodage_MCU.o: $(SRC_DIR)/2_decodage_MCU.c $(INC_DIR)/2a_reconstruction_bloc.h $(INC_DIR)/2c_conversion_RGB.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/2_decodage_MCU.c -o $(OBJ_DIR)/2_decodage_MCU.o

# compilation 2 progressif
$(OBJ_DIR)/2_decodage_progressif.o: $(SRC_DIR)/2_decodage_progressif.c $(INC_DIR)/2a_reconstruction_bloc.h $(INC_DIR)/2c_conversion_RGB.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/2_decodage_progressif.c -o $(OBJ_DIR)/2_decodage_progressif.o

# compilation jpeg2ppm
$(OBJ_DIR)/jpeg2ppm.o: $(SRC_DIR)/jpeg2ppm.c $(INC_DIR)/outils.h $(INC_DIR)/2_decodage_progressif.h $(INC_DIR)/3_ecriture_PPM.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/jpeg2ppm.c -o $(OBJ_DIR)/jpeg2ppm.o

.PHONY: clean

clean:
	rm -f $(TARGET) $(OBJ_FILES)
