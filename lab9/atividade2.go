// introduz canais na aplicação Go
package main

import (
	"fmt"
	"time"
)

func tarefa(str chan string) {
	fmt.Println(<-str)
	str <- "Oi Main, bom dia, tudo bem?"

	fmt.Println(<-str)
	str <- "Certo, entendido!"

	fmt.Printf("Terminando...\n")
}

func main() {
	var msg = "Olá, Goroutine, bom dia!" //string inicial

	//cria um canal de comunicacao nao-bufferizado
	str := make(chan string)

	//cria um goroutine que executará a função 'tarefa'
	go tarefa(str)

	str <- msg

	//lê do canal
	msg = <-str
	fmt.Println(msg)

	str <- "Tudo bem! Vou terminar, tá?"

	fmt.Println(<-str)

	time.Sleep(time.Second / 100)

	fmt.Printf("Terminando...\n")
}
