CXX = g++ # Compiler to use
CFLAGS = -lpthread -std=c++17 -fdiagnostics-color=always # Flags to pass


pi_cercle_unite: pi_cercle_unite.cpp
	$(CXX) pi_cercle_unite.cpp -o pi_cercle_unite $(CFLAGS)

# Merge

# Billes

# Game of life

clean:
	rm *.o pi_cercle_unite