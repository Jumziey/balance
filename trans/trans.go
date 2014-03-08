package main

import (
	"fmt"
	"bufio"
	"os"
	"time"
)

type point3d struct {
	x int
	y int
	z int
}

const accScale double = 127
const gyroScale double = 14.07

func main() {
	scanner := bufio.NewScanner(os.Stdin);
	var acc, gyro point3d
	t := 0.0 //time in milliseconds
	tDiff := 0.0
	
	for {
		if !scanner.Scan() {
			time.Sleep(10 * time.Milliseconds)
			continue; //we assume we'll get input
		}
		in := scanner.Text()
		acc.x = in[0]
		acc.y = in[1]
		acc.z = in[2]
		
		gyro.x = in[3]
		gyro.y = in[4]
		gyro.z = in[5]
		
		tDiff = in[6] - t
		t = in[6]
		
		fmt.Print(acc);fmt.Println(gyro)
	}
}
		
	
	
