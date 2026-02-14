PARAMS := -O3 -Wall -v -pthread
app:
	gcc deadlocks.c -odeadlock $(PARAMS)
