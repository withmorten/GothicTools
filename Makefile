CC = g++

FLAGS = -O3 -g

SOURCES_ZEN = Src/GothicZEN.cpp Src/GothicTools.cpp Src/Bsp.cpp Src/Disk.cpp Src/Material.cpp Src/Mesh.cpp Src/String.cpp Src/World.cpp
SOURCES_ANIMS = Src/GothicAnims.cpp Src/GothicTools.cpp Src/Disk.cpp Src/Material.cpp Src/Mesh.cpp Src/Model.cpp Src/ModelAni.cpp Src/MorphMesh.cpp Src/ProgMesh.cpp Src/ScanDir.cpp Src/String.cpp

all:
	mkdir -p Bin
	$(CC) -o "Bin/GothicZEN" -Wall $(FLAGS) $(SOURCES_ZEN)
	$(CC) -o "Bin/GothicAnims" -Wall $(FLAGS) $(SOURCES_ANIMS)
