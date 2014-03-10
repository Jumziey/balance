package main

import (
	"flag"
	"fmt"
	"log"
	"math"
	"os"
	"strconv"
	"strings"
	//"time"
)

type point3d struct {
	x float64
	y float64
	z float64
}

const accScale float64 = 127
const gyroScale float64 = 14.07

func main() {
	var acc, gyro point3d
	var dt float64
	var trans func(string)(point3d,point3d,float64)
	
	flag.Parse()
	file, err := os.Open(flag.Arg(0))
	if err != nil {
		log.Fatal(err)
	}

	ch := make([]byte, 1)
	data := make([]byte, 1)
	
	for {
		_, err := file.Read(ch)
		if err != nil {
			continue
		}
		if rune(ch[0]) == 'x' { //we're now properly synced
			break
		}
	}
	
	for {
		_, err := file.Read(ch)
		if err != nil {
			continue
		}
		data = append(data, ch[0])
		if rune(ch[0]) == '\n' {
			fmt.Println(string(data))
			trans = translate(string(data))
			break
		}
	}
	fmt.Println("Do i even get here?")

	for {
		_, err := file.Read(ch)
		if err != nil {
			continue
		}
		data = append(data, ch...)
		if rune(ch[0]) == '\n' {
			fmt.Println(string(data))
			acc, gyro, dt = trans(string(data))
			fmt.Print(acc, " \t:\t")
			fmt.Print(gyro, "\t:\t")
			fmt.Println(dt)
			data = nil
		}
	}
	println("How can i even get here???\n")
}

func stringToFloat(in []string) []float64 {
	out := make([]float64, len(in))
	for i, v := range in {
		out[i], _ = strconv.ParseFloat(v, 64)
	}
	return out
}

//does the first "estimate" and initializes values
func translate(data string) (func(data string) (point3d, point3d, float64)) {
	var in []float64
	var t float64 = 0.0
	est := point3d{0, 0, 0} //gravity vector estimate
	acc := point3d{0, 0, 0}
	gyro := point3d{0, 0, 0}
	dt := 0.0

	in = stringToFloat(strings.Fields(data))
	if len(in) < 7 {
		log.Fatalln("Bad init of acc card")
	}
	acc.x = in[0] / accScale
	acc.y = in[1] / accScale
	acc.z = in[2] / accScale
	accLength := math.Sqrt(acc.x*acc.x + acc.y*acc.y + acc.z*acc.z)
	acc.x = acc.x / accLength
	acc.y = acc.y / accLength
	acc.z = acc.z / accLength

	//first time we assume est = acc
	est.x = acc.x
	est.y = acc.y
	est.z = acc.z

	t = in[6]
	return func(data string) (point3d, point3d, float64) {

		in = stringToFloat(strings.Fields(data))
		
		acc.x = in[0] / accScale
		acc.y = in[1] / accScale
		acc.z = in[2] / accScale
		gyro.x = in[3] / gyroScale
		gyro.y = in[4] / gyroScale
		gyro.z = in[5] / gyroScale

		//Normalizing
		accLength := math.Sqrt(acc.x*acc.x + acc.y*acc.y + acc.z*acc.z)
		acc.x = acc.x / accLength
		acc.y = acc.y / accLength
		acc.z = acc.z / accLength
		/*
			gyroLength = math.Sqrt(gyro.x*gyro.x + gyro.y*gyro.y + gyro.z*gyro.z)
			gyro.x = gyro.x/gyroLength
			gyro.y = gyro.y/gyroLength
			gyro.z = gyro.z/gyroLength
		*/

		dt = in[6] - t
		t = in[6]

		return acc, gyro, dt
	}
}
