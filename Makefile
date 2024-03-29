CC = g++

FLAGS = -flto -O3 -g

SOURCES_TOOLS = src/GothicTools.cpp src/Archiver.cpp src/Bsp.cpp src/Disk.cpp src/Material.cpp src/Mesh.cpp src/Model.cpp src/ModelAni.cpp src/MorphMesh.cpp src/Object.cpp src/ObjectRegistry.cpp src/ProgMesh.cpp src/ScanDir.cpp src/String.cpp src/Texture.cpp src/Visual.cpp src/Vob.cpp src/VobDiff.cpp src/WayNet.cpp src/World.cpp
SOURCES_ZEN = src/GothicZEN.cpp $(SOURCES_TOOLS)
SOURCES_ANIMS = src/GothicAnims.cpp $(SOURCES_TOOLS)

all:
	mkdir -p bin
	$(CC) -o "bin/GothicZEN" -Wall $(FLAGS) $(SOURCES_ZEN)
	$(CC) -o "bin/GothicAnims" -Wall $(FLAGS) $(SOURCES_ANIMS)
