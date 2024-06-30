all:
	cmake -B build/ -S . && make -C build/ -j$(nproc)
