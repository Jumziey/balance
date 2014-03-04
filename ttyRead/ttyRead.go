package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	//"time"
)

func main() {
	flag.Parse();
	file, err := os.Open(flag.Arg(0))
	if err !=nil {
		log.Fatal(err)
	}
	
	data := make([]byte,1)
	for {
		_, err := file.Read(data)
		if err != nil {
			continue
		}
		fmt.Print(string(data))
	}
}
	
