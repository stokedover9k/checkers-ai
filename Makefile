OBJS = main.o checkers-env.o checkers-game.o player.o player-human.o player-basic.o player-minimax.o checkers-eval.o exceptions.o
CC = g++
CFLAGS = -c
LFLAGS_DB = 

play: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o play

exceptions.o: exceptions.cpp exceptions.h
	$(CC) $(CFLAGS) $<

checkers-env.o: checkers-env.cpp checkers-env.h exceptions.h
	$(CC) $(CFLAGS) $<

checkers-game.o: checkers-game.cpp checkers-game.h checkers-env.h
	$(CC) $(CFLAGS) $<

checkers-eval.o: checkers-eval.cpp checkers-eval.h checkers-env.o exceptions.o
	$(CC) $(CFLAGS) $<

player.o: player.cpp player.h checkers-env.o
	$(CC) $(CFLAGS) $<

player-human.o: player-human.cpp player-human.h player.o
	$(CC) $(CFLAGS) $<

player-basic.o: player-basic.cpp player-basic.h player.o
	$(CC) $(CFLAGS) $<

player-minimax.o: player-minimax.cpp player-minimax.h player.o checkers-eval.o
	$(CC) $(CFLAGS) $<

main.o: main.cpp checkers-game.o player-basic.o player-human.o checkers-eval.o player-minimax.o
	$(CC) $(CFLAGS) $<



clean: 
	rm -f $(OBJS) play *.gch *~
