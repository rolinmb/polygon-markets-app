package main

import (
	"context"
	"fmt"
	"log"
	"os"
	"strconv"
	"strings"
	"time"

	polygon "github.com/polygon-io/client-go/rest"
	"github.com/polygon-io/client-go/rest/models"
)

const (
	APIKEY = "HA"
)

func main() {
	if len(os.Args) != 5 {
		log.Fatalf("main.go :: Usage: go run main.go <MODE_NUMBER(0=Equities,1=Forex,2=Crypto)> <ASSET> <FROM_DATE(YYYY-MM-DD)> <TO_DATE(YYYY-MM-DD)>")
	}
	// First arg: mode number
	mode, err := strconv.Atoi(os.Args[1])
	if err != nil {
		log.Fatalf("main.go :: Mode number must be an integer, got %q", os.Args[1])
	}
	if mode < 0 || mode > 2 {
		log.Fatalf("main.go :: Mode number must be in range [0,2], got %d", mode)
	}
	// Second arg: asset symbol
	asset := strings.ToUpper(os.Args[2])
	switch mode {
	case 0: // Equities
		if len(asset) > 4 {
			log.Fatalf("main.go :: Equities asset must be at most 4 letters, got %q", asset)
		}
	case 1: // Forex
		if len(asset) != 6 {
			log.Fatalf("main.go :: Forex asset must be 6 letters (e.g. EURUSD), got %q", asset)
		}
	case 2: // Crypto
		if len(asset) < 6 || len(asset) > 7 {
			log.Fatalf("main.go :: Crypto asset must be 6 or 7 letters (e.g. BTCUSD, DOGEUSD), got %q", asset)
		}
	}
	// Build ticker prefix depending on mode
	var ticker string
	switch mode {
	case 0: // Equities
		ticker = asset
	case 1: // Forex
		ticker = "C:" + asset
	case 2: // Crypto
		ticker = "X:" + asset
	}
	// Third arg: from date
	from, err := time.Parse("2006-01-02", os.Args[3])
	if err != nil {
		log.Fatalf("main.go :: Error parsing 'from' date (%s): %v", os.Args[3], err)
	}
	// Fourth arg: to date
	to, err := time.Parse("2006-01-02", os.Args[4])
	if err != nil {
		log.Fatalf("main.go :: Error parsing 'to' date (%s): %v", os.Args[4], err)
	}
	// Polygon client
	c := polygon.New(APIKEY)
	// Build query parameters
	params := models.ListAggsParams{
		Ticker:     ticker,
		Multiplier: 1,
		Timespan:   "day",
		From:       models.Millis(from),
		To:         models.Millis(to),
	}.WithAdjusted(true).WithOrder(models.Order("asc")).WithLimit(120)
	// Run query
	iter := c.ListAggs(context.Background(), params)
	for iter.Next() {
		item := iter.Item()
		fmt.Printf("%s :: %+v\n", ticker, item)
	}
	if iter.Err() != nil {
		log.Fatal("main.go :: " + iter.Err().Error())
	}
}
