CC = g++

FLAGS = -flto -O3 -g

SOURCES_ZEN = src/GothicZEN.cpp src/GothicTools.cpp src/Bsp.cpp src/Disk.cpp src/Material.cpp src/Mesh.cpp src/String.cpp src/World.cpp
SOURCES_ANIMS = src/GothicAnims.cpp src/GothicTools.cpp src/Disk.cpp src/Material.cpp src/Mesh.cpp src/Model.cpp src/ModelAni.cpp src/MorphMesh.cpp src/ProgMesh.cpp src/ScanDir.cpp src/String.cpp

all:
	mkdir -p bin
	$(CC) -o "bin/GothicZEN" -Wall $(FLAGS) $(SOURCES_ZEN)
	$(CC) -o "bin/GothicAnims" -Wall $(FLAGS) $(SOURCES_ANIMS)
