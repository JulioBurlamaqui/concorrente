package main

import (
	"fmt"
	"math"
	"sync"
)

var wg sync.WaitGroup

func ehPrimo(n int64) bool {
	if n <= 1 {
		return false
	}
	if n == 2 {
		return true
	}
	if n%2 == 0 {
		return false
	}
	for i := int64(3); i <= int64(math.Sqrt(float64(n))); i += 2 {
		if n%i == 0 {
			return false
		}
	}
	return true
}

func verificaPrimo(n int64, wg *sync.WaitGroup, numeros chan<- string) {
	defer wg.Done()

	if ehPrimo(n) {
		numeros <- fmt.Sprintf("%d é primo", n)
	} else {
		numeros <- fmt.Sprintf("%d não é primo", n)
	}
}

func main() {
	var N int64
	fmt.Println("Digite um número:")
	fmt.Scanf("%d", &N)
	//N = 30

	numeros := make(chan string)

	for i := N; i > 0; i-- {
		wg.Add(1)
		go verificaPrimo(i, &wg, numeros)
	}

	go func() {
		wg.Wait()
		close(numeros)
	}()

	for numero := range numeros {
		fmt.Println(numero)
	}
}
