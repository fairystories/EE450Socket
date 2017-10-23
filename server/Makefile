all: aws client serverA serverB serverC

aws: aws.c
	gcc -o aws aws.c

client: client.c
	gcc -o client client.c

serverA: serverA.c
	gcc -o serverA serverA.c

serverB: serverB.c
	gcc -o serverB serverB.c

serverC: serverC.c
	gcc -o serverC serverC.c

clean: 
	$(RM) aws client serverA serverB serverC