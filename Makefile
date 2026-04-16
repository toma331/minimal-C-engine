CC = gcc 
CFLAGS = -Wall -Wextra  
GLFLAGS = -lglfw -lGLU -lGL -lm

SRC = main.c events.c math.c render.c camera.c debug_draw.c obj_loader.c

engine: $(SRC)
	$(CC) $(CFLAGS) $^ $(GLFLAGS) -o $@

debug: $(SRC) 
	$(CC) $(CFLAGS) $^ $(GLFLAGS) -g -O0 -o debug

clean:
	rm -f engine debug
