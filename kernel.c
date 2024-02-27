
void kmain() {
	char *video_memory = (char *) 0xb8000;
	char hello[30] = "HELLO FROM C KERNEL B!!!";

	for (int i = 0; i < 30; i++) {
		*(video_memory + i * 2) = hello[i];
		*(video_memory + i * 2 + 1) = '4';
	}
}
