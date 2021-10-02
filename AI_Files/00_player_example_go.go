package main

import (
	"fmt"
)

const (
	protocol = "unix"
	sockAddr = "./serversocket"
)

func main() {
	fmt.Println("Hello Go")
}
