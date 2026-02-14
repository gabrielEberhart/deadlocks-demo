PARAMS := -Os -Wall -pthread
app:
	gcc deadlocks.c -odeadlock $(PARAMS)
